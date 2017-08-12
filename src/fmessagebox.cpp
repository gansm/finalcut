// File: fmessagebox.cpp
// Provides: class FMessageBox

#include "fapp.h"
#include "fmessagebox.h"


static const char* const button_text[] =
{
  0,
  "&OK",
  "&Cancel",
  "&Yes",
  "&No",
  "&Abort",
  "&Retry",
  "&Ignore",
  0
};

//----------------------------------------------------------------------
// class FMessageBox
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FMessageBox::FMessageBox (FWidget* parent)
  : FDialog(parent)
  , headline_text()
  , text()
  , text_components(0)
  , text_split()
  , max_line_width(0)
  , center_text(false)
  , emphasis_color(wc.dialog_emphasis_fg)
  , num_buttons(0)
  , text_num_lines(0)
  , button_digit()
  , button()
{
  setTitlebarText("Message for you");
  init(FMessageBox::Ok, 0, 0);
}

//----------------------------------------------------------------------
FMessageBox::FMessageBox (const FMessageBox& mbox)
  : FDialog(mbox.getParentWidget())
  , headline_text(mbox.headline_text)
  , text(mbox.text)
  , text_components(mbox.text_components)
  , text_split(mbox.text_split)
  , max_line_width(mbox.max_line_width)
  , center_text(mbox.center_text)
  , emphasis_color(mbox.emphasis_color)
  , num_buttons(mbox.num_buttons)
  , text_num_lines(mbox.text_num_lines)
  , button_digit()
  , button()
{
  setTitlebarText (mbox.getTitlebarText());
  init ( mbox.button_digit[0]
       , mbox.button_digit[1]
       , mbox.button_digit[2] );
}

//----------------------------------------------------------------------
FMessageBox::FMessageBox ( const FString& caption
                         , const FString& message
                         , int button0
                         , int button1
                         , int button2
                         , FWidget* parent )
  : FDialog(parent)
  , headline_text()
  , text(message)
  , text_components(0)
  , text_split()
  , max_line_width(0)
  , center_text(false)
  , emphasis_color(wc.dialog_emphasis_fg)
  , num_buttons(0)
  , text_num_lines(0)
  , button_digit()
  , button()
{
  setTitlebarText(caption);
  init(button0, button1, button2);
}

//----------------------------------------------------------------------
FMessageBox::~FMessageBox()  // destructor
{
  for (uInt n=0; n < num_buttons; n++)
    delete button[n];
}


// public methods of FMessageBox
//----------------------------------------------------------------------
FMessageBox& FMessageBox::operator = (const FMessageBox& mbox)
{
  if ( &mbox == this )
  {
    return *this;
  }
  else
  {
    for (uInt n=0; n < num_buttons; n++)
      delete button[n];

    if ( mbox.getParentWidget() )
      mbox.getParentWidget()->addChild (this);

    headline_text   = mbox.headline_text;
    text            = mbox.text;
    text_components = mbox.text_components;
    text_split      = mbox.text_split;
    max_line_width  = mbox.max_line_width;
    center_text     = mbox.center_text;
    emphasis_color  = mbox.emphasis_color;
    num_buttons     = mbox.num_buttons;
    text_num_lines  = mbox.text_num_lines;

    setTitlebarText (mbox.getTitlebarText());
    init ( mbox.button_digit[0]
         , mbox.button_digit[1]
         , mbox.button_digit[2] );

    return *this;
  }
}

//----------------------------------------------------------------------
void FMessageBox::setHeadline (const FString& headline)
{
  headline_text = headline;
  setHeight(getHeight() + 2, true);

  for (uInt n=0; n < num_buttons; n++)
    button[n]->setY(getHeight()-4, false);

  uInt len = headline_text.getLength();

  if ( len > max_line_width )
    max_line_width = len;
}

//----------------------------------------------------------------------
void FMessageBox::setText (const FString& txt)
{
  text = txt;
  calculateDimensions();
  button[0]->setY(getHeight()-4, false);

  if ( button_digit[1] != 0 )
    button[1]->setY(getHeight()-4, false);

  if ( button_digit[2] != 0 )
    button[2]->setY(getHeight()-4, false);

  adjustButtons();
}

