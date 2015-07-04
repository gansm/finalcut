// ui.cpp

#include <iostream>
#include <fstream>

#include "final.h"

//----------------------------------------------------------------------
// class ProgressDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class ProgressDialog : public FDialog
{
 public:
   FButton* reset;
   FButton* more;
   FButton* quit;
   FProgressbar* progressBar;

 public:
   explicit ProgressDialog (FWidget* parent=0);  // constructor
  ~ProgressDialog();  // destructor

   void onShow (FShowEvent*);
   void onTimer (FTimerEvent*);
   void cb_reset_bar (FWidget*, void*);
   void cb_more_bar (FWidget*, void*);
   void cb_exit_bar (FWidget*, void*);
};
#pragma pack(pop)

//----------------------------------------------------------------------
ProgressDialog::ProgressDialog (FWidget* parent) : FDialog(parent)
{
  setGeometry (int((this->parentWidget()->getWidth()-40)/2), 7, 40, 10);
  setText("Progress bar");
  setTransparentShadow();
  //setModal();

  reset = new FButton(this);
  reset->setText("&Reset");
  reset->setGeometry(2, 6, 8, 1, false);
  reset->setShadow();
  reset->setDisable();

  more = new FButton(this);
  more->setText("&More");
  more->setGeometry(15, 6, 8, 1, false);
  more->setShadow();
  more->setDisable();

  quit = new FButton(this);
  quit->setText("E&xit");
  quit->setGeometry(28, 6, 8, 1, false);
  quit->setShadow();
  quit->setDisable();

  progressBar = new FProgressbar(this);
  progressBar->setGeometry(2, 3, 34, 1, false);
  //progressBar->setPercentage(78);
  progressBar->setShadow();

  reset->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&ProgressDialog::cb_reset_bar),
    null
  );

  more->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&ProgressDialog::cb_more_bar),
    null
  );

  quit->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&ProgressDialog::cb_exit_bar),
    null
  );
}

//----------------------------------------------------------------------
ProgressDialog::~ProgressDialog()
{
  delAllTimer();
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

  if ( p == 100 )
  {
    delAllTimer();
    activateWindow();
    raiseWindow();
    reset->setEnable();
    reset->setFocus();
    more->setEnable();
    quit->setEnable();
    redraw();
  }
}

//----------------------------------------------------------------------
void ProgressDialog::cb_reset_bar (FWidget*, void*)
{
  progressBar->reset();
}

//----------------------------------------------------------------------
void ProgressDialog::cb_more_bar (FWidget*, void*)
{
  int p = progressBar->getPercentage();
  progressBar->setPercentage(++p);
}

//----------------------------------------------------------------------
void ProgressDialog::cb_exit_bar (FWidget*, void*)
{
  this->close();
}


//----------------------------------------------------------------------
// class MyDialog
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class MyDialog : public FDialog
{
 private:
   FListBox* myList;
 public:
   explicit MyDialog (FWidget* parent=0);  // constructor
  ~MyDialog();  // destructor
   void onClose (FCloseEvent*);
   void cb_noFunctionMsg (FWidget*, void*);
   void cb_about (FWidget*, void*);
   void cb_info (FWidget*, void*);
   void cb_input2buttonText (FWidget*, void*);
   void cb_setTitlebar (FWidget*, void*);
   void cb_ProgressBar (FWidget*, void*);
   void cb_updateNumber (FWidget*, void*);
   void cb_activateButton (FWidget*, void*);
   void cb_view (FWidget*, void*);
   void cb_setInput (FWidget*, void*);
   void cb_exitApp (FWidget*, void*);
 protected:
   void adjustSize();
};
#pragma pack(pop)

