// File: fvterm.cpp
// Provides: class FVTerm

#include "fapp.h"
#include "fvterm.h"
#include "fwidget.h"
#include "fwindow.h"

// global FVTerm object
static FVTerm* init_object = 0;

// static class attributes
bool                 FVTerm::hidden_cursor;
bool                 FVTerm::terminal_update_complete;
bool                 FVTerm::terminal_update_pending;
bool                 FVTerm::force_terminal_update;
bool                 FVTerm::stop_terminal_updates;
int                  FVTerm::skipped_terminal_update = 0;
std::queue<int>*     FVTerm::output_buffer           = 0;
FPoint*              FVTerm::term_pos                = 0;
FVTerm::term_area*   FVTerm::vterm                   = 0;
FVTerm::term_area*   FVTerm::vdesktop                = 0;
FVTerm::term_area*   FVTerm::active_area             = 0;
FVTerm::termcap_map* FVTerm::tcap                    = 0;
FTermcap::tcap_map*  FTermcap::tcap                  = 0;
FVTerm::char_data    FVTerm::term_attribute;
FVTerm::char_data    FVTerm::next_attribute;


//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FVTerm::FVTerm (FVTerm* parent, bool disable_alt_screen)
  : FObject(parent)
  , FTerm(disable_alt_screen)
  , print_area(0)
  , child_print_area(0)
  , vwin(0)
{
  terminal_update_complete = false;

  if ( ! parent )
  {
    init();
  }
}

//----------------------------------------------------------------------
FVTerm::~FVTerm()  // destructor
{
  if ( init_object == this )
  {
    finish();
  }
}


// public methods of FVTerm
//----------------------------------------------------------------------
FPoint FVTerm::getPrintCursor()
{
  term_area* win = getPrintArea();

  if ( win )
    return FPoint ( win->offset_left + win->cursor_x
                  , win->offset_top + win->cursor_y );

  return FPoint(0,0);
}

//----------------------------------------------------------------------
void FVTerm::setTermXY (register int x, register int y)
{
  // Sets the hardware cursor to the given (x,y) position
  int term_x, term_y, term_width, term_height;
  char* move_str;

  if ( term_pos->getX() == x && term_pos->getY() == y )
    return;

  term_width = getColumnNumber();
  term_height = getLineNumber();

  if ( x >= term_width )
  {
    y += x / term_width;
    x %= term_width;
  }

  if ( term_pos->getY() >= term_height )
    term_pos->setY(term_height - 1);

  if ( y >= term_height )
    y = term_height - 1;

  term_x = term_pos->getX();
  term_y = term_pos->getY();

  move_str = moveCursor (term_x, term_y, x, y);

  if ( move_str )
    appendOutputBuffer(move_str);

  flush_out();
  term_pos->setPoint(x,y);
}

//----------------------------------------------------------------------
bool FVTerm::hideCursor (bool on)
{
  // Hides or shows the input cursor on the terminal
  if ( on == hidden_cursor )
    return hidden_cursor;

  if ( on )
  {
    char* hide_str = disableCursor();

    if ( hide_str )
    {
      appendOutputBuffer (hide_str);
      hidden_cursor = true;  // global
    }
  }
  else
  {
    char* show_str = enableCursor();

    if ( show_str )
    {
      appendOutputBuffer (show_str);
      hidden_cursor = false;
    }
  }

  flush_out();

#if defined(__linux__)
  if ( ! hidden_cursor && isLinuxTerm() )
    setLinuxConsoleCursorStyle (getLinuxConsoleCursorStyle(), false);
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
  if ( ! hidden_cursor )
    setFreeBSDConsoleCursorStyle (getFreeBSDConsoleCursorStyle(), false);
#endif

  return hidden_cursor;
}

//----------------------------------------------------------------------
void FVTerm::setPrintCursor (register int x, register int y)
{
  term_area* win = getPrintArea();

  if ( win )
  {
    win->cursor_x = x - win->offset_left;
    win->cursor_y = y - win->offset_top;
  }
}

//----------------------------------------------------------------------
void FVTerm::clearArea (int fillchar)
{
  clearArea (vwin, fillchar);
}

