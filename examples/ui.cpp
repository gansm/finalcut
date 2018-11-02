/***********************************************************************
* ui.cpp - Example of a user interface                                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2018 Markus Gans                                      *
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
#include <iostream>
#include <string>

#include <final/final.h>


//----------------------------------------------------------------------
// class ProgressDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class ProgressDialog : public finalcut::FDialog
{
  public:
    // Constructor
    explicit ProgressDialog (finalcut::FWidget* = 0);

    // Destructor
    ~ProgressDialog();

  private:
    // Disable copy constructor
    ProgressDialog (const ProgressDialog&);
    // Disable assignment operator (=)
    ProgressDialog& operator = (const ProgressDialog&);

    // Event handlers
    virtual void onShow (finalcut::FShowEvent*);
    virtual void onTimer (finalcut::FTimerEvent*);

    // Callback methods
    void cb_reset_bar (finalcut::FWidget*, data_ptr);
    void cb_more_bar (finalcut::FWidget*, data_ptr);
    void cb_exit_bar (finalcut::FWidget*, data_ptr);

    // Data Members
    finalcut::FProgressbar progressBar;
    finalcut::FButton      reset;
    finalcut::FButton      more;
    finalcut::FButton      quit;
};
#pragma pack(pop)

//----------------------------------------------------------------------
ProgressDialog::ProgressDialog (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , progressBar(this)
  , reset(this)
  , more(this)
  , quit(this)
{
  setGeometry (int((getParentWidget()->getWidth() - 40) / 2), 7, 40, 10);
  setText("Progress bar");
  //setModal();

  reset.setText("&Reset");
  reset.setStatusbarMessage ("Reset the progress bar");
  reset.setGeometry(2, 6, 8, 1, false);
  reset.setDisable();

  more.setText("&More");
  more.setStatusbarMessage ("Increases the progress bar position");
  more.setGeometry(15, 6, 8, 1, false);
  more.setDisable();

  quit.setText("E&xit");
  quit.setGeometry(28, 6, 8, 1, false);
  quit.setDisable();

  progressBar.setGeometry(2, 3, 34, 1, false);
  //progressBar.setPercentage(78);

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
  addTimer(15);
}

//----------------------------------------------------------------------
void ProgressDialog::onTimer (finalcut::FTimerEvent*)
{
  std::size_t p = progressBar.getPercentage();
  progressBar.setPercentage(++p);
  flush_out();

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
  flush_out();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_reset_bar (finalcut::FWidget*, data_ptr)
{
  progressBar.reset();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_more_bar (finalcut::FWidget*, data_ptr)
{
  std::size_t p = progressBar.getPercentage();
  progressBar.setPercentage(++p);
}

//----------------------------------------------------------------------
void ProgressDialog::cb_exit_bar (finalcut::FWidget*, data_ptr)
{
  close();
}


//----------------------------------------------------------------------
// class TextWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class TextWindow : public finalcut::FDialog
{
  public:
    // Constructor
    explicit TextWindow (finalcut::FWidget* = 0);

    // Destructor
    ~TextWindow();

    void append (const finalcut::FString&);

  private:
    // Disable copy constructor
    TextWindow (const TextWindow&);
    // Disable assignment operator (=)
    TextWindow& operator = (const TextWindow&);

    // Method
    virtual void adjustSize();

    // Data Members
    finalcut::FTextView scrollText;
};
#pragma pack(pop)

//----------------------------------------------------------------------
TextWindow::TextWindow (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , scrollText(this)
{
  scrollText.ignorePadding();
  scrollText.setGeometry (1, 2, getWidth(), getHeight() - 1);
  setMinimumSize (51, 6);
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
  scrollText.setGeometry (1, 2, getWidth(), getHeight() - 1);
}


//----------------------------------------------------------------------
// class MyDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class MyDialog : public finalcut::FDialog
{
  public:
    // Constructor
    explicit MyDialog (finalcut::FWidget* = 0);
    // Destructor
    ~MyDialog();

  private:
    // Disable copy constructor
    MyDialog (const MyDialog&);
    // Disable assignment operator (=)
    MyDialog& operator = (const MyDialog&);

    // Method
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
    virtual void adjustSize();

    // Event handlers
    virtual void onClose (finalcut::FCloseEvent*);

    // Callback methods
    void cb_noFunctionMsg (finalcut::FWidget*, data_ptr);
    void cb_about (finalcut::FWidget*, data_ptr);
    void cb_terminfo (finalcut::FWidget*, data_ptr);
    void cb_drives (finalcut::FWidget*, data_ptr);
    void cb_cutClipboard (finalcut::FWidget*, data_ptr);
    void cb_copyClipboard (finalcut::FWidget*, data_ptr);
    void cb_pasteClipboard (finalcut::FWidget*, data_ptr);
    void cb_clearInput (finalcut::FWidget*, data_ptr);
    void cb_input2buttonText (finalcut::FWidget*, data_ptr);
    void cb_setTitlebar (finalcut::FWidget*, data_ptr);
    void cb_ProgressBar (finalcut::FWidget*, data_ptr);
    void cb_updateNumber (finalcut::FWidget*, data_ptr);
    void cb_activateButton (finalcut::FWidget*, data_ptr);
    void cb_view (finalcut::FWidget*, data_ptr);
    void cb_setInput (finalcut::FWidget*, data_ptr);

    // Data Members
    bool                      initialized;
    finalcut::FMenuBar        Menubar;
    finalcut::FMenu           File;     // Menu bar items
    finalcut::FMenu           Edit;
    finalcut::FMenu           View;
    finalcut::FMenuItem       Options;
    finalcut::FDialogListMenu Window;
    finalcut::FMenuItem       Help;
    finalcut::FMenuItem       Open;     // "File" menu items
    finalcut::FMenu           Recent;
    finalcut::FMenuItem       Line1;
    finalcut::FMenuItem       Quit;
    finalcut::FMenuItem       File1;    // "Recent" menu items
    finalcut::FMenuItem       File2;
    finalcut::FMenuItem       File3;
    finalcut::FMenuItem       Undo;
    finalcut::FMenuItem       Redo;
    finalcut::FMenuItem       Line2;
    finalcut::FMenuItem       Cut;
    finalcut::FMenuItem       Copy;
    finalcut::FMenuItem       Paste;
    finalcut::FMenuItem       Clear;
    finalcut::FMenuItem       Env;
    finalcut::FMenuItem       Drive;
    finalcut::FStatusBar      Statusbar;
    finalcut::FStatusKey      key_F1;
    finalcut::FStatusKey      key_F2;
    finalcut::FStatusKey      key_F3;
    finalcut::FButton         MyButton1;
    finalcut::FButton         MyButton2;
    finalcut::FButton         MyButton3;
    finalcut::FButtonGroup    radioButtonGroup;
    finalcut::FRadioButton    radio1;
    finalcut::FRadioButton    radio2;
    finalcut::FButtonGroup    checkButtonGroup;
    finalcut::FCheckBox       check1;
    finalcut::FCheckBox       check2;
    finalcut::FLineEdit       myLineEdit;
    finalcut::FButton         MyButton4;
    finalcut::FButton         MyButton5;
    finalcut::FButton         MyButton6;
    finalcut::FListBox        myList;
    finalcut::FLabel          headline;
    finalcut::FLabel          tagged;
    finalcut::FLabel          tagged_count;
    finalcut::FLabel          sum;
    finalcut::FLabel          sum_count;
    finalcut::FString         clipboard;
};
#pragma pack(pop)

//----------------------------------------------------------------------
MyDialog::MyDialog (finalcut::FWidget* parent)
  : finalcut::FDialog(parent)
  , initialized(false)
  , Menubar(this)
  , File("&File", &Menubar)
  , Edit("&Edit", &Menubar)
  , View("&View", &Menubar)
  , Options("&Options", &Menubar)
  , Window("&Window", &Menubar)
  , Help("&Help", &Menubar)
  , Open("&Open...", &File)
  , Recent("&System files", &File)
  , Line1(&File)
  , Quit("&Quit", &File)
  , File1("/etc/services", &Recent)
  , File2("/etc/fstab", &Recent)
  , File3("/etc/passwd", &Recent)
  , Undo(finalcut::fc::Fckey_z, "Undo", &Edit)
  , Redo(finalcut::fc::Fckey_y, "Redo", &Edit)
  , Line2(&Edit)
  , Cut(finalcut::fc::Fckey_x, "Cu&t", &Edit)
  , Copy(finalcut::fc::Fckey_c, "&Copy", &Edit)
  , Paste(finalcut::fc::Fckey_v, "&Paste", &Edit)
  , Clear(finalcut::fc::Fkey_dc, "C&lear", &Edit)
  , Env("&Terminal...", &View)
  , Drive("&Drive symbols...", &View)
  , Statusbar(this)
  , key_F1(finalcut::fc::Fkey_f1, "About", &Statusbar)
  , key_F2(finalcut::fc::Fkey_f2, "View", &Statusbar)
  , key_F3(finalcut::fc::Fkey_f3, "Quit", &Statusbar)
  , MyButton1(this)
  , MyButton2(this)
  , MyButton3(this)
  , radioButtonGroup("Button", this)
  , radio1("E&nable", &radioButtonGroup)
  , radio2(&radioButtonGroup)
  , checkButtonGroup("Options", this)
  , check1("&Bitmode", &checkButtonGroup)
  , check2("&8-Bit", &checkButtonGroup)
  , myLineEdit(this)
  , MyButton4(this)
  , MyButton5(this)
  , MyButton6(this)
  , myList(this)
  , headline(this)
  , tagged(L"Tagged:", this)
  , tagged_count(this)
  , sum(L"Sum:", this)
  , sum_count(this)
  , clipboard()
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
  Open.addAccelerator (finalcut::fc::Fckey_o);  // Ctrl + O
  Open.setStatusbarMessage ("Locate and open a text file");
  Recent.setStatusbarMessage ("View text file");
  Line1.setSeparator();
  Quit.addAccelerator (finalcut::fc::Fmkey_x);  // Meta/Alt + X
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
    static_cast<finalcut::FWidget::data_ptr>(&File1)
  );

  File2.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view),
    static_cast<finalcut::FWidget::data_ptr>(&File2)
  );

  File3.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view),
    static_cast<finalcut::FWidget::data_ptr>(&File3)
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
  myLineEdit.setGeometry(22, 1, 10, 1);
  myLineEdit.setLabelText (L"&Input");
  myLineEdit.setStatusbarMessage ("Press Enter to set the title");
  myLineEdit << finalcut::FString("EnTry").toLower();

  // Buttons
  initButtons();

  // A multiple selection listbox
  myList.setGeometry(38, 1, 14, 17);
  myList.setText ("Items");
  myList.setStatusbarMessage ("99 items in a list");
  myList.setMultiSelection();

  for (int z = 1; z < 100; z++)
    myList.insert (finalcut::FString() << z << L" placeholder");

  // Text labels
  initLabels();
}

//----------------------------------------------------------------------
void MyDialog::initFlatButtons()
{
  // Flat buttons
  MyButton1.setGeometry(3, 3, 5, 1);
  MyButton1.setText (L"&SIN");
  MyButton1.setStatusbarMessage ("Sine function");
  MyButton1.setNoUnderline();
  MyButton1.setFlat();
  MyButton1.setDoubleFlatLine (finalcut::fc::bottom);

  MyButton2.setGeometry(3, 5, 5, 1);
  MyButton2.setText (L"&COS");
  MyButton2.setStatusbarMessage ("Cosine function");
  MyButton2.setNoUnderline();
  MyButton2.setFlat();
  MyButton2.setDoubleFlatLine (finalcut::fc::top);

  MyButton3.setGeometry(10, 3, 5, 3);
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
  radioButtonGroup.setGeometry(3, 8, 14, 4);
  //radioButtonGroup->unsetBorder();

  radio1.setGeometry(1, 1, 10, 1);
  radio1.setStatusbarMessage ("Enable button Test");

  radio2.setGeometry(1, 2, 11, 1);
  radio2.setText ("&Disable");
  radio2.setStatusbarMessage ("Disable button Test");
  radio2.setChecked();
  //radio2.setDisable();

  // Checkboxes in a group
  checkButtonGroup.setGeometry(3, 12, 14, 4);

  check1.setGeometry(1, 1, 11, 1);
  check1.setNoUnderline();

  check2.setGeometry(1, 2, 9, 1);
  check2.setChecked();
  check2.setNoUnderline();
}

//----------------------------------------------------------------------
void MyDialog::initButtons()
{
  // Buttons
  MyButton4.setGeometry(20, 8, 12, 1);
  MyButton4.setText (L"&Get input");
  MyButton4.setStatusbarMessage ("Take text from input field");
  MyButton4.setFocus();

  MyButton5.setGeometry(20, 11, 12, 1);
  MyButton5.setText (L"&Test");
  MyButton5.setStatusbarMessage ("Progressbar testing dialog");
  MyButton5.setDisable();

  MyButton6.setGeometry(20, 14, 12, 1);
  MyButton6.setText (L"&Quit");
  MyButton6.setStatusbarMessage ("Exit the program");
  MyButton6.addAccelerator('x');

  // Add button callback functions
  MyButton4.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_input2buttonText),
    static_cast<finalcut::FWidget::data_ptr>(&myLineEdit)
  );

  MyButton5.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_ProgressBar)
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
  headline.setGeometry(21, 3, 10, 1);
  headline.setEmphasis();
  headline.setAlignment (finalcut::fc::alignCenter);
  headline = L"List items";

  tagged.setGeometry(21, 4, 7, 1);

  tagged_count.setGeometry(29, 4, 5, 1);
  tagged_count << 0;

  sum.setGeometry(21, 5, 7, 3);
  sum.setAlignment (finalcut::fc::alignRight);

  sum_count.setGeometry(29, 5, 5, 3);
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
    static_cast<finalcut::FWidget::data_ptr>(&MyButton5)
  );

  myList.addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_setInput),
    static_cast<finalcut::FWidget::data_ptr>(&myLineEdit)
  );

  myList.addCallback
  (
    "row-selected",
    F_METHOD_CALLBACK (this, &MyDialog::cb_updateNumber),
    static_cast<finalcut::FWidget::data_ptr>(&tagged_count)
  );
}

//----------------------------------------------------------------------
void MyDialog::adjustSize()
{
  std::size_t h = getParentWidget()->getHeight() - 4;
  setHeight (h, false);
  int X = int((getParentWidget()->getWidth() - getWidth()) / 2);

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
void MyDialog::cb_noFunctionMsg (finalcut::FWidget* widget, data_ptr)
{
  finalcut::FButton* button = static_cast<finalcut::FButton*>(widget);
  finalcut::FString text = button->getText();
  text = text.replace('&', "");
  finalcut::FMessageBox::error ( this
                               , "The \"" + text + "\" button has\n"
                                 "no function");
}

//----------------------------------------------------------------------
void MyDialog::cb_about (finalcut::FWidget*, data_ptr)
{
  const char libver[] = F_VERSION;
  finalcut::FString line(2, wchar_t(finalcut::fc::BoxDrawingsHorizontal));

  finalcut::FMessageBox info ( "About"
                             , line + L" The Final Cut " + line + "\n\n"
                               L"Version " + libver + "\n\n"
                               L"(c) 2018 by Markus Gans"
                             , finalcut::FMessageBox::Ok, 0, 0, this );
  info.setCenterText();
  info.show();
}

//----------------------------------------------------------------------
void MyDialog::cb_terminfo (finalcut::FWidget*, data_ptr)
{
  std::size_t x = getDesktopWidth();
  std::size_t y = getDesktopHeight();
  finalcut::FMessageBox info1 \
  (
    "Environment"
    , finalcut::FString()
      << "  Type: " << getTermType() << "\n"
      << "  Name: " << getTermFileName() << "\n"
      << "  Mode: " << getEncodingString() << "\n"
      << "  Size: " << x << wchar_t(finalcut::fc::Times)
                    << y << "\n"
      << "Colors: " << getMaxColor()
    , finalcut::FMessageBox::Ok, 0, 0, this
  );
  info1.setHeadline("Terminal:");
  info1.exec();
}

//----------------------------------------------------------------------
void MyDialog::cb_drives (finalcut::FWidget*, data_ptr)
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
    drive.setGeometry (11, 2, 4, 1);
    finalcut::FLabel net (finalcut::NF_Net_Drive, &info2);
    net.setGeometry (11, 4, 4, 1);
    finalcut::FLabel cd (finalcut::NF_CD_ROM, &info2);
    cd.setGeometry (11, 6, 4, 1);
    info2.exec();
  }
  else
  {
    finalcut::FLabel drive ("  - ", &info2);
    drive.setGeometry (11, 2, 4, 1);
    finalcut::FLabel net ("  N ", &info2);
    net.setGeometry (11, 4, 4, 1);
    finalcut::FLabel cd (" CD ", &info2);
    cd.setGeometry (11, 6, 4, 1);

    if ( isMonochron() )
    {
      net.setReverseMode();
      drive.setReverseMode();
      cd.setReverseMode();
    }
    else
    {
      net.setForegroundColor (finalcut::fc::White);
      net.setBackgroundColor (finalcut::fc::DarkGray);
      drive.setForegroundColor (finalcut::fc::White);
      drive.setBackgroundColor (finalcut::fc::DarkGray);
      cd.setForegroundColor (finalcut::fc::White);
      cd.setBackgroundColor (finalcut::fc::DarkGray);
    }

    info2.exec();
  }
}

//----------------------------------------------------------------------
void MyDialog::cb_cutClipboard (finalcut::FWidget*, data_ptr)
{
  clipboard = myLineEdit.getText();
  myLineEdit.clear();
  myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_copyClipboard (finalcut::FWidget*, data_ptr)
{
  clipboard = myLineEdit.getText();
}

//----------------------------------------------------------------------
void MyDialog::cb_pasteClipboard (finalcut::FWidget*, data_ptr)
{
  myLineEdit = clipboard;
  myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_clearInput (finalcut::FWidget*, data_ptr)
{
  clipboard.clear();
  myLineEdit.clear();
  myLineEdit.redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_input2buttonText (finalcut::FWidget* widget, data_ptr data)
{
  finalcut::FButton* button = static_cast<finalcut::FButton*>(widget);
  finalcut::FLineEdit* lineedit = static_cast<finalcut::FLineEdit*>(data);
  button->setText( lineedit->getText() );
  button->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_setTitlebar (finalcut::FWidget* widget, data_ptr)
{
  finalcut::FLineEdit* lineedit = static_cast<finalcut::FLineEdit*>(widget);
  finalcut::FString title;
  *lineedit >> title;
  setTermTitle (title);
  setText (title);
  redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_ProgressBar (finalcut::FWidget*, data_ptr)
{
  ProgressDialog* p_dgl = new ProgressDialog(this);
  p_dgl->show();
}

//----------------------------------------------------------------------
void MyDialog::cb_updateNumber (finalcut::FWidget* widget, data_ptr data)
{
  finalcut::FListBox* list = static_cast<finalcut::FListBox*>(widget);
  finalcut::FLabel* num = static_cast<finalcut::FLabel*>(data);
  int select_num = 0;
  std::size_t count = list->getCount();

  for (std::size_t n = 1; n <= count; n++)
    if ( list->isSelected(n) )
      select_num++;

  num->clear();
  *num << select_num;
  num->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_activateButton (finalcut::FWidget* widget, data_ptr data)
{
  finalcut::FRadioButton* rb = static_cast<finalcut::FRadioButton*>(widget);
  finalcut::FButton* button = static_cast<finalcut::FButton*>(data);

  if ( rb->isChecked() )
    button->setEnable();
  else
    button->setDisable();

  button->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_view (finalcut::FWidget*, data_ptr data)
{
  finalcut::FString file;
  finalcut::FMenuItem* item = static_cast<finalcut::FMenuItem*>(data);

  if ( item && ! item->getText().isEmpty() )
    file = item->getText();
  else
    file = finalcut::FFileDialog::fileOpenChooser (this);

  if ( file.isNull() )
    return;

  TextWindow* view = new TextWindow(this);
  finalcut::FString filename(basename(const_cast<char*>(file.c_str())));
  view->setText ("Viewer: " + filename);
  view->setGeometry ( 1 + int((getRootWidget()->getWidth() - 60) / 2),
                      int(getRootWidget()->getHeight() / 6),
                      60,
                      getRootWidget()->getHeight() * 3 / 4 );
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
void MyDialog::cb_setInput (finalcut::FWidget* widget, data_ptr data)
{
  finalcut::FListBox* ListBox = static_cast<finalcut::FListBox*>(widget);
  finalcut::FLineEdit* lineedit = static_cast<finalcut::FLineEdit*>(data);
  *lineedit = ListBox->getItem(ListBox->currentItem()).getText();
  lineedit->redraw();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  finalcut::FString ver = F_VERSION;  // Library version
  finalcut::FString title = "The FINAL CUT "
                          + ver
                          + " (C) 2018 by Markus Gans";

  // Create the application object app
  finalcut::FApplication app(argc, argv);
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
  d.setGeometry (int((app.getWidth() - 56) / 2), 2, 56, app.getHeight() - 4);
  d.setShadow();

  // Set the dialog object d as the main widget of the application.
  // When you close the main widget, the application will be closed.
  app.setMainWidget(&d);

  // Show the dialog d
  d.show();

  // Start the application
  // and return the result to the operating system
  return app.exec();
}