//----------------------------------------------------------------------
int FMessageBox::info ( FWidget* parent
                      , const FString& caption
                      , const FString& message
                      , int button0
                      , int button1
                      , int button2 )
{
  int reply;
  FMessageBox* mbox = new FMessageBox ( caption, message
                                      , button0, button1, button2
                                      , parent );
  reply = mbox->exec();
  delete mbox;
  return reply;
}

//----------------------------------------------------------------------
int FMessageBox::info ( FWidget* parent
                      , const FString& caption
                      , int num
                      , int button0
                      , int button1
                      , int button2 )
{
  int reply;
  FMessageBox* mbox = new FMessageBox ( caption
                                      , FString().setNumber(num)
                                      , button0, button1, button2
                                      , parent );
  reply = mbox->exec();
  delete mbox;
  return reply;
}

//----------------------------------------------------------------------
int FMessageBox::error ( FWidget* parent
                       , const FString& message
                       , int button0
                       , int button1
                       , int button2 )
{
  int reply;
  const FString& caption = "Error message";
  FMessageBox* mbox = new FMessageBox ( caption, message
                                      , button0, button1, button2
                                      , parent );
  mbox->beep();
  mbox->setHeadline("Warning:");
  mbox->setCenterText();
  mbox->setForegroundColor(mbox->wc.error_box_fg);
  mbox->setBackgroundColor(mbox->wc.error_box_bg);
  mbox->emphasis_color  = mbox->wc.error_box_emphasis_fg;
  reply = mbox->exec();
  delete mbox;
  return reply;
}


// protected methods of FMessageBox
//----------------------------------------------------------------------
void FMessageBox::adjustSize()
{
  int X, Y, max_width, max_height;
  FWidget* root_widget = getRootWidget();

  if ( root_widget )
  {
    max_width = root_widget->getClientWidth();
    max_height = root_widget->getClientHeight();
  }
  else
  {
    // fallback to xterm default size
    max_width = 80;
    max_height = 24;
  }

  X = 1 + int((max_width-getWidth())/2);
  Y = 1 + int((max_height-getHeight())/3);
  setPos(X, Y, false);
  FDialog::adjustSize();
}

//----------------------------------------------------------------------
void FMessageBox::cb_processClick (FWidget*, data_ptr data)
{
  int* reply = static_cast<int*>(data);
  done (*reply);
}


// private methods of FMessageBox
//----------------------------------------------------------------------
void FMessageBox::init (int button0, int button1, int button2)
{
  calculateDimensions();

  if ( (button2 && ! button1) || (button1 && ! button0) )
  {
    button0 = button1 = button2 = 0;
  }

  if ( button0 == 0 )
    button0 = FMessageBox::Ok;

  if ( button1 == 0 && button2 == 0 )
    num_buttons = 1;
  else if ( button2 == 0 )
    num_buttons = 2;
  else
    num_buttons = 3;

  button_digit[0] = button0;
  button_digit[1] = button1;
  button_digit[2] = button2;

  button[0] = new FButton (this);
  button[0]->setText(button_text[button0]);
  button[0]->setPos(3, getHeight()-4, false);
  button[0]->setWidth(1, false);
  button[0]->setHeight(1, false);
  button[0]->setFocus();

  if ( button1 > 0 )
  {
    button[1] = new FButton(this);
    button[1]->setText(button_text[button1]);
    button[1]->setPos(17, getHeight()-4, false);
    button[1]->setWidth(0, false);
    button[1]->setHeight(1, false);
  }

  if ( button2 > 0 )
  {
    button[2] = new FButton(this);
    button[2]->setText(button_text[button2]);
    button[2]->setPos(32, getHeight()-4, false);
    button[2]->setWidth(0, false);
    button[2]->setHeight(1, false);
  }

  resizeButtons();
  adjustButtons();

  if ( button_digit[0] != 0 )
  {
    button[0]->addCallback
    (
      "clicked",
      F_METHOD_CALLBACK (this, &FMessageBox::cb_processClick),
      static_cast<FWidget::data_ptr>(&button_digit[0])
    );
  }

  if ( button_digit[1] != 0 )
  {
    button[1]->addCallback
    (
      "clicked",
      F_METHOD_CALLBACK (this, &FMessageBox::cb_processClick),
      static_cast<FWidget::data_ptr>(&button_digit[1])
    );
  }

  if ( button_digit[2] != 0 )
  {
    button[2]->addCallback
    (
      "clicked",
      F_METHOD_CALLBACK (this, &FMessageBox::cb_processClick),
      static_cast<FWidget::data_ptr>(&button_digit[2])
    );
  }

  setModal();
}

