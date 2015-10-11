// File: fmessagebox.cpp
// Provides: class FMessageBox

#include "fapp.h"
#include "fmessagebox.h"


static const char* button_text[] =
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
FMessageBox::FMessageBox(FWidget* parent)
  : FDialog(parent)
  , headline_text()
  , text()
  , text_components(0)
  , text_split()
  , maxLineWidth(0)
  , center_text(false)
  , emphasis_color(wc.dialog_emphasis_fg)
  , numButtons(0)
  , text_num_lines(0)
  , button_digit()
  , button()
{
  setTitlebarText("Message for you");
  init(FMessageBox::Ok, 0, 0);
}

//----------------------------------------------------------------------
FMessageBox::FMessageBox (const FMessageBox& mbox)
  : FDialog(mbox.parentWidget())
  , headline_text(mbox.headline_text)
  , text(mbox.text)
  , text_components(mbox.text_components)
  , text_split(mbox.text_split)
  , maxLineWidth(mbox.maxLineWidth)
  , center_text(mbox.center_text)
  , emphasis_color(mbox.emphasis_color)
  , numButtons(mbox.numButtons)
  , text_num_lines(mbox.text_num_lines)
  , button_digit()
  , button()
{
  setTitlebarText (mbox.getTitlebarText());
  init ( *mbox.button_digit[0]
       , *mbox.button_digit[1]
       , *mbox.button_digit[2] );
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
  , maxLineWidth(0)
  , center_text(false)
  , emphasis_color(wc.dialog_emphasis_fg)
  , numButtons(0)
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
  for (uInt n=0; n < numButtons; n++)
    delete button[n];

  delete button_digit[2];
  delete button_digit[1];
  delete button_digit[0];
}


// private methods of FMessageBox
//----------------------------------------------------------------------
void FMessageBox::init(int button0, int button1, int button2)
{
  msg_dimension();

  if ( (button2 && ! button1) || (button1 && ! button0) )
  {
    button0 = button1 = button2 = 0;
  }
  if ( button0 == 0 )
    button0 = FMessageBox::Ok;
  if ( button1 == 0 && button2 == 0 )
    numButtons = 1;
  else if ( button2 == 0 )
    numButtons = 2;
  else
    numButtons = 3;

  button_digit[0] = new int(button0);
  button_digit[1] = new int(button1);
  button_digit[2] = new int(button2);

  button[0] = new FButton (this);
  button[0]->setText(button_text[button0]);
  button[0]->setX(3, false);
  button[0]->setY(height-4, false);
  button[0]->setWidth(1, false);
  button[0]->setHeight(1, false);
  button[0]->setFocus();
  button[0]->setShadow();

  if ( button1 > 0 )
  {
    button[1] = new FButton(this);
    button[1]->setText(button_text[button1]);
    button[1]->setX(17, false);
    button[1]->setY(height-4, false);
    button[1]->setWidth(0, false);
    button[1]->setHeight(1, false);
    button[1]->setShadow();
  }

  if ( button2 > 0 )
  {
    button[2] = new FButton(this);
    button[2]->setText(button_text[button2]);
    button[2]->setX(32, false);
    button[2]->setY(height-4, false);
    button[2]->setWidth(0, false);
    button[2]->setHeight(1, false);
    button[2]->setShadow();
  }

  resizeButtons();
  adjustButtons();

  if ( *button_digit[0] != 0 )
    button[0]->addCallback
    (
      "clicked",
      _METHOD_CALLBACK (this, &FMessageBox::cb_processClick),
      static_cast<FWidget::data_ptr>(button_digit[0])
    );


  if ( *button_digit[1] != 0 )
    button[1]->addCallback
    (
      "clicked",
      _METHOD_CALLBACK (this, &FMessageBox::cb_processClick),
      static_cast<FWidget::data_ptr>(button_digit[1])
    );

  if ( *button_digit[2] != 0 )
    button[2]->addCallback
    (
      "clicked",
      _METHOD_CALLBACK (this, &FMessageBox::cb_processClick),
      static_cast<FWidget::data_ptr>(button_digit[2])
    );

  setModal();
  setTransparentShadow();
}

//----------------------------------------------------------------------
void FMessageBox::msg_dimension()
{
  int x, y, w, h;
  int headline_height = 0;
  text_split = text.split("\n");
  text_num_lines = uInt(text_split.size());
  text_components = &text_split[0];
  maxLineWidth = 0;

  if ( ! headline_text.isNull() )
    headline_height = 2;

  for (uInt i=0; i < text_num_lines; i++)
  {
    uInt len = text_components[i].getLength();
    if ( len > maxLineWidth )
      maxLineWidth = len;
  }
  h = int(text_num_lines) + 8 + headline_height;
  w = int(maxLineWidth + 4);
  if ( w < 20 )
    w = 20;
  x = 1 + int((parentWidget()->getWidth()-w)/2);
  y = 1 + int((parentWidget()->getHeight()-h)/3);
  setGeometry (x, y, w, h);
}