//----------------------------------------------------------------------
void FVTerm::createVTerm (const FRect& r)
{
  // initialize virtual terminal
  const FPoint shadow(0,0);
  createArea (r, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::createVTerm (int width, int height)
{
  // initialize virtual terminal
  createArea (0, 0, width, height, 0, 0, vterm);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (const FRect& r)
{
  const FPoint shadow(0,0);
  resizeArea (r, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (int width, int height)
{
  resizeArea (0, 0, width, height, 0, 0, vterm);
}

//----------------------------------------------------------------------
void FVTerm::putVTerm()
{
  for (int i=0; i < vterm->height; i++)
  {
    vterm->changes[i].xmin = 0;
    vterm->changes[i].xmax = uInt(vterm->width - 1);
  }

  updateTerminal();
}

//----------------------------------------------------------------------
void FVTerm::updateTerminal (bool on)
{
  stop_terminal_updates = bool(! on);

  if ( on )
    updateTerminal();
}

//----------------------------------------------------------------------
void FVTerm::updateTerminal()
{
  // Updates pending changes to the terminal

  if ( stop_terminal_updates
      || static_cast<FApplication*>(init_object)->isQuit() )
    return;

  if ( ! force_terminal_update )
  {
    if ( ! terminal_update_complete )
      return;

    if ( isInputDataPending() )
    {
      terminal_update_pending = true;
      return;
    }
  }

  // Update data on VTerm
  updateVTerm();

  for (register uInt y=0; y < uInt(vterm->height); y++)
    updateTerminalLine (y);

  // sets the new input cursor position
  updateTerminalCursor();
}

//----------------------------------------------------------------------
void FVTerm::addPreprocessingHandler ( FVTerm* instance
                                     , FPreprocessingHandler handler )
{
  if ( ! print_area )
    FVTerm::getPrintArea();

  if ( print_area )
  {
    vterm_preprocessing obj = { instance, handler };
    delPreprocessingHandler (instance);
    print_area->preprocessing_call.push_back(obj);
  }
}

//----------------------------------------------------------------------
void FVTerm::delPreprocessingHandler (FVTerm* instance)
{
  if ( ! print_area )
    FVTerm::getPrintArea();

  if ( ! print_area || print_area->preprocessing_call.empty() )
    return;

  FPreprocessing::iterator iter, end;
  iter = print_area->preprocessing_call.begin();

  while ( iter != print_area->preprocessing_call.end() )
  {
    if ( iter->instance == instance )
      iter = print_area->preprocessing_call.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
int FVTerm::printf (const wchar_t* format, ...)
{
  assert ( format != 0 );
  static const int buf_size = 1024;
  wchar_t buffer[buf_size];
  va_list args;

  va_start (args, format);
  std::vswprintf (buffer, buf_size, format, args);
  va_end (args);

  FString str(buffer);
  return print(str);
}

//----------------------------------------------------------------------
int FVTerm::printf (const char* format, ...)
{
  assert ( format != 0 );
  int   len;
  char  buf[512];
  char* buffer;
  va_list args;

  buffer = buf;
  va_start (args, format);
  len = vsnprintf (buffer, sizeof(buf), format, args);
  va_end (args);

  if ( len >= int(sizeof(buf)) )
  {
    buffer = new char[len+1]();
    va_start (args, format);
    vsnprintf (buffer, uLong(len+1), format, args);
    va_end (args);
  }

  FString str(buffer);
  int ret = print(str);

  if ( buffer != buf )
    delete[] buffer;

  return ret;
}

//----------------------------------------------------------------------
int FVTerm::print (const std::wstring& s)
{
  assert ( ! s.empty() );
  return print (FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const std::wstring& s)
{
  assert ( area != 0 );
  assert ( ! s.empty() );
  return print (area, FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (const wchar_t* s)
{
  assert ( s != 0 );
  return print (FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const wchar_t* s)
{
  assert ( area != 0 );
  assert ( s != 0 );
  return print (area, FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (const char* s)
{
  assert ( s != 0 );
  FString str(s);
  return print(str);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const char* s)
{
  assert ( area != 0 );
  assert ( s != 0 );
  FString str(s);
  return print(area, str);
}

//----------------------------------------------------------------------
int FVTerm::print (const std::string& s)
{
  assert ( ! s.empty() );
  return print (FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const std::string& s)
{
  assert ( area != 0 );
  assert ( ! s.empty() );
  return print (area, FString(s));
}

//----------------------------------------------------------------------
int FVTerm::print (const FString& s)
{
  assert ( ! s.isNull() );
  term_area* area = getPrintArea();

  if ( ! area )
  {
    if ( vdesktop )
      area = vdesktop;
    else
      return -1;
  }

  return print (area, s);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const FString& s)
{
  assert ( ! s.isNull() );
  register int len = 0;
  const wchar_t* p;
  uInt tabstop = uInt(getTabstop());

  if ( ! area )
    return -1;

  p = s.wc_str();

  if ( p )
  {
    if ( *p )
      area->has_changes = true;

    while ( *p )
    {
      int width, height, rsh, bsh;
      width  = area->width;
      height = area->height;
      rsh    = area->right_shadow;
      bsh    = area->bottom_shadow;

      switch ( *p )
      {
        case '\n':
          area->cursor_y++;

        case '\r':
          area->cursor_x = 1;
          break;

        case '\t':
          area->cursor_x = short ( uInt(area->cursor_x)
                                 + tabstop
                                 - uInt(area->cursor_x)
                                 + 1
                                 % tabstop );
          break;

        case '\b':
          area->cursor_x--;
          break;

        case '\a':
          beep();
          break;

        default:
        {
          int ax = area->cursor_x - 1;
          int ay = area->cursor_y - 1;

          char_data  nc; // next character
          nc.code          = *p;
          nc.fg_color      = next_attribute.fg_color;
          nc.bg_color      = next_attribute.bg_color;
          nc.bold          = next_attribute.bold;
          nc.dim           = next_attribute.dim;
          nc.italic        = next_attribute.italic;
          nc.underline     = next_attribute.underline;
          nc.blink         = next_attribute.blink;
          nc.reverse       = next_attribute.reverse;
          nc.standout      = next_attribute.standout;
          nc.invisible     = next_attribute.invisible;
          nc.protect       = next_attribute.protect;
          nc.crossed_out   = next_attribute.crossed_out;
          nc.dbl_underline = next_attribute.dbl_underline;
          nc.alt_charset   = next_attribute.alt_charset;
          nc.pc_charset    = next_attribute.pc_charset;
          nc.transparent   = next_attribute.transparent;
          nc.trans_shadow  = next_attribute.trans_shadow;
          nc.inherit_bg    = next_attribute.inherit_bg;
          nc.no_changes    = false;
          nc.printed       = false;

          if ( area
              && area->cursor_x > 0
              && area->cursor_y > 0
              && ax < area->width + area->right_shadow
              && ay < area->height + area->bottom_shadow )
          {
            char_data* ac; // area character
            int line_len = area->width + area->right_shadow;
            ac = &area->text[ay * line_len + ax];

            if ( *ac != nc )  // compare with an overloaded operator
            {
              if (   ( ! ac->transparent  && nc.transparent )
                  || ( ! ac->trans_shadow && nc.trans_shadow )
                  || ( ! ac->inherit_bg   && nc.inherit_bg ) )
              {
                // add one transparent character form line
                area->changes[ay].trans_count++;
              }
              else if (   ( ac->transparent  && ! nc.transparent )
                       || ( ac->trans_shadow && ! nc.trans_shadow )
                       || ( ac->inherit_bg   && ! nc.inherit_bg ) )
              {
                // remove one transparent character from line
                area->changes[ay].trans_count--;
              }

              // copy character to area
              std::memcpy (ac, &nc, sizeof(nc));

              if ( ax < short(area->changes[ay].xmin) )
                area->changes[ay].xmin = uInt(ax);

              if ( ax > short(area->changes[ay].xmax) )
                area->changes[ay].xmax = uInt(ax);
            }
          }

          area->cursor_x++;
        }
      }

      if ( area->cursor_x > width + rsh )
      {
        area->cursor_x = 1;
        area->cursor_y++;
      }

      if ( area->cursor_y > height + bsh )
      {
        area->cursor_y--;
        break;
      }

      p++;
      len++;
    } // end of while
  }

  return len;
}

//----------------------------------------------------------------------
int FVTerm::print (register int c)
{
  term_area* area = getPrintArea();

  if ( ! area )
  {
    if ( vdesktop )
      area = vdesktop;
    else
      return -1;
  }

  return print (area, c);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, register int c)
{
  char_data nc; // next character
  int width, height, rsh, bsh, ax, ay;

  if ( ! area )
    return -1;

  width  = area->width;
  height = area->height;
  rsh    = area->right_shadow;
  bsh    = area->bottom_shadow;
  ax     = area->cursor_x - 1;
  ay     = area->cursor_y - 1;

  nc.code          = c;
  nc.fg_color      = next_attribute.fg_color;
  nc.bg_color      = next_attribute.bg_color;
  nc.bold          = next_attribute.bold;
  nc.dim           = next_attribute.dim;
  nc.italic        = next_attribute.italic;
  nc.underline     = next_attribute.underline;
  nc.blink         = next_attribute.blink;
  nc.reverse       = next_attribute.reverse;
  nc.standout      = next_attribute.standout;
  nc.invisible     = next_attribute.invisible;
  nc.protect       = next_attribute.protect;
  nc.crossed_out   = next_attribute.crossed_out;
  nc.dbl_underline = next_attribute.dbl_underline;
  nc.alt_charset   = next_attribute.alt_charset;
  nc.pc_charset    = next_attribute.pc_charset;
  nc.transparent   = next_attribute.transparent;
  nc.trans_shadow  = next_attribute.trans_shadow;
  nc.inherit_bg    = next_attribute.inherit_bg;
  nc.no_changes    = false;
  nc.printed       = false;

  if ( area->cursor_x > 0
      && area->cursor_y > 0
      && ax < area->width + area->right_shadow
      && ay < area->height + area->bottom_shadow )
  {
    char_data* ac; // area character
    int line_len = area->width + area->right_shadow;
    ac = &area->text[ay * line_len + ax];

    if ( *ac != nc )  // compare with an overloaded operator
    {
      if (   ( ! ac->transparent  && nc.transparent )
          || ( ! ac->trans_shadow && nc.trans_shadow )
          || ( ! ac->inherit_bg   && nc.inherit_bg ) )
      {
        // add one transparent character form line
        area->changes[ay].trans_count++;
      }

      if (   ( ac->transparent  && ! nc.transparent )
          || ( ac->trans_shadow && ! nc.trans_shadow )
          || ( ac->inherit_bg   && ! nc.inherit_bg ) )
      {
        // remove one transparent character from line
        area->changes[ay].trans_count--;
      }

      // copy character to area
      std::memcpy (ac, &nc, sizeof(nc));

      if ( ax < short(area->changes[ay].xmin) )
        area->changes[ay].xmin = uInt(ax);

      if ( ax > short(area->changes[ay].xmax) )
        area->changes[ay].xmax = uInt(ax);
    }
  }

  area->cursor_x++;
  area->has_changes = true;

  if ( area->cursor_x > width + rsh )
  {
    area->cursor_x = 1;
    area->cursor_y++;
  }

  if ( area->cursor_y > height + bsh )
  {
    area->cursor_y--;
    return -1;
  }

  return 1;
}


// protected methods of FVTerm
//----------------------------------------------------------------------
FVTerm::term_area* FVTerm::getPrintArea()
{
  // returns the print area of this object

  if ( print_area )
    return print_area;
  else
  {
    FVTerm* obj = static_cast<FVTerm*>(this);
    FVTerm* p_obj = static_cast<FVTerm*>(obj->getParent());

    while ( ! obj->vwin && ! obj->child_print_area && p_obj )
    {
      obj = p_obj;
      p_obj = static_cast<FVTerm*>(p_obj->getParent());
    }

    if ( obj->vwin )
    {
      print_area = obj->vwin;
      return print_area;
    }
    else if ( obj->child_print_area )
    {
      print_area = obj->child_print_area;
      return print_area;
    }
  }

  return vdesktop;
}

//----------------------------------------------------------------------
bool FVTerm::isChildPrintArea() const
{
  FVTerm* p_obj = static_cast<FVTerm*>(getParent());

  if ( p_obj
      && p_obj->child_print_area
      && p_obj->child_print_area == this->print_area )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FVTerm::setInsertCursorStyle (bool on)
{
  if ( on )
  {
    setXTermCursorStyle(fc::blinking_underline);
    setKDECursor(fc::UnderlineCursor);

#if defined(__linux__)
    setLinuxConsoleCursorStyle (fc::underscore_cursor, isCursorHidden());
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
    setFreeBSDConsoleCursorStyle (fc::destructive_cursor, isCursorHidden());
#endif

    if ( isUrxvtTerminal() )
      setXTermCursorColor("rgb:ffff/ffff/ffff");
  }
  else
  {
    setXTermCursorStyle(fc::steady_block);
    setKDECursor(fc::BlockCursor);

#if defined(__linux__)
    setLinuxConsoleCursorStyle (fc::full_block_cursor, isCursorHidden());
#endif

#if defined(__FreeBSD__) || defined(__DragonFly__)
    setFreeBSDConsoleCursorStyle (fc::normal_cursor, isCursorHidden());
#endif

    if ( isUrxvtTerminal() )
      setXTermCursorColor("rgb:eeee/0000/0000");
  }
}

//----------------------------------------------------------------------
void FVTerm::createArea ( const FRect& r
                        , const FPoint& p
                        , term_area*& area )
{
  createArea ( r.getX()
             , r.getY()
             , r.getWidth()
             , r.getHeight()
             , p.getX()
             , p.getY()
             , area );
}

//----------------------------------------------------------------------
void FVTerm::createArea ( int offset_left, int offset_top
                        , int width, int height
                        , int rsw, int bsh
                        , term_area*& area )
{
  // initialize virtual window

  area = new term_area;
  area->widget = static_cast<FWidget*>(this);
  resizeArea (offset_left, offset_top, width, height, rsw, bsh, area);
}

//----------------------------------------------------------------------
void FVTerm::resizeArea ( const FRect& r
                        , const FPoint& p
                        , term_area* area )
{
  resizeArea ( r.getX()
             , r.getY()
             , r.getWidth()
             , r.getHeight()
             , p.getX()
             , p.getY()
             , area );
}

//----------------------------------------------------------------------
void FVTerm::resizeArea ( int offset_left, int offset_top
                        , int width, int height
                        , int rsw, int bsh
                        , term_area* area )
{
  // Resize the virtual window to a new size.

  assert ( offset_top >= 0 );
  assert ( width > 0 );
  assert ( height > 0 );
  assert ( rsw >= 0 );
  assert ( bsh >= 0 );
  int area_size;
  char_data default_char;
  line_changes unchanged;

  if ( ! area )
    return;

  if ( width == area->width
      && height == area->height
      && rsw == area->right_shadow
      && bsh == area->bottom_shadow )
  {
    if ( offset_left != area->offset_left )
      area->offset_left = offset_left;

    if ( offset_top != area->offset_top )
      area->offset_top = offset_top;

    return;
  }

  area_size = (width+rsw) * (height+bsh);

  if ( area->height + area->bottom_shadow != height + bsh )
  {
    if ( area->changes != 0 )
      delete[] area->changes;

    if ( area->text != 0 )
      delete[] area->text;

    area->changes = new line_changes[height + bsh];
    area->text    = new char_data[area_size];
  }
  else if ( area->width + area->right_shadow != width + rsw )
  {
    if ( area->text != 0 )
      delete[] area->text;

    area->text = new char_data[area_size];
  }
  else
    return;

  area->offset_left = offset_left;
  area->offset_top = offset_top;
  area->width = width;
  area->height = height;
  area->right_shadow = rsw;
  area->bottom_shadow = bsh;
  area->has_changes = false;

  default_char.code          = ' ';
  default_char.fg_color      = fc::Default;
  default_char.bg_color      = fc::Default;
  default_char.bold          = 0;
  default_char.dim           = 0;
  default_char.italic        = 0;
  default_char.underline     = 0;
  default_char.blink         = 0;
  default_char.reverse       = 0;
  default_char.standout      = 0;
  default_char.invisible     = 0;
  default_char.protect       = 0;
  default_char.crossed_out   = 0;
  default_char.dbl_underline = 0;
  default_char.alt_charset   = 0;
  default_char.pc_charset    = 0;
  default_char.transparent   = 0;
  default_char.trans_shadow  = 0;
  default_char.inherit_bg    = 0;
  default_char.no_changes    = 0;
  default_char.printed       = 0;

  std::fill_n (area->text, area_size, default_char);

  unchanged.xmin = uInt(width+rsw);
  unchanged.xmax = 0;
  unchanged.trans_count = 0;

  std::fill_n (area->changes, height+bsh, unchanged);
}

//----------------------------------------------------------------------
void FVTerm::removeArea (term_area*& area)
{
  // remove the virtual window

  if ( area != 0 )
  {
    if ( area->changes != 0 )
    {
      delete[] area->changes;
      area->changes = 0;
    }

    if ( area->text != 0 )
    {
      delete[] area->text;
      area->text = 0;
    }

    delete area;
    area = 0;
  }
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (const FRect& box)
{
  restoreVTerm ( box.getX()
               , box.getY()
               , box.getWidth()
               , box.getHeight() );
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (int x, int y, int w, int h)
{
  char_data* tc;   // terminal character
  char_data* sc;   // shown character
  char_data  s_ch; // shadow character
  char_data  i_ch; // inherit background character
  FWidget*   widget;

  x--;
  y--;

  if ( x < 0 )
    x = 0;

  if ( y < 0 )
    y = 0;

  if ( w < 0 || h < 0 )
    return;

  if ( x+w > vterm->width )
    w = vterm->width - x;

  if ( w < 0 )
    return;

  if ( y+h > vterm->height )
    h = vterm->height - y;

  if ( h < 0 )
    return;

  widget = static_cast<FWidget*>(vterm->widget);

  for (register int ty=0; ty < h; ty++)
  {
    for (register int tx=0; tx < w; tx++)
    {
      tc = &vterm->text[(y+ty) * vterm->width + (x+tx)];
      sc = &vdesktop->text[(y+ty) * vdesktop->width + (x+tx)];

      if ( widget->window_list && ! widget->window_list->empty() )
      {
        FWidget::widgetList::const_iterator iter, end;
        iter = widget->window_list->begin();
        end  = widget->window_list->end();

        for (; iter != end; ++iter)
        {
          term_area* win = (*iter)->getVWin();

          if ( ! win )
            continue;

          if ( ! win->visible )
            continue;

          int win_x = win->offset_left;
          int win_y = win->offset_top;
          FRect geometry ( win_x
                         , win_y
                         , win->width + win->right_shadow
                         , win->height + win->bottom_shadow );

          // window visible and contains current character
          if ( geometry.contains(tx+x, ty+y) )
          {
            char_data* tmp;
            int line_len = win->width + win->right_shadow;
            tmp = &win->text[(ty+y-win_y) * line_len + (tx+x-win_x)];

            if ( ! tmp->transparent )   // current character not transparent
            {
              if ( tmp->trans_shadow )  // transparent shadow
              {
                // keep the current vterm character
                std::memcpy (&s_ch, sc, sizeof(char_data));
                s_ch.fg_color = tmp->fg_color;
                s_ch.bg_color = tmp->bg_color;
                s_ch.reverse  = false;
                s_ch.standout = false;

                if ( s_ch.code == fc::LowerHalfBlock
                    || s_ch.code == fc::UpperHalfBlock
                    || s_ch.code == fc::LeftHalfBlock
                    || s_ch.code == fc::RightHalfBlock
                    || s_ch.code == fc::MediumShade
                    || s_ch.code == fc::FullBlock )
                  s_ch.code = ' ';

                sc = &s_ch;
              }
              else if ( tmp->inherit_bg )
              {
                // add the covered background to this character
                std::memcpy (&i_ch, tmp, sizeof(char_data));
                i_ch.bg_color = sc->bg_color;  // last background color;
                sc = &i_ch;
              }
              else  // default
                sc = tmp;
            }
          }
        }
      }

      std::memcpy (tc, sc, sizeof(char_data));

      if ( short(vterm->changes[y+ty].xmin) > x )
        vterm->changes[y+ty].xmin = uInt(x);

      if ( short(vterm->changes[y+ty].xmax) < x+w-1 )
        vterm->changes[y+ty].xmax = uInt(x+w-1);
    }
  }
}

//----------------------------------------------------------------------
FVTerm::covered_state FVTerm::isCovered ( const FPoint& pos
                                        , term_area* area )
{
  return isCovered (pos.getX(), pos.getY(), area);
}

//----------------------------------------------------------------------
FVTerm::covered_state FVTerm::isCovered ( int x, int y
                                        , term_area* area )
{
  bool found;
  covered_state is_covered;
  FWidget* w;

  if ( ! area )
    return non_covered;

  is_covered = non_covered;
  found = bool(area == vdesktop);

  w = static_cast<FWidget*>(area->widget);

  if ( w->window_list && ! w->window_list->empty() )
  {
    FWidget::widgetList::const_iterator iter, end;
    iter = w->window_list->begin();
    end  = w->window_list->end();

    for (; iter != end; ++iter)
    {
      term_area* win = (*iter)->getVWin();

      if ( ! win )
        continue;

      if ( ! win->visible )
        continue;

      int win_x = win->offset_left;
      int win_y = win->offset_top;
      FRect geometry ( win_x
                     , win_y
                     , win->width + win->right_shadow
                     , win->height + win->bottom_shadow );

      if ( found && geometry.contains(x,y) )
      {
        char_data* tmp;
        int line_len = win->width + win->right_shadow;
        tmp = &win->text[(y-win_y) * line_len + (x-win_x)];

        if ( tmp->trans_shadow )
        {
          is_covered = half_covered;
        }
        else if ( ! tmp->transparent )
        {
          is_covered = fully_covered;
          break;
        }
      }

      if ( area == win )
        found = true;
    }
  }

  return is_covered;
}

//----------------------------------------------------------------------
void FVTerm::updateVTerm()
{
  // Updates the character data from all areas to VTerm

  if ( vdesktop && vdesktop->has_changes )
  {
    updateVTerm(vdesktop);
    vdesktop->has_changes = false;
  }

  FWidget* widget = static_cast<FWidget*>(vterm->widget);

  if ( ! widget->window_list || widget->window_list->empty() )
    return;

  FWidget::widgetList::const_iterator iter, end;
  iter = widget->window_list->begin();
  end  = widget->window_list->end();

  for (; iter != end; ++iter)
  {
    term_area* win = (*iter)->getVWin();

    if ( ! win )
      continue;

    if ( ! win->visible )
      continue;

    if ( win->has_changes )
    {
      updateVTerm(win);
      win->has_changes = false;
    }
    else if ( ! win->preprocessing_call.empty() )
    {
      FPreprocessing::const_iterator iter2, end2;
      iter2 = win->preprocessing_call.begin();
      end2 = win->preprocessing_call.end();

      while ( iter2 != end2 )
      {
        if ( iter2->instance->child_print_area
            && iter2->instance->child_print_area->has_changes )
        {
          updateVTerm(win);
          iter2->instance->child_print_area->has_changes = false;
          break;
        }

        ++iter2;
      }
    }
  }
}

//----------------------------------------------------------------------
void FVTerm::updateVTerm (term_area* area)
{
  // Update area data on VTerm

  int ax, ay, aw, ah, rsh, bsh, y_end, ol;
  char_data* tc; // terminal character
  char_data* ac; // area character

  if ( ! area )
    return;

  if ( ! area->visible )
    return;

  // Call preprocessing handler
  if ( ! area->preprocessing_call.empty() )
  {
    FPreprocessing::const_iterator iter, end;
    iter = area->preprocessing_call.begin();
    end = area->preprocessing_call.end();

    while ( iter != end )
    {
      FPreprocessingHandler handler = iter->handler;
      // call the preprocessing handler
      (iter->instance->*handler)();
      ++iter;
    }
  }

  ax  = area->offset_left;
  ay  = area->offset_top;
  aw  = area->width;
  ah  = area->height;
  rsh = area->right_shadow;
  bsh = area->bottom_shadow;
  ol  = 0;  // outside left

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  if ( ah + bsh + ay > vterm->height )
    y_end = vterm->height - ay;
  else
    y_end = ah + bsh;

  for (int y=0; y < y_end; y++)  // line loop
  {
    int line_xmin = int(area->changes[y].xmin);
    int line_xmax = int(area->changes[y].xmax);

    if ( line_xmin <= line_xmax )
    {
      int _xmin, _xmax;
      bool modified = false;

      if ( ax == 0 )
        line_xmin = ol;

      if ( aw + rsh + ax - ol >= vterm->width )
        line_xmax = vterm->width + ol - ax - 1;

      if ( ax + line_xmin >= vterm->width )
        continue;

      for (int x=line_xmin; x <= line_xmax; x++)  // column loop
      {
        int gx, gy, line_len;
        covered_state is_covered;
        // global terminal positions
        gx = ax + x;
        gy = ay + y;

        if ( gx < 0 || gy < 0 )
          continue;

        line_len = aw + rsh;
        ac = &area->text[y * line_len + x];
        tc = &vterm->text[gy * vterm->width + gx - ol];

        is_covered = isCovered(gx-ol, gy, area);  // get covered state

        if ( is_covered != fully_covered )
        {
          if ( is_covered == half_covered )
          {
            // add the overlapping color to this character
            char_data ch, oc;
            std::memcpy (&ch, ac, sizeof(char_data));
            oc = getOverlappedCharacter (gx+1 - ol, gy+1, area->widget);
            ch.fg_color = oc.fg_color;
            ch.bg_color = oc.bg_color;
            ch.reverse  = false;
            ch.standout = false;

            if ( ch.code == fc::LowerHalfBlock
                || ch.code == fc::UpperHalfBlock
                || ch.code == fc::LeftHalfBlock
                || ch.code == fc::RightHalfBlock
                || ch.code == fc::MediumShade
                || ch.code == fc::FullBlock )
              ch.code = ' ';

            ch.no_changes = bool(tc->printed && *tc == ch);
            std::memcpy (tc, &ch, sizeof(char_data));
          }
          else if ( ac->transparent )   // transparent
          {
            // restore one character on vterm
            char_data ch;
            ch = getCoveredCharacter (gx+1 - ol, gy+1, area->widget);
            ch.no_changes = bool(tc->printed && *tc == ch);
            std::memcpy (tc, &ch, sizeof(char_data));
          }
          else  // not transparent
          {
            if ( ac->trans_shadow )  // transparent shadow
            {
              // get covered character + add the current color
              char_data ch;
              ch = getCoveredCharacter (gx+1 - ol, gy+1, area->widget);
              ch.fg_color = ac->fg_color;
              ch.bg_color = ac->bg_color;
              ch.reverse  = false;
              ch.standout = false;

              if ( ch.code == fc::LowerHalfBlock
                  || ch.code == fc::UpperHalfBlock
                  || ch.code == fc::LeftHalfBlock
                  || ch.code == fc::RightHalfBlock
                  || ch.code == fc::MediumShade
                  || ch.code == fc::FullBlock )
                ch.code = ' ';

              ch.no_changes = bool(tc->printed && *tc == ch);
              std::memcpy (tc, &ch, sizeof(char_data));
            }
            else if ( ac->inherit_bg )
            {
              // add the covered background to this character
              char_data ch, cc;
              std::memcpy (&ch, ac, sizeof(char_data));
              cc = getCoveredCharacter (gx+1 - ol, gy+1, area->widget);
              ch.bg_color = cc.bg_color;
              ch.no_changes = bool(tc->printed && *tc == ch);
              std::memcpy (tc, &ch, sizeof(char_data));
            }
            else  // default
            {
              if ( tc->printed && *tc == *ac )
              {
                std::memcpy (tc, ac, sizeof(char_data));
                tc->no_changes = true;
              }
              else
              {
                std::memcpy (tc, ac, sizeof(char_data));
                tc->no_changes = false;
              }
            }
          }

          modified = true;
        }
        else if ( ! modified )
          line_xmin++;  // don't update covered character

      }

      _xmin = ax + line_xmin - ol;
      _xmax = ax + line_xmax;

      if ( _xmin < short(vterm->changes[ay+y].xmin) )
        vterm->changes[ay+y].xmin = uInt(_xmin);

      if ( _xmax >= vterm->width )
        _xmax = vterm->width - 1;

      if ( _xmax > short(vterm->changes[ay+y].xmax) )
        vterm->changes[ay+y].xmax = uInt(_xmax);

      area->changes[y].xmin = uInt(aw + rsh);
      area->changes[y].xmax = 0;
    }
  }

  updateVTermCursor(area);
}

//----------------------------------------------------------------------
bool FVTerm::updateVTermCursor (term_area* area)
{
  if ( ! area )
    return false;

  if ( area != active_area )
    return false;

  if ( ! area->visible )
    return false;

  if ( area->input_cursor_visible )
  {
    int cx, cy, ax, ay, x, y;
    // area offset
    ax  = area->offset_left;
    ay  = area->offset_top;
    // area cursor position
    cx = area->input_cursor_x;
    cy = area->input_cursor_y;
    // terminal position
    x  = ax + cx;
    y  = ay + cy;

    if ( isInsideArea(cx, cy, area)
        && isInsideTerminal(x, y)
        && isCovered(x, y, area) == non_covered )
    {
      vterm->input_cursor_x = x;
      vterm->input_cursor_y = y;
      vterm->input_cursor_visible = true;
      return true;
    }
  }

  vterm->input_cursor_visible = false;
  return false;
}

//----------------------------------------------------------------------
bool FVTerm::isInsideArea (int x, int y, term_area* area)
{
  // Check whether the coordinates are within the area
  int ax, ay, aw, ah;
  ax  = 0;
  ay  = 0;
  aw  = area->width;
  ah  = area->height;
  FRect area_geometry(ax, ay, aw, ah);

  if ( area_geometry.contains(x,y) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FVTerm::setAreaCursor ( const FPoint& pos
                           , bool visible
                           , term_area* area )
{
  setAreaCursor (pos.getX(), pos.getY(), visible, area);
}

//----------------------------------------------------------------------
void FVTerm::setAreaCursor ( int x, int y
                           , bool visible
                           , term_area* area )
{
  if ( ! area )
    return;

  area->input_cursor_x = x - 1;
  area->input_cursor_y = y - 1;
  area->input_cursor_visible = visible;
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FPoint& pos, term_area* area)
{
  return getArea (pos.getX(), pos.getY(), area);
}

//----------------------------------------------------------------------
void FVTerm::getArea (int ax, int ay, term_area* area)
{
  // Copies a block from the virtual terminal position to the given area
  int y_end;
  int length;
  char_data* tc; // terminal character
  char_data* ac; // area character

  if ( ! area )
    return;

  ax--;
  ay--;

  if ( area->height+ay > vterm->height )
    y_end = area->height - ay;
  else
    y_end = area->height;

  if ( area->width+ax > vterm->width )
    length = vterm->width - ax;
  else
    length = area->width;

  for (int y=0; y < y_end; y++)  // line loop
  {
    tc = &vterm->text[(ay+y) * vterm->width + ax];
    ac = &area->text[y * area->width];
    std::memcpy (ac, tc, sizeof(char_data) * unsigned(length));

    if ( short(area->changes[y].xmin) > 0 )
      area->changes[y].xmin = 0;

    if ( short(area->changes[y].xmax) < length-1 )
      area->changes[y].xmax = uInt(length-1);
  }
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FRect& box, term_area* area)
{
  getArea ( box.getX()
          , box.getY()
          , box.getWidth()
          , box.getHeight()
          , area );
}

//----------------------------------------------------------------------
void FVTerm::getArea (int x, int y, int w, int h, term_area* area)
{
  // Copies a block from the virtual terminal rectangle to the given area
  int y_end, length, dx, dy;
  char_data* tc; // terminal character
  char_data* ac; // area character

  if ( ! area )
    return;

  dx = x - area->offset_left + 1;
  dy = y - area->offset_top + 1;

  if ( x < 0 || y < 0 )
    return;

  if ( y-1+h > vterm->height )
    y_end = vterm->height - y + 1;
  else
    y_end = h - 1;

  if ( x-1+w > vterm->width )
    length = vterm->width - x + 1;
  else
    length = w;

  if ( length < 1 )
    return;

  for (int _y=0; _y < y_end; _y++)  // line loop
  {
    int line_len = area->width + area->right_shadow;
    tc = &vterm->text[(y+_y-1) * vterm->width + x-1];
    ac = &area->text[(dy+_y) * line_len + dx];
    std::memcpy (ac, tc, sizeof(char_data) * unsigned(length));

    if ( short(area->changes[dy+_y].xmin) > dx )
      area->changes[dy+_y].xmin = uInt(dx);

    if ( short(area->changes[dy+_y].xmax) < dx+length-1 )
      area->changes[dy+_y].xmax = uInt(dx+length-1);
  }
}

//----------------------------------------------------------------------
void FVTerm::putArea (const FPoint& pos, term_area* area)
{
  // Copies the given area block to the virtual terminal position
  if ( ! area )
    return;

  if ( ! area->visible )
    return;

  putArea (pos.getX(), pos.getY(), area);
}

//----------------------------------------------------------------------
void FVTerm::putArea (int ax, int ay, term_area* area)
{
  // Copies the given area block to the virtual terminal position
  int aw, ah, rsh, bsh, y_end, length, ol;
  char_data* tc; // terminal character
  char_data* ac; // area character

  if ( ! area )
    return;

  if ( ! area->visible )
    return;

  ax--;
  ay--;
  aw   = area->width;
  ah   = area->height;
  rsh  = area->right_shadow;
  bsh  = area->bottom_shadow;
  ol   = 0;  // outside left

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  if ( ay + ah + bsh > vterm->height )
    y_end = vterm->height - ay;
  else
    y_end = ah + bsh;

  if ( aw + rsh - ol + ax > vterm->width )
    length = vterm->width - ax;
  else
    length = aw + rsh - ol;

  if ( length < 1 )
    return;

  for (register int y=0; y < y_end; y++)  // line loop
  {
    int line_len = aw + rsh;

    if ( area->changes[y].trans_count == 0 )
    {
      // Line has only covered characters
      tc = &vterm->text[(ay+y) * vterm->width + ax];
      ac = &area->text[y * line_len + ol];
      std::memcpy (tc, ac, sizeof(char_data) * unsigned(length));
    }
    else
    {
      // Line has one or more transparent characters
      for (register int x=0; x < length; x++)  // column loop
      {
        tc = &vterm->text[(ay+y) * vterm->width + (ax+x)];
        ac = &area->text[y * line_len + ol + x];

        if ( ac->transparent )  // transparent
        {
          // restore one character on vterm
          char_data ch;
          ch = getCoveredCharacter (ax+x+1, ay+y+1, area->widget);
          std::memcpy (tc, &ch, sizeof(char_data));
        }
        else  // not transparent
        {
          if ( ac->trans_shadow )  // transparent shadow
          {
            // get covered character + add the current color
            char_data ch;
            ch = getCoveredCharacter (ax+x+1, ay+y+1, area->widget);
            ch.fg_color = ac->fg_color;
            ch.bg_color = ac->bg_color;
            ch.reverse  = false;
            ch.standout = false;

            if ( ch.code == fc::LowerHalfBlock
                || ch.code == fc::UpperHalfBlock
                || ch.code == fc::LeftHalfBlock
                || ch.code == fc::RightHalfBlock
                || ch.code == fc::MediumShade
                || ch.code == fc::FullBlock )
              ch.code = ' ';

            std::memcpy (tc, &ch, sizeof(char_data));
          }
          else if ( ac->inherit_bg )
          {
            // add the covered background to this character
            char_data ch, cc;
            std::memcpy (&ch, ac, sizeof(char_data));
            cc = getCoveredCharacter (ax+x+1, ay+y+1, area->widget);
            ch.bg_color = cc.bg_color;
            std::memcpy (tc, &ch, sizeof(char_data));
          }
          else  // default
            std::memcpy (tc, ac, sizeof(char_data));
        }
      }
    }

    if ( ax < short(vterm->changes[ay+y].xmin) )
      vterm->changes[ay+y].xmin = uInt(ax);

    if ( ax+length-1 > short(vterm->changes[ay+y].xmax) )
      vterm->changes[ay+y].xmax = uInt(ax+length-1);
  }
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaForward (term_area* area)
{
  // Scrolls the entire area up line down
  int total_width;
  int length;
  int y_max;
  char_data  nc; // next character
  char_data* lc; // last character
  char_data* sc; // source character
  char_data* dc; // destination character

  if ( ! area )
    return;

  if ( area->height <= 1 )
    return;

  length = area->width;
  total_width = area->width + area->right_shadow;
  y_max = area->height - 1;

  for (int y=0; y < y_max; y++)
  {
    int pos1 = y * total_width;
    int pos2 = (y+1) * total_width;
    sc = &area->text[pos2];
    dc = &area->text[pos1];
    std::memcpy (dc, sc, sizeof(char_data) * unsigned(length));
    area->changes[y].xmin = 0;
    area->changes[y].xmax = uInt(area->width - 1);
  }

  // insert a new line below
  lc = &area->text[(y_max * total_width) - area->right_shadow - 1];
  std::memcpy (&nc, lc, sizeof(char_data));
  nc.code = ' ';
  dc = &area->text[y_max * total_width];
  std::fill_n (dc, area->width, nc);
  area->changes[y_max].xmin = 0;
  area->changes[y_max].xmax = uInt(area->width - 1);
  area->has_changes = true;

  if ( area == vdesktop )
  {
    if ( tcap[fc::t_scroll_forward].string  )
    {
      setTermXY (0, vdesktop->height);
      scrollTermForward();
      putArea (1, 1, vdesktop);

      // avoid update lines from 0 to (y_max-1)
      for (int y=0; y < y_max; y++)
      {
        area->changes[y].xmin = uInt(area->width - 1);
        area->changes[y].xmax = 0;
      }
    }
  }
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaReverse (term_area* area)
{
  // Scrolls the entire area one line down
  int total_width;
  int length;
  int y_max;
  char_data  nc; // next character
  char_data* lc; // last character
  char_data* sc; // source character
  char_data* dc; // destination character

  if ( ! area )
    return;

  if ( area->height <= 1 )
    return;

  length = area->width;
  total_width = area->width + area->right_shadow;
  y_max = area->height - 1;

  for (int y=y_max; y > 0; y--)
  {
    int pos1 = (y-1) * total_width;
    int pos2 = y * total_width;
    sc = &area->text[pos1];
    dc = &area->text[pos2];
    std::memcpy (dc, sc, sizeof(char_data) * unsigned(length));
    area->changes[y].xmin = 0;
    area->changes[y].xmax = uInt(area->width - 1);
  }

  // insert a new line above
  lc = &area->text[total_width];
  std::memcpy (&nc, lc, sizeof(char_data));
  nc.code = ' ';
  dc = &area->text[0];
  std::fill_n (dc, area->width, nc);
  area->changes[0].xmin = 0;
  area->changes[0].xmax = uInt(area->width - 1);
  area->has_changes = true;

  if ( area == vdesktop )
  {
    if ( tcap[fc::t_scroll_reverse].string  )
    {
      setTermXY (0, 0);
      scrollTermReverse();
      putArea (1, 1, vdesktop);

      // avoid update lines from 1 to y_max
      for (int y=1; y <= y_max; y++)
      {
        area->changes[y].xmin = uInt(area->width - 1);
        area->changes[y].xmax = 0;
      }
    }
  }
}

//----------------------------------------------------------------------
void FVTerm::clearArea (term_area* area, int fillchar)
{
  // clear the area with the current attributes
  char_data nc;  // next character
  int  total_width;
  uInt w;

  // current attributes with a space character
  std::memcpy (&nc, &next_attribute, sizeof(char_data));
  nc.code = fillchar;

  if ( ! (area && area->text) )
    return;

  total_width = area->width + area->right_shadow;
  w = uInt(total_width);

  if ( area->right_shadow == 0 )
  {
    int area_size = area->width * area->height;
    std::fill_n (area->text, area_size, nc);

    if ( area == vdesktop )
    {
      if ( clearTerm (fillchar) )
      {
        nc.printed = true;
        std::fill_n (vterm->text, area_size, nc);
      }
      else
      {
        for (int i=0; i < vdesktop->height; i++)
        {
          vdesktop->changes[i].xmin = 0;
          vdesktop->changes[i].xmax = uInt(vdesktop->width) - 1;
          vdesktop->changes[i].trans_count = 0;
        }

        vdesktop->has_changes = true;
      }

      return;
    }
  }
  else
  {
    char_data t_char = nc;
    t_char.transparent = true;

    for (int y=0; y < area->height; y++)
    {
      int pos = y * total_width;
      // area
      std::fill_n (&area->text[pos], total_width, nc);
      // right shadow
      std::fill_n (&area->text[pos+area->width], area->right_shadow, t_char);
    }

    // bottom shadow
    for (int y=0; y < area->bottom_shadow; y++)
    {
      int pos = total_width * (y + area->height);
      std::fill_n (&area->text[pos], total_width, t_char);
    }
  }

  for (int i=0; i < area->height; i++)
  {
    area->changes[i].xmin = 0;
    area->changes[i].xmax = w - 1;

    if ( nc.transparent
        || nc.trans_shadow
        || nc.inherit_bg )
      area->changes[i].trans_count = w;
    else if ( area->right_shadow != 0 )
      area->changes[i].trans_count = uInt(area->right_shadow);
    else
      area->changes[i].trans_count = 0;
  }

  for (int i=0; i < area->bottom_shadow; i++)
  {
    int y = area->height + i;
    area->changes[y].xmin = 0;
    area->changes[y].xmax = w - 1;
    area->changes[y].trans_count = w;
  }

  area->has_changes = true;
}

//----------------------------------------------------------------------
FVTerm::char_data FVTerm::getCharacter ( character_type type
                                       , const FPoint& pos
                                       , FVTerm* obj )
{
  // Gets the overlapped or the covered character for a given position
  return getCharacter (type, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
FVTerm::char_data FVTerm::getCharacter ( character_type char_type
                                       , int x
                                       , int y
                                       , FVTerm* obj )
{
  // Gets the overlapped or the covered character for the position (x,y)
  int xx, yy;
  char_data* cc;   // covered character
  char_data  s_ch; // shadow character
  char_data  i_ch; // inherit background character
  FWidget*   w;

  x--;
  y--;
  xx = x;
  yy = y;

  if ( xx < 0 )
    xx = 0;

  if ( yy < 0 )
    yy = 0;

  if ( xx >= vterm->width )
    xx = vterm->width - 1;

  if ( yy >= vterm->height )
    yy = vterm->height - 1;

  cc = &vdesktop->text[yy * vdesktop->width + xx];
  w = static_cast<FWidget*>(obj);

  if ( w->window_list && ! w->window_list->empty() )
  {
    FWidget::widgetList::const_iterator iter, end;
    // get the window layer of this object
    int layer = FWindow::getWindowLayer(w);
    iter = w->window_list->begin();
    end  = w->window_list->end();

    for (; iter != end; ++iter)
    {
      bool significant_char;

      // char_type can be "overlapped_character"
      // or "covered_character"
      if ( char_type == covered_character )
        significant_char = bool(layer >= FWindow::getWindowLayer(*iter));
      else
        significant_char = bool(layer < FWindow::getWindowLayer(*iter));

      if ( obj && *iter != obj && significant_char )
      {
        term_area* win = (*iter)->getVWin();

        if ( ! win )
          continue;

        if ( ! win->visible )
          continue;

        int win_x = win->offset_left;
        int win_y = win->offset_top;
        FRect geometry ( win_x
                       , win_y
                       , win->width + win->right_shadow
                       , win->height + win->bottom_shadow );

        // window visible and contains current character
        if ( geometry.contains(x,y) )
        {
          char_data* tmp;
          int line_len = win->width + win->right_shadow;
          tmp = &win->text[(y-win_y) * line_len + (x-win_x)];

          // current character not transparent
          if ( ! tmp->transparent )
          {
            if ( tmp->trans_shadow )  // transparent shadow
            {
              // keep the current vterm character
              std::memcpy (&s_ch, cc, sizeof(char_data));
              s_ch.fg_color = tmp->fg_color;
              s_ch.bg_color = tmp->bg_color;
              s_ch.reverse  = false;
              s_ch.standout = false;
              cc = &s_ch;
            }
            else if ( tmp->inherit_bg )
            {
              // add the covered background to this character
              std::memcpy (&i_ch, tmp, sizeof(char_data));
              i_ch.bg_color = cc->bg_color;  // last background color
              cc = &i_ch;
            }
            else  // default
              cc = tmp;
          }
        }
      }
      else if ( char_type == covered_character )
        break;
    }
  }

  return *cc;
}

//----------------------------------------------------------------------
FVTerm::char_data FVTerm::getCoveredCharacter ( const FPoint& pos
                                              , FVTerm* obj )
{
  // Gets the covered character for a given position
  return getCharacter (covered_character, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
FVTerm::char_data FVTerm::getCoveredCharacter ( int x
                                              , int y
                                              , FVTerm* obj)
{
  // Gets the covered character for the position (x,y)
  return getCharacter (covered_character, x, y, obj);
}

//----------------------------------------------------------------------
FVTerm::char_data FVTerm::getOverlappedCharacter ( const FPoint& pos
                                                 , FVTerm* obj )
{
  // Gets the overlapped character for a given position
  return getCharacter (overlapped_character, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
FVTerm::char_data FVTerm::getOverlappedCharacter ( int x
                                                 , int y
                                                 , FVTerm* obj)
{
  // Gets the overlapped character for the position (x,y)
  return getCharacter (overlapped_character, x, y, obj);
}

//----------------------------------------------------------------------
void FVTerm::processTerminalUpdate()
{
  // Retains terminal updates if there are unprocessed inputs
  static const int max_skip = 8;

  if ( ! terminal_update_pending )
    return;

  if ( ! unprocessedInput() )
  {
    updateTerminal();
    terminal_update_pending = false;
    skipped_terminal_update = 0;
  }
  else if ( skipped_terminal_update > max_skip )
  {
    force_terminal_update = true;
    updateTerminal();
    force_terminal_update = false;
    terminal_update_pending = false;
    skipped_terminal_update = 0;
  }
  else
    skipped_terminal_update++;
}

//----------------------------------------------------------------------
void FVTerm::startTerminalUpdate()
{
  // Pauses the terminal updates for the printing phase
  terminal_update_complete = false;
}

//----------------------------------------------------------------------
void FVTerm::finishTerminalUpdate()
{
  // After the printing phase is completed, the terminal will be updated
  terminal_update_complete = true;
}

//----------------------------------------------------------------------
void FVTerm::flush_out()
{
  while ( ! output_buffer->empty() )
  {
    Fputchar (output_buffer->front());
    output_buffer->pop();
  }

  std::fflush(stdout);
}


// private methods of FVTerm
//----------------------------------------------------------------------
void FVTerm::init()
{
  init_object   = this;
  vterm         =  0;
  vdesktop      =  0;
  term_pos      = new FPoint(-1,-1);
  output_buffer = new std::queue<int>;

  // Preset to false
  hidden_cursor           = \
  terminal_update_pending = \
  force_terminal_update   = \
  stop_terminal_updates   = false;

  // term_attribute stores the current state of the terminal
  term_attribute.code          = '\0';
  term_attribute.fg_color      = fc::Default;
  term_attribute.bg_color      = fc::Default;
  term_attribute.bold          = \
  term_attribute.dim           = \
  term_attribute.italic        = \
  term_attribute.underline     = \
  term_attribute.blink         = \
  term_attribute.reverse       = \
  term_attribute.standout      = \
  term_attribute.invisible     = \
  term_attribute.protect       = \
  term_attribute.crossed_out   = \
  term_attribute.dbl_underline = \
  term_attribute.alt_charset   = \
  term_attribute.pc_charset    = \
  term_attribute.transparent   = \
  term_attribute.trans_shadow  = \
  term_attribute.inherit_bg    = \
  term_attribute.no_changes    = \
  term_attribute.printed       = false;

  // next_attribute contains the state of the next printed character
  std::memcpy (&next_attribute, &term_attribute, sizeof(char_data));

  // receive the terminal capabilities
  tcap = FTermcap().getTermcapMap();

  // create virtual terminal
  FRect term_geometry (0, 0, getColumnNumber(), getLineNumber());
  createVTerm (term_geometry);

  // create virtual desktop area
  FPoint shadow_size(0,0);
  createArea (term_geometry, shadow_size, vdesktop);
  vdesktop->visible = true;
  active_area = vdesktop;

  // Hide the input cursor
  hideCursor();
}

//----------------------------------------------------------------------
void FVTerm::finish()
{
  // Show the input cursor
  showCursor();

  // Clear the terminal
  setNormal();

  if ( use_alternate_screen )
    clearTerm();

  flush_out();

  if ( output_buffer )
    delete output_buffer;

  // remove virtual terminal + virtual desktop area
  removeArea (vdesktop);
  removeArea (vterm);

  if ( term_pos )
    delete term_pos;
}

//----------------------------------------------------------------------
bool FVTerm::clearTerm (int fillchar)
{
  // Clear the real terminal and put cursor at home
  char*& cl = tcap[fc::t_clear_screen].string;
  char*& cd = tcap[fc::t_clr_eos].string;
  char*& cb = tcap[fc::t_clr_eol].string;
  bool ut = FTermcap::background_color_erase;
  char_data* next = &next_attribute;
  bool normal = isNormal(next);
  appendAttributes(next);

  if ( ! ( (cl || cd || cb) && (normal || ut) )
      || fillchar != ' ' )
  {
    return false;
  }

  if ( cl )
  {
    appendOutputBuffer (cl);
    term_pos->setPoint(0,0);
  }
  else if ( cd )
  {
    setTermXY (0, 0);
    appendOutputBuffer (cd);
    term_pos->setPoint(-1,-1);
  }
  else if ( cb )
  {
    term_pos->setPoint(-1,-1);

    for (int i=0; i < getLineNumber(); i++)
    {
      setTermXY (0, i);
      appendOutputBuffer (cb);
    }

    setTermXY (0,0);
  }

  flush_out();
  return true;
}

//----------------------------------------------------------------------
void FVTerm::updateTerminalLine (uInt y)
{
  // Updates pending changes from line y to the terminal
  term_area* vt = vterm;
  uInt& xmin = vt->changes[y].xmin;
  uInt& xmax = vt->changes[y].xmax;
  int term_width = vt->width - 1;
  int term_height = vt->height - 1;

  if ( xmin <= xmax )
  {
    bool is_eol_clean = false;
    bool draw_leading_ws = false;
    bool draw_tailing_ws = false;
    char*& ce = tcap[fc::t_clr_eol].string;
    char*& cb = tcap[fc::t_clr_bol].string;
    char*& ec = tcap[fc::t_erase_chars].string;
    char*& rp = tcap[fc::t_repeat_char].string;
    bool ut = FTermcap::background_color_erase;
    char_data* first_char = &vt->text[y * uInt(vt->width)];
    char_data* last_char  = &vt->text[(y+1) * uInt(vt->width) - 1];
    char_data* min_char   = &vt->text[y * uInt(vt->width) + xmin];

    // Is the line from xmin to the end of the line blank?
    if ( ce && min_char->code == ' ' )
    {
      uInt beginning_whitespace = 1;
      bool normal = isNormal(min_char);

      for (uInt x=xmin+1; x < uInt(vt->width); x++)
      {
        char_data* ch = &vt->text[y * uInt(vt->width) + x];

        if ( *min_char == *ch )
          beginning_whitespace++;
        else
          break;
      }

      if ( beginning_whitespace == uInt(vt->width) - xmin
          && (ut || normal)
          && clr_eol_length < int(beginning_whitespace) )
        is_eol_clean = true;
    }

    if ( ! is_eol_clean )
    {
      // leading whitespace
      if ( cb && first_char->code == ' ' )
      {
        uInt leading_whitespace = 1;
        bool normal = isNormal(first_char);

        for (uInt x=1; x < uInt(vt->width); x++)
        {
          char_data* ch = &vt->text[y * uInt(vt->width) + x];

          if ( *first_char == *ch )
            leading_whitespace++;
          else
            break;
        }

        if ( leading_whitespace > xmin
            && (ut || normal)
            && clr_bol_length < int(leading_whitespace) )
        {
          draw_leading_ws = true;
          xmin = leading_whitespace - 1;
        }
      }

      // tailing whitespace
      if ( ce && last_char->code == ' ' )
      {
        uInt tailing_whitespace = 1;
        bool normal = isNormal(last_char);

        for (uInt x=uInt(vt->width)-1; x >  0 ; x--)
        {
          char_data* ch = &vt->text[y * uInt(vt->width) + x];

          if ( *last_char == *ch )
            tailing_whitespace++;
          else
            break;
        }

        if ( tailing_whitespace > uInt(vt->width) - xmax
            && (ut || normal)
            && clr_bol_length < int(tailing_whitespace) )
        {
          draw_tailing_ws = true;
          xmax = uInt(vt->width) - tailing_whitespace;
        }
      }
    }

    setTermXY (int(xmin), int(y));

    if ( is_eol_clean )
    {
      appendAttributes (min_char);
      appendOutputBuffer (ce);
      markAsPrinted (xmin, uInt(vt->width - 1), y);
    }
    else
    {
      if ( draw_leading_ws )
      {
        appendAttributes (first_char);
        appendOutputBuffer (cb);
        markAsPrinted (0, xmin, y);
      }

      for (uInt x=xmin; x <= xmax; x++)
      {
        char_data* print_char;
        print_char = &vt->text[y * uInt(vt->width) + x];
        print_char->printed = true;

        // skip character with no changes
       if ( print_char->no_changes )
        {
          uInt count = 1;

          for (uInt i=x+1; i <= xmax; i++)
          {
            char_data* ch = &vt->text[y * uInt(vt->width) + i];

            if ( ch->no_changes )
              count++;
            else
              break;
          }

          if ( count > uInt(cursor_addres_lengths) )
          {
            setTermXY (int(x + count), int(y));
            x = x + count - 1;
            continue;
          }
        }

        // Erase a number of characters to draw simple whitespaces
        if ( ec && print_char->code == ' ' )
        {
          uInt whitespace = 1;
          bool normal = isNormal(print_char);

          for (uInt i=x+1; i <= xmax; i++)
          {
            char_data* ch = &vt->text[y * uInt(vt->width) + i];

            if ( *print_char == *ch )
              whitespace++;
            else
              break;
          }

          if ( whitespace == 1 )
          {
            appendCharacter (print_char);
            markAsPrinted (x, y);
          }
          else
          {
            uInt start_pos = x;

            if ( whitespace > uInt(erase_ch_length) + uInt(cursor_addres_lengths)
                && (ut || normal) )
            {
              appendAttributes (print_char);
              appendOutputBuffer (tparm(ec, whitespace, 0, 0, 0, 0, 0, 0, 0, 0));

              if ( x + whitespace - 1 < xmax || draw_tailing_ws )
                setTermXY (int(x + whitespace), int(y));
              else
                break;

              x = x + whitespace - 1;
            }
            else
            {
              x--;

              for (uInt i=0; i < whitespace; i++, x++)
                appendCharacter (print_char);
            }

            markAsPrinted (start_pos, x, y);
          }
        }
        else if ( rp )  // Repeat one character n-fold
        {
          uInt repetitions = 1;

          for (uInt i=x+1; i <= xmax; i++)
          {
            char_data* ch = &vt->text[y * uInt(vt->width) + i];

            if ( *print_char == *ch )
              repetitions++;
            else
              break;
          }

          if ( repetitions == 1 )
          {
            appendCharacter (print_char);
            markAsPrinted (x, y);
          }
          else
          {
            uInt start_pos = x;

            if ( repetitions > uInt(repeat_char_length)
                && print_char->code < 128 )
            {
              newFontChanges (print_char);
              charsetChanges (print_char);
              appendAttributes (print_char);
              appendOutputBuffer (tparm(rp, print_char->code, repetitions, 0, 0, 0, 0, 0, 0, 0));
              term_pos->x_ref() += short(repetitions);
              x = x + repetitions - 1;
            }
            else
            {
              x--;

              for (uInt i=0; i < repetitions; i++, x++)
                appendCharacter (print_char);
            }

            markAsPrinted (start_pos, x, y);
          }
        }
        else  // General character output
        {
          appendCharacter (print_char);
          markAsPrinted (x, y);
        }
      }

      if ( draw_tailing_ws )
      {
        appendAttributes (last_char);
        appendOutputBuffer (ce);
        markAsPrinted (xmax+1, uInt(vt->width - 1), y);
      }
    }

    // Reset line changes
    xmin = uInt(vt->width);
    xmax = 0;
  }

  // cursor wrap
  if ( term_pos->getX() > term_width )
  {
    if ( term_pos->getY() == term_height )
      term_pos->x_ref()--;
    else
    {
      if ( FTermcap::eat_nl_glitch )
      {
        term_pos->setPoint(-1,-1);
      }
      else if ( FTermcap::automatic_right_margin )
      {
        term_pos->setX(0);
        term_pos->y_ref()++;
      }
      else
        term_pos->x_ref()--;
    }
  }
}

//----------------------------------------------------------------------
bool FVTerm::updateTerminalCursor()
{
  // Updates the input cursor visibility and the position
  if ( vterm && vterm->input_cursor_visible )
  {
    int x = vterm->input_cursor_x;
    int y = vterm->input_cursor_y;

    if ( isInsideTerminal(x, y) )
    {
      setTermXY (x,y);
      showCursor();
      return true;
    }
  }
  else
    hideCursor();

  return false;
}

//----------------------------------------------------------------------
bool FVTerm::isInsideTerminal (int x, int y)
{
  // Check whether the coordinates are within the virtual terminal
  FRect term_geometry (0, 0, getColumnNumber(), getLineNumber());

  if ( term_geometry.contains(x,y) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline void FVTerm::markAsPrinted (uInt pos, uInt line)
{
  // Marks a character as printed

  vterm->text[line * uInt(vterm->width) + pos].printed = true;
}

//----------------------------------------------------------------------
inline void FVTerm::markAsPrinted (uInt from, uInt to, uInt line)
{
  // Marks characters in the specified range [from .. to] as printed

  for (uInt x=from; x <= to; x++)
    vterm->text[line * uInt(vterm->width) + x].printed = true;
}

//----------------------------------------------------------------------
inline void FVTerm::newFontChanges (char_data*& next_char)
{
  // NewFont special cases
  if ( NewFont )
  {
    switch ( next_char->code )
    {
      case fc::LowerHalfBlock:
        next_char->code = fc::UpperHalfBlock;
        // fall through
      case fc::NF_rev_left_arrow2:
      case fc::NF_rev_right_arrow2:
      case fc::NF_rev_border_corner_upper_right:
      case fc::NF_rev_border_line_right:
      case fc::NF_rev_border_line_vertical_left:
      case fc::NF_rev_border_corner_lower_right:
      case fc::NF_rev_up_arrow2:
      case fc::NF_rev_down_arrow2:
      case fc::NF_rev_up_arrow1:
      case fc::NF_rev_down_arrow1:
      case fc::NF_rev_left_arrow1:
      case fc::NF_rev_right_arrow1:
      case fc::NF_rev_menu_button1:
      case fc::NF_rev_menu_button2:
      case fc::NF_rev_up_pointing_triangle1:
      case fc::NF_rev_down_pointing_triangle1:
      case fc::NF_rev_up_pointing_triangle2:
      case fc::NF_rev_down_pointing_triangle2:
      case fc::NF_rev_menu_button3:
      case fc::NF_rev_border_line_right_and_left:
        // swap foreground and background color
        std::swap (next_char->fg_color, next_char->bg_color);
        break;

      default:
        break;
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::charsetChanges (char_data*& next_char)
{
  if ( Encoding == fc::UTF8 )
    return;

  uInt code = uInt(next_char->code);
  uInt ch_enc = charEncode(code);

  if ( ch_enc != code )
  {
    if ( ch_enc == 0 )
    {
      next_char->code = int(charEncode(code, fc::ASCII));
      return;
    }

    next_char->code = int(ch_enc);

    if ( Encoding == fc::VT100 )
      next_char->alt_charset = true;
    else if ( Encoding == fc::PC )
    {
      next_char->pc_charset = true;

      if ( isXTerminal() && hasUTF8() && ch_enc < 0x20 )  // Character 0x00..0x1f
        next_char->code = int(charEncode(code, fc::ASCII));
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::appendCharacter (char_data*& next_char)
{
  int term_width = vterm->width - 1;
  int term_height = vterm->height - 1;

  if ( term_pos->getX() == term_width
      && term_pos->getY() == term_height )
    appendLowerRight (next_char);
  else
    appendChar (next_char);

  term_pos->x_ref()++;
}

//----------------------------------------------------------------------
inline void FVTerm::appendChar (char_data*& next_char)
{
  newFontChanges (next_char);
  charsetChanges (next_char);

  appendAttributes (next_char);
  appendOutputBuffer (next_char->code);
}

//----------------------------------------------------------------------
inline void FVTerm::appendAttributes (char_data*& next_attr)
{
  char* attr_str;
  char_data* term_attr = &term_attribute;

  // generate attribute string for the next character
  attr_str = changeAttribute (term_attr, next_attr);

  if ( attr_str )
    appendOutputBuffer (attr_str);
}

//----------------------------------------------------------------------
int FVTerm::appendLowerRight (char_data*& screen_char)
{
  char* SA = tcap[fc::t_enter_am_mode].string;
  char* RA = tcap[fc::t_exit_am_mode].string;

  if ( ! FTermcap::automatic_right_margin )
  {
    appendChar (screen_char);
  }
  else if ( SA && RA )
  {
    appendOutputBuffer (RA);
    appendChar (screen_char);
    appendOutputBuffer (SA);
  }
  else
  {
    int x, y;
    char* IC = tcap[fc::t_parm_ich].string;
    char* im = tcap[fc::t_enter_insert_mode].string;
    char* ei = tcap[fc::t_exit_insert_mode].string;
    char* ip = tcap[fc::t_insert_padding].string;
    char* ic = tcap[fc::t_insert_character].string;

    x = getColumnNumber() - 2;
    y = getLineNumber() - 1;
    setTermXY (x, y);
    appendChar (screen_char);
    term_pos->x_ref()++;

    setTermXY (x, y);
    screen_char--;

    if ( IC )
    {
      appendOutputBuffer (tparm(IC, 1, 0, 0, 0, 0, 0, 0, 0, 0));
      appendChar (screen_char);
    }
    else if ( im && ei )
    {
      appendOutputBuffer (im);
      appendChar (screen_char);

      if ( ip )
        appendOutputBuffer (ip);

      appendOutputBuffer (ei);
    }
    else if ( ic )
    {
      appendOutputBuffer (ic);
      appendChar (screen_char);

      if ( ip )
        appendOutputBuffer (ip);
    }
  }

  return screen_char->code;
}

//----------------------------------------------------------------------
inline void FVTerm::appendOutputBuffer (std::string& s)
{
  const char* const& c_string = s.c_str();
  tputs (c_string, 1, appendOutputBuffer);
}

//----------------------------------------------------------------------
inline void FVTerm::appendOutputBuffer (const char* const& s)
{
  tputs (s, 1, appendOutputBuffer);
}

//----------------------------------------------------------------------
int FVTerm::appendOutputBuffer (int ch)
{
  output_buffer->push(ch);

  if ( output_buffer->size() >= TERMINAL_OUTPUT_BUFFER_SIZE )
    flush_out();

  return ch;
}
