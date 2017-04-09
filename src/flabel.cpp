// File: flabel.cpp
// Provides: class FLabel

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
  , alignment(fc::alignLeft)
  , emphasis_color(wc.label_emphasis_fg)
  , ellipsis_color(wc.label_ellipsis_fg)
  , emphasis(false)
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
  , alignment(fc::alignLeft)
  , emphasis_color(wc.label_emphasis_fg)
  , ellipsis_color(wc.label_ellipsis_fg)
  , emphasis(false)
  , reverse_mode(false)
  , accel_widget(0)
{
  init();
  setText(txt);
}

//----------------------------------------------------------------------
FLabel::~FLabel() // destructor
{
  delAccelerator();
}


// public methods of FLabel
//----------------------------------------------------------------------
void FLabel::setAccelWidget (FWidget* widget)
{
  if ( widget )
    accel_widget = widget;

  accel_widget->addCallback
  (
    "destroy",
    F_METHOD_CALLBACK (this, &FLabel::cb_accel_widget_destroyed)
  );
}

//----------------------------------------------------------------------
void FLabel::setAlignment (uInt align)
{
  if ( align != fc::alignLeft
      && align != fc::alignCenter
      && align != fc::alignRight )
    alignment = fc::alignLeft;
  else
    alignment = align;
}

//----------------------------------------------------------------------
bool FLabel::setEmphasis (bool on)
{
  if ( emphasis != on )
    emphasis = on;

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
    flags |= fc::active;
    setHotkeyAccelerator();
  }
  else
  {
    flags &= ~fc::active;
    delAccelerator();
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
    delAccelerator();
    setHotkeyAccelerator();
  }
}

//----------------------------------------------------------------------
void FLabel::hide()
{
  short fg, bg;
  int size;
  char* blank;
  FWidget* parent_widget = getParentWidget();

  FWidget::hide();

  if ( parent_widget )
  {
    fg = parent_widget->getForegroundColor();
    bg = parent_widget->getBackgroundColor();
  }
  else
  {
    fg = wc.dialog_fg;
    bg = wc.dialog_bg;
  }

  setColor (fg, bg);
  size = getWidth();

  if ( size < 0 )
    return;

  blank = new char[size+1];
  std::memset(blank, ' ', uLong(size));
  blank[getWidth()] = '\0';
  setPrintPos (1,1);
  print (blank);
  delete[] blank;
}

//----------------------------------------------------------------------
void FLabel::onMouseDown (FMouseEvent* ev)
{
  if ( ev->getButton() != fc::LeftButton )
    return;

  if ( ! (isEnabled() && accel_widget) )
  {
    // send click to the parent widget
    if ( FWidget* parent = getParentWidget() )
    {
      int b = ev->getButton();
      const FPoint& tp = ev->getTermPos();
      const FPoint& p = parent->termToWidgetPos(tp);
      FMouseEvent* _ev = new FMouseEvent (fc::MouseDown_Event, p, tp, b);
      FApplication::sendEvent (parent, _ev);
      delete _ev;
    }

    return;
  }

  if ( ! accel_widget->hasFocus() )
  {
    // focus the accelerator widget
    FWidget* focused_widget = getFocusWidget();
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    accel_widget->setFocus();

    if ( focused_widget )
      focused_widget->redraw();

    accel_widget->redraw();

    if ( getStatusBar() )
    {
      accel_widget->getStatusBar()->drawMessage();
      updateTerminal();
      flush_out();
    }
  }
}

//----------------------------------------------------------------------
void FLabel::onAccel (FAccelEvent* ev)
{
  if ( ! (isEnabled() && accel_widget) )
    return;

  if ( ! accel_widget->hasFocus() )
  {
    FWidget* focused_widget = static_cast<FWidget*>(ev->focusedWidget());
    FFocusEvent out (fc::FocusOut_Event);
    FApplication::queueEvent(focused_widget, &out);
    accel_widget->setFocus();


    if ( focused_widget )
      focused_widget->redraw();

    accel_widget->redraw();
    FFocusEvent in (fc::FocusIn_Event);
    FApplication::sendEvent(accel_widget, &in);

    if ( getStatusBar() )
    {
      accel_widget->getStatusBar()->drawMessage();
      updateTerminal();
      flush_out();
    }
  }

  ev->accept();
}

//----------------------------------------------------------------------
void FLabel::cb_accel_widget_destroyed (FWidget*, data_ptr)
{
  accel_widget = 0;
  delAccelerator();
}


