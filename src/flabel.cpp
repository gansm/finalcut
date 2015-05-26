// flabel.cpp
// class FLabel

#include "fapp.h"
#include "flabel.h"
#include "fstatusbar.h"


//----------------------------------------------------------------------
// class FLabel
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FLabel::FLabel(FWidget* parent) : FWidget(parent)
{
  this->init();
}

//----------------------------------------------------------------------
FLabel::FLabel (const FString& txt, FWidget* parent) : FWidget(parent)
{
  this->init();
  setText(txt);
}

//----------------------------------------------------------------------
FLabel::~FLabel() // destructor
{
  delAccelerator (this);
}


// private methods of FLabel
//----------------------------------------------------------------------
void FLabel::init()
{
  flags = 0;
  emphasis = 0;
  xoffset = 0;
  alignment = fc::alignLeft;
  this->text = "";
  accel_widget = 0;

  if ( isEnabled() )
    this->flags |= ACTIVE;

  unsetFocusable();

  foregroundColor = parentWidget()->getForegroundColor();
  backgroundColor = parentWidget()->getBackgroundColor();
  emphasis_color = wc.label_emphasis_fg;
  ellipsis_color = wc.label_ellipsis_fg;
  reverse_mode = false;
}

//----------------------------------------------------------------------
uChar FLabel::getHotkey()
{
  uInt length;

  if ( text.isEmpty() )
    return 0;

  length = text.getLength();

  for (uInt i=0; i < length; i++)
    if ( (i+1 < length) && (text[i] == '&') )
      return uChar(text[++i]);
  return 0;
}

//----------------------------------------------------------------------
void FLabel::setHotkeyAccelerator()
{
  int hotkey = getHotkey();
  if ( hotkey )
  {
    if ( isalpha(hotkey) || isdigit(hotkey) )
    {
      addAccelerator (tolower(hotkey), this);
      addAccelerator (toupper(hotkey), this);
      // Meta + hotkey
      addAccelerator (0x20000e0 + tolower(hotkey), this);
    }
    else
      addAccelerator (getHotkey(), this);
  }
  else
    delAccelerator(this);
}

//----------------------------------------------------------------------
void FLabel::draw()
{
  register wchar_t* src;
  register wchar_t* dest;
  wchar_t* LabelText;
  FString txt;
  uInt length;
  int hotkeypos, to_char;
  bool isActive, isNoUnderline;

  if ( text.isNull() || text.isEmpty() )
    return;

  if ( Encoding == fc::VT100 )
    unsetVT100altChar();

  setUpdateVTerm(false);
  if ( hasEmphasis() )
    setColor (emphasis_color, backgroundColor);
  else
    setColor (foregroundColor, backgroundColor);

  length = text.getLength();
  hotkeypos = -1;
  LabelText = new wchar_t[length+1];
  txt  = this->text;
  src  = const_cast<wchar_t*>(txt.wc_str());
  dest = const_cast<wchar_t*>(LabelText);

  isActive = ((flags & ACTIVE) != 0);
  isNoUnderline = ((flags & NO_UNDERLINE) != 0);

  // find hotkey position in string
  // + generate a new string without the '&'-sign
  for (uInt i=0; i < length; i++)
  {
    if ( (i < length) && (txt[i] == '&') && (hotkeypos == -1) )
    {
      hotkeypos = int(i);
      i++;
      src++;
    }
    *dest++ = *src++;
  }

  if ( hotkeypos != -1 )
    length--;

  gotoxy (xpos+xmin-1, ypos+ymin-1);

  for (int x=0; x < xoffset; x++)
    print (' ');

  if ( length <= uInt(width) )
    to_char = int(length);
  else
    to_char = width - 2;

  if ( hasReverseMode() )
    setReverse(true);

  for (int z=0; z < to_char; z++)
  {
    if ( (z == hotkeypos) && isActive )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);
      if ( ! isNoUnderline )
        setUnderline();
      print ( LabelText[z] );
      if ( ! isNoUnderline )
        unsetUnderline();
      if ( hasEmphasis() )
        setColor (emphasis_color, backgroundColor);
      else
        setColor (foregroundColor, backgroundColor);
    }
    else
      print ( LabelText[z] );
  }

  for (int x=to_char+2; x < width; x++)
    print (' ');

  if ( length > uInt(width) )
  {
    setColor (ellipsis_color, backgroundColor);
    print ("..");
  }
  if ( hasReverseMode() )
    setReverse(false);

  setUpdateVTerm(true);
  delete[] LabelText;
}


