/***********************************************************************
* fvterm.cpp - Virtual terminal implementation                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2018 Markus Gans                                      *
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

#include <queue>
#include <string>
#include <vector>

#include "final/fapplication.h"
#include "final/fterm.h"
#include "final/fvterm.h"
#include "final/fwidget.h"
#include "final/fwindow.h"

namespace finalcut
{

// global FVTerm object
static FVTerm* init_object = 0;

// static class attributes
bool                 FVTerm::terminal_update_complete;
bool                 FVTerm::terminal_update_pending;
bool                 FVTerm::force_terminal_update;
bool                 FVTerm::stop_terminal_updates;
int                  FVTerm::skipped_terminal_update = 0;
uInt                 FVTerm::erase_char_length;
uInt                 FVTerm::repeat_char_length;
uInt                 FVTerm::clr_bol_length;
uInt                 FVTerm::clr_eol_length;
uInt                 FVTerm::cursor_address_length;
std::queue<int>*     FVTerm::output_buffer = 0;
FPoint*              FVTerm::term_pos      = 0;
FTerm*               FVTerm::fterm         = 0;
FVTerm::term_area*   FVTerm::vterm         = 0;
FVTerm::term_area*   FVTerm::vdesktop      = 0;
FVTerm::term_area*   FVTerm::active_area   = 0;
FTermcap::tcap_map*  FVTerm::tcap          = 0;
FKeyboard*           FVTerm::keyboard      = 0;
FVTerm::charData     FVTerm::term_attribute;
FVTerm::charData     FVTerm::next_attribute;
FVTerm::charData     FVTerm::s_ch;
FVTerm::charData     FVTerm::i_ch;


//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FVTerm::FVTerm (bool initialize, bool disable_alt_screen)
  : print_area(0)
  , child_print_area(0)
  , vwin(0)
{
  terminal_update_complete = false;

  if ( initialize )
    init (disable_alt_screen);
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

  return FPoint(0, 0);
}

//----------------------------------------------------------------------
void FVTerm::setTermXY (int x, int y)
{
  // Sets the hardware cursor to the given (x,y) position
  int term_x, term_y, term_width, term_height;
  char* move_str;

  if ( term_pos->getX() == x && term_pos->getY() == y )
    return;

  term_width = int(getColumnNumber());
  term_height = int(getLineNumber());

  if ( x >= term_width && term_width > 0 )
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

  move_str = FTerm::moveCursor (term_x, term_y, x, y);

  if ( move_str )
    appendOutputBuffer(move_str);

  flush_out();
  term_pos->setPoint(x, y);
}

//----------------------------------------------------------------------
void FVTerm::hideCursor (bool on)
{
  // Hides or shows the input cursor on the terminal

  char* visibility_str = FTerm::cursorsVisibility (on);

  if ( visibility_str )
    appendOutputBuffer(visibility_str);

  flush_out();
}

//----------------------------------------------------------------------
void FVTerm::setPrintCursor (int x, int y)
{
  term_area* win = getPrintArea();

  if ( win )
  {
    win->cursor_x = x - win->offset_left;
    win->cursor_y = y - win->offset_top;
  }
}

//----------------------------------------------------------------------
FColor FVTerm::rgb2ColorIndex (uInt8 r, uInt8 g, uInt8 b)
{
  // Converts a 24-bit RGB color to a 256-color compatible approximation

  FColor ri = (((r * 5) + 127) / 255) * 36;
  FColor gi = (((g * 5) + 127) / 255) * 6;
  FColor bi = (((b * 5) + 127) / 255);
  return 16 + ri + gi + bi;
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
  const FPoint shadow(0, 0);
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
  const FPoint shadow(0, 0);
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
  for (int i = 0; i < vterm->height; i++)
  {
    vterm->changes[i].xmin = 0;
    vterm->changes[i].xmax = uInt(vterm->width - 1);
  }

  updateTerminal();
}

//----------------------------------------------------------------------
void FVTerm::updateTerminal (terminal_update refresh_state)
{
  switch ( refresh_state )
  {
    case stop_refresh:
      stop_terminal_updates = true;
      break;

    case continue_refresh:
    case start_refresh:
      stop_terminal_updates = false;
  }

  if ( refresh_state == start_refresh )
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

    if ( keyboard->isInputDataPending() )
    {
      terminal_update_pending = true;
      return;
    }
  }

  // Update data on VTerm
  updateVTerm();

  for (uInt y = 0; y < uInt(vterm->height); y++)
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

  FPreprocessing::iterator iter;
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
int FVTerm::printf (const FString format, ...)
{
  static const int BUFSIZE = 4096;
  wchar_t buffer[BUFSIZE];
  va_list args;

  if ( format.isEmpty() )
    return 0;

  va_start (args, format);
  std::vswprintf (buffer, BUFSIZE, format.wc_str(), args);
  va_end (args);

  FString str(buffer);
  return print(str);
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
  const wchar_t* p;
  std::vector<charData> term_string;

  if ( ! area )
    return -1;

  p = s.wc_str();

  if ( p )
  {
    while ( *p )
    {
      charData nc;  // next character
      nc.code         = *p;
      nc.fg_color     = next_attribute.fg_color;
      nc.bg_color     = next_attribute.bg_color;
      nc.attr.byte[0] = next_attribute.attr.byte[0];
      nc.attr.byte[1] = next_attribute.attr.byte[1];
      nc.attr.byte[2] = 0;
      term_string.push_back(nc);
      p++;
    }  // end of while

    return print (area, term_string);
  }

  return 0;
}

//----------------------------------------------------------------------
int FVTerm::print (const std::vector<charData>& term_string)
{
  if ( term_string.empty() )
    return 0;

  term_area* area = getPrintArea();

  if ( ! area )
  {
    if ( vdesktop )
      area = vdesktop;
    else
      return -1;
  }

  return print (area, term_string);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const std::vector<charData>& term_string)
{
  int len = 0;
  std::vector<charData>::const_iterator iter;
  iter = term_string.begin();
  uInt tabstop = uInt(getTabstop());

  if ( ! area )
    return -1;

  if ( term_string.empty() )
    return 0;

  while ( iter != term_string.end() )
  {
    bool printable_character = false;

    switch ( iter->code )
    {
      case '\n':
        area->cursor_y++;
        // fall through
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
        charData nc = *iter;  // next character
        print (area, nc);
        printable_character = true;
      }
    }

    if ( ! printable_character && printWrap(area) )
      break;  // end of area reached

    len++;
    ++iter;
  }

  return len;
}

//----------------------------------------------------------------------
int FVTerm::print (int c)
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
int FVTerm::print (term_area* area, int c)
{
  charData nc;  // next character

  if ( ! area )
    return -1;

  nc.code         = c;
  nc.fg_color     = next_attribute.fg_color;
  nc.bg_color     = next_attribute.bg_color;
  nc.attr.byte[0] = next_attribute.attr.byte[0];
  nc.attr.byte[1] = next_attribute.attr.byte[1];
  nc.attr.byte[2] = 0;

  return print (area, nc);
}

//----------------------------------------------------------------------
int FVTerm::print (charData& term_char)
{
  term_area* area = getPrintArea();

  if ( ! area )
  {
    if ( vdesktop )
      area = vdesktop;
    else
      return -1;
  }

  return print (area, term_char);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, charData& term_char)
{
  charData& nc = term_char;  // next character
  int width, height, rsh, bsh, ax, ay;

  if ( ! area )
    return -1;

  width  = area->width;
  height = area->height;
  rsh    = area->right_shadow;
  bsh    = area->bottom_shadow;
  ax     = area->cursor_x - 1;
  ay     = area->cursor_y - 1;

  if ( area->cursor_x > 0
    && area->cursor_y > 0
    && ax < area->width + area->right_shadow
    && ay < area->height + area->bottom_shadow )
  {
    charData* ac;  // area character
    int line_len = area->width + area->right_shadow;
    ac = &area->text[ay * line_len + ax];

    if ( *ac != nc )  // compare with an overloaded operator
    {
      if ( ( ! ac->attr.bit.transparent  && nc.attr.bit.transparent )
        || ( ! ac->attr.bit.trans_shadow && nc.attr.bit.trans_shadow )
        || ( ! ac->attr.bit.inherit_bg   && nc.attr.bit.inherit_bg ) )
      {
        // add one transparent character form line
        area->changes[ay].trans_count++;
      }

      if ( ( ac->attr.bit.transparent  && ! nc.attr.bit.transparent )
        || ( ac->attr.bit.trans_shadow && ! nc.attr.bit.trans_shadow )
        || ( ac->attr.bit.inherit_bg   && ! nc.attr.bit.inherit_bg ) )
      {
        // remove one transparent character from line
        area->changes[ay].trans_count--;
      }

      // copy character to area
      std::memcpy (ac, &nc, sizeof(*ac));

      if ( ax < short(area->changes[ay].xmin) )
        area->changes[ay].xmin = uInt(ax);

      if ( ax > short(area->changes[ay].xmax) )
        area->changes[ay].xmax = uInt(ax);
    }
  }

  area->cursor_x++;
  area->has_changes = true;

  // Line break at right margin
  if ( area->cursor_x > width + rsh )
  {
    area->cursor_x = 1;
    area->cursor_y++;
  }

  // Prevent up scrolling
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
    if ( vwin )
    {
      print_area = vwin;
      return print_area;
    }
    else if ( child_print_area )
    {
      print_area = child_print_area;
      return print_area;
    }
  }

  return vdesktop;
}

//----------------------------------------------------------------------
void FVTerm::createArea ( const FRect& r
                        , const FPoint& p
                        , term_area*& area )
{
  createArea ( r.getX()
             , r.getY()
             , int(r.getWidth())
             , int(r.getHeight())
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

  try
  {
    area = new term_area;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return;
  }

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
             , int(r.getWidth())
             , int(r.getHeight())
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
  std::size_t area_size;
  bool realloc_success = false;

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

  area_size = std::size_t((width + rsw) * (height + bsh));

  if ( area->height + area->bottom_shadow != height + bsh )
  {
    realloc_success = reallocateTextArea ( area
                                         , std::size_t(height + bsh)
                                         , area_size );
  }
  else if ( area->width + area->right_shadow != width + rsw )
  {
    realloc_success = reallocateTextArea (area, area_size);
  }
  else
    return;

  if ( ! realloc_success )
    return;

  area->offset_left    = offset_left;
  area->offset_top     = offset_top;
  area->width          = width;
  area->height         = height;
  area->right_shadow   = rsw;
  area->bottom_shadow  = bsh;
  area->has_changes    = false;

  setTextToDefault (area, width + rsw, height + bsh);
}

//----------------------------------------------------------------------
inline void FVTerm::setTextToDefault ( term_area* area
                                     , int width
                                     , int height )
{
  charData default_char;
  line_changes unchanged;
  int size = width * height;

  default_char.code         = ' ';
  default_char.fg_color     = fc::Default;
  default_char.bg_color     = fc::Default;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;
  default_char.attr.byte[2] = 0;

  std::fill_n (area->text, size, default_char);

  unchanged.xmin = uInt(width);
  unchanged.xmax = 0;
  unchanged.trans_count = 0;

  std::fill_n (area->changes, height, unchanged);
}

//----------------------------------------------------------------------
inline bool FVTerm::reallocateTextArea ( term_area* area
                                       , std::size_t height
                                       , std::size_t size )
{
  if ( area->changes != 0 )
    delete[] area->changes;

  if ( area->text != 0 )
    delete[] area->text;

  try
  {
    area->changes = new line_changes[height];
    area->text    = new charData[size];
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
inline bool FVTerm::reallocateTextArea (term_area* area, std::size_t size)
{
  if ( area->text != 0 )
    delete[] area->text;

  try
  {
    area->text = new charData[size];
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return false;
  }

  return true;
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
               , int(box.getWidth())
               , int(box.getHeight()) );
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (int x, int y, int w, int h)
{
  charData* tc;  // terminal character
  charData  sc;  // shown character

  x--;
  y--;

  if ( x < 0 )
    x = 0;

  if ( y < 0 )
    y = 0;

  if ( w < 0 || h < 0 )
    return;

  if ( x + w > vterm->width )
    w = vterm->width - x;

  if ( w < 0 )
    return;

  if ( y + h > vterm->height )
    h = vterm->height - y;

  if ( h < 0 )
    return;

  for (int ty = 0; ty < h; ty++)
  {
    int ypos = y + ty;

    for (int tx = 0; tx < w; tx++)
    {
      int xpos = x + tx;
      tc = &vterm->text[ypos * vterm->width + xpos];
      sc = generateCharacter(xpos, ypos);
      std::memcpy (tc, &sc, sizeof(*tc));
    }

    if ( short(vterm->changes[ypos].xmin) > x )
      vterm->changes[ypos].xmin = uInt(x);

    if ( short(vterm->changes[ypos].xmax) < x + w - 1 )
      vterm->changes[ypos].xmax = uInt(x + w - 1);
  }
}

//----------------------------------------------------------------------
FVTerm::covered_state FVTerm::isCovered ( const FPoint& pos
                                        , term_area* area )
{
  // Determines the covered state for the given position

  return isCovered (pos.getX(), pos.getY(), area);
}

//----------------------------------------------------------------------
FVTerm::covered_state FVTerm::isCovered ( int x, int y
                                        , term_area* area )
{
  // Determines the covered state for the given position

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
                     , std::size_t(win->width + win->right_shadow)
                     , std::size_t(win->height + win->bottom_shadow) );

      if ( found && geometry.contains(x, y) )
      {
        charData* tmp;
        int line_len = win->width + win->right_shadow;
        tmp = &win->text[(y - win_y) * line_len + (x - win_x)];

        if ( tmp->attr.bit.trans_shadow )
        {
          is_covered = half_covered;
        }
        else if ( ! tmp->attr.bit.transparent )
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
void FVTerm::updateOverlappedColor ( term_area* area
                                   , int x, int y, int tx, int ty )
{
  // Add the overlapping color to this character

  int& aw  = area->width;
  int& rsh = area->right_shadow;
  int line_len = aw + rsh;
  // Area character
  charData* ac = &area->text[y * line_len + x];
  // Terminal character
  charData* tc = &vterm->text[ty * vterm->width + tx];
  // New character
  charData nc;
  std::memcpy (&nc, ac, sizeof(nc));
  // Overlapped character
  charData oc = getOverlappedCharacter (tx + 1, ty + 1, area->widget);
  nc.fg_color = oc.fg_color;
  nc.bg_color = oc.bg_color;
  nc.attr.bit.reverse  = false;
  nc.attr.bit.standout = false;

  if ( nc.code == fc::LowerHalfBlock
    || nc.code == fc::UpperHalfBlock
    || nc.code == fc::LeftHalfBlock
    || nc.code == fc::RightHalfBlock
    || nc.code == fc::MediumShade
    || nc.code == fc::FullBlock )
    nc.code = ' ';

  nc.attr.bit.no_changes = bool(tc->attr.bit.printed && *tc == nc);
  std::memcpy (tc, &nc, sizeof(*tc));
}

//----------------------------------------------------------------------
void FVTerm::updateOverlappedCharacter (term_area* area, int tx, int ty)
{
  // Restore one character on vterm

  // Terminal character
  charData* tc = &vterm->text[ty * vterm->width + tx];
  // Overlapped character
  charData oc = getCoveredCharacter (tx + 1, ty + 1, area->widget);
  oc.attr.bit.no_changes = bool(tc->attr.bit.printed && *tc == oc);
  std::memcpy (tc, &oc, sizeof(*tc));
}

//----------------------------------------------------------------------
void FVTerm::updateShadedCharacter ( term_area* area
                                   , int x, int y, int tx, int ty )
{
  // Get covered character + add the current color

  int& aw  = area->width;
  int& rsh = area->right_shadow;
  int line_len = aw + rsh;
  // Area character
  charData* ac = &area->text[y * line_len + x];
  // Terminal character
  charData* tc = &vterm->text[ty * vterm->width + tx];
  // Overlapped character
  charData oc = getCoveredCharacter (tx + 1, ty + 1, area->widget);
  oc.fg_color = ac->fg_color;
  oc.bg_color = ac->bg_color;
  oc.attr.bit.reverse  = false;
  oc.attr.bit.standout = false;

  if ( oc.code == fc::LowerHalfBlock
    || oc.code == fc::UpperHalfBlock
    || oc.code == fc::LeftHalfBlock
    || oc.code == fc::RightHalfBlock
    || oc.code == fc::MediumShade
    || oc.code == fc::FullBlock )
    oc.code = ' ';

  oc.attr.bit.no_changes = bool(tc->attr.bit.printed && *tc == oc);
  std::memcpy (tc, &oc, sizeof(*tc));
}

//----------------------------------------------------------------------
void FVTerm::updateInheritBackground ( term_area* area
                                     , int x, int y, int tx, int ty )
{
  // Add the covered background to this character

  int& aw  = area->width;
  int& rsh = area->right_shadow;
  int line_len = aw + rsh;
  // Area character
  charData* ac = &area->text[y * line_len + x];
  // Terminal character
  charData* tc = &vterm->text[ty * vterm->width + tx];
  // New character
  charData nc;
  std::memcpy (&nc, ac, sizeof(nc));
  // Covered character
  charData cc = getCoveredCharacter (tx + 1, ty + 1, area->widget);
  nc.bg_color = cc.bg_color;
  nc.attr.bit.no_changes = bool(tc->attr.bit.printed && *tc == nc);
  std::memcpy (tc, &nc, sizeof(*tc));
}

//----------------------------------------------------------------------
void FVTerm::updateCharacter ( term_area* area
                             , int x, int y, int tx, int ty )
{
  // Copy a area character to the virtual terminal

  int& aw  = area->width;
  int& rsh = area->right_shadow;
  int line_len = aw + rsh;
  // Area character
  charData* ac = &area->text[y * line_len + x];
  // Terminal character
  charData* tc = &vterm->text[ty * vterm->width + tx];
  std::memcpy (tc, ac, sizeof(*tc));

  if ( tc->attr.bit.printed && *tc == *ac )
    tc->attr.bit.no_changes = true;
  else
    tc->attr.bit.no_changes = false;
}

//----------------------------------------------------------------------
bool FVTerm::updateVTermCharacter ( term_area* area
                                  , int x, int y, int tx, int ty )
{
  int& aw  = area->width;
  int& rsh = area->right_shadow;
  int line_len = aw + rsh;
  // Area character
  charData* ac = &area->text[y * line_len + x];

  // Get covered state
  covered_state is_covered = isCovered(tx, ty, area);

  if ( is_covered == fully_covered )
    return false;

  if ( is_covered == half_covered )
  {
    updateOverlappedColor(area, x, y, tx, ty);
  }
  else if ( ac->attr.bit.transparent )   // Transparent
  {
    updateOverlappedCharacter(area, tx, ty);
  }
  else  // Not transparent
  {
    if ( ac->attr.bit.trans_shadow )  // Transparent shadow
    {
      updateShadedCharacter (area, x, y, tx, ty);
    }
    else if ( ac->attr.bit.inherit_bg )
    {
      updateInheritBackground (area, x, y, tx, ty);
    }
    else  // Default
    {
      updateCharacter (area, x, y, tx, ty);
    }
  }

  return true;
}

//----------------------------------------------------------------------
void FVTerm::callPreprocessingHandler (term_area* area)
{
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

  if ( ! area || ! area->visible )
    return;

  int ax  = area->offset_left
    , ay  = area->offset_top
    , aw  = area->width
    , ah  = area->height
    , rsh = area->right_shadow
    , bsh = area->bottom_shadow
    , ol  = 0  // Outside left
    , y_end;

  // Call the processing handler methods
  callPreprocessingHandler(area);

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  if ( ah + bsh + ay > vterm->height )
    y_end = vterm->height - ay;
  else
    y_end = ah + bsh;

  for (int y = 0; y < y_end; y++)  // Line loop
  {
    int _xmin, _xmax;
    bool modified = false;
    int line_xmin = int(area->changes[y].xmin);
    int line_xmax = int(area->changes[y].xmax);

    if ( line_xmin > line_xmax )
      continue;

    if ( ax == 0 )
      line_xmin = ol;

    if ( aw + rsh + ax - ol >= vterm->width )
      line_xmax = vterm->width + ol - ax - 1;

    if ( ax + line_xmin >= vterm->width )
      continue;

    for (int x = line_xmin; x <= line_xmax; x++)  // Column loop
    {
      // Global terminal positions
      int tx = ax + x
        , ty = ay + y;

      if ( tx < 0 || ty < 0 )
        continue;

      tx -= ol;

      if ( updateVTermCharacter (area, x, y, tx, ty) )
        modified = true;

      if ( ! modified )
        line_xmin++;  // Don't update covered character
    }

    _xmin = ax + line_xmin - ol;
    _xmax = ax + line_xmax;

    if ( _xmin < short(vterm->changes[ay + y].xmin) )
      vterm->changes[ay + y].xmin = uInt(_xmin);

    if ( _xmax >= vterm->width )
      _xmax = vterm->width - 1;

    if ( _xmax > short(vterm->changes[ay + y].xmax) )
      vterm->changes[ay + y].xmax = uInt(_xmax);

    area->changes[y].xmin = uInt(aw + rsh);
    area->changes[y].xmax = 0;
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

  int ax = 0, ay = 0;
  std::size_t aw = std::size_t(area->width);
  std::size_t ah = std::size_t(area->height);
  FRect area_geometry(ax, ay, aw, ah);

  if ( area_geometry.contains(x, y) )
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

  if ( ! area )
    return;

  ax--;
  ay--;

  if ( area->height + ay > vterm->height )
    y_end = area->height - ay;
  else
    y_end = area->height;

  if ( area->width + ax > vterm->width )
    length = vterm->width - ax;
  else
    length = area->width;

  for (int y = 0; y < y_end; y++)  // line loop
  {
    charData* tc;  // terminal character
    charData* ac;  // area character
    tc = &vterm->text[(ay + y) * vterm->width + ax];
    ac = &area->text[y * area->width];
    std::memcpy (ac, tc, sizeof(*ac) * unsigned(length));

    if ( short(area->changes[y].xmin) > 0 )
      area->changes[y].xmin = 0;

    if ( short(area->changes[y].xmax) < length - 1 )
      area->changes[y].xmax = uInt(length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FRect& box, term_area* area)
{
  getArea ( box.getX()
          , box.getY()
          , int(box.getWidth())
          , int(box.getHeight())
          , area );
}

//----------------------------------------------------------------------
void FVTerm::getArea (int x, int y, int w, int h, term_area* area)
{
  // Copies a block from the virtual terminal rectangle to the given area

  if ( ! area )
    return;

  int dx = x - area->offset_left + 1
    , dy = y - area->offset_top + 1
    , y_end
    , length;

  if ( x < 0 || y < 0 )
    return;

  if ( y - 1 + h > vterm->height )
    y_end = vterm->height - y + 1;
  else
    y_end = h - 1;

  if ( x - 1 + w > vterm->width )
    length = vterm->width - x + 1;
  else
    length = w;

  if ( length < 1 )
    return;

  for (int _y = 0; _y < y_end; _y++)  // line loop
  {
    charData* tc;  // terminal character
    charData* ac;  // area character
    int line_len = area->width + area->right_shadow;
    tc = &vterm->text[(y + _y - 1) * vterm->width + x - 1];
    ac = &area->text[(dy + _y) * line_len + dx];
    std::memcpy (ac, tc, sizeof(*ac) * unsigned(length));

    if ( short(area->changes[dy + _y].xmin) > dx )
      area->changes[dy + _y].xmin = uInt(dx);

    if ( short(area->changes[dy + _y].xmax) < dx + length - 1 )
      area->changes[dy + _y].xmax = uInt(dx + length - 1);
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

  charData* tc;  // terminal character
  charData* ac;  // area character

  if ( ! area )
    return;

  if ( ! area->visible )
    return;

  ax--;
  ay--;

  int aw  = area->width
    , ah  = area->height
    , rsh = area->right_shadow
    , bsh = area->bottom_shadow
    , ol  = 0  // outside left
    , y_end
    , length;

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

  for (int y = 0; y < y_end; y++)  // line loop
  {
    int line_len = aw + rsh;

    if ( area->changes[y].trans_count == 0 )
    {
      // Line has only covered characters
      ac = &area->text[y * line_len + ol];
      tc = &vterm->text[(ay + y) * vterm->width + ax];
      putAreaLine (ac, tc, length);
    }
    else
    {
      // Line has one or more transparent characters
      for (int x = 0; x < length; x++)  // column loop
      {
        int cx = ax + x;
        int cy = ay + y;
        ac = &area->text[y * line_len + ol + x];
        tc = &vterm->text[cy * vterm->width + cx];
        putAreaCharacter (cx + 1, cy + 1, area->widget, ac, tc);
      }
    }

    if ( ax < short(vterm->changes[ay + y].xmin) )
      vterm->changes[ay + y].xmin = uInt(ax);

    if ( ax + length - 1 > short(vterm->changes[ay + y].xmax) )
      vterm->changes[ay + y].xmax = uInt(ax + length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaForward (term_area* area)
{
  // Scrolls the entire area up line down
  int total_width
    , length
    , y_max;
  charData  nc;  // next character
  charData* lc;  // last character
  charData* dc;  // destination character

  if ( ! area )
    return;

  if ( area->height <= 1 )
    return;

  length = area->width;
  total_width = area->width + area->right_shadow;
  y_max = area->height - 1;

  for (int y = 0; y < y_max; y++)
  {
    charData* sc;  // source character
    int pos1 = y * total_width;
    int pos2 = (y + 1) * total_width;
    sc = &area->text[pos2];
    dc = &area->text[pos1];
    std::memcpy (dc, sc, sizeof(*dc) * unsigned(length));
    area->changes[y].xmin = 0;
    area->changes[y].xmax = uInt(area->width - 1);
  }

  // insert a new line below
  lc = &area->text[(y_max * total_width) - area->right_shadow - 1];
  std::memcpy (&nc, lc, sizeof(nc));
  nc.code = ' ';
  dc = &area->text[y_max * total_width];
  std::fill_n (dc, area->width, nc);
  area->changes[y_max].xmin = 0;
  area->changes[y_max].xmax = uInt(area->width - 1);
  area->has_changes = true;

  if ( area == vdesktop )
  {
    if ( TCAP(fc::t_scroll_forward)  )
    {
      setTermXY (0, vdesktop->height);
      FTerm::scrollTermForward();
      putArea (1, 1, vdesktop);

      // avoid update lines from 0 to (y_max - 1)
      for (int y = 0; y < y_max; y++)
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
  int total_width
    , length
    , y_max;
  charData  nc;  // next character
  charData* lc;  // last character
  charData* dc;  // destination character

  if ( ! area )
    return;

  if ( area->height <= 1 )
    return;

  length = area->width;
  total_width = area->width + area->right_shadow;
  y_max = area->height - 1;

  for (int y = y_max; y > 0; y--)
  {
    charData* sc;  // source character
    int pos1 = (y - 1) * total_width;
    int pos2 = y * total_width;
    sc = &area->text[pos1];
    dc = &area->text[pos2];
    std::memcpy (dc, sc, sizeof(*dc) * unsigned(length));
    area->changes[y].xmin = 0;
    area->changes[y].xmax = uInt(area->width - 1);
  }

  // insert a new line above
  lc = &area->text[total_width];
  std::memcpy (&nc, lc, sizeof(nc));
  nc.code = ' ';
  dc = &area->text[0];
  std::fill_n (dc, area->width, nc);
  area->changes[0].xmin = 0;
  area->changes[0].xmax = uInt(area->width - 1);
  area->has_changes = true;

  if ( area == vdesktop )
  {
    if ( TCAP(fc::t_scroll_reverse)  )
    {
      setTermXY (0, 0);
      FTerm::scrollTermReverse();
      putArea (1, 1, vdesktop);

      // avoid update lines from 1 to y_max
      for (int y = 1; y <= y_max; y++)
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
  // Clear the area with the current attributes

  charData nc;  // next character
  uInt w;

  // Current attributes with a space character
  std::memcpy (&nc, &next_attribute, sizeof(nc));
  nc.code = fillchar;

  if ( ! (area && area->text) )
  {
    clearTerm (fillchar);
    return;
  }

  w = uInt(area->width + area->right_shadow);

  if ( area->right_shadow == 0 )
  {
    if ( clearFullArea(area, nc) )
      return;
  }
  else
    clearAreaWithShadow(area, nc);

  for (int i = 0; i < area->height; i++)
  {
    area->changes[i].xmin = 0;
    area->changes[i].xmax = w - 1;

    if ( nc.attr.bit.transparent
      || nc.attr.bit.trans_shadow
      || nc.attr.bit.inherit_bg )
      area->changes[i].trans_count = w;
    else if ( area->right_shadow != 0 )
      area->changes[i].trans_count = uInt(area->right_shadow);
    else
      area->changes[i].trans_count = 0;
  }

  for (int i = 0; i < area->bottom_shadow; i++)
  {
    int y = area->height + i;
    area->changes[y].xmin = 0;
    area->changes[y].xmax = w - 1;
    area->changes[y].trans_count = w;
  }

  area->has_changes = true;
}

//----------------------------------------------------------------------
FVTerm::charData FVTerm::generateCharacter (const FPoint& pos)
{
  // Generates characters for a given position considering all areas
  return generateCharacter (pos.getX(), pos.getY());
}

//----------------------------------------------------------------------
FVTerm::charData FVTerm::generateCharacter (int x, int y)
{
  // Generates characters for a given position considering all areas
  FWidget::widgetList::const_iterator iter, end;
  charData* sc;    // shown character
  FWidget* widget;

  widget = static_cast<FWidget*>(vterm->widget);
  sc = &vdesktop->text[y * vdesktop->width + x];

  if ( ! widget->window_list || widget->window_list->empty() )
    return *sc;

  iter = widget->window_list->begin();
  end  = widget->window_list->end();

  for (; iter != end; ++iter)
  {
    term_area* win = (*iter)->getVWin();

    if ( ! win || ! win->visible )
      continue;

    int win_x = win->offset_left;
    int win_y = win->offset_top;
    FRect geometry ( win_x
                   , win_y
                   , std::size_t(win->width + win->right_shadow)
                   , std::size_t(win->height + win->bottom_shadow) );

    // Window is visible and contains current character
    if ( geometry.contains(x, y) )
    {
      charData* tmp;
      int line_len = win->width + win->right_shadow;
      tmp = &win->text[(y - win_y) * line_len + (x - win_x)];

      if ( ! tmp->attr.bit.transparent )   // Current character not transparent
      {
        if ( tmp->attr.bit.trans_shadow )  // Transparent shadow
        {
          // Keep the current vterm character
          std::memcpy (&s_ch, sc, sizeof(s_ch));
          s_ch.fg_color = tmp->fg_color;
          s_ch.bg_color = tmp->bg_color;
          s_ch.attr.bit.reverse  = false;
          s_ch.attr.bit.standout = false;

          if ( s_ch.code == fc::LowerHalfBlock
            || s_ch.code == fc::UpperHalfBlock
            || s_ch.code == fc::LeftHalfBlock
            || s_ch.code == fc::RightHalfBlock
            || s_ch.code == fc::MediumShade
            || s_ch.code == fc::FullBlock )
            s_ch.code = ' ';

          sc = &s_ch;
        }
        else if ( tmp->attr.bit.inherit_bg )
        {
          // Add the covered background to this character
          std::memcpy (&i_ch, tmp, sizeof(i_ch));
          i_ch.bg_color = sc->bg_color;  // Last background color
          sc = &i_ch;
        }
        else  // Default
          sc = tmp;
      }
    }
  }

  return *sc;
}

//----------------------------------------------------------------------
FVTerm::charData FVTerm::getCharacter ( character_type type
                                       , const FPoint& pos
                                       , FVTerm* obj )
{
  // Gets the overlapped or the covered character for a given position
  return getCharacter (type, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
FVTerm::charData FVTerm::getCharacter ( character_type char_type
                                       , int x
                                       , int y
                                       , FVTerm* obj )
{
  // Gets the overlapped or the covered character for the position (x,y)
  int xx, yy, layer;
  charData* cc;  // covered character
  FWidget* w;
  FWidget::widgetList::const_iterator iter, end;

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

  if ( ! w->window_list || w->window_list->empty() )
    return *cc;

  // Get the window layer of this object
  layer = FWindow::getWindowLayer(w);
  iter  = w->window_list->begin();
  end   = w->window_list->end();

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

      if ( ! win || ! win->visible )
        continue;

      FRect geometry ( win->offset_left
                     , win->offset_top
                     , std::size_t(win->width + win->right_shadow)
                     , std::size_t(win->height + win->bottom_shadow) );

      // Window visible and contains current character
      if ( geometry.contains(x, y) )
        getAreaCharacter (x, y, win, cc);
    }
    else if ( char_type == covered_character )
      break;
  }

  return *cc;
}

//----------------------------------------------------------------------
FVTerm::charData FVTerm::getCoveredCharacter ( const FPoint& pos
                                              , FVTerm* obj )
{
  // Gets the covered character for a given position
  return getCharacter (covered_character, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
FVTerm::charData FVTerm::getCoveredCharacter ( int x
                                              , int y
                                              , FVTerm* obj)
{
  // Gets the covered character for the position (x,y)
  return getCharacter (covered_character, x, y, obj);
}

//----------------------------------------------------------------------
FVTerm::charData FVTerm::getOverlappedCharacter ( const FPoint& pos
                                                 , FVTerm* obj )
{
  // Gets the overlapped character for a given position
  return getCharacter (overlapped_character, pos.getX(), pos.getY(), obj);
}

//----------------------------------------------------------------------
FVTerm::charData FVTerm::getOverlappedCharacter ( int x
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

  if ( ! keyboard->isInputDataPending() )
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
    FTerm::Fputchar(output_buffer->front());
    output_buffer->pop();
  }

  std::fflush(stdout);
}


// private methods of FVTerm
//----------------------------------------------------------------------
void FVTerm::init (bool disable_alt_screen)
{
  init_object = this;
  vterm       = 0;
  vdesktop    = 0;

  try
  {
    fterm         = new FTerm (disable_alt_screen);
    term_pos      = new FPoint(-1, -1);
    output_buffer = new std::queue<int>;
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    std::abort();
  }

  // Preset to false
  terminal_update_pending = \
  force_terminal_update   = \
  stop_terminal_updates   = false;

  // term_attribute stores the current state of the terminal
  term_attribute.code         = '\0';
  term_attribute.fg_color     = fc::Default;
  term_attribute.bg_color     = fc::Default;
  term_attribute.attr.byte[0] = 0;
  term_attribute.attr.byte[0] = 0;
  term_attribute.attr.byte[0] = 0;

  // next_attribute contains the state of the next printed character
  std::memcpy (&next_attribute, &term_attribute, sizeof(next_attribute));

  // Receive the terminal capabilities
  tcap = FTermcap::getTermcapMap();

  // Create virtual terminal
  FRect term_geometry (0, 0, getColumnNumber(), getLineNumber());
  createVTerm (term_geometry);

  // Create virtual desktop area
  FPoint shadow_size(0, 0);
  createArea (term_geometry, shadow_size, vdesktop);
  vdesktop->visible = true;
  active_area = vdesktop;

  // Initialize keyboard
  keyboard = FTerm::getKeyboard();

  // Hide the input cursor
  hideCursor();

  // Initialize character lengths
  init_characterLengths (FTerm::getFOptiMove());
}

//----------------------------------------------------------------------
void FVTerm::init_characterLengths (FOptiMove* optimove)
{
  if ( optimove )
  {
    cursor_address_length = optimove->getCursorAddressLength();
    erase_char_length     = optimove->getEraseCharsLength();
    repeat_char_length    = optimove->getRepeatCharLength();
    clr_bol_length        = optimove->getClrBolLength();
    clr_eol_length        = optimove->getClrEolLength();
  }
  else
  {
    cursor_address_length = INT_MAX;
    erase_char_length     = INT_MAX;
    repeat_char_length    = INT_MAX;
    clr_bol_length        = INT_MAX;
    clr_eol_length        = INT_MAX;
  }
}

//----------------------------------------------------------------------
void FVTerm::finish()
{
  // Show the input cursor
  showCursor();

  // Clear the terminal
  setNormal();

  if ( FTerm::hasAlternateScreen() )
    clearTerm();

  flush_out();

  if ( output_buffer )
    delete output_buffer;

  // remove virtual terminal + virtual desktop area
  removeArea (vdesktop);
  removeArea (vterm);

  if ( term_pos )
    delete term_pos;

  if ( fterm )
    delete fterm;
}

//----------------------------------------------------------------------
void FVTerm::putAreaLine (charData* ac, charData* tc, int length)
{
  // copy "length" characters from area to terminal

  std::memcpy (tc, ac, sizeof(*tc) * unsigned(length));
}

//----------------------------------------------------------------------
void FVTerm::putAreaCharacter ( int x, int y, FVTerm* obj
                              , charData* ac
                              , charData* tc )
{
  if ( ac->attr.bit.transparent )  // Transparent
  {
    // Restore one character on vterm
    charData ch;
    ch = getCoveredCharacter (x, y, obj);
    std::memcpy (tc, &ch, sizeof(*tc));
  }
  else  // Mot transparent
  {
    if ( ac->attr.bit.trans_shadow )  // Transparent shadow
    {
      // Get covered character + add the current color
      charData ch;
      ch = getCoveredCharacter (x, y, obj);
      ch.fg_color = ac->fg_color;
      ch.bg_color = ac->bg_color;
      ch.attr.bit.reverse  = false;
      ch.attr.bit.standout = false;

      if ( ch.code == fc::LowerHalfBlock
        || ch.code == fc::UpperHalfBlock
        || ch.code == fc::LeftHalfBlock
        || ch.code == fc::RightHalfBlock
        || ch.code == fc::MediumShade
        || ch.code == fc::FullBlock )
        ch.code = ' ';

      std::memcpy (tc, &ch, sizeof(*tc));
    }
    else if ( ac->attr.bit.inherit_bg )
    {
      // Add the covered background to this character
      charData ch, cc;
      std::memcpy (&ch, ac, sizeof(ch));
      cc = getCoveredCharacter (x, y, obj);
      ch.bg_color = cc.bg_color;
      std::memcpy (tc, &ch, sizeof(*tc));
    }
    else  // Default
      std::memcpy (tc, ac, sizeof(*tc));
  }
}

//----------------------------------------------------------------------
void FVTerm::getAreaCharacter ( int x, int y, term_area* area
                              , charData*& cc )
{
  charData* tmp;
  int area_x = area->offset_left;
  int area_y = area->offset_top;
  int line_len = area->width + area->right_shadow;
  tmp = &area->text[(y - area_y) * line_len + (x - area_x)];

  // Current character not transparent
  if ( ! tmp->attr.bit.transparent )
  {
    if ( tmp->attr.bit.trans_shadow )  // transparent shadow
    {
      // Keep the current vterm character
      std::memcpy (&s_ch, cc, sizeof(s_ch));
      s_ch.fg_color = tmp->fg_color;
      s_ch.bg_color = tmp->bg_color;
      s_ch.attr.bit.reverse  = false;
      s_ch.attr.bit.standout = false;
      cc = &s_ch;
    }
    else if ( tmp->attr.bit.inherit_bg )
    {
      // Add the covered background to this character
      std::memcpy (&i_ch, tmp, sizeof(i_ch));
      i_ch.bg_color = cc->bg_color;  // last background color
      cc = &i_ch;
    }
    else  // default
      cc = tmp;
  }
}

//----------------------------------------------------------------------
bool FVTerm::clearTerm (int fillchar)
{
  // Clear the real terminal and put cursor at home

  char*& cl = TCAP(fc::t_clear_screen);
  char*& cd = TCAP(fc::t_clr_eos);
  char*& cb = TCAP(fc::t_clr_eol);
  bool ut = FTermcap::background_color_erase;
  charData* next = &next_attribute;
  bool normal = FTerm::isNormal(next);
  appendAttributes(next);

  if ( ! ( (cl || cd || cb) && (normal || ut) )
    || fillchar != ' ' )
  {
    return false;
  }

  if ( cl )  // Clear screen
  {
    appendOutputBuffer (cl);
    term_pos->setPoint(0, 0);
  }
  else if ( cd )  // Clear to end of screen
  {
    setTermXY (0, 0);
    appendOutputBuffer (cd);
    term_pos->setPoint(-1, -1);
  }
  else if ( cb )  // Clear to end of line
  {
    term_pos->setPoint(-1, -1);

    for (int i = 0; i < int(getLineNumber()); i++)
    {
      setTermXY (0, i);
      appendOutputBuffer (cb);
    }

    setTermXY (0, 0);
  }

  flush_out();
  return true;
}

//----------------------------------------------------------------------
bool FVTerm::clearFullArea (term_area* area, charData& nc)
{
  // Clear area
  int area_size = area->width * area->height;
  std::fill_n (area->text, area_size, nc);

  if ( area != vdesktop )  // Is the area identical to the desktop?
    return false;

  // Try to clear the terminal rapidly with a control sequence
  if ( clearTerm (nc.code) )
  {
    nc.attr.bit.printed = true;
    std::fill_n (vterm->text, area_size, nc);
  }
  else
  {
    for (int i = 0; i < vdesktop->height; i++)
    {
      vdesktop->changes[i].xmin = 0;
      vdesktop->changes[i].xmax = uInt(vdesktop->width) - 1;
      vdesktop->changes[i].trans_count = 0;
    }

    vdesktop->has_changes = true;
  }

  return true;
}

//----------------------------------------------------------------------
void FVTerm::clearAreaWithShadow (term_area* area, charData& nc)
{
  charData t_char = nc;
  int total_width = area->width + area->right_shadow;
  t_char.attr.bit.transparent = true;

  for (int y = 0; y < area->height; y++)
  {
    int pos = y * total_width;
    // Clear area
    std::fill_n (&area->text[pos], total_width, nc);
    // Make right shadow transparent
    std::fill_n (&area->text[pos + area->width], area->right_shadow, t_char);
  }

  // Make bottom shadow transparent
  for (int y = 0; y < area->bottom_shadow; y++)
  {
    int pos = total_width * (y + area->height);
    std::fill_n (&area->text[pos], total_width, t_char);
  }
}

//----------------------------------------------------------------------
bool FVTerm::canClearToEOL (uInt xmin, uInt y)
{
  // Is the line from xmin to the end of the line blank?
  // => clear to end of line

  term_area*& vt = vterm;
  char*& ce = TCAP(fc::t_clr_eol);
  charData* min_char = &vt->text[y * uInt(vt->width) + xmin];

  if ( ce && min_char->code == ' ' )
  {
    uInt beginning_whitespace = 1;
    bool normal = FTerm::isNormal(min_char);
    bool& ut = FTermcap::background_color_erase;

    for (uInt x = xmin + 1; x < uInt(vt->width); x++)
    {
      charData* ch = &vt->text[y * uInt(vt->width) + x];

      if ( *min_char == *ch )
        beginning_whitespace++;
      else
        break;
    }

    if ( beginning_whitespace == uInt(vt->width) - xmin
      && (ut || normal)
      && clr_eol_length < beginning_whitespace )
      return true;
  }

  return false;
}

//----------------------------------------------------------------------
bool FVTerm::canClearLeadingWS (uInt& xmin, uInt y)
{
  // Line has leading whitespace
  // => clear from xmin to beginning of line

  term_area*& vt = vterm;
  char*& cb = TCAP(fc::t_clr_bol);
  charData* first_char = &vt->text[y * uInt(vt->width)];

  if ( cb && first_char->code == ' ' )
  {
    uInt leading_whitespace = 1;
    bool normal = FTerm::isNormal(first_char);
    bool& ut = FTermcap::background_color_erase;

    for (uInt x = 1; x < uInt(vt->width); x++)
    {
      charData* ch = &vt->text[y * uInt(vt->width) + x];

      if ( *first_char == *ch )
        leading_whitespace++;
      else
        break;
    }

    if ( leading_whitespace > xmin
      && (ut || normal)
      && clr_bol_length < leading_whitespace )
    {
      xmin = leading_whitespace - 1;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
bool FVTerm::canClearTrailingWS (uInt& xmax, uInt y)
{
  // Line has trailing whitespace
  // => clear from xmax to end of line

  term_area*& vt = vterm;
  char*& ce = TCAP(fc::t_clr_eol);
  charData* last_char = &vt->text[(y + 1) * uInt(vt->width) - 1];

  if ( ce && last_char->code == ' ' )
  {
    uInt trailing_whitespace = 1;
    bool normal = FTerm::isNormal(last_char);
    bool& ut = FTermcap::background_color_erase;

    for (uInt x = uInt(vt->width) - 1; x >  0 ; x--)
    {
      charData* ch = &vt->text[y * uInt(vt->width) + x];

      if ( *last_char == *ch )
        trailing_whitespace++;
      else
        break;
    }

    if ( trailing_whitespace > uInt(vt->width) - xmax
      && (ut || normal)
      && clr_bol_length < trailing_whitespace )
    {
      xmax = uInt(vt->width) - trailing_whitespace;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
bool FVTerm::skipUnchangedCharacters(uInt& x, uInt xmax, uInt y)
{
  // Skip characters without changes if it is faster than redrawing

  term_area*& vt = vterm;
  charData* print_char = &vt->text[y * uInt(vt->width) + x];
  print_char->attr.bit.printed = true;

  if ( print_char->attr.bit.no_changes )
  {
    uInt count = 1;

    for (uInt i = x + 1; i <= xmax; i++)
    {
      charData* ch = &vt->text[y * uInt(vt->width) + i];

      if ( ch->attr.bit.no_changes )
        count++;
      else
        break;
    }

    if ( count > cursor_address_length )
    {
      setTermXY (int(x + count), int(y));
      x = x + count - 1;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------
void FVTerm::printRange ( uInt xmin, uInt xmax, uInt y
                        , bool draw_trailing_ws )
{
  for (uInt x = xmin; x <= xmax; x++)
  {
    charData* print_char;
    term_area*& vt = vterm;
    char*& ec = TCAP(fc::t_erase_chars);
    char*& rp = TCAP(fc::t_repeat_char);
    print_char = &vt->text[y * uInt(vt->width) + x];
    print_char->attr.bit.printed = true;

    // skip character with no changes
    if ( skipUnchangedCharacters(x, xmax, y) )
      continue;

    // Erase character
    if ( ec && print_char->code == ' ' )
    {
      exit_state erase_state = \
          eraseCharacters(x, xmax, y, draw_trailing_ws);

      if ( erase_state == line_completely_printed )
        break;
    }
    else if ( rp )  // Repeat one character n-fold
    {
      repeatCharacter(x, xmax, y);
    }
    else
    {
      // General character output
      appendCharacter (print_char);
      markAsPrinted (x, y);
    }
  }
}
//----------------------------------------------------------------------
FVTerm::exit_state FVTerm::eraseCharacters ( uInt& x, uInt xmax, uInt y
                                           , bool draw_trailing_ws )
{
  // Erase a number of characters to draw simple whitespaces

  term_area*& vt = vterm;
  char*& ec = TCAP(fc::t_erase_chars);
  charData* print_char = &vt->text[y * uInt(vt->width) + x];

  if ( ! ec || print_char->code != ' ' )
    return not_used;

  uInt whitespace = 1;
  bool normal = FTerm::isNormal(print_char);

  for (uInt i = x + 1; i <= xmax; i++)
  {
    charData* ch = &vt->text[y * uInt(vt->width) + i];

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
    bool& ut = FTermcap::background_color_erase;

    if ( whitespace > erase_char_length + cursor_address_length
      && (ut || normal) )
    {
      appendAttributes (print_char);
      appendOutputBuffer (tparm(ec, whitespace, 0, 0, 0, 0, 0, 0, 0, 0));

      if ( x + whitespace - 1 < xmax || draw_trailing_ws )
        setTermXY (int(x + whitespace), int(y));
      else
        return line_completely_printed;

      x = x + whitespace - 1;
    }
    else
    {
      x--;

      for (uInt i = 0; i < whitespace; i++, x++)
        appendCharacter (print_char);
    }

    markAsPrinted (start_pos, x, y);
  }

  return used;
}

//----------------------------------------------------------------------
FVTerm::exit_state FVTerm::repeatCharacter (uInt& x, uInt xmax, uInt y)
{
  // Repeat one character n-fold

  term_area*& vt = vterm;
  char*& rp = TCAP(fc::t_repeat_char);
  charData* print_char = &vt->text[y * uInt(vt->width) + x];

  if ( ! rp )
    return not_used;

  uInt repetitions = 1;

  for (uInt i = x + 1; i <= xmax; i++)
  {
    charData* ch = &vt->text[y * uInt(vt->width) + i];

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

    if ( repetitions > repeat_char_length
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

      for (uInt i = 0; i < repetitions; i++, x++)
        appendCharacter (print_char);
    }

    markAsPrinted (start_pos, x, y);
  }

  return used;
}

//----------------------------------------------------------------------
void FVTerm::cursorWrap()
{
  // Wrap the cursor
  term_area*& vt = vterm;

  if ( term_pos->getX() >= vt->width )
  {
    if ( term_pos->getY() == vt->height - 1 )
      term_pos->x_ref()--;
    else
    {
      if ( FTermcap::eat_nl_glitch )
      {
        term_pos->setPoint(-1, -1);
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
bool FVTerm::printWrap (term_area* area)
{
  bool end_of_area = false;
  int width  = area->width,
      height = area->height,
      rsh    = area->right_shadow,
      bsh    = area->bottom_shadow;

  // Line break at right margin
  if ( area->cursor_x > width + rsh )
  {
    area->cursor_x = 1;
    area->cursor_y++;
  }

  // Prevent up scrolling
  if ( area->cursor_y > height + bsh )
  {
    area->cursor_y--;
    end_of_area = true;
  }

  return end_of_area;
}

//----------------------------------------------------------------------
void FVTerm::updateTerminalLine (uInt y)
{
  // Updates pending changes from line y to the terminal

  term_area*& vt = vterm;
  uInt& xmin = vt->changes[y].xmin;
  uInt& xmax = vt->changes[y].xmax;

  if ( xmin <= xmax )
  {
    bool draw_leading_ws = false;
    bool draw_trailing_ws = false;
    char*& ce = TCAP(fc::t_clr_eol);
    charData* first_char = &vt->text[y * uInt(vt->width)];
    charData* last_char  = &vt->text[(y + 1) * uInt(vt->width) - 1];
    charData* min_char   = &vt->text[y * uInt(vt->width) + xmin];

    // Clear rest of line
    bool is_eol_clean = canClearToEOL (xmin, y);

    if ( ! is_eol_clean )
    {
      // leading whitespace
      draw_leading_ws = canClearLeadingWS (xmin, y);

      // trailing whitespace
      draw_trailing_ws = canClearTrailingWS (xmax, y);
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
        char*& cb = TCAP(fc::t_clr_bol);
        appendAttributes (first_char);
        appendOutputBuffer (cb);
        markAsPrinted (0, xmin, y);
      }

      printRange (xmin, xmax, y, draw_trailing_ws);

      if ( draw_trailing_ws )
      {
        appendAttributes (last_char);
        appendOutputBuffer (ce);
        markAsPrinted (xmax + 1, uInt(vt->width - 1), y);
      }
    }

    // Reset line changes
    xmin = uInt(vt->width);
    xmax = 0;
  }

  cursorWrap();
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
      setTermXY (x, y);
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

  if ( term_geometry.contains(x, y) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline void FVTerm::markAsPrinted (uInt pos, uInt line)
{
  // Marks a character as printed

  vterm->text[line * uInt(vterm->width) + pos].attr.bit.printed = true;
}

//----------------------------------------------------------------------
inline void FVTerm::markAsPrinted (uInt from, uInt to, uInt line)
{
  // Marks characters in the specified range [from .. to] as printed

  for (uInt x = from; x <= to; x++)
    vterm->text[line * uInt(vterm->width) + x].attr.bit.printed = true;
}

//----------------------------------------------------------------------
inline void FVTerm::newFontChanges (charData*& next_char)
{
  // NewFont special cases
  if ( isNewFont() )
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
inline void FVTerm::charsetChanges (charData*& next_char)
{
  if ( getEncoding() == fc::UTF8 )
    return;

  uInt code = uInt(next_char->code);
  uInt ch_enc = FTerm::charEncode(code);

  if ( ch_enc == code )
    return;

  if ( ch_enc == 0 )
  {
    next_char->code = int(FTerm::charEncode(code, fc::ASCII));
    return;
  }

  next_char->code = int(ch_enc);

  if ( getEncoding() == fc::VT100 )
    next_char->attr.bit.alt_charset = true;
  else if ( getEncoding() == fc::PC )
  {
    next_char->attr.bit.pc_charset = true;

    if ( isPuttyTerminal() )
      return;

    if ( isXTerminal() && ch_enc < 0x20 )  // Character 0x00..0x1f
    {
      if ( hasUTF8() )
        next_char->code = int(FTerm::charEncode(code, fc::ASCII));
      else
      {
        next_char->code += 0x5f;
        next_char->attr.bit.alt_charset = true;
      }
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::appendCharacter (charData*& next_char)
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
inline void FVTerm::appendChar (charData*& next_char)
{
  newFontChanges (next_char);
  charsetChanges (next_char);

  appendAttributes (next_char);
  appendOutputBuffer (next_char->code);
}

//----------------------------------------------------------------------
inline void FVTerm::appendAttributes (charData*& next_attr)
{
  char* attr_str;
  charData* term_attr = &term_attribute;

  // generate attribute string for the next character
  attr_str = FTerm::changeAttribute (term_attr, next_attr);

  if ( attr_str )
    appendOutputBuffer (attr_str);
}

//----------------------------------------------------------------------
int FVTerm::appendLowerRight (charData*& screen_char)
{
  char* SA = TCAP(fc::t_enter_am_mode);
  char* RA = TCAP(fc::t_exit_am_mode);

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
    char* IC = TCAP(fc::t_parm_ich);
    char* im = TCAP(fc::t_enter_insert_mode);
    char* ei = TCAP(fc::t_exit_insert_mode);
    char* ip = TCAP(fc::t_insert_padding);
    char* ic = TCAP(fc::t_insert_character);

    x = int(getColumnNumber()) - 2;
    y = int(getLineNumber()) - 1;
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
inline void FVTerm::appendOutputBuffer (const std::string& s)
{
#if defined(__sun) && defined(__SVR4)
  char* c_string = C_STR(s.c_str());
#else
  const char* const& c_string = s.c_str();
#endif

  tputs (c_string, 1, appendOutputBuffer);
}

//----------------------------------------------------------------------
inline void FVTerm::appendOutputBuffer (const char s[])
{
#if defined(__sun) && defined(__SVR4)
  tputs (C_STR(s), 1, appendOutputBuffer);
#else
  tputs (s, 1, appendOutputBuffer);
#endif
}

#if defined(__sun) && defined(__SVR4)
//----------------------------------------------------------------------
int FVTerm::appendOutputBuffer (char ch)
{
  // This method is required by tputs under Solaris
  output_buffer->push(ch);

  if ( output_buffer->size() >= TERMINAL_OUTPUT_BUFFER_SIZE )
    flush_out();

  return ch;
}
#endif  // defined(__sun) && defined(__SVR4)

//----------------------------------------------------------------------
int FVTerm::appendOutputBuffer (int ch)
{
  // append method for unicode character
  output_buffer->push(ch);

  if ( output_buffer->size() >= TERMINAL_OUTPUT_BUFFER_SIZE )
    flush_out();

  return ch;
}

}  // namespace finalcut
