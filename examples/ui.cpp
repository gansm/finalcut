/***********************************************************************
* ui.cpp - Example of a user interface                                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2024 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include <final/final.h>

using FKey = finalcut::FKey;
using finalcut::FColor;
using finalcut::FPoint;
using finalcut::FSize;
using namespace std::literals::string_literals;

//----------------------------------------------------------------------
// class ProgressDialog
//----------------------------------------------------------------------

class ProgressDialog final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit ProgressDialog (finalcut::FWidget* = nullptr);

    // Destructor
    ~ProgressDialog() override;

  private:
    // Method
    void initLayout() override;

    // Event handlers
    void onShow (finalcut::FShowEvent*) override;
    void onTimer (finalcut::FTimerEvent*) override;

    // Callback methods
    void cb_reset_bar();
    void cb_more_bar();
    void cb_exit_bar();

    // Data members
    finalcut::FProgressbar progressbar{this};
    finalcut::FButton      reset{this};
    finalcut::FButton      more{this};
    finalcut::FButton      quit{this};
};

//----------------------------------------------------------------------
ProgressDialog::ProgressDialog (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  //setModal();
  reset.setText("&Reset");
  reset.setStatusbarMessage ("Reset the progress bar");
  reset.setDisable();

  more.setText("&More");
  more.setStatusbarMessage ("Increases the progress bar position");
  more.setDisable();

  quit.setText("E&xit");
  quit.setDisable();

  //progressbar.setPercentage(78);

  reset.addCallback
  (
    "clicked",
    this, &ProgressDialog::cb_reset_bar
  );

  more.addCallback
  (
    "clicked",
    this, &ProgressDialog::cb_more_bar
  );

  quit.addCallback
  (
    "clicked",
    this, &ProgressDialog::cb_exit_bar
  );
}

//----------------------------------------------------------------------
ProgressDialog::~ProgressDialog()  // destructor
{
  delOwnTimers();
  delCallback(&quit);
  delCallback(&more);
  delCallback(&reset);
}

//----------------------------------------------------------------------
void ProgressDialog::initLayout()
{
  FDialog::setGeometry ( FPoint{int((getParentWidget()->getWidth() - 40) / 2), 7}
                       , FSize{40, 10} );
  FDialog::setText("Progress bar");
  reset.setGeometry(FPoint{2, 6}, FSize{8, 1}, false);
  more.setGeometry(FPoint{15, 6}, FSize{8, 1}, false);
  quit.setGeometry(FPoint{28, 6}, FSize{8, 1}, false);
  progressbar.setGeometry(FPoint{2, 3}, FSize{34, 1}, false);
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void ProgressDialog::onShow (finalcut::FShowEvent*)
{
  addTimer(15);  // Starts the timer every 15 ms
}

//----------------------------------------------------------------------
void ProgressDialog::onTimer (finalcut::FTimerEvent*)
{
  auto p = progressbar.getPercentage();
  p++;
  progressbar.setPercentage(p);
  flush();

  if ( p != 100 )
    return;

  delOwnTimers();
  activateWindow();
  raiseWindow();
  reset.setEnable();
  reset.setFocus();
  more.setEnable();
  quit.setEnable();
  redraw();
  finalcut::drawStatusBarMessage();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_reset_bar()
{
  progressbar.reset();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_more_bar()
{
  auto p = progressbar.getPercentage();
  p++;
  progressbar.setPercentage(p);
}

//----------------------------------------------------------------------
void ProgressDialog::cb_exit_bar()
{
  close();
}


//----------------------------------------------------------------------
// class TextWindow
//----------------------------------------------------------------------

class TextWindow final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit TextWindow (finalcut::FWidget* = nullptr);

    // Method
    void append (const finalcut::FString&);

  private:
    // Method
    void initLayout() override;
    void adjustSize() override;

    // Data members
    finalcut::FTextView scrolltext{this};
};

//----------------------------------------------------------------------
TextWindow::TextWindow (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  scrolltext.ignorePadding();
  scrolltext.setFocus();
  scrolltext.insert(" -----------------------------------------------\n"
                    " line 1\n"
                    " -----------------------------------------------\n"
                    " line 3\n"
                    " line 4"
                    , -1);
  scrolltext.replaceRange("                   File viewer", 1, 1);
  scrolltext.deleteRange(3, 4);
}

//----------------------------------------------------------------------
void TextWindow::append (const finalcut::FString& str)
{
  scrolltext.append(str);
}

//----------------------------------------------------------------------
void TextWindow::initLayout()
{
  scrolltext.setGeometry (FPoint{1, 2}, FSize{getWidth(), getHeight() - 1});
  setMinimumSize (FSize{51, 6});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void TextWindow::adjustSize()
{
  finalcut::FDialog::adjustSize();
  scrolltext.setGeometry (FPoint{1, 2}, FSize(getWidth(), getHeight() - 1));
}


//----------------------------------------------------------------------
// class MyDialog
//----------------------------------------------------------------------

class MyDialog final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit MyDialog (finalcut::FWidget* = nullptr);

  private:
    struct FileMenuItems
    {
      explicit FileMenuItems (finalcut::FMenu& menu)
        : super_menu{menu}
      { }

      // "File" menu items
      finalcut::FMenu& super_menu;
      finalcut::FMenuItem Open{"&Open...", &super_menu};
      finalcut::FMenu Recent{"&System files", &super_menu};
      finalcut::FMenuItem Line1{&super_menu};
      finalcut::FMenuItem Quit{"&Quit", &super_menu};
      // "Recent" menu items
      finalcut::FMenuItem File1{"/etc/services", &Recent};
      finalcut::FMenuItem File2{"/etc/fstab", &Recent};
      finalcut::FMenuItem File3{"/etc/passwd", &Recent};
    };

    struct EditMenuItems
    {
      explicit EditMenuItems (finalcut::FMenu& menu)
        : super_menu{menu}
      { }

      // "Edit" menu items
      finalcut::FMenu& super_menu;
      finalcut::FMenuItem Undo{FKey::Ctrl_z, "Undo", &super_menu};
      finalcut::FMenuItem Redo{FKey::Ctrl_y, "Redo", &super_menu};
      finalcut::FMenuItem Line2{&super_menu};
      finalcut::FMenuItem Cut{FKey::Ctrl_x, "Cu&t", &super_menu};
      finalcut::FMenuItem Copy{FKey::Ctrl_c, "&Copy", &super_menu};
      finalcut::FMenuItem Paste{FKey::Ctrl_v, "&Paste", &super_menu};
      finalcut::FMenuItem Clear{FKey::Del_char, "C&lear", &super_menu};
    };

    struct ViewMenuItems
    {
      explicit ViewMenuItems (finalcut::FMenu& menu)
        : super_menu{menu}
      { }

      // "View" menu items
      finalcut::FMenu& super_menu;
      finalcut::FMenuItem Env{"&Terminal...", &super_menu};
      finalcut::FMenuItem Drive{"&Drive symbols...", &super_menu};
      finalcut::FMenuItem Line3{&super_menu};
      finalcut::FCheckMenuItem Theme{"Dark &mode", &super_menu};
    };

    struct StatusBarItems
    {
      explicit StatusBarItems (finalcut::FStatusBar& sbar)
        : statusbar{sbar}
      { }

      // Statusbar items
      finalcut::FStatusBar& statusbar;
      finalcut::FStatusKey key_F1{FKey::F1, "About", &statusbar};
      finalcut::FStatusKey key_F2{FKey::F2, "View", &statusbar};
      finalcut::FStatusKey key_F3{FKey::F3, "Quit", &statusbar};
    };

    struct DialogWidgets
    {
      explicit DialogWidgets (MyDialog& dgl)
        : dialog{dgl}
      { }

      MyDialog& dialog;
      finalcut::FButton MyButton1{&dialog};
      finalcut::FButton MyButton2{&dialog};
      finalcut::FButton MyButton3{&dialog};
      finalcut::FButtonGroup radioButtonGroup{"Button", &dialog};
      finalcut::FRadioButton radio1{"E&nable", &radioButtonGroup};
      finalcut::FRadioButton radio2{&radioButtonGroup};
      finalcut::FButtonGroup checkButtonGroup{"Options", &dialog};
      finalcut::FCheckBox check1{"&Bitmode", &checkButtonGroup};
      finalcut::FCheckBox check2{"&8-Bit", &checkButtonGroup};
      finalcut::FLineEdit myLineEdit{&dialog};
      finalcut::FButton MyButton4{&dialog};
      finalcut::FButton MyButton5{&dialog};
      finalcut::FButton MyButton6{&dialog};
      finalcut::FListBox myList{&dialog};
      finalcut::FLabel headline{&dialog};
      finalcut::FLabel tagged{L"Tagged:", &dialog};
      finalcut::FLabel tagged_count{&dialog};
      finalcut::FLabel sum{L"Sum:", &dialog};
      finalcut::FLabel sum_count{&dialog};
    };

    // Methods
    void init();
    void initMenu();
    void initMenuCallbacks();
    void initFileMenuCallbacks();
    void initEditMenuCallbacks();
    void initViewMenuCallbacks();
    void initHelpMenuCallback();
    void initStatusBarCallbacks();
    void initWidgets();
    void initFlatButtons();
    void initToggleButtons();
    void initButtons();
    void initLabels();
    void initWidgetsCallbacks();
    void initLayout() override;
    void adjustSize() override;

    // Event handlers
    void onClose (finalcut::FCloseEvent*) override;

    // Callback methods
    void cb_noFunctionMsg (const finalcut::FButton&);
    void cb_about();
    void cb_terminfo();
    void cb_drives();
    void cb_cutClipboard();
    void cb_copyClipboard();
    void cb_pasteClipboard();
    void cb_clearInput();
    void cb_switchTheme (const finalcut::FCheckMenuItem*) const;
    void cb_input2buttonText ( finalcut::FButton&
                             , const finalcut::FLineEdit& ) const;
    void cb_setTitlebar (const finalcut::FLineEdit&);
    void cb_showProgressBar();
    void cb_updateNumber();
    void cb_activateButton ( const finalcut::FRadioButton&
                           , finalcut::FButton& ) const;
    void cb_view (const finalcut::FMenuItem*);
    void cb_setInput (const finalcut::FListBox&, finalcut::FLineEdit&) const;

    // Data members
    bool                      initialized{false};
    finalcut::FMenuBar        Menubar{this};
    // Menu bar items
    finalcut::FMenu           File{"&File", &Menubar};
    finalcut::FMenu           Edit{"&Edit", &Menubar};
    finalcut::FMenu           View{"&View", &Menubar};
    finalcut::FMenuItem       Options{"&Options", &Menubar};
    finalcut::FDialogListMenu Window{"&Window", &Menubar};
    finalcut::FMenuItem       Help{"&Help", &Menubar};
    // Menus
    FileMenuItems             file_menu{File};
    EditMenuItems             edit_menu{Edit};
    ViewMenuItems             view_menu{View};
    // Statusbar
    finalcut::FStatusBar      Statusbar{this};
    StatusBarItems            statusbar_items{Statusbar};
    // Dialog widgets
    DialogWidgets             dialog_widgets{*this};
    finalcut::FString         clipboard{};
};

//----------------------------------------------------------------------
MyDialog::MyDialog (finalcut::FWidget* parent)
  : finalcut::FDialog{parent}
{
  init();
}

//----------------------------------------------------------------------
void MyDialog::init()
{
  initMenu();                // Initialize the program menu
  initMenuCallbacks();       // Initialize program menu callbacks
  initStatusBarCallbacks();  // Initialize status bar callbacks
  initWidgets();             // Initialize the dialog widgets
  initWidgetsCallbacks();    // Initialize dialog widget callbacks
  initialized = true;
}

//----------------------------------------------------------------------
void MyDialog::initMenu()
{
  // Menu bar items
  File.setStatusbarMessage ("File management commands");
  Edit.setStatusbarMessage ("Cut-and-paste editing commands");
  View.setStatusbarMessage ("Show internal informations");
  Options.setStatusbarMessage ("Set program defaults");
  Options.setDisable();
  Window.setStatusbarMessage ("List of all the active dialogs");
  Help.setStatusbarMessage ("Show version and copyright information");

  // "File" menu items
  file_menu.Open.addAccelerator (FKey::Ctrl_o);  // Ctrl + O
  file_menu.Open.setStatusbarMessage ("Locate and open a text file");
  file_menu.Recent.setStatusbarMessage ("View text file");
  file_menu.Line1.setSeparator();
  file_menu.Quit.addAccelerator (FKey::Meta_x);  // Meta/Alt + X
  file_menu.Quit.setStatusbarMessage ("Exit the program");

  // "Edit" menu items
  edit_menu.Undo.setDisable();
  edit_menu.Redo.setDisable();
  edit_menu.Line2.setSeparator();
  edit_menu.Cut.setStatusbarMessage ( "Remove the input text"
                                      " and put it in the clipboard" );
  edit_menu.Copy.setStatusbarMessage ("Copy the input text into the clipboad");
  edit_menu.Paste.setStatusbarMessage ("Insert text form clipboard");
  edit_menu.Clear.setStatusbarMessage ("Delete input text");

  // "View" menu items
  view_menu.Env.setStatusbarMessage ("Informations about this terminal");
  view_menu.Drive.setStatusbarMessage ("Show drive symbols");
  view_menu.Line3.setSeparator();

  if ( finalcut::FStartOptions::getInstance().dark_theme )
    view_menu.Theme.setChecked();
}

//----------------------------------------------------------------------
void MyDialog::initMenuCallbacks()
{
  // Menu function callbacks
  initFileMenuCallbacks();
  initEditMenuCallbacks();
  initViewMenuCallbacks();
  initHelpMenuCallback();
}

//----------------------------------------------------------------------
void MyDialog::initFileMenuCallbacks()
{
  // File menu
  file_menu.Open.addCallback
  (
    "clicked",
    this, &MyDialog::cb_view,
    nullptr
  );

  file_menu.Quit.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );

  // System files submenu
  file_menu.File1.addCallback
  (
    "clicked",
    this, &MyDialog::cb_view,
    &file_menu.File1
  );

  file_menu.File2.addCallback
  (
    "clicked",
    this, &MyDialog::cb_view,
    &file_menu.File2
  );

  file_menu.File3.addCallback
  (
    "clicked",
    this, &MyDialog::cb_view,
    &file_menu.File3
  );
}

//----------------------------------------------------------------------
void MyDialog::initEditMenuCallbacks()
{
  // Edit menu
  edit_menu.Cut.addCallback
  (
    "clicked",
    this, &MyDialog::cb_cutClipboard
  );

  edit_menu.Copy.addCallback
  (
    "clicked",
    this, &MyDialog::cb_copyClipboard
  );

  edit_menu.Paste.addCallback
  (
    "clicked",
    this, &MyDialog::cb_pasteClipboard
  );

  edit_menu.Clear.addCallback
  (
    "clicked",
    this, &MyDialog::cb_clearInput
  );
}

//----------------------------------------------------------------------
void MyDialog::initViewMenuCallbacks()
{
  // View menu
  view_menu.Env.addCallback
  (
    "clicked",
    this, &MyDialog::cb_terminfo
  );

  view_menu.Drive.addCallback
  (
    "clicked",
    this, &MyDialog::cb_drives
  );

  view_menu.Theme.addCallback
  (
    "clicked",
    this, &MyDialog::cb_switchTheme,
    &view_menu.Theme
  );
}

//----------------------------------------------------------------------
void MyDialog::initHelpMenuCallback()
{
  Help.addCallback
  (
    "clicked",
    this, &MyDialog::cb_about
  );
}

//----------------------------------------------------------------------
void MyDialog::initStatusBarCallbacks()
{
  // Add statusbar function callbacks

  statusbar_items.key_F1.addCallback
  (
    "activate",
    this, &MyDialog::cb_about
  );

  statusbar_items.key_F2.addCallback
  (
    "activate",
    this, &MyDialog::cb_view,
    nullptr
  );

  statusbar_items.key_F3.addCallback
  (
    "activate",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void MyDialog::initWidgets()
{
  // Flat buttons
  initFlatButtons();

  // Radio buttons and check boxes
  initToggleButtons();

  // A text input field
  dialog_widgets.myLineEdit.setLabelText (L"&Input");
  dialog_widgets.myLineEdit.setStatusbarMessage ("Press Enter to set the title");
  dialog_widgets.myLineEdit << finalcut::FString{"EnTry"}.toLower();

  // Buttons
  initButtons();

  // A multiple selection listbox

  dialog_widgets.myList.setText ("Items");
  dialog_widgets.myList.setStatusbarMessage ("99 items in a list");
  dialog_widgets.myList.setMultiSelection();
  dialog_widgets.myList.reserve(100);

  for (auto z{1}; z < 100; z++)
    dialog_widgets.myList.insert (finalcut::FString{} << z << L" placeholder");

  // Text labels
  initLabels();
}

//----------------------------------------------------------------------
void MyDialog::initFlatButtons()
{
  // Flat buttons
  dialog_widgets.MyButton1.setText (L"&SIN");
  dialog_widgets.MyButton1.setStatusbarMessage ("Sine function");
  dialog_widgets.MyButton1.setNoUnderline();
  dialog_widgets.MyButton1.setFlat();

  dialog_widgets.MyButton2.setText (L"&COS");
  dialog_widgets.MyButton2.setStatusbarMessage ("Cosine function");
  dialog_widgets.MyButton2.setNoUnderline();
  dialog_widgets.MyButton2.setFlat();

  dialog_widgets.MyButton3.setText (L"&=");
  dialog_widgets.MyButton3.setStatusbarMessage ("Equal");
  dialog_widgets.MyButton3.setNoUnderline();
  dialog_widgets.MyButton3.setFlat();

  // Add button callback functions
  dialog_widgets.MyButton1.addCallback
  (
    "clicked",
    this, &MyDialog::cb_noFunctionMsg,
    std::ref(dialog_widgets.MyButton1)
  );

  dialog_widgets.MyButton2.addCallback
  (
    "clicked",
    this, &MyDialog::cb_noFunctionMsg,
    std::ref(dialog_widgets.MyButton2)
  );

  dialog_widgets.MyButton3.addCallback
  (
    "clicked",
    this, &MyDialog::cb_noFunctionMsg,
    std::ref(dialog_widgets.MyButton3)
  );
}

//----------------------------------------------------------------------
void MyDialog::initToggleButtons()
{
  // Radio buttons in a group

  //dialog_widgets.radioButtonGroup->unsetBorder();
  dialog_widgets.radio1.setStatusbarMessage ("Enable button Test");
  dialog_widgets.radio2.setText ("&Disable");
  dialog_widgets.radio2.setStatusbarMessage ("Disable button Test");
  dialog_widgets.radio2.setChecked();
  //dialog_widgets.radio2.setDisable();

  // Checkboxes in a group
  dialog_widgets.check1.setNoUnderline();
  dialog_widgets.check2.setChecked();
  dialog_widgets.check2.setNoUnderline();
}

//----------------------------------------------------------------------
void MyDialog::initButtons()
{
  // Buttons

  dialog_widgets.MyButton4.setText (L"&Get input");
  dialog_widgets.MyButton4.setStatusbarMessage ("Take text from input field");
  dialog_widgets.MyButton4.setFocus();

  dialog_widgets.MyButton5.setText (L"&Test");
  dialog_widgets.MyButton5.setStatusbarMessage ("Progressbar testing dialog");
  dialog_widgets.MyButton5.setDisable();

  dialog_widgets.MyButton6.setText (L"&Quit");
  dialog_widgets.MyButton6.setStatusbarMessage ("Exit the program");
  dialog_widgets.MyButton6.addAccelerator(FKey('x'));

  // Add button callback functions
  dialog_widgets.MyButton4.addCallback
  (
    "clicked",
    this, &MyDialog::cb_input2buttonText,
    std::ref(dialog_widgets.MyButton4),
    std::cref(dialog_widgets.myLineEdit)
  );

  dialog_widgets.MyButton5.addCallback
  (
    "clicked",
    this, &MyDialog::cb_showProgressBar
  );

  dialog_widgets.MyButton6.addCallback
  (
    "clicked",
    finalcut::getFApplication(),
    &finalcut::FApplication::cb_exitApp,
    this
  );
}

//----------------------------------------------------------------------
void MyDialog::initLabels()
{
  // Text labels

  dialog_widgets.headline.setEmphasis();
  dialog_widgets.headline.setAlignment (finalcut::Align::Center);
  dialog_widgets.headline = L"List items";
  dialog_widgets.tagged_count << 0;
  dialog_widgets.sum.setAlignment (finalcut::Align::Right);
  dialog_widgets.sum_count << dialog_widgets.myList.getCount();
}

//----------------------------------------------------------------------
void MyDialog::initWidgetsCallbacks()
{
  // Add some function callbacks

  dialog_widgets.myLineEdit.addCallback
  (
    "activate",  // e.g. on <Enter>
    this, &MyDialog::cb_setTitlebar,
    std::ref(dialog_widgets.myLineEdit)
  );

  dialog_widgets.radio1.addCallback
  (
    "toggled",
    this, &MyDialog::cb_activateButton,
    std::ref(dialog_widgets.radio1),
    std::ref(dialog_widgets.MyButton5)
  );

  dialog_widgets.myList.addCallback
  (
    "clicked",
    this, &MyDialog::cb_setInput,
    std::ref(dialog_widgets.myList),
    std::ref(dialog_widgets.myLineEdit)
  );

  dialog_widgets.myList.addCallback
  (
    "row-selected",
    this, &MyDialog::cb_updateNumber
  );
}

//----------------------------------------------------------------------
void MyDialog::initLayout()
{
  dialog_widgets.MyButton1.setGeometry(FPoint{3, 3}, FSize{5, 1});
  dialog_widgets.MyButton1.setDoubleFlatLine (finalcut::Side::Bottom);
  dialog_widgets.MyButton2.setGeometry(FPoint{3, 5}, FSize{5, 1});
  dialog_widgets.MyButton2.setDoubleFlatLine (finalcut::Side::Top);
  dialog_widgets.MyButton3.setGeometry(FPoint{10, 3}, FSize{5, 3});
  dialog_widgets.MyButton4.setGeometry(FPoint{20, 8}, FSize{12, 1});
  dialog_widgets.MyButton5.setGeometry(FPoint{20, 11}, FSize{12, 1});
  dialog_widgets.MyButton6.setGeometry(FPoint{20, 14}, FSize{12, 1});
  dialog_widgets.radioButtonGroup.setGeometry(FPoint{3, 8}, FSize{14, 4});
  dialog_widgets.radio1.setGeometry(FPoint{1, 1}, FSize{10, 1});
  dialog_widgets.radio2.setGeometry(FPoint{1, 2}, FSize{11, 1});
  dialog_widgets.checkButtonGroup.setGeometry(FPoint{3, 12}, FSize{14, 4});
  dialog_widgets.check1.setGeometry(FPoint{1, 1}, FSize{11, 1});
  dialog_widgets.check2.setGeometry(FPoint{1, 2}, FSize{9, 1});
  dialog_widgets.headline.setGeometry(FPoint{21, 3}, FSize{10, 1});
  dialog_widgets.tagged.setGeometry(FPoint{21, 4}, FSize{7, 1});
  dialog_widgets.tagged_count.setGeometry(FPoint{29, 4}, FSize{5, 1});
  dialog_widgets.sum.setGeometry(FPoint{21, 5}, FSize{7, 3});
  dialog_widgets.sum_count.setGeometry(FPoint{29, 5}, FSize{5, 3});
  dialog_widgets.myLineEdit.setGeometry(FPoint{22, 1}, FSize{10, 1});
  dialog_widgets.myList.setGeometry(FPoint{38, 1}, FSize{14, 17});
  FDialog::initLayout();
}

//----------------------------------------------------------------------
void MyDialog::adjustSize()
{
  auto h = getDesktopHeight();

  if ( h > 4 )
    h -= 4;

  setHeight (h, false);
  finalcut::FDialog::adjustSize();  // with new client area size
  auto x = int((getDesktopWidth() - getWidth()) / 2);

  if ( x < 1 )
    x = 1;

  setPos (FPoint{x, 2}, false);

  if ( initialized && h > 3 )
    dialog_widgets.myList.setHeight (h - 3, true);
}

//----------------------------------------------------------------------
void MyDialog::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void MyDialog::cb_noFunctionMsg (const finalcut::FButton& button)
{
  auto text = button.getText();
  text = text.replace('&', "");
  finalcut::FMessageBox::error ( this
                               , "The \"" + text + "\" button has\n"
                                 "no function");
}

//----------------------------------------------------------------------
void MyDialog::cb_about()
{
  const finalcut::FString line(2, finalcut::UniChar::BoxDrawingsHorizontal);

  finalcut::FMessageBox info ( "About"
                             , line + L" FINAL CUT " + line + L"\n\n"
                               L"Version " + finalcut::fc_release + L"\n\n"
                               L"(c) 2024 by Markus Gans"
                             , finalcut::FMessageBox::ButtonType::Ok
                             , finalcut::FMessageBox::ButtonType::Reject
                             , finalcut::FMessageBox::ButtonType::Reject
                             , this );
  info.setCenterText();
  info.show();
}

//----------------------------------------------------------------------
void MyDialog::cb_terminfo()
{
  const auto x = getDesktopWidth();
  const auto y = getDesktopHeight();
  finalcut::FMessageBox info1 \
  (
    "Environment"
    , std::move(finalcut::FString()
      << "  Type: " << finalcut::FTerm::getTermType() << "\n"
      << "  Name: " << finalcut::FTerm::getTermFileName() << "\n"
      << "  Mode: " << finalcut::FTerm::getEncodingString() << "\n"
      << "  Size: " << x << finalcut::UniChar::Times
                    << y << "\n"
      << "Colors: " << finalcut::FVTerm::getFOutput()->getMaxColor())
    , finalcut::FMessageBox::ButtonType::Ok
    , finalcut::FMessageBox::ButtonType::Reject
    , finalcut::FMessageBox::ButtonType::Reject
    , this
  );
  info1.setHeadline("Terminal:");
  info1.exec();
}

//----------------------------------------------------------------------
void MyDialog::cb_drives()
{
  finalcut::FMessageBox info2 \
  (
    "Drive symbols"
    , "Generic:       \n\n"
      "Network:       \n\n"
      "     CD:"
    , finalcut::FMessageBox::ButtonType::Ok
    , finalcut::FMessageBox::ButtonType::Reject
    , finalcut::FMessageBox::ButtonType::Reject
    , this
  );

  if ( finalcut::FVTerm::getFOutput()->isNewFont() )
  {
    finalcut::FLabel drive {finalcut::NF_Drive, &info2};
    drive.setGeometry (FPoint{11, 2}, FSize{4, 1});
    finalcut::FLabel net {finalcut::NF_Net_Drive, &info2};
    net.setGeometry (FPoint{11, 4}, FSize{4, 1});
    finalcut::FLabel cd {finalcut::NF_CD_ROM, &info2};
    cd.setGeometry (FPoint{11, 6}, FSize{4, 1});
    info2.exec();
  }
  else
  {
    finalcut::FLabel drive {"  - ", &info2};
    drive.setGeometry (FPoint{11, 2}, FSize{4, 1});
    finalcut::FLabel net {"  N ", &info2};
    net.setGeometry (FPoint{11, 4}, FSize{4, 1});
    finalcut::FLabel cd {" CD ", &info2};
    cd.setGeometry (FPoint{11, 6}, FSize{4, 1});

    if ( finalcut::FVTerm::getFOutput()->isMonochron() )
    {
      net.setReverseMode();
      drive.setReverseMode();
      cd.setReverseMode();
    }
    else
    {
      net.setForegroundColor (FColor::White);
      net.setBackgroundColor (FColor::DarkGray);
      drive.setForegroundColor (FColor::White);
      drive.setBackgroundColor (FColor::DarkGray);
      cd.setForegroundColor (FColor::White);
      cd.setBackgroundColor (FColor::DarkGray);
    }

    info2.exec();
  }
}

//----------------------------------------------------------------------
void MyDialog::cb_cutClipboard()
{
  clipboard = dialog_widgets.myLineEdit.getText();
  dialog_widgets.myLineEdit.clear();
  dialog_widgets.myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_copyClipboard()
{
  clipboard = dialog_widgets.myLineEdit.getText();
}

//----------------------------------------------------------------------
void MyDialog::cb_pasteClipboard()
{
  dialog_widgets.myLineEdit = clipboard;
  dialog_widgets.myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_clearInput()
{
  clipboard.clear();
  dialog_widgets.myLineEdit.clear();
  dialog_widgets.myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_switchTheme (const finalcut::FCheckMenuItem* check_menu) const
{
  if ( check_menu->isChecked() )
    finalcut::FApplication::setDarkTheme();
  else
    finalcut::FApplication::setDefaultTheme();

  auto root_widget = getRootWidget();
  root_widget->resetColors();
  root_widget->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_input2buttonText ( finalcut::FButton& button
                                   , const finalcut::FLineEdit& lineedit ) const
{
  button.setText(lineedit.getText());
  button.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_setTitlebar (const finalcut::FLineEdit& lineedit)
{
  finalcut::FString title{};
  lineedit >> title;
  finalcut::FTerm::setTermTitle (title);
  setText (title);
  redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_showProgressBar()
{
  auto p_dgl = new ProgressDialog(this);
  p_dgl->show();
}

//----------------------------------------------------------------------
void MyDialog::cb_updateNumber()
{
  auto is_selected = [] (auto item) { return item.isSelected(); };
  auto select_num = std::count_if ( std::begin(dialog_widgets.myList.getData())
                                  , std::end(dialog_widgets.myList.getData())
                                  , is_selected );
  dialog_widgets.tagged_count.clear();
  dialog_widgets.tagged_count << select_num;
  dialog_widgets.tagged_count.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_activateButton ( const finalcut::FRadioButton& rb
                                 , finalcut::FButton& button) const
{
  if ( rb.isChecked() )
    button.setEnable();
  else
    button.setDisable();

  button.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_view (const finalcut::FMenuItem* item)
{
  finalcut::FString file = [&item, this] ()
  {
    if ( item && ! item->getText().isEmpty() )
      return item->getText();

    return finalcut::FFileDialog::fileOpenChooser (this);
  }();

  if ( file.isEmpty() )
    return;

  std::string path = file.toString();
  std::ifstream infile(path, std::ios::in | std::ios::binary);

  if ( ! infile )
  {
    finalcut::FMessageBox::error (this, "Cannot open file\n" + file);
    return;
  }

  const auto& view = new TextWindow(this);
  finalcut::FString filename(basename(const_cast<char*>(path.c_str())));
  view->setText ("Viewer: " + filename);
  view->setGeometry ( FPoint { 1 + int((getRootWidget()->getWidth() - 60) / 2)
                             , int(getRootWidget()->getHeight() / 6) }
                    , FSize{60, getRootWidget()->getHeight() * 3 / 4} );
  view->setMinimizable();
  view->setResizeable();
  std::string line{};

  while ( std::getline(infile, line) )
  {
    view->append(line);
  }

  infile.close();
  view->show();
}

//----------------------------------------------------------------------
void MyDialog::cb_setInput ( const finalcut::FListBox& listbox
                           , finalcut::FLineEdit& lineedit) const
{
  lineedit = listbox.getItem(listbox.currentItem()).getText();
  lineedit.redraw();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

auto main (int argc, char* argv[]) -> int
{
  const finalcut::FString title { "FINAL CUT "s + finalcut::fc_release
                                + " (C) 2024 by Markus Gans" };

  // Create the application object app
  finalcut::FApplication app{argc, argv};
  finalcut::FVTerm::setNonBlockingRead();

  // Create main dialog object d
  MyDialog d{&app};
  d.setText (title);
  d.setSize (FSize{56, app.getHeight() - 4});
  d.setShadow();  // Instead of the transparent window shadow

  // Set the dialog object d as the main widget of the application.
  // When you close the main widget, the application will be closed.
  finalcut::FWidget::setMainWidget(&d);

  // Enable the final cut graphical font
  //finalcut::FVTerm::getFOutput()->setNewFont();

  // Show the dialog d
  d.show();

  finalcut::FTerm::redefineDefaultColors(true);
  finalcut::FTerm::setTermTitle (title);

  // Sets the terminal size to 94×30
  //app.setTerminalSize(FSize{94, 30});

  // Start the application
  // and return the result to the operating system
  return app.exec();
}
