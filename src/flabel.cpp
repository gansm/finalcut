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
FLabel::FLabel(FWidget* parent)
  : FWidget(parent)
  , multiline_text()
  , multiline(false)
  , text()
  , emphasis(0)
  , alignment(fc::alignLeft)
  , emphasis_color(wc.label_emphasis_fg)
  , ellipsis_color(wc.label_ellipsis_fg)
  , reverse_mode(false)
  , accel_widget(0)
{
  init();
}

//----------------------------------------------------------------------
FLabel::FLabel (const FString& txt, FWidget* parent)
  : FWidget(parent)
  , multiline_text()
  , multiline(false)
  , text(txt)
  , emphasis(0)
  , alignment(fc::alignLeft)
  , emphasis_color(wc.label_emphasis_fg)
  , ellipsis_color(wc.label_ellipsis_fg)
  , reverse_mode(false)
  , accel_widget(0)
{
  init();
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
  if ( isEnabled() )
    flags |= ACTIVE;

  unsetFocusable();

  foregroundColor = parentWidget()->getForegroundColor();
  backgroundColor = parentWidget()->getBackgroundColor();
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
int FLabel::getHotkeyPos (wchar_t*& src, wchar_t*& dest, uInt length)
{
  // find hotkey position in string
  // + generate a new string without the '&'-sign
  int hotkeypos = -1;
  wchar_t* txt = src;

  for (uInt i=0; i < length; i++)
  {
    if ( (i < length) && (txt[i] == L'&') && (hotkeypos == -1) )
    {
      hotkeypos = int(i);
      i++;
      src++;
    }
    *dest++ = *src++;
  }
  return hotkeypos;
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
int FLabel::getXOffset(int length)
{
  switch ( alignment )
  {
    case fc::alignLeft:
      return 0;

    case fc::alignCenter:
      if ( length < width )
        return int((width - length) / 2);
      else
        return 0;

    case fc::alignRight:
      if ( length < width )
        return width - length;
      else
        return 0;

    default:
      return 0;
  }
}

//----------------------------------------------------------------------
void FLabel::printLine ( wchar_t*& line
                       , uInt length
                       , int  hotkeypos
                       , int  xoffset )
{
  int to_char;
  bool isActive, isNoUnderline;

  isActive = ((flags & ACTIVE) != 0);
  isNoUnderline = ((flags & NO_UNDERLINE) != 0);

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
    if ( ! iswprint(wint_t(line[z])) )
      line[z] = L' ';
    if ( (z == hotkeypos) && isActive )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);
      if ( ! isNoUnderline )
        setUnderline();
      print ( line[z] );
      if ( ! isNoUnderline )
        unsetUnderline();
      if ( hasEmphasis() )
        setColor (emphasis_color, backgroundColor);
      else
        setColor (foregroundColor, backgroundColor);
    }
    else
      print ( line[z] );
  }

  if ( length > uInt(width) )
  {
    setColor (ellipsis_color, backgroundColor);
    print ("..");
    setColor (foregroundColor, backgroundColor);
  }
  else
  {
    for (int x=xoffset+to_char; x < width; x++)
      print (' ');
  }

  if ( hasReverseMode() )
    setReverse(false);
}

//----------------------------------------------------------------------
void FLabel::draw()
{
  wchar_t* src;
  wchar_t* dest;
  wchar_t* LabelText;
  uInt length;
  int hotkeypos, xoffset;

  if ( text.isNull() || text.isEmpty() )
    return;

  if ( Encoding == fc::VT100 )
    unsetVT100altChar();

  setUpdateVTerm(false);

  if ( hasEmphasis() )
    setColor (emphasis_color, backgroundColor);
  else
    setColor (foregroundColor, backgroundColor);

  hotkeypos = -1;

  if ( multiline && height > 1 )
  {
    uInt y = 0;
    uInt text_lines = uInt(multiline_text.size());
    bool hotkey_printed = false;

    while ( y < text_lines && y < uInt(height) )
    {
      length = multiline_text[y].getLength();
      LabelText = new wchar_t[length+1];

      if ( ! hotkey_printed )
      {
        src  = const_cast<wchar_t*>(multiline_text[y].wc_str());
        dest = const_cast<wchar_t*>(LabelText);
        hotkeypos = getHotkeyPos(src, dest, length);
      }
      else
        LabelText = const_cast<wchar_t*>(multiline_text[y].wc_str());

      gotoxy (xpos+xmin-1, ypos+ymin-1+int(y));

      if ( hotkeypos != -1 )
      {
        xoffset = getXOffset (int(length-1));
        printLine (LabelText, length-1, hotkeypos, xoffset);
        hotkey_printed = true;
        hotkeypos = -1;
      }
      else
      {
        xoffset = getXOffset (int(length));
        printLine (LabelText, length, -1, xoffset);
      }
      y++;
      delete[] LabelText;
    }
  }
  else
  {
    length = text.getLength();
    LabelText = new wchar_t[length+1];
    src  = const_cast<wchar_t*>(text.wc_str());
    dest = const_cast<wchar_t*>(LabelText);

    hotkeypos = getHotkeyPos (src, dest, length);

    if ( hotkeypos != -1 )
      length--;

    gotoxy (xpos+xmin-1, ypos+ymin-1);
    xoffset = getXOffset (int(length));
    printLine (LabelText, length, hotkeypos, xoffset);
    delete[] LabelText;
  }
  setUpdateVTerm(true);
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
void FLabel::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != LeftButton )
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
void FLabel::onAccel (FAccelEvent* ev)
{
  if ( ! isEnabled() || ! accel_widget  )
    return;

  if ( ! accel_widget->hasFocus() )
  {
    FWidget* focused_widget = static_cast<FWidget*>(ev->focusedWidget());
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
  ev->accept();
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
  if (  align != fc::alignLeft
     && align != fc::alignCenter
     && align != fc::alignRight )
    alignment = fc::alignLeft;
  else
    alignment = align;
}

//----------------------------------------------------------------------
bool FLabel::setEmphasis (bool on)
{
  if ( on )
    emphasis |= EMPHASIS;
  else
    emphasis &= ~EMPHASIS;
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
    flags |= ACTIVE;
    setHotkeyAccelerator();
  }
  else
  {
    flags &= ~ACTIVE;
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
  text = txt;
  multiline_text = text.split("\r\n");
  if ( int(multiline_text.size()) > 1 )
    multiline = true;
  else
    multiline = false;

  if ( isEnabled() )
  {
    delAccelerator (this);
    setHotkeyAccelerator();
  }
}