//----------------------------------------------------------------------
MyDialog::MyDialog (FWidget* parent) : FDialog(parent)
{
  FButton* MyButton1 = new FButton(this);
  MyButton1->setGeometry(3, 3, 5, 1);
  MyButton1->setText(L"&SIN");
  MyButton1->setStatusbarMessage("Sine function");
  MyButton1->setNoUnderline();
  MyButton1->setFlat();
  MyButton1->setDoubleFlatLine(fc::bottom);

  FButton* MyButton2 = new FButton(this);
  MyButton2->setGeometry(3, 5, 5, 1);
  MyButton2->setText(L"&COS");
  MyButton2->setStatusbarMessage("Cosine function");
  MyButton2->setNoUnderline();
  MyButton2->setFlat();
  MyButton2->setDoubleFlatLine(fc::top);

  FButton* MyButton3 = new FButton(this);
  MyButton3->setGeometry(10, 3, 5, 3);
  MyButton3->setText(L"&=");
  MyButton3->setStatusbarMessage("Equal");
  MyButton3->setNoUnderline();
  MyButton3->setFlat();

  FButtonGroup* radioButtonGroup = new FButtonGroup("Button", this);
  radioButtonGroup->setGeometry(3, 8, 14, 4);
  //radioButtonGroup->unsetBorder();

  FRadioButton* radio1 = new FRadioButton("&Enable", radioButtonGroup);
  radio1->setGeometry(1, 1, 7, 1);
  radio1->setStatusbarMessage("Enable button Test");

  FRadioButton* radio2 = new FRadioButton(radioButtonGroup);
  radio2->setGeometry(1, 2, 7, 1);
  radio2->setText("&Disable");
  radio2->setStatusbarMessage("Disable button Test");
  radio2->setChecked();
  //radio2->setDisable();

  FButtonGroup* checkButtonGroup = new FButtonGroup("Options", this);
  checkButtonGroup->setGeometry(3, 12, 14, 4);

  FCheckBox* check1 = new FCheckBox("&Fastmode", checkButtonGroup);
  check1->setGeometry(1, 1, 7, 1);
  check1->setNoUnderline();

  FCheckBox* check2 = new FCheckBox("&8-Bit", checkButtonGroup);
  check2->setGeometry(1, 2, 7, 1);
  check2->setChecked();
  check2->setNoUnderline();

  FLineEdit* MyLineEdit = new FLineEdit(this);
  MyLineEdit->setGeometry(21, 1, 10, 1);
  MyLineEdit->setText( FString("EnTry").toLower());
  MyLineEdit->setLabelText(L"&Input:");
  MyLineEdit->setStatusbarMessage("Press Enter to set the title");
  MyLineEdit->setShadow();

  FButton* MyButton4 = new FButton(this);
  MyButton4->setGeometry(19, 8, 12, 1);
  MyButton4->setText(L"&Test");
  MyButton4->setStatusbarMessage("Progressbar testing dialog");
  MyButton4->setShadow();
  MyButton4->setDisable();

  FButton* MyButton5 = new FButton(this);
  MyButton5->setGeometry(19, 11, 12, 1);
  MyButton5->setText(L"Environs");
  MyButton5->setStatusbarMessage("Show environment settings");
  MyButton5->setShadow();

  FButton* MyButton6 = new FButton(this);
  MyButton6->setGeometry(19, 14, 12, 1);
  MyButton6->setText(L"&Get input");
  MyButton6->setStatusbarMessage("Take text from input field");
  MyButton6->setShadow();
  MyButton6->setFocus();

  FButton* MyButton7 = new FButton(this);
  MyButton7->setGeometry(19, 17, 12, 1);
  MyButton7->setText(L"&Quit");
  MyButton7->setStatusbarMessage("Exit the program");
  MyButton7->setShadow();
  MyButton7->addAccelerator('x');

  myList = new FListBox(this);
  myList->setGeometry(38, 1, 14, 17);
  myList->setText("Items");
  myList->setStatusbarMessage("99 items in a list");
  myList->setMultiSelection();
  for (int z=1; z < 100; z++)
    myList->insert( FString().setNumber(z) + L" placeholder" );

  FLabel* headline = new FLabel(this);
  headline->setGeometry(20, 3, 10, 1);
  headline->setText(L"List items");
  headline->setEmphasis();
  headline->setAlignment(fc::alignCenter);

  FLabel* tagged = new FLabel(L"Tagged:", this);
  tagged->setGeometry(20, 4, 7, 1);

  FLabel* tagged_count = new FLabel(this);
  tagged_count->setGeometry(28, 4, 5, 1);
  tagged_count->setNumber(0);

  FLabel* sum = new FLabel(L"Sum:", this);
  sum->setGeometry(20, 5, 7, 1);
  sum->setAlignment(fc::alignRight);

  FLabel* sum_count = new FLabel(this);
  sum_count->setGeometry(28, 5, 5, 1);
  sum_count->setNumber(myList->count());

  FStatusBar* statusbar = new FStatusBar(this);
  FStatusKey* key_F1 = new FStatusKey(fc::Fkey_f1, "About", statusbar);
  FStatusKey* key_F2 = new FStatusKey(fc::Fkey_f2, "View", statusbar);
  FStatusKey* key_F3 = new FStatusKey(fc::Fkey_f3, "Quit", statusbar);

  MyButton1->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_noFunctionMsg),
    null
  );

  MyButton2->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_noFunctionMsg),
    null
  );

  MyButton3->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_noFunctionMsg),
    null
  );

  MyButton4->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_ProgressBar),
    null
  );

  MyButton5->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_info),
    null
  );

  MyButton6->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_input2buttonText),
    dynamic_cast<FWidget::data_ptr>(MyLineEdit)
  );

  MyButton7->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_exitApp),
    null
  );

  MyLineEdit->addCallback
  (
    "activate",  // e.g. on <Enter>
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_setTitlebar),
    null
  );

  radio1->addCallback
  (
    "toggled",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_activateButton),
    dynamic_cast<FWidget::data_ptr>(MyButton4)
  );

  myList->addCallback
  (
    "clicked",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_setInput),
    dynamic_cast<FWidget::data_ptr>(MyLineEdit)
  );

  myList->addCallback
  (
    "row-selected",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_updateNumber),
    dynamic_cast<FWidget::data_ptr>(tagged_count)
  );

  key_F1->addCallback
  (
    "activate",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_about),
    null
  );

  key_F2->addCallback
  (
    "activate",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_view),
    null
  );

  key_F3->addCallback
  (
    "activate",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&MyDialog::cb_exitApp),
    key_F3
  );
}