// public methods of FLabel
//----------------------------------------------------------------------
void FLabel::hide()
{
  int fg, bg;
  char* blank;

  FWidget::hide();

  fg = parentWidget()->getForegroundColor();
  bg = parentWidget()->getBackgroundColor();
  setColor (fg, bg);

  blank = new char[width+1];
  memset(blank, ' ', uLong(width));
  blank[width] = '\0';
  print (blank);
  delete[] blank;
}

//----------------------------------------------------------------------
void FLabel::onMouseDown (FMouseEvent* event)
{
  if ( event->getButton() != LeftButton )
    return;

  if ( ! isEnabled() || ! accel_widget  )
    return;

  if ( ! accel_widget->hasFocus() )
  {
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    accel_widget->setFocus();
    if ( focused_widget )
      focused_widget->redraw();
    accel_widget->redraw();
    if ( statusBar() )
    {
      accel_widget->statusBar()->drawMessage();
      updateTerminal();
      flush_out();
    }
  }
}

//----------------------------------------------------------------------
void FLabel::onAccel (FAccelEvent* event)
{
  if ( ! isEnabled() || ! accel_widget  )
    return;

  if ( ! accel_widget->hasFocus() )
  {
    FWidget* focused_widget = static_cast<FWidget*>(event->focusedWidget());
    FFocusEvent out (FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    accel_widget->setFocus();
    if ( focused_widget )
      focused_widget->redraw();
    accel_widget->redraw();
    if ( statusBar() )
    {
      accel_widget->statusBar()->drawMessage();
      updateTerminal();
      flush_out();
    }
  }
  event->accept();
}

//----------------------------------------------------------------------
void FLabel::cb_accel_widget_destroyed (FWidget*, void*)
{
  accel_widget = 0;
  delAccelerator (this);
}

//----------------------------------------------------------------------
void FLabel::setAccelWidget (FWidget* widget)
{
  if ( widget )
    accel_widget = widget;

  accel_widget->addCallback
  (
    "destroy",
    this,
    reinterpret_cast<FWidget::FMemberCallback>(&FLabel::cb_accel_widget_destroyed),
    null
  );
}

//----------------------------------------------------------------------
void FLabel::setAlignment (uInt align)
{
  int text_length;

  if (  align != fc::alignLeft
     && align != fc::alignCenter
     && align != fc::alignRight )
    alignment = fc::alignLeft;
  else
    alignment = align;

  text_length = int(text.getLength());

  if ( getHotkey() )
    text_length--;

  if ( text_length > width )
   text_length = width;

  switch ( alignment )
  {                                    // convert type to msg string
    case fc::alignLeft:
      xoffset = 0;
      break;
    case fc::alignCenter:
      xoffset = int((width - text_length) / 2);
      break;
    case fc::alignRight:
      xoffset = width - text_length;
      break;
  }
}

//----------------------------------------------------------------------
bool FLabel::setEmphasis (bool on)
{
  if ( on )
    this->emphasis |= EMPHASIS;
  else
    this->emphasis &= ~EMPHASIS;
  return on;
}

//----------------------------------------------------------------------
bool FLabel::setReverseMode (bool on)
{
  if ( reverse_mode != on )
    reverse_mode = on;
  return on;
}

//----------------------------------------------------------------------
bool FLabel::setEnable (bool on)
{
  FWidget::setEnable(on);

  if ( on )
  {
    this->flags |= ACTIVE;
    setHotkeyAccelerator();
  }
  else
  {
    this->flags &= ~ACTIVE;
    delAccelerator (this);
  }
  return on;
}

//----------------------------------------------------------------------
void FLabel::setNumber (long num)
{
  setText(FString().setNumber(num));
}

//----------------------------------------------------------------------
void FLabel::setText (const FString& txt)
{
  this->text = txt;
  if ( isEnabled() )
  {
    delAccelerator (this);
    setHotkeyAccelerator();
  }
}