//----------------------------------------------------------------------
void FMessageBox::draw()
{
  FDialog::draw();

  if ( Encoding == fc::VT100 )
    unsetVT100altChar();

  int head_offset = 0;
  int center_x = 0;
  int msg_x = int((width - int(maxLineWidth)) / 2);  // center the whole block

  setUpdateVTerm(false);

  if ( isMonochron() )
    setReverse(true);

  if ( ! headline_text.isNull() )
  {
    setColor(emphasis_color, backgroundColor);
    uInt headline_length = headline_text.getLength();
    if ( center_text )  // center one line
      center_x = int((maxLineWidth - headline_length) / 2);
    gotoxy (xpos+xmin-1+msg_x+center_x, ypos+ymin+2);
    print (headline_text);
    head_offset = 2;
  }

  setColor(foregroundColor, backgroundColor);

  for (int i=0; i < int(text_num_lines); i++)
  {
    uInt line_length = text_components[i].getLength();
    if ( center_text )  // center one line
      center_x = int((maxLineWidth - line_length) / 2);
    gotoxy (xpos+xmin-1+msg_x+center_x, ypos+ymin+2+head_offset+i);
    print(text_components[i]);
  }

  if ( isMonochron() )
    setReverse(false);

  setUpdateVTerm(true);
}

//----------------------------------------------------------------------
void FMessageBox::resizeButtons()
{
  uInt len[3], max_size;
  for (uInt n=0; n < numButtons; n++)
  {
    len[n] = button[n]->getText().getLength();
    if ( button[n]->getText().includes('&') )
      len[n]--;
  }

  if ( numButtons == 1 )
    max_size = len[0];
  else
  {
    assert ( numButtons > 1 );
    max_size = std::max(len[0], len[1]);
    if ( numButtons == 3 )
      max_size = std::max(max_size, len[2]);
  }
  if ( max_size < 7 )
    max_size = 7;

  for (uInt n=0; n < numButtons; n++)
    button[n]->setWidth(int(max_size + 3), false);
}

//----------------------------------------------------------------------
void FMessageBox::adjustButtons()
{
  int btn_width=0;
  int gap = 4;
  for (uInt n=0; n < numButtons; n++)
  {
    if ( n == numButtons-1 )
      btn_width += button[n]->getWidth();
    else
      btn_width += button[n]->getWidth() + gap;
  }

  if ( btn_width >= width-4 )
  {
    setWidth(btn_width + 5);
    int max_width = getRootWidget()->getClientWidth();
    setX(int((max_width-width) / 2));
  }

  int btn_x = int((width-btn_width) / 2);

  for (uInt n=0; n < numButtons; n++)
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

//----------------------------------------------------------------------
void FMessageBox::cb_processClick (FWidget*, void* data_ptr)
{
  int* reply = static_cast<int*>(data_ptr);
  done (*reply);
}


// protected methods of FMessageBox
//----------------------------------------------------------------------
void FMessageBox::adjustSize()
{
  int X, Y, max_width, max_height;
  max_height = getRootWidget()->getClientHeight();
  max_width = getRootWidget()->getClientWidth();
  X = 1 + int((max_width-width)/2);
  Y = 1 + int((max_height-height)/3);
  setX(X, false);
  setY(Y, false);
  FDialog::adjustSize();
}


// public methods of FMessageBox
//----------------------------------------------------------------------
FMessageBox& FMessageBox::operator = (const FMessageBox& mbox)
{
  if ( &mbox == this )
    return *this;
  else
  {
    for (uInt n=0; n < numButtons; n++)
      delete button[n];

    delete button_digit[2];
    delete button_digit[1];
    delete button_digit[0];

    mbox.parentWidget()->addChild (this);

    headline_text   = mbox.headline_text;
    text            = mbox.text;
    text_components = mbox.text_components;
    text_split      = mbox.text_split;
    maxLineWidth    = mbox.maxLineWidth;
    center_text     = mbox.center_text;
    emphasis_color  = mbox.emphasis_color;
    numButtons      = mbox.numButtons;
    text_num_lines  = mbox.text_num_lines;

    setTitlebarText (mbox.getTitlebarText());
    init ( *mbox.button_digit[0]
         , *mbox.button_digit[1]
         , *mbox.button_digit[2] );

    return *this;
  }
}

//----------------------------------------------------------------------
void FMessageBox::setHeadline (const FString& headline)
{
  int old_height = height;
  headline_text = headline;
  setHeight(height + 2, true);
  for (uInt n=0; n < numButtons; n++)
    button[n]->setY(height-4, false);
  uInt len = headline_text.getLength();
  if ( len > maxLineWidth )
    maxLineWidth = len;
  if ( vwin && height != old_height )
    resizeArea (vwin);
}

//----------------------------------------------------------------------
void FMessageBox::setHeadline (const std::string& headline)
{
  FString headline_txt(headline);
  setHeadline( headline_txt );
}

//----------------------------------------------------------------------
void FMessageBox::setHeadline (const char* headline)
{
  FString headline_txt(headline);
  setHeadline( headline_txt );
}

//----------------------------------------------------------------------
void FMessageBox::setText (const FString& txt)
{
  text = txt;
  msg_dimension();
  button[0]->setY(height-4, false);
  if ( *button_digit[1] != 0 )
    button[1]->setY(height-4, false);
  if ( *button_digit[2] != 0 )
    button[2]->setY(height-4, false);
  adjustButtons();
}

//----------------------------------------------------------------------
void FMessageBox::setText (const std::string& txt)
{
  FString message_text(txt);
  setText( message_text );
}

//----------------------------------------------------------------------
void FMessageBox::setText (const char* txt)
{
  FString message_text(txt);
  setText( message_text );
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
  const FString caption = "Error message";
  FMessageBox* mbox = new FMessageBox ( caption, message
                                      , button0, button1, button2
                                      , parent );
  mbox->beep();
  mbox->setHeadline("Warning:");
  mbox->setCenterText();
  mbox->foregroundColor = mbox->wc.error_box_fg;
  mbox->backgroundColor = mbox->wc.error_box_bg;
  mbox->emphasis_color  = mbox->wc.error_box_emphasis_fg;
  reply = mbox->exec();
  delete mbox;
  return reply;
}