//----------------------------------------------------------------------
MyDialog::~MyDialog()
{
}

//----------------------------------------------------------------------
void MyDialog::onClose (FCloseEvent* event)
{
  int ret = FMessageBox::info ( this, "Quit",
                                "Do you really want\n"
                                "to quit the program ?",
                                FMessageBox::Yes,
                                FMessageBox::No );
  if ( ret == FMessageBox::Yes )
    event->accept();
  else
    event->ignore();
}

//----------------------------------------------------------------------
void MyDialog::cb_noFunctionMsg (FWidget* widget, void*)
{
  FButton* button = static_cast<FButton*>(widget);
  FString text = button->getText();
  text = text.replace('&', "");
  FMessageBox::error (this, "The \"" + text + "\" button has\n"
                            "no function");
}

//----------------------------------------------------------------------
void MyDialog::cb_about (FWidget* widget, void*)
{
  FStatusKey* skey = static_cast<FStatusKey*>(widget);
  FString line(2, wchar_t(fc::BoxDrawingsHorizontal));

  FMessageBox info ( "About", line + L" The Final Cut " + line + "\n\n"
                     L"Version 0.1.1\n\n"
                     L"(c) 2015 by Markus Gans",
                     FMessageBox::Ok, 0, 0, this );
  info.setCenterText();
  info.show();
  skey->unsetActive();
}