//----------------------------------------------------------------------
void FMessageBox::calculateDimensions()
{
  int x, y, w, h;
  int headline_height = 0;
  FWidget* parent_widget = getParentWidget();
  text_split = text.split("\n");
  text_num_lines = uInt(text_split.size());
  text_components = &text_split[0];
  max_line_width = 0;

  if ( ! headline_text.isNull() )
    headline_height = 2;

  for (uInt i=0; i < text_num_lines; i++)
  {
    uInt len = text_components[i].getLength();

    if ( len > max_line_width )
      max_line_width = len;
  }

  h = int(text_num_lines) + 8 + headline_height;
  w = int(max_line_width + 4);

  if ( w < 20 )
    w = 20;

  if ( parent_widget )
  {
    x = 1 + int((parent_widget->getWidth()-w)/2);
    y = 1 + int((parent_widget->getHeight()-h)/3);
  }
  else
    x = y = 1;

  setGeometry (x, y, w, h);
}

//----------------------------------------------------------------------
void FMessageBox::draw()
{
  FDialog::draw();

  int head_offset = 0;
  int center_x = 0;
  int msg_x = int((getWidth() - int(max_line_width)) / 2);  // center the whole block

  if ( isMonochron() )
    setReverse(true);

  if ( ! headline_text.isNull() )
  {
    setColor(emphasis_color, getBackgroundColor());
    uInt headline_length = headline_text.getLength();

    if ( center_text )  // center one line
      center_x = int((max_line_width - headline_length) / 2);

    setPrintPos (1 + msg_x + center_x, 4);
    print (headline_text);
    head_offset = 2;
  }

  setColor();

  for (int i=0; i < int(text_num_lines); i++)
  {
    uInt line_length = text_components[i].getLength();

    if ( center_text )  // center one line
      center_x = int((max_line_width - line_length) / 2);

    setPrintPos (1 + msg_x + center_x, 4 + head_offset + i);
    print(text_components[i]);
  }

  if ( isMonochron() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FMessageBox::resizeButtons()
{
  uInt len[3], max_size;

  for (uInt n=0; n < num_buttons; n++)
  {
    len[n] = button[n]->getText().getLength();

    if ( button[n]->getText().includes('&') )
      len[n]--;
  }

  if ( num_buttons == 1 )
    max_size = len[0];
  else
  {
    assert ( num_buttons > 1 );
    max_size = std::max(len[0], len[1]);

    if ( num_buttons == 3 )
      max_size = std::max(max_size, len[2]);
  }

  if ( max_size < 7 )
    max_size = 7;

  for (uInt n=0; n < num_buttons; n++)
    button[n]->setWidth(int(max_size + 3), false);
}

//----------------------------------------------------------------------
void FMessageBox::adjustButtons()
{
  static const int gap = 4;
  int btn_width = 0;

  for (uInt n=0; n < num_buttons; n++)
  {
    if ( n == num_buttons-1 )
      btn_width += button[n]->getWidth();
    else
      btn_width += button[n]->getWidth() + gap;
  }

  if ( btn_width >= getWidth()-4 )
  {
    int max_width;
    FWidget* root_widget = getRootWidget();
    setWidth(btn_width + 5);
    max_width = ( root_widget ) ? root_widget->getClientWidth() : 80;
    setX (int((max_width-getWidth()) / 2));
  }

  int btn_x = int((getWidth()-btn_width) / 2);

  for (uInt n=0; n < num_buttons; n++)
  {
    if ( n == 0 )
      button[n]->setX(btn_x);
    else
    {
      int btn_size = button[n]->getWidth();
      button[n]->setX( btn_x + int(n) * (btn_size + gap) );
    }
  }
}
