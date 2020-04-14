/***********************************************************************
* ui.cpp - Example of a user interface                                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2019 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <fstream>
#include <functional>
#include <map>
#include <iostream>
#include <string>
#include <vector>

#include <final/final.h>

namespace fc = finalcut::fc;
using finalcut::FPoint;
using finalcut::FSize;


//----------------------------------------------------------------------
// class ProgressDialog
//----------------------------------------------------------------------

class ProgressDialog final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit ProgressDialog (finalcut::FWidget* = nullptr);

    // Copy constructor
    ProgressDialog (const ProgressDialog&) = default;

    // Destructor
    ~ProgressDialog() override;

    // Assignment operator (=)
    ProgressDialog& operator = (const ProgressDialog&) = default;

  private:
    // Event handlers
    void onShow (finalcut::FShowEvent*) override;
    void onTimer (finalcut::FTimerEvent*) override;

    // Callback methods
    void cb_reset_bar (const finalcut::FWidget*, const FDataPtr);
    void cb_more_bar (const finalcut::FWidget*, const FDataPtr);
    void cb_exit_bar (const finalcut::FWidget*, const FDataPtr);

    // Data members
    finalcut::FProgressbar progressBar{this};
    finalcut::FButton      reset{this};
    finalcut::FButton      more{this};
    finalcut::FButton      quit{this};
};

//----------------------------------------------------------------------
ProgressDialog::ProgressDialog (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  setGeometry ( FPoint(int((getParentWidget()->getWidth() - 40) / 2), 7)
              , FSize(40, 10) );
  setText("Progress bar");
  //setModal();

  reset.setText("&Reset");
  reset.setStatusbarMessage ("Reset the progress bar");
  reset.setGeometry(FPoint(2, 6), FSize(8, 1), false);
  reset.setDisable();

  more.setText("&More");
  more.setStatusbarMessage ("Increases the progress bar position");
  more.setGeometry(FPoint(15, 6), FSize(8, 1), false);
  more.setDisable();

  quit.setText("E&xit");
  quit.setGeometry(FPoint(28, 6), FSize(8, 1), false);
  quit.setDisable();

  progressBar.setGeometry(FPoint(2, 3), FSize(34, 1), false);
  //progressBar.setPercentage(78);

  using namespace std::placeholders;
  reset.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &ProgressDialog::cb_reset_bar)
  );

  more.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &ProgressDialog::cb_more_bar)
  );

  quit.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &ProgressDialog::cb_exit_bar)
  );
}

//----------------------------------------------------------------------
ProgressDialog::~ProgressDialog()  // destructor
{
  delOwnTimer();
  delCallback(&quit);
  delCallback(&more);
  delCallback(&reset);
}

//----------------------------------------------------------------------
void ProgressDialog::onShow (finalcut::FShowEvent*)
{
  addTimer(15);  // Starts the timer every 15 ms
}

//----------------------------------------------------------------------
void ProgressDialog::onTimer (finalcut::FTimerEvent*)
{
  auto p = progressBar.getPercentage();
  progressBar.setPercentage(++p);
  flush();

  if ( p != 100 )
    return;

  delOwnTimer();
  activateWindow();
  raiseWindow();
  reset.setEnable();
  reset.setFocus();
  more.setEnable();
  quit.setEnable();
  redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_reset_bar (const finalcut::FWidget*, const FDataPtr)
{
  progressBar.reset();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_more_bar (const finalcut::FWidget*, const FDataPtr)
{
  auto p = progressBar.getPercentage();
  progressBar.setPercentage(++p);
}

//----------------------------------------------------------------------
void ProgressDialog::cb_exit_bar (const finalcut::FWidget*, const FDataPtr)
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

    // Disable copy constructor
    TextWindow (const TextWindow&) = delete;

    // Destructor
    ~TextWindow() override;

    // Disable assignment operator (=)
    TextWindow& operator = (const TextWindow&) = delete;

    // Method
    void append (const finalcut::FString&);

  private:
    // Method
    void adjustSize() override;

    // Data members
    finalcut::FTextView scrollText{this};
};

//----------------------------------------------------------------------
TextWindow::TextWindow (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  scrollText.ignorePadding();
  scrollText.setGeometry (FPoint(1, 2), FSize(getWidth(), getHeight() - 1));
  setMinimumSize (FSize(51, 6));
  scrollText.setFocus();
  scrollText.insert(" -----------------------------------------------\n"
                    " line 1\n"
                    " -----------------------------------------------\n"
                    " line 3\n"
                    " line 4"
                    , -1);
  scrollText.replaceRange("                   File viewer", 1, 1);
  scrollText.deleteRange(3, 4);
}

//----------------------------------------------------------------------
TextWindow::~TextWindow()  // destructor
{ }

//----------------------------------------------------------------------
void TextWindow::append (const finalcut::FString& str)
{
  scrollText.append(str);
}

//----------------------------------------------------------------------
void TextWindow::adjustSize()
{
  finalcut::FDialog::adjustSize();
  scrollText.setGeometry (FPoint(1, 2), FSize(getWidth(), getHeight() - 1));
}


//----------------------------------------------------------------------
// class MyDialog
//----------------------------------------------------------------------

class MyDialog final : public finalcut::FDialog
{
  public:
    // Constructor
    explicit MyDialog (finalcut::FWidget* = nullptr);

    // Disable copy constructor
    MyDialog (const MyDialog&) = delete;

    // Destructor
    ~MyDialog() override;

    // Disable assignment operator (=)
    MyDialog& operator = (const MyDialog&) = delete;

  private:
    // Methods
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
    void adjustSize() override;

    // Event handlers
    void onClose (finalcut::FCloseEvent*) override;

    // Callback methods
    void cb_noFunctionMsg (finalcut::FWidget*, const FDataPtr);
    void cb_about (const finalcut::FWidget*, const FDataPtr);
    void cb_terminfo (const finalcut::FWidget*, const FDataPtr);
    void cb_drives (const finalcut::FWidget*, const FDataPtr);
    void cb_cutClipboard (const finalcut::FWidget*, const FDataPtr);
    void cb_copyClipboard (const finalcut::FWidget*, const FDataPtr);
    void cb_pasteClipboard (const finalcut::FWidget*, const FDataPtr);
    void cb_clearInput (const finalcut::FWidget*, const FDataPtr);
    void cb_input2buttonText (finalcut::FWidget*, FDataPtr);
    void cb_setTitlebar (finalcut::FWidget*, const FDataPtr);
    void cb_showProgressBar (const finalcut::FWidget*, const FDataPtr);
    void cb_updateNumber (finalcut::FWidget*, FDataPtr);
    void cb_activateButton (finalcut::FWidget*, FDataPtr);
    void cb_view (const finalcut::FWidget*, FDataPtr);
    void cb_setInput (finalcut::FWidget*, FDataPtr);

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
    // "File" menu items
    finalcut::FMenuItem       Open{"&Open...", &File};
    finalcut::FMenu           Recent{"&System files", &File};
    finalcut::FMenuItem       Line1{&File};
    finalcut::FMenuItem       Quit{"&Quit", &File};
    // "Recent" menu items
    finalcut::FMenuItem       File1{"/etc/services", &Recent};
    finalcut::FMenuItem       File2{"/etc/fstab", &Recent};
    finalcut::FMenuItem       File3{"/etc/passwd", &Recent};
    // "Edit" menu items
    finalcut::FMenuItem       Undo{fc::Fckey_z, "Undo", &Edit};
    finalcut::FMenuItem       Redo{fc::Fckey_y, "Redo", &Edit};
    finalcut::FMenuItem       Line2{&Edit};
    finalcut::FMenuItem       Cut{fc::Fckey_x, "Cu&t", &Edit};
    finalcut::FMenuItem       Copy{fc::Fckey_c, "&Copy", &Edit};
    finalcut::FMenuItem       Paste{fc::Fckey_v, "&Paste", &Edit};
    finalcut::FMenuItem       Clear{fc::Fkey_dc, "C&lear", &Edit};
    // "View" menu items
    finalcut::FMenuItem       Env{"&Terminal...", &View};
    finalcut::FMenuItem       Drive{"&Drive symbols...", &View};
    // Statusbar
    finalcut::FStatusBar      Statusbar{this};
    finalcut::FStatusKey      key_F1{fc::Fkey_f1, "About", &Statusbar};
    finalcut::FStatusKey      key_F2{fc::Fkey_f2, "View", &Statusbar};
    finalcut::FStatusKey      key_F3{fc::Fkey_f3, "Quit", &Statusbar};
    // Dialog widgets
    finalcut::FButton         MyButton1{this};
    finalcut::FButton         MyButton2{this};
    finalcut::FButton         MyButton3{this};
    finalcut::FButtonGroup    radioButtonGroup{"Button", this};
    finalcut::FRadioButton    radio1{"E&nable", &radioButtonGroup};
    finalcut::FRadioButton    radio2{&radioButtonGroup};
    finalcut::FButtonGroup    checkButtonGroup{"Options", this};
    finalcut::FCheckBox       check1{"&Bitmode", &checkButtonGroup};
    finalcut::FCheckBox       check2{"&8-Bit", &checkButtonGroup};
    finalcut::FLineEdit       myLineEdit{this};
    finalcut::FButton         MyButton4{this};
    finalcut::FButton         MyButton5{this};
    finalcut::FButton         MyButton6{this};
    finalcut::FListBox        myList{this};
    finalcut::FLabel          headline{this};
    finalcut::FLabel          tagged{L"Tagged:", this};
    finalcut::FLabel          tagged_count{this};
    finalcut::FLabel          sum{L"Sum:", this};
    finalcut::FLabel          sum_count{this};
    finalcut::FString         clipboard{};
};

//----------------------------------------------------------------------
MyDialog::MyDialog (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
{
  initMenu();                // Initialize the program menu
  initMenuCallbacks();       // Initialize program menu callbacks
  initStatusBarCallbacks();  // Initialize status bar callbacks
  initWidgets();             // Initialize the dialog widgets
  initWidgetsCallbacks();    // Initialize dialog widget callbacks
  initialized = true;
}

//----------------------------------------------------------------------
MyDialog::~MyDialog()  // destructor
{ }

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
  Open.addAccelerator (fc::Fckey_o);  // Ctrl + O
  Open.setStatusbarMessage ("Locate and open a text file");
  Recent.setStatusbarMessage ("View text file");
  Line1.setSeparator();
  Quit.addAccelerator (fc::Fmkey_x);  // Meta/Alt + X
  Quit.setStatusbarMessage ("Exit the program");

  // "Edit" menu items
  Undo.setDisable();
  Redo.setDisable();
  Line2.setSeparator();
  Cut.setStatusbarMessage ( "Remove the input text"
                            " and put it in the clipboard" );
  Copy.setStatusbarMessage ("Copy the input text into the clipboad");
  Paste.setStatusbarMessage ("Insert text form clipboard");
  Clear.setStatusbarMessage ("Delete input text");

  // "View" menu items
  Env.setStatusbarMessage ("Informations about this terminal");
  Drive.setStatusbarMessage ("Show drive symbols");
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
  Open.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view)
  );

  Quit.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );

  // System files submenu
  File1.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view),
    static_cast<FDataPtr>(&File1)
  );

  File2.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view),
    static_cast<FDataPtr>(&File2)
  );

  File3.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view),
    static_cast<FDataPtr>(&File3)
  );
}

//----------------------------------------------------------------------
void MyDialog::initEditMenuCallbacks()
{
  // Edit menu
  Cut.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_cutClipboard)
  );

  Copy.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_copyClipboard)
  );

  Paste.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_pasteClipboard)
  );

  Clear.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_clearInput)
  );
}

//----------------------------------------------------------------------
void MyDialog::initViewMenuCallbacks()
{
  // View menu
  Env.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_terminfo)
  );

  Drive.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_drives)
  );
}

//----------------------------------------------------------------------
void MyDialog::initHelpMenuCallback()
{
  Help.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_about)
  );
}

//----------------------------------------------------------------------
void MyDialog::initStatusBarCallbacks()
{
  // Add statusbar function callbacks

  key_F1.addCallback
  (
    "activate",
    F_METHOD_CALLBACK (this, &MyDialog::cb_about)
  );

  key_F2.addCallback
  (
    "activate",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view)
  );

  key_F3.addCallback
  (
    "activate",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
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
  myLineEdit.setGeometry(FPoint(22, 1), FSize(10, 1));
  myLineEdit.setLabelText (L"&Input");
  myLineEdit.setStatusbarMessage ("Press Enter to set the title");
  myLineEdit << finalcut::FString("EnTry").toLower();

  // Buttons
  initButtons();

  // A multiple selection listbox
  myList.setGeometry(FPoint(38, 1), FSize(14, 17));
  myList.setText ("Items");

  myList.setStatusbarMessage ("99 items in a list");
  myList.setMultiSelection();
  myList.reserve(100);

  for (int z{1}; z < 100; z++)
    myList.insert (finalcut::FString() << z << L" placeholder");

  // Text labels
  initLabels();
}

//----------------------------------------------------------------------
void MyDialog::initFlatButtons()
{
  // Flat buttons
  MyButton1.setGeometry(FPoint(3, 3), FSize(5, 1));
  MyButton1.setText (L"&SIN");
  MyButton1.setStatusbarMessage ("Sine function");
  MyButton1.setNoUnderline();
  MyButton1.setFlat();
  MyButton1.setDoubleFlatLine (fc::bottom);

  MyButton2.setGeometry(FPoint(3, 5), FSize(5, 1));
  MyButton2.setText (L"&COS");
  MyButton2.setStatusbarMessage ("Cosine function");
  MyButton2.setNoUnderline();
  MyButton2.setFlat();
  MyButton2.setDoubleFlatLine (fc::top);

  MyButton3.setGeometry(FPoint(10, 3), FSize(5, 3));
  MyButton3.setText (L"&=");
  MyButton3.setStatusbarMessage ("Equal");
  MyButton3.setNoUnderline();
  MyButton3.setFlat();

  // Add button callback functions
  MyButton1.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_noFunctionMsg)
  );

  MyButton2.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_noFunctionMsg)
  );

  MyButton3.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_noFunctionMsg)
  );
}

//----------------------------------------------------------------------
void MyDialog::initToggleButtons()
{
  // Radio buttons in a group
  radioButtonGroup.setGeometry(FPoint(3, 8), FSize(14, 4));
  //radioButtonGroup->unsetBorder();

  radio1.setGeometry(FPoint(1, 1), FSize(10, 1));
  radio1.setStatusbarMessage ("Enable button Test");

  radio2.setGeometry(FPoint(1, 2), FSize(11, 1));
  radio2.setText ("&Disable");
  radio2.setStatusbarMessage ("Disable button Test");
  radio2.setChecked();
  //radio2.setDisable();

  // Checkboxes in a group
  checkButtonGroup.setGeometry(FPoint(3, 12), FSize(14, 4));

  check1.setGeometry(FPoint(1, 1), FSize(11, 1));
  check1.setNoUnderline();

  check2.setGeometry(FPoint(1, 2), FSize(9, 1));
  check2.setChecked();
  check2.setNoUnderline();
}

//----------------------------------------------------------------------
void MyDialog::initButtons()
{
  // Buttons
  MyButton4.setGeometry(FPoint(20, 8), FSize(12, 1));
  MyButton4.setText (L"&Get input");
  MyButton4.setStatusbarMessage ("Take text from input field");
  MyButton4.setFocus();

  MyButton5.setGeometry(FPoint(20, 11), FSize(12, 1));
  MyButton5.setText (L"&Test");
  MyButton5.setStatusbarMessage ("Progressbar testing dialog");
  MyButton5.setDisable();

  MyButton6.setGeometry(FPoint(20, 14), FSize(12, 1));
  MyButton6.setText (L"&Quit");
  MyButton6.setStatusbarMessage ("Exit the program");
  MyButton6.addAccelerator('x');

  // Add button callback functions
  MyButton4.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_input2buttonText),
    static_cast<FDataPtr>(&myLineEdit)
  );

  MyButton5.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_showProgressBar)
  );

  MyButton6.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &finalcut::FApplication::cb_exitApp)
  );
}

//----------------------------------------------------------------------
void MyDialog::initLabels()
{
  // Text labels
  headline.setGeometry(FPoint(21, 3), FSize(10, 1));
  headline.setEmphasis();
  headline.setAlignment (fc::alignCenter);
  headline = L"List items";

  tagged.setGeometry(FPoint(21, 4), FSize(7, 1));

  tagged_count.setGeometry(FPoint(29, 4), FSize(5, 1));
  tagged_count << 0;

  sum.setGeometry(FPoint(21, 5), FSize(7, 3));
  sum.setAlignment (fc::alignRight);

  sum_count.setGeometry(FPoint(29, 5), FSize(5, 3));
  sum_count << myList.getCount();
}

//----------------------------------------------------------------------
void MyDialog::initWidgetsCallbacks()
{
  // Add some function callbacks

  myLineEdit.addCallback
  (
    "activate",  // e.g. on <Enter>
    F_METHOD_CALLBACK (this, &MyDialog::cb_setTitlebar)
  );

  radio1.addCallback
  (
    "toggled",
    F_METHOD_CALLBACK (this, &MyDialog::cb_activateButton),
    static_cast<FDataPtr>(&MyButton5)
  );

  myList.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_setInput),
    static_cast<FDataPtr>(&myLineEdit)
  );

  myList.addCallback
  (
    "row-selected",
    F_METHOD_CALLBACK (this, &MyDialog::cb_updateNumber),
    static_cast<FDataPtr>(&tagged_count)
  );
}

//----------------------------------------------------------------------
void MyDialog::adjustSize()
{
  const auto h = getParentWidget()->getHeight() - 4;
  setHeight (h, false);
  int X = int((getDesktopWidth() - getWidth()) / 2);

  if ( X < 1 )
    X = 1;

  setX (X, false);

  if ( initialized )
    myList.setHeight (getHeight() - 3, false);

  finalcut::FDialog::adjustSize();
}

//----------------------------------------------------------------------
void MyDialog::onClose (finalcut::FCloseEvent* ev)
{
  finalcut::FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void MyDialog::cb_noFunctionMsg (finalcut::FWidget* widget, const FDataPtr)
{
  auto& button = *(static_cast<finalcut::FButton*>(widget));
  auto text = button.getText();
  text = text.replace('&', "");
  finalcut::FMessageBox::error ( this
                               , "The \"" + text + "\" button has\n"
                                 "no function");
}

//----------------------------------------------------------------------
void MyDialog::cb_about (const finalcut::FWidget*, const FDataPtr)
{
  constexpr char libver[] = F_VERSION;
  const finalcut::FString line(2, fc::BoxDrawingsHorizontal);

  finalcut::FMessageBox info ( "About"
                             , line + L" The Final Cut " + line + L"\n\n"
                               L"Version " + libver + L"\n\n"
                               L"(c) 2020 by Markus Gans"
                             , finalcut::FMessageBox::Ok, 0, 0, this );
  info.setCenterText();
  info.show();
}

//----------------------------------------------------------------------
void MyDialog::cb_terminfo (const finalcut::FWidget*, const FDataPtr)
{
  const auto x = getDesktopWidth();
  const auto y = getDesktopHeight();
  finalcut::FMessageBox info1 \
  (
    "Environment"
    , finalcut::FString()
      << "  Type: " << getTermType() << "\n"
      << "  Name: " << getTermFileName() << "\n"
      << "  Mode: " << getEncodingString() << "\n"
      << "  Size: " << x << fc::Times
                    << y << "\n"
      << "Colors: " << getMaxColor()
    , finalcut::FMessageBox::Ok, 0, 0, this
  );
  info1.setHeadline("Terminal:");
  info1.exec();
}

//----------------------------------------------------------------------
void MyDialog::cb_drives (const finalcut::FWidget*, const FDataPtr)
{
  finalcut::FMessageBox info2 \
  (
    "Drive symbols"
    , "Generic:       \n\n"
      "Network:       \n\n"
      "     CD:"
    , finalcut::FMessageBox::Ok, 0, 0, this
  );

  if ( isNewFont() )
  {
    finalcut::FLabel drive (finalcut::NF_Drive, &info2);
    drive.setGeometry (FPoint(11, 2), FSize(4, 1));
    finalcut::FLabel net (finalcut::NF_Net_Drive, &info2);
    net.setGeometry (FPoint(11, 4), FSize(4, 1));
    finalcut::FLabel cd (finalcut::NF_CD_ROM, &info2);
    cd.setGeometry (FPoint(11, 6), FSize(4, 1));
    info2.exec();
  }
  else
  {
    finalcut::FLabel drive ("  - ", &info2);
    drive.setGeometry (FPoint(11, 2), FSize(4, 1));
    finalcut::FLabel net ("  N ", &info2);
    net.setGeometry (FPoint(11, 4), FSize(4, 1));
    finalcut::FLabel cd (" CD ", &info2);
    cd.setGeometry (FPoint(11, 6), FSize(4, 1));

    if ( isMonochron() )
    {
      net.setReverseMode();
      drive.setReverseMode();
      cd.setReverseMode();
    }
    else
    {
      net.setForegroundColor (fc::White);
      net.setBackgroundColor (fc::DarkGray);
      drive.setForegroundColor (fc::White);
      drive.setBackgroundColor (fc::DarkGray);
      cd.setForegroundColor (fc::White);
      cd.setBackgroundColor (fc::DarkGray);
    }

    info2.exec();
  }
}

//----------------------------------------------------------------------
void MyDialog::cb_cutClipboard (const finalcut::FWidget*, const FDataPtr)
{
  clipboard = myLineEdit.getText();
  myLineEdit.clear();
  myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_copyClipboard (const finalcut::FWidget*, const FDataPtr)
{
  clipboard = myLineEdit.getText();
}

//----------------------------------------------------------------------
void MyDialog::cb_pasteClipboard (const finalcut::FWidget*, const FDataPtr)
{
  myLineEdit = clipboard;
  myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_clearInput (const finalcut::FWidget*, const FDataPtr)
{
  clipboard.clear();
  myLineEdit.clear();
  myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_input2buttonText (finalcut::FWidget* widget, FDataPtr data)
{
  auto& button = *(static_cast<finalcut::FButton*>(widget));
  const auto& lineedit = *(static_cast<finalcut::FLineEdit*>(data));
  button.setText(lineedit.getText());
  button.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_setTitlebar (finalcut::FWidget* widget, const FDataPtr)
{
  auto& lineedit = *(static_cast<finalcut::FLineEdit*>(widget));
  finalcut::FString title;
  lineedit >> title;
  setTermTitle (title);
  setText (title);
  redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_showProgressBar (const finalcut::FWidget*, const FDataPtr)
{
  auto p_dgl = new ProgressDialog(this);
  p_dgl->show();
}

//----------------------------------------------------------------------
void MyDialog::cb_updateNumber (finalcut::FWidget* widget, FDataPtr data)
{
  auto& list = *(static_cast<finalcut::FListBox*>(widget));
  auto& num = *(static_cast<finalcut::FLabel*>(data));
  const auto count = list.getCount();
  int select_num = 0;

  for (std::size_t n{1}; n <= count; n++)
    if ( list.isSelected(n) )
      select_num++;

  num.clear();
  num << select_num;
  num.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_activateButton (finalcut::FWidget* widget, FDataPtr data)
{
  auto& rb = *(static_cast<finalcut::FRadioButton*>(widget));
  auto& button = *(static_cast<finalcut::FButton*>(data));

  if ( rb.isChecked() )
    button.setEnable();
  else
    button.setDisable();

  button.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_view (const finalcut::FWidget*, FDataPtr data)
{
  finalcut::FString file;
  const auto& item = static_cast<finalcut::FMenuItem*>(data);

  if ( item && ! item->getText().isEmpty() )
    file = item->getText();
  else
    file = finalcut::FFileDialog::fileOpenChooser (this);

  if ( file.isNull() )
    return;

  const auto& view = new TextWindow(this);
  finalcut::FString filename(basename(const_cast<char*>(file.c_str())));
  view->setText ("Viewer: " + filename);
  view->setGeometry ( FPoint ( 1 + int((getRootWidget()->getWidth() - 60) / 2),
                               int(getRootWidget()->getHeight() / 6) )
                    , FSize(60, getRootWidget()->getHeight() * 3 / 4) );
  view->setResizeable();
  std::string line = "";
  std::ifstream infile;
  infile.open(file);

  while ( ! infile.eof() && infile.good() )
  {
    getline(infile, line);
    view->append(line);
  }

  if ( infile.is_open() )
    infile.close();

  view->show();
}

//----------------------------------------------------------------------
void MyDialog::cb_setInput (finalcut::FWidget* widget, FDataPtr data)
{
  auto& ListBox = *(static_cast<finalcut::FListBox*>(widget));
  auto& lineedit = *(static_cast<finalcut::FLineEdit*>(data));
  lineedit = ListBox.getItem(ListBox.currentItem()).getText();
  lineedit.redraw();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  const finalcut::FString ver{F_VERSION};  // Library version
  const finalcut::FString title { "The FINAL CUT "
                                + ver
                                + " (C) 2020 by Markus Gans" };

  // Create the application object app
  finalcut::FApplication app(argc, argv);
  app.setNonBlockingRead();
  app.redefineDefaultColors(true);
  app.setTermTitle (title);

  // Force vt100 encoding
  //app.setEncoding(finalcut::fc::VT100);

  // Sets the terminal size to 94×30
  //app.setTermSize(94,30);

  // Enable the final cut graphical font
  //app.setNewFont();

  // Create main dialog object d
  MyDialog d(&app);
  d.setText (title);
  d.setGeometry ( FPoint(int((app.getWidth() - 56) / 2), 2)
                , FSize(56, app.getHeight() - 4) );
  d.setShadow();

  // Set the dialog object d as the main widget of the application.
  // When you close the main widget, the application will be closed.
  finalcut::FWidget::setMainWidget(&d);

  // Show the dialog d
  d.show();

  // Start the application
  // and return the result to the operating system
  return app.exec();
}
