/***********************************************************************
* ui.cpp - Example of a user interface                                 *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2017 Markus Gans                                      *
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

class ProgressDialog : public FDialog
{
  public:
    // Constructor
    explicit ProgressDialog (FWidget* = 0);
    // Destructor
    ~ProgressDialog();

  private:
    // Disable copy constructor
    ProgressDialog (const ProgressDialog&);
    // Disable assignment operator (=)
    ProgressDialog& operator = (const ProgressDialog&);

    // Event handlers
    void onShow (FShowEvent*);
    void onTimer (FTimerEvent*);

    // Callback methods
    void cb_reset_bar (FWidget*, data_ptr);
    void cb_more_bar (FWidget*, data_ptr);
    void cb_exit_bar (FWidget*, data_ptr);

    // Data Members
    FProgressbar* progressBar;
    FButton*      reset;
    FButton*      more;
    FButton*      quit;
};
#pragma pack(pop)

//----------------------------------------------------------------------
ProgressDialog::ProgressDialog (FWidget* parent)
  : FDialog(parent)
  , progressBar()
  , reset()
  , more()
  , quit()
{
  setGeometry (int((getParentWidget()->getWidth() - 40) / 2), 7, 40, 10);
  setText("Progress bar");
  //setModal();

  reset = new FButton(this);
  reset->setText("&Reset");
  reset->setStatusbarMessage ("Reset the progress bar");
  reset->setGeometry(2, 6, 8, 1, false);
  reset->setDisable();

  more = new FButton(this);
  more->setText("&More");
  more->setStatusbarMessage ("Increases the progress bar position");
  more->setGeometry(15, 6, 8, 1, false);
  more->setDisable();

  quit = new FButton(this);
  quit->setText("E&xit");
  quit->setGeometry(28, 6, 8, 1, false);
  quit->setDisable();

  progressBar = new FProgressbar(this);
  progressBar->setGeometry(2, 3, 34, 1, false);
  //progressBar->setPercentage(78);

  reset->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &ProgressDialog::cb_reset_bar)
  );

  more->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &ProgressDialog::cb_more_bar)
  );

  quit->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &ProgressDialog::cb_exit_bar)
  );
}

//----------------------------------------------------------------------
ProgressDialog::~ProgressDialog()  // destructor
{
  delOwnTimer();
  delCallback(quit);
  delCallback(more);
  delCallback(reset);
  delete(progressBar);
  delete(quit);
  delete(more);
  delete(reset);
}

//----------------------------------------------------------------------
void ProgressDialog::onShow (FShowEvent*)
{
  addTimer(15);
}

//----------------------------------------------------------------------
void ProgressDialog::onTimer (FTimerEvent*)
{
  int p = progressBar->getPercentage();
  progressBar->setPercentage(++p);
  flush_out();

  if ( p != 100 )
    return;

  delOwnTimer();
  activateWindow();
  raiseWindow();
  reset->setEnable();
  reset->setFocus();
  more->setEnable();
  quit->setEnable();
  redraw();

  if ( getStatusBar() )
    getStatusBar()->drawMessage();

  updateTerminal();
  flush_out();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_reset_bar (FWidget*, data_ptr)
{
  progressBar->reset();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_more_bar (FWidget*, data_ptr)
{
  int p = progressBar->getPercentage();
  progressBar->setPercentage(++p);
}

//----------------------------------------------------------------------
void ProgressDialog::cb_exit_bar (FWidget*, data_ptr)
{
  close();
}


//----------------------------------------------------------------------
// class TextWindow
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class TextWindow : public FDialog
{
  public:
    // Constructor
    explicit TextWindow (FWidget* = 0);
    // Destructor
    ~TextWindow();

    void append (const FString&);

  private:
    // Disable copy constructor
    TextWindow (const TextWindow&);
    // Disable assignment operator (=)
    TextWindow& operator = (const TextWindow&);

    void adjustSize();

    // Data Members
    FTextView* scrollText;
};
#pragma pack(pop)

//----------------------------------------------------------------------
TextWindow::TextWindow (FWidget* parent)
  : FDialog(parent)
  , scrollText()
{
  scrollText = new FTextView(this);
  scrollText->ignorePadding();
  scrollText->setGeometry (1, 2, getWidth(), getHeight() - 1);
  setMinimumSize (51, 6);
  scrollText->setFocus();
  scrollText->insert(" -----------------------------------------------\n"
                     " line 1\n"
                     " -----------------------------------------------\n"
                     " line 3\n"
                     " line 4"
                     , -1);
  scrollText->replaceRange("                   File viewer", 1, 1);
  scrollText->deleteRange(3, 4);
}

//----------------------------------------------------------------------
TextWindow::~TextWindow()  // destructor
{ }

//----------------------------------------------------------------------
void TextWindow::append (const FString& str)
{
  scrollText->append(str);
}

//----------------------------------------------------------------------
void TextWindow::adjustSize()
{
  FDialog::adjustSize();
  scrollText->setGeometry (1, 2, getWidth(), getHeight() - 1);
}


//----------------------------------------------------------------------
// class MyDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class MyDialog : public FDialog
{
  public:
    // Constructor
    explicit MyDialog (FWidget* = 0);
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
    void initStatusBar();
    void initStatusBarCallbacks();
    void initWidgets();
    void initWidgetsCallbacks();
    void adjustSize();

    // Event handlers
    void onClose (FCloseEvent*);

    // Callback methods
    void cb_noFunctionMsg (FWidget*, data_ptr);
    void cb_about (FWidget*, data_ptr);
    void cb_terminfo (FWidget*, data_ptr);
    void cb_drives (FWidget*, data_ptr);
    void cb_cutClipboard (FWidget*, data_ptr);
    void cb_copyClipboard (FWidget*, data_ptr);
    void cb_pasteClipboard (FWidget*, data_ptr);
    void cb_clearInput (FWidget*, data_ptr);
    void cb_input2buttonText (FWidget*, data_ptr);
    void cb_setTitlebar (FWidget*, data_ptr);
    void cb_ProgressBar (FWidget*, data_ptr);
    void cb_updateNumber (FWidget*, data_ptr);
    void cb_activateButton (FWidget*, data_ptr);
    void cb_view (FWidget*, data_ptr);
    void cb_setInput (FWidget*, data_ptr);

    // Data Members
    FMenuItem*    Open;
    FMenuItem*    Quit;
    FMenuItem*    File1;
    FMenuItem*    File2;
    FMenuItem*    File3;
    FMenuItem*    Cut;
    FMenuItem*    Copy;
    FMenuItem*    Paste;
    FMenuItem*    Clear;
    FMenuItem*    Env;
    FMenuItem*    Drive;
    FMenuItem*    Help;
    FStatusKey*   key_F1;
    FStatusKey*   key_F2;
    FStatusKey*   key_F3;
    FButton*      MyButton1;
    FButton*      MyButton2;
    FButton*      MyButton3;
    FButton*      MyButton4;
    FButton*      MyButton5;
    FButton*      MyButton6;
    FRadioButton* radio1;
    FLabel*       tagged_count;
    FLineEdit*    myLineEdit;
    FListBox*     myList;
    FString       clipboard;
};
#pragma pack(pop)

//----------------------------------------------------------------------
MyDialog::MyDialog (FWidget* parent)
  : FDialog(parent)
  , myLineEdit()
  , myList()
  , clipboard()
{
  initMenu();                // Initialize the program menu
  initMenuCallbacks();       // Initialize program menu callbacks
  initStatusBar();           // Initialize the status bar
  initStatusBarCallbacks();  // Initialize status bar callbacks
  initWidgets();             // Initialize the dialog widgets
  initWidgetsCallbacks();    // Initialize dialog widget callbacks
}

//----------------------------------------------------------------------
MyDialog::~MyDialog()  // destructor
{ }

//----------------------------------------------------------------------
void MyDialog::initMenu()
{
  // Menu bar
  FMenuBar* Menubar  = new FMenuBar (this);

  // Menu bar items
  FMenu* File        = new FMenu ("&File", Menubar);
  File->setStatusbarMessage ("File management commands");
  FMenu* Edit        = new FMenu ("&Edit", Menubar);
  Edit->setStatusbarMessage ("Cut-and-paste editing commands");
  FMenu* View        = new FMenu ("&View", Menubar);
  View->setStatusbarMessage ("Show internal informations");
  FMenuItem* Options = new FMenuItem ("&Options", Menubar);
  Options->setStatusbarMessage ("Set program defaults");
  Options->setDisable();
  FDialogListMenu* Window = new FDialogListMenu ("&Window", Menubar);
  Window->setStatusbarMessage ("List of all the active dialogs");
  Help = new FMenuItem ("&Help", Menubar);
  Help->setStatusbarMessage ("Show version and copyright information");

  // "File" menu items
  Open = new FMenuItem ("&Open...", File);
  Open->addAccelerator (fc::Fckey_o);  // Ctrl + O
  Open->setStatusbarMessage ("Locate and open a text file");
  FMenu* Recent   = new FMenu ("&System files", File);
  Recent->setStatusbarMessage ("View text file");

  FMenuItem* Line1   = new FMenuItem (File);
  Line1->setSeparator();
  Quit = new FMenuItem ("&Quit", File);
  Quit->addAccelerator (fc::Fmkey_x);  // Meta/Alt + X
  Quit->setStatusbarMessage ("Exit the program");

  // "Recent" menu items
  File1 = new FMenuItem ("/etc/services", Recent);
  File2 = new FMenuItem ("/etc/fstab", Recent);
  File3 = new FMenuItem ("/etc/passwd", Recent);

  // "Edit" menu items
  FMenuItem* Undo    = new FMenuItem (fc::Fckey_z, "Undo", Edit);
  Undo->setDisable();
  FMenuItem* Redo    = new FMenuItem (fc::Fckey_y, "Redo", Edit);
  Redo->setDisable();
  FMenuItem* Line2   = new FMenuItem (Edit);
  Line2->setSeparator();
  Cut = new FMenuItem (fc::Fckey_x, "Cu&t", Edit);
  Cut->setStatusbarMessage ( "Remove the input text"
                             " and put it in the clipboard" );
  Copy= new FMenuItem (fc::Fckey_c, "&Copy", Edit);
  Copy->setStatusbarMessage ("Copy the input text into the clipboad");
  Paste = new FMenuItem (fc::Fckey_v, "&Paste", Edit);
  Paste->setStatusbarMessage ("Insert text form clipboard");
  Clear = new FMenuItem (fc::Fkey_dc, "C&lear", Edit);
  Clear->setStatusbarMessage ("Delete input text");

  // "View" menu items
  Env = new FMenuItem ("&Terminal...", View);
  Env->setStatusbarMessage ("Informations about this terminal");
  Drive = new FMenuItem ("&Drive symbols...", View);
  Drive->setStatusbarMessage ("Show drive symbols");
}

//----------------------------------------------------------------------
void MyDialog::initMenuCallbacks()
{
  // Menu function callbacks

  Open->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view)
  );

  Quit->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FApplication::cb_exitApp)
  );

  Cut->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_cutClipboard)
  );

  Copy->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_copyClipboard)
  );

  Paste->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_pasteClipboard)
  );

  Clear->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_clearInput)
  );

  Env->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_terminfo)
  );

  Drive->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_drives)
  );

  Help->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_about)
  );

  File1->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view),
    static_cast<FWidget::data_ptr>(File1)
  );

  File2->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view),
    static_cast<FWidget::data_ptr>(File2)
  );

  File3->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view),
    static_cast<FWidget::data_ptr>(File3)
  );
}

//----------------------------------------------------------------------
void MyDialog::initStatusBar()
{
  // Statusbar at the bottom
  FStatusBar* statusbar = new FStatusBar (this);

  // Statusbar keys
  key_F1 = new FStatusKey (fc::Fkey_f1, "About", statusbar);
  key_F2 = new FStatusKey (fc::Fkey_f2, "View", statusbar);
  key_F3 = new FStatusKey (fc::Fkey_f3, "Quit", statusbar);
}

//----------------------------------------------------------------------
void MyDialog::initStatusBarCallbacks()
{
  // Add some function callbacks

  key_F1->addCallback
  (
    "activate",
    F_METHOD_CALLBACK (this, &MyDialog::cb_about)
  );

  key_F2->addCallback
  (
    "activate",
    F_METHOD_CALLBACK (this, &MyDialog::cb_view)
  );

  key_F3->addCallback
  (
    "activate",
    F_METHOD_CALLBACK (this, &FApplication::cb_exitApp)
  );
}

//----------------------------------------------------------------------
void MyDialog::initWidgets()
{
  // Buttons
  MyButton1 = new FButton (this);
  MyButton1->setGeometry(3, 3, 5, 1);
  MyButton1->setText (L"&SIN");
  MyButton1->setStatusbarMessage ("Sine function");
  MyButton1->setNoUnderline();
  MyButton1->setFlat();
  MyButton1->setDoubleFlatLine (fc::bottom);

  MyButton2 = new FButton (this);
  MyButton2->setGeometry(3, 5, 5, 1);
  MyButton2->setText (L"&COS");
  MyButton2->setStatusbarMessage ("Cosine function");
  MyButton2->setNoUnderline();
  MyButton2->setFlat();
  MyButton2->setDoubleFlatLine (fc::top);

  MyButton3 = new FButton (this);
  MyButton3->setGeometry(10, 3, 5, 3);
  MyButton3->setText (L"&=");
  MyButton3->setStatusbarMessage ("Equal");
  MyButton3->setNoUnderline();
  MyButton3->setFlat();

  // Radio buttons in a group
  FButtonGroup* radioButtonGroup = new FButtonGroup ("Button", this);
  radioButtonGroup->setGeometry(3, 8, 14, 4);
  //radioButtonGroup->unsetBorder();

  radio1 = new FRadioButton ("E&nable", radioButtonGroup);
  radio1->setGeometry(1, 1, 10, 1);
  radio1->setStatusbarMessage ("Enable button Test");

  FRadioButton* radio2 = new FRadioButton (radioButtonGroup);
  radio2->setGeometry(1, 2, 11, 1);
  radio2->setText ("&Disable");
  radio2->setStatusbarMessage ("Disable button Test");
  radio2->setChecked();
  //radio2->setDisable();

  // Checkboxes in a group
  FButtonGroup* checkButtonGroup = new FButtonGroup ("Options", this);
  checkButtonGroup->setGeometry(3, 12, 14, 4);

  FCheckBox* check1 = new FCheckBox ("&Bitmode", checkButtonGroup);
  check1->setGeometry(1, 1, 11, 1);
  check1->setNoUnderline();

  FCheckBox* check2 = new FCheckBox ("&8-Bit", checkButtonGroup);
  check2->setGeometry(1, 2, 9, 1);
  check2->setChecked();
  check2->setNoUnderline();

  // A text input field
  myLineEdit = new FLineEdit (this);
  myLineEdit->setGeometry(22, 1, 10, 1);
  myLineEdit->setLabelText (L"&Input");
  myLineEdit->setStatusbarMessage ("Press Enter to set the title");
  *myLineEdit << FString("EnTry").toLower();

  // Buttons
  MyButton4 = new FButton (this);
  MyButton4->setGeometry(20, 8, 12, 1);
  MyButton4->setText (L"&Get input");
  MyButton4->setStatusbarMessage ("Take text from input field");
  MyButton4->setFocus();

  MyButton5 = new FButton (this);
  MyButton5->setGeometry(20, 11, 12, 1);
  MyButton5->setText (L"&Test");
  MyButton5->setStatusbarMessage ("Progressbar testing dialog");
  MyButton5->setDisable();

  MyButton6 = new FButton (this);
  MyButton6->setGeometry(20, 14, 12, 1);
  MyButton6->setText (L"&Quit");
  MyButton6->setStatusbarMessage ("Exit the program");
  MyButton6->addAccelerator('x');

  // A multiple selection listbox
  myList = new FListBox (this);
  myList->setGeometry(38, 1, 14, 17);
  myList->setText ("Items");
  myList->setStatusbarMessage ("99 items in a list");
  myList->setMultiSelection();

  for (int z = 1; z < 100; z++)
    myList->insert (FString() << z << L" placeholder");

  // Text labels
  FLabel* headline = new FLabel (this);
  headline->setGeometry(21, 3, 10, 1);
  headline->setEmphasis();
  headline->setAlignment (fc::alignCenter);
  *headline = L"List items";

  FLabel* tagged = new FLabel (L"Tagged:", this);
  tagged->setGeometry(21, 4, 7, 1);

  tagged_count = new FLabel(this);
  tagged_count->setGeometry(29, 4, 5, 1);
  *tagged_count << 0;

  FLabel* sum = new FLabel (L"Sum:", this);
  sum->setGeometry(21, 5, 7, 3);
  sum->setAlignment (fc::alignRight);

  FLabel* sum_count = new FLabel (this);
  sum_count->setGeometry(29, 5, 5, 3);
  *sum_count << myList->getCount();
}

//----------------------------------------------------------------------
void MyDialog::initWidgetsCallbacks()
{
  // Add some function callbacks

  MyButton1->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_noFunctionMsg)
  );

  MyButton2->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_noFunctionMsg)
  );

  MyButton3->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_noFunctionMsg)
  );

  MyButton4->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_input2buttonText),
    static_cast<FWidget::data_ptr>(myLineEdit)
  );

  MyButton5->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_ProgressBar)
  );

  MyButton6->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &FApplication::cb_exitApp)
  );

  myLineEdit->addCallback
  (
    "activate",  // e.g. on <Enter>
    F_METHOD_CALLBACK (this, &MyDialog::cb_setTitlebar)
  );

  radio1->addCallback
  (
    "toggled",
    F_METHOD_CALLBACK (this, &MyDialog::cb_activateButton),
    static_cast<FWidget::data_ptr>(MyButton5)
  );

  myList->addCallback
  (
    "clicked",
    F_METHOD_CALLBACK (this, &MyDialog::cb_setInput),
    static_cast<FWidget::data_ptr>(myLineEdit)
  );

  myList->addCallback
  (
    "row-selected",
    F_METHOD_CALLBACK (this, &MyDialog::cb_updateNumber),
    static_cast<FWidget::data_ptr>(tagged_count)
  );
}

//----------------------------------------------------------------------
void MyDialog::adjustSize()
{
  int h = getParentWidget()->getHeight() - 4;
  setHeight (h, false);
  int X = int((getParentWidget()->getWidth() - getWidth()) / 2);

  if ( X < 1 )
    X = 1;

  setX (X, false);

  if ( myList )
    myList->setHeight (getHeight() - 3, false);

  FDialog::adjustSize();
}

//----------------------------------------------------------------------
void MyDialog::onClose (FCloseEvent* ev)
{
  FApplication::closeConfirmationDialog (this, ev);
}

//----------------------------------------------------------------------
void MyDialog::cb_noFunctionMsg (FWidget* widget, data_ptr)
{
  FButton* button = static_cast<FButton*>(widget);
  FString text = button->getText();
  text = text.replace('&', "");
  FMessageBox::error (this, "The \"" + text + "\" button has\n"
                            "no function");
}

//----------------------------------------------------------------------
void MyDialog::cb_about (FWidget*, data_ptr)
{
  const char libver[] = F_VERSION;
  FString line(2, wchar_t(fc::BoxDrawingsHorizontal));

  FMessageBox info ( "About"
                   , line + L" The Final Cut " + line + "\n\n"
                     L"Version " + libver + "\n\n"
                     L"(c) 2017 by Markus Gans"
                   , FMessageBox::Ok, 0, 0, this );
  info.setCenterText();
  info.show();
}

//----------------------------------------------------------------------
void MyDialog::cb_terminfo (FWidget*, data_ptr)
{
  int x = getColumnNumber();
  int y = getLineNumber();
  FMessageBox info1 ( "Environment"
                    , FString()
                      << "  Type: " << getTermType() << "\n"
                      << "  Name: " << getTermName() << "\n"
                      << "  Mode: " << getEncoding() << "\n"
                      << "  Size: " << x << wchar_t(fc::Times)
                                    << y << "\n"
                      << "Colors: " << getMaxColor()
                    , FMessageBox::Ok, 0, 0, this );
  info1.setHeadline("Terminal:");
  info1.exec();
}

//----------------------------------------------------------------------
void MyDialog::cb_drives (FWidget*, data_ptr)
{
  FMessageBox info2 ( "Drive symbols"
                    , "Generic:       \n\n"
                      "Network:       \n\n"
                      "     CD:"
                    , FMessageBox::Ok, 0, 0, this );
  if ( isNewFont() )
  {
    FLabel drive (NF_Drive, &info2);
    drive.setGeometry (11, 2, 4, 1);
    FLabel net (NF_Net_Drive, &info2);
    net.setGeometry (11, 4, 4, 1);
    FLabel cd (NF_CD_ROM, &info2);
    cd.setGeometry (11, 6, 4, 1);
    info2.exec();
  }
  else
  {
    FLabel drive ("  - ", &info2);
    drive.setGeometry (11, 2, 4, 1);
    FLabel net ("  N ", &info2);
    net.setGeometry (11, 4, 4, 1);
    FLabel cd (" CD ", &info2);
    cd.setGeometry (11, 6, 4, 1);

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
void MyDialog::cb_cutClipboard (FWidget*, data_ptr)
{
  if ( ! myLineEdit )
    return;

  clipboard = myLineEdit->getText();
  myLineEdit->clear();
  myLineEdit->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_copyClipboard (FWidget*, data_ptr)
{
  if ( ! myLineEdit )
    return;

  clipboard = myLineEdit->getText();
}

//----------------------------------------------------------------------
void MyDialog::cb_pasteClipboard (FWidget*, data_ptr)
{
  if ( ! myLineEdit )
    return;

  *myLineEdit = clipboard;
  myLineEdit->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_clearInput (FWidget*, data_ptr)
{
  if ( ! myLineEdit )
    return;

  clipboard.clear();
  myLineEdit->clear();
  myLineEdit->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_input2buttonText (FWidget* widget, data_ptr data)
{
  FButton* button = static_cast<FButton*>(widget);
  FLineEdit* lineedit = static_cast<FLineEdit*>(data);
  button->setText( lineedit->getText() );
  button->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_setTitlebar (FWidget* widget, data_ptr)
{
  FLineEdit* lineedit = static_cast<FLineEdit*>(widget);
  FString title;
  *lineedit >> title;
  setXTermTitle(title);;
  setText(title);
  redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_ProgressBar (FWidget*, data_ptr)
{
  ProgressDialog* p_dgl = new ProgressDialog(this);
  p_dgl->show();
}

//----------------------------------------------------------------------
void MyDialog::cb_updateNumber (FWidget* widget, data_ptr data)
{
  FListBox* list = static_cast<FListBox*>(widget);
  FLabel* num = static_cast<FLabel*>(data);
  int select_num = 0;
  uInt count = list->getCount();

  for (uInt n = 1; n <= count; n++)
    if ( list->isSelected(int(n)) )
      select_num++;

  num->clear();
  *num << select_num;
  num->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_activateButton (FWidget* widget, data_ptr data)
{
  FRadioButton* rb = static_cast<FRadioButton*>(widget);
  FButton* button = static_cast<FButton*>(data);

  if ( rb->isChecked() )
    button->setEnable();
  else
    button->setDisable();

  button->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_view (FWidget*, data_ptr data)
{
  FString file;
  FMenuItem* item = static_cast<FMenuItem*>(data);

  if ( item && item->getText() )
    file = item->getText();
  else
    file = FFileDialog::fileOpenChooser (this);

  if ( file.isNull() )
    return;

  TextWindow* view = new TextWindow(this);
  FString filename(basename(const_cast<char*>(file.c_str())));
  view->setText ("Viewer: " + filename);
  view->setGeometry (1 + int((getRootWidget()->getWidth() - 60) / 2),
                     int(getRootWidget()->getHeight() / 6),
                     60,
                     int(getRootWidget()->getHeight() * 3 / 4));
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
void MyDialog::cb_setInput (FWidget* widget, data_ptr data)
{
  FListBox* ListBox = static_cast<FListBox*>(widget);
  FLineEdit* lineedit = static_cast<FLineEdit*>(data);
  *lineedit = ListBox->getItem(ListBox->currentItem()).getText();
  lineedit->redraw();
}


//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  FString ver = F_VERSION;  // Library version
  FString title = "The FINAL CUT " + ver + " (C) 2017 by Markus Gans";

  // Create the application object app
  FApplication app(argc, argv);
  app.setXTermDefaultColors(true);
  app.setXTermTitle (title);

  // Force vt100 encoding
  //app.setEncoding("VT100");

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