//----------------------------------------------------------------------
void MyDialog::cb_info (FWidget*, void*)
{
  {
    FMessageBox info1 ( "Environment",
                        "  Type: " + FString(getTermType()) + "\n"
                        "  Name: " + FString(getTermName()) + "\n"
                        "  Mode: " + FString(getEncoding()),
                        FMessageBox::Ok, 0, 0, this );
    info1.setHeadline("Terminal:");
    info1.exec();
  } // end of scope => delete info1

  FString line(15, wchar_t(fc::BoxDrawingsHorizontal));
  FMessageBox info2 ( "Drive symbols",
                      "Generic:       \n\n"
                      "Network:       \n\n"
                      "     CD:",
                      FMessageBox::Ok, 0, 0, this );
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
void MyDialog::cb_input2buttonText (FWidget* widget, void* data_ptr)
{
  FButton* button = static_cast<FButton*>(widget);
  FLineEdit* lineedit = static_cast<FLineEdit*>(data_ptr);
  button->setText( lineedit->getText() );
  button->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_setTitlebar (FWidget* widget, void*)
{
  FLineEdit* lineedit = static_cast<FLineEdit*>(widget);
  lineedit->setXTermTitle(lineedit->getText());
  this->setText(lineedit->getText());
  this->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_ProgressBar (FWidget*, void*)
{
  ProgressDialog* p_dgl = new ProgressDialog(this);
  p_dgl->show();
}

//----------------------------------------------------------------------
void MyDialog::cb_updateNumber (FWidget* widget, void* data_ptr)
{
  FListBox* list = static_cast<FListBox*>(widget);
  FLabel* num = static_cast<FLabel*>(data_ptr);
  int select_num = 0;
  uInt end = list->count();
  for (uInt n=1; n <= end; n++)
    if ( list->isSelected(int(n)) )
      select_num++;
  num->setNumber(select_num);
  num->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_activateButton (FWidget* widget, void* data_ptr)
{
  FRadioButton* rb = static_cast<FRadioButton*>(widget);
  FButton* button = static_cast<FButton*>(data_ptr);
  if ( rb->isChecked() )
    button->setEnable();
  else
    button->setDisable();
  button->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_view (FWidget* widget, void*)
{
  FStatusKey* skey = static_cast<FStatusKey*>(widget);
  FString file = FFileDialog::fileOpenChooser (this);
  if ( file.isNull() )
  {
    skey->unsetActive();
    return;
  }

  FDialog* view = new FDialog(this);
  view->setText ("viewer");
  view->setGeometry (1+int((getRootWidget()->getWidth()-60)/2),
                     int(getRootWidget()->getHeight()/6),
                     60,
                     int(getRootWidget()->getHeight()*3/4));
  view->setTransparentShadow();

  FTextView* scrollText = new FTextView(view);
  scrollText->ignorePadding();
  scrollText->setGeometry (1, 2, view->getWidth(), view->getHeight()-1);
  scrollText->setFocus();
  scrollText->insert(" -----------------------------------------------\n"
                     " line 1\n"
                     " -----------------------------------------------\n"
                     " line 3\n"
                     " line 4"
                     , -1);
  scrollText->replaceRange("                   File viewer", 1, 1);
  scrollText->deleteRange(3, 4);
  std::string line = "";
  std::ifstream infile;
  infile.open(file);
  while ( ! infile.eof() && infile.good() )
  {
    getline(infile, line);
    scrollText->append(line);
  }
  if ( infile.is_open() )
    infile.close();
  view->show();
  skey->unsetActive();
}

//----------------------------------------------------------------------
void MyDialog::cb_setInput (FWidget* widget, void* data_ptr)
{
  FListBox* ListBox = static_cast<FListBox*>(widget);
  FLineEdit* lineedit = static_cast<FLineEdit*>(data_ptr);
  lineedit->setText( ListBox->Item(ListBox->currentItem()).getText() );
  lineedit->redraw();
}

//----------------------------------------------------------------------
void MyDialog::cb_exitApp (FWidget*, void* data_ptr)
{
  FStatusKey* skey = static_cast<FStatusKey*>(data_ptr);
  close();
  if ( skey )
    skey->unsetActive();
}

//----------------------------------------------------------------------
void MyDialog::adjustSize()
{
  int h = parentWidget()->getHeight() - 3;
  setHeight (h, false);
  int X = int((parentWidget()->getWidth() - getWidth()) / 2);
  if ( X < 1 )
    X = 1;
  setX (X, false);
  myList->setHeight (getHeight() - 3, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
//                               main part
//----------------------------------------------------------------------

int main (int argc, char* argv[])
{
  if ( argv[1] && (  strcmp(argv[1], "--help") == 0
                  || strcmp(argv[1], "-h") == 0 ) )
  {
    std::cout << "Generic options:" << std::endl
              << "  -h, --help                  "
              << "Display this help and exit" << std::endl;
    FApplication::print_cmd_Options();
    exit(EXIT_SUCCESS);
  }

  FApplication app(argc, argv);
  app.setXTermTitle ("The FINAL CUT 0.1.1 (C) 2015 by Markus Gans");

  //app.setEncoding("VT100");
  //app.setTermGeometry(94,30);
  //app.setNewFont();

  MyDialog d(&app);
  d.setText ("The FINAL CUT 0.1.1 (C) 2015 by Markus Gans");
  d.setGeometry (int((app.getWidth()-56)/2), 2, 56, app.getHeight()-3);
  d.setShadow();

  app.setMainWidget(&d);
  d.show();
  return app.exec();
}