// private methods of FLabel
//----------------------------------------------------------------------
void FLabel::init()
{
  FWidget* parent_widget = getParentWidget();

  if ( isEnabled() )
    flags |= fc::active;

  unsetFocusable();

  if ( parent_widget )
  {
    setForegroundColor (parent_widget->getForegroundColor());
    setBackgroundColor (parent_widget->getBackgroundColor());
  }
  else
  {
    setForegroundColor (wc.dialog_fg);
    setBackgroundColor (wc.dialog_bg);
  }
}

//----------------------------------------------------------------------
uChar FLabel::getHotkey()
{
  uInt length;

  if ( text.isEmpty() )
    return 0;

  length = text.getLength();

  for (uInt i=0; i < length; i++)
  {
    try
    {
      if ( (i+1 < length) && (text[i] == '&') )
        return uChar(text[++i]);
    }
    catch (const std::out_of_range&)
    {
      return 0;;
    }
  }

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
    if ( std::isalpha(hotkey) || std::isdigit(hotkey) )
    {
      addAccelerator (std::tolower(hotkey));
      addAccelerator (std::toupper(hotkey));
      // Meta + hotkey
      addAccelerator (fc::Fmkey_meta + std::tolower(hotkey));
    }
    else
      addAccelerator (getHotkey());
  }
  else
    delAccelerator();
}

//----------------------------------------------------------------------
int FLabel::getXOffset(int length)
{
  switch ( alignment )
  {
    case fc::alignLeft:
      return 0;

    case fc::alignCenter:
      if ( length < getWidth() )
        return int((getWidth() - length) / 2);
      else
        return 0;

    case fc::alignRight:
      if ( length < getWidth() )
        return getWidth() - length;
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
  isActive = ((flags & fc::active) != 0);
  isNoUnderline = ((flags & fc::no_underline) != 0);

  for (int x=0; x < xoffset; x++)
    print (' ');

  if ( length <= uInt(getWidth()) )
    to_char = int(length);
  else
    to_char = getWidth() - 2;

  if ( hasReverseMode() )
    setReverse(true);

  for (int z=0; z < to_char; z++)
  {
    if ( ! std::iswprint(wint_t(line[z])) )
    {
      if ( ! isNewFont() && ( int(line[z]) < fc::NF_rev_left_arrow2
                             || int(line[z]) > fc::NF_check_mark ) )
      {
        line[z] = L' ';
      }
    }
    if ( (z == hotkeypos) && isActive )
    {
      setColor (wc.label_hotkey_fg, wc.label_hotkey_bg);

      if ( ! isNoUnderline )
        setUnderline();

      print ( line[z] );

      if ( ! isNoUnderline )
        unsetUnderline();

      if ( hasEmphasis() )
        setColor (emphasis_color, getBackgroundColor());
      else
        setColor();
    }
    else
      print ( line[z] );
  }

  if ( length > uInt(getWidth()) )
  {
    setColor (ellipsis_color, getBackgroundColor());
    print ("..");
    setColor();
  }
  else
  {
    for (int x=xoffset+to_char; x < getWidth(); x++)
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

  if ( isMonochron() )
  {
    setReverse(true);

    if ( hasEmphasis() )
      setBold(true);
  }

  if ( hasEmphasis() )
    setColor (emphasis_color, getBackgroundColor());
  else
    setColor();

  hotkeypos = -1;

  if ( multiline && getHeight() >= 2 )
  {
    uInt y = 0;
    uInt text_lines = uInt(multiline_text.size());
    bool hotkey_printed = false;

    while ( y < text_lines && y < uInt(getHeight()) )
    {
      length = multiline_text[y].getLength();
      LabelText = new wchar_t[length+1]();
      src  = const_cast<wchar_t*>(multiline_text[y].wc_str());
      dest = const_cast<wchar_t*>(LabelText);

      if ( ! hotkey_printed )
        hotkeypos = getHotkeyPos(src, dest, length);
      else
        std::wcsncpy(dest, src, length);

      setPrintPos (1, 1+int(y));

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
    LabelText = new wchar_t[length+1]();
    src  = const_cast<wchar_t*>(text.wc_str());
    dest = const_cast<wchar_t*>(LabelText);
    hotkeypos = getHotkeyPos (src, dest, length);

    if ( hotkeypos != -1 )
      length--;

    setPrintPos (1,1);
    xoffset = getXOffset (int(length));
    printLine (LabelText, length, hotkeypos, xoffset);
    delete[] LabelText;
  }

  if ( isMonochron() )
  {
    setReverse(false);

    if ( hasEmphasis() )
      setBold(false);
  }
}
