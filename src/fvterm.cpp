/***********************************************************************
* fvterm.cpp - Virtual terminal implementation                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2016-2019 Markus Gans                                      *
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
#include "final/fc.h"
#include "final/fcharmap.h"
#include "final/fcolorpair.h"
#include "final/fkeyboard.h"
#include "final/foptiattr.h"
#include "final/foptimove.h"
#include "final/fsystem.h"
#include "final/fterm.h"
#include "final/ftermdata.h"
#include "final/ftermbuffer.h"
#include "final/ftermcap.h"
#include "final/ftypes.h"
#include "final/fvterm.h"
#include "final/fwidget.h"
#include "final/fwindow.h"

namespace finalcut
{

// global FVTerm object
static FVTerm* init_object{nullptr};

// static class attributes
bool                 FVTerm::terminal_update_complete{false};
bool                 FVTerm::terminal_update_pending{false};
bool                 FVTerm::force_terminal_update{false};
bool                 FVTerm::stop_terminal_updates{false};
int                  FVTerm::skipped_terminal_update{};
uInt                 FVTerm::erase_char_length{};
uInt                 FVTerm::repeat_char_length{};
uInt                 FVTerm::clr_bol_length{};
uInt                 FVTerm::clr_eol_length{};
uInt                 FVTerm::cursor_address_length{};
std::queue<int>*     FVTerm::output_buffer{nullptr};
FPoint*              FVTerm::term_pos{nullptr};
FSystem*             FVTerm::fsystem{nullptr};
FTerm*               FVTerm::fterm{nullptr};
FVTerm::term_area*   FVTerm::vterm{nullptr};
FVTerm::term_area*   FVTerm::vdesktop{nullptr};
FVTerm::term_area*   FVTerm::active_area{nullptr};
FKeyboard*           FVTerm::keyboard{nullptr};
charData             FVTerm::term_attribute{};
charData             FVTerm::next_attribute{};
charData             FVTerm::s_ch{};
charData             FVTerm::i_ch{};


//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FVTerm::FVTerm (bool initialize, bool disable_alt_screen)
{
  if ( initialize )
    init (disable_alt_screen);
}

//----------------------------------------------------------------------
FVTerm::~FVTerm()  // destructor
{
  if ( init_object == this )
    finish();
}


// Overloaded operators
//----------------------------------------------------------------------
FVTerm& FVTerm::operator << (const FTermBuffer& term_buffer)
{
  print (term_buffer);
  return *this;
}

// public methods of FVTerm
//----------------------------------------------------------------------
FPoint FVTerm::getPrintCursor()
{
  auto win = getPrintArea();

  if ( win )
    return FPoint ( win->offset_left + win->cursor_x
                  , win->offset_top + win->cursor_y );

  return FPoint(0, 0);
}

//----------------------------------------------------------------------
void FVTerm::setTermXY (int x, int y)
{
  // Sets the hardware cursor to the given (x,y) position

  if ( term_pos->getX() == x && term_pos->getY() == y )
    return;

  int term_width = int(getColumnNumber());
  int term_height = int(getLineNumber());

  if ( x >= term_width && term_width > 0 )
  {
    y += x / term_width;
    x %= term_width;
  }

  if ( term_pos->getY() >= term_height )
    term_pos->setY(term_height - 1);

  if ( y >= term_height )
    y = term_height - 1;

  int term_x = term_pos->getX();
  int term_y = term_pos->getY();

  const char* move_str = FTerm::moveCursorString (term_x, term_y, x, y);

  if ( move_str )
    appendOutputBuffer(move_str);

  flush_out();
  term_pos->setPoint(x, y);
}

//----------------------------------------------------------------------
void FVTerm::hideCursor (bool enable)
{
  // Hides or shows the input cursor on the terminal

  const char* visibility_str = FTerm::cursorsVisibilityString (enable);

  if ( visibility_str )
    appendOutputBuffer(visibility_str);

  flush_out();
}

//----------------------------------------------------------------------
void FVTerm::setPrintCursor (const FPoint& pos)
{
  if ( auto win = getPrintArea() )
  {
    win->cursor_x = pos.getX() - win->offset_left;
    win->cursor_y = pos.getY() - win->offset_top;
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
void FVTerm::createVTerm (const FSize& size)
{
  // initialize virtual terminal

  const FRect box(0, 0, size.getWidth(), size.getHeight());
  const FSize shadow(0, 0);
  createArea (box, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (const FSize& size)
{
  // resize virtual terminal

  const FRect box(0, 0, size.getWidth(), size.getHeight());
  const FSize shadow(0, 0);
  resizeArea (box, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::putVTerm()
{
  for (int i{0}; i < vterm->height; i++)
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
    || FApplication::getApplicationObject()->isQuit() )
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

  auto data = getFTerm().getFTermData();

  // Checks if the resizing of the terminal is not finished
  if ( data && data->hasTermResized() )
    return;

  // Monitor whether the terminal size has changed
  if ( isTermSizeChanged() )
  {
    raise (SIGWINCH);  // Send SIGWINCH
    return;
  }

  // Update data on VTerm
  updateVTerm();

  // Checks if VTerm has changes
  if ( ! vterm->has_changes )
    return;

  for (uInt y{0}; y < uInt(vterm->height); y++)
    updateTerminalLine (y);

  vterm->has_changes = false;

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
    vterm_preprocessing obj{ instance, handler };
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

  auto iter = print_area->preprocessing_call.begin();

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
  static constexpr int BUFSIZE = 4096;
  wchar_t buffer[BUFSIZE]{};
  va_list args{};

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
  if ( s.isNull() )
    return -1;

  auto area = getPrintArea();

  if ( ! area )
    return -1;

  return print (area, s);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const FString& s)
{
  if ( s.isNull() || ! area )
    return -1;

  std::vector<charData> term_string{};
  const wchar_t* p = s.wc_str();

  if ( p )
  {
    while ( *p )
    {
      charData nc{};  // next character
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
int FVTerm::print (const FTermBuffer& term_buffer)
{
  if ( term_buffer.isEmpty() )
    return -1;

  auto area = getPrintArea();

  if ( ! area )
    return -1;

  return print (area, term_buffer);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const FTermBuffer& term_buffer)
{
  const auto& term_string = term_buffer.getBuffer();
  return print (area, term_string);
}

//----------------------------------------------------------------------
int FVTerm::print (const std::vector<charData>& term_string)
{
  if ( term_string.empty() )
    return 0;

  auto area = getPrintArea();

  if ( ! area )
    return -1;

  return print (area, term_string);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, const std::vector<charData>& term_string)
{
  int len{0};
  uInt tabstop = uInt(getTabstop());

  if ( ! area )
    return -1;

  if ( term_string.empty() )
    return 0;

  for (auto&& ch : term_string)
  {
    bool printable_character{false};

    switch ( ch.code )
    {
      case '\n':
        area->cursor_y++;
        // fall through
      case '\r':
        area->cursor_x = 1;
        break;

      case '\t':
        area->cursor_x = int ( uInt(area->cursor_x)
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
        auto nc = ch;  // next character
        print (area, nc);
        printable_character = true;
      }
    }

    if ( ! printable_character && printWrap(area) )
      break;  // end of area reached

    len++;
  }

  return len;
}

//----------------------------------------------------------------------
int FVTerm::print (wchar_t c)
{
  auto area = getPrintArea();

  if ( ! area )
    return -1;

  return print (area, c);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, wchar_t c)
{
  charData nc{};  // next character

  if ( ! area )
    return -1;

  nc.code         = wchar_t(c);
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
  auto area = getPrintArea();

  if ( ! area )
    return -1;

  return print (area, term_char);
}

//----------------------------------------------------------------------
int FVTerm::print (term_area* area, charData& term_char)
{
  charData& nc = term_char;  // next character

  if ( ! area )
    return -1;

  int width  = area->width;
  int height = area->height;
  int rsh    = area->right_shadow;
  int bsh    = area->bottom_shadow;
  int ax     = area->cursor_x - 1;
  int ay     = area->cursor_y - 1;

  if ( area->cursor_x > 0
    && area->cursor_y > 0
    && ax < area->width + area->right_shadow
    && ay < area->height + area->bottom_shadow )
  {
    int line_len = area->width + area->right_shadow;
    auto ac = &area->text[ay * line_len + ax];  // area character

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

      if ( ax < int(area->changes[ay].xmin) )
        area->changes[ay].xmin = uInt(ax);

      if ( ax > int(area->changes[ay].xmax) )
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

//----------------------------------------------------------------------
void FVTerm::print (const FPoint& p)
{
  setPrintCursor (p);
}

//----------------------------------------------------------------------
void FVTerm::print (const FColorPair& pair)
{
  setColor (pair.getForegroundColor(), pair.getBackgroundColor());
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
void FVTerm::createArea ( const FRect& box
                        , const FSize& shadow
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
  resizeArea (box, shadow, area);
}

//----------------------------------------------------------------------
void FVTerm::resizeArea ( const FRect& box
                        , const FSize& shadow
                        , term_area* area )
{
  // Resize the virtual window to a new size.

  int offset_left = box.getX();
  int offset_top  = box.getY();
  int width       = int(box.getWidth());
  int height      = int(box.getHeight());
  int rsw         = int(shadow.getWidth());
  int bsh         = int(shadow.getHeight());

  assert ( offset_top >= 0 );
  assert ( width > 0 && width + rsw > 0 );
  assert ( height > 0 && height + bsh > 0 );
  assert ( rsw >= 0 );
  assert ( bsh >= 0 );

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

  bool realloc_success{false};
  std::size_t area_size = std::size_t((width + rsw) * (height + bsh));

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

  area->offset_left   = offset_left;
  area->offset_top    = offset_top;
  area->width         = width;
  area->height        = height;
  area->right_shadow  = rsw;
  area->bottom_shadow = bsh;
  area->has_changes   = false;

  FSize size(std::size_t(width + rsw), std::size_t(height + bsh));
  setTextToDefault (area, size);
}

//----------------------------------------------------------------------
inline void FVTerm::setTextToDefault ( term_area* area
                                     , const FSize& size )
{
  charData default_char;
  line_changes unchanged;

  default_char.code         = ' ';
  default_char.fg_color     = fc::Default;
  default_char.bg_color     = fc::Default;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;
  default_char.attr.byte[2] = 0;

  std::fill_n (area->text, size.getArea(), default_char);

  unchanged.xmin = uInt(size.getWidth());
  unchanged.xmax = 0;
  unchanged.trans_count = 0;

  std::fill_n (area->changes, size.getHeight(), unchanged);
}

//----------------------------------------------------------------------
inline bool FVTerm::reallocateTextArea ( term_area* area
                                       , std::size_t height
                                       , std::size_t size )
{
  // Reallocate "height" lines for changes
  // and "size" bytes for the text area

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
  // Reallocate "size" bytes for the text area

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
      area->changes = nullptr;
    }

    if ( area->text != 0 )
    {
      delete[] area->text;
      area->text = nullptr;
    }

    delete area;
    area = nullptr;
  }
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (const FRect& box)
{
  int x = box.getX() - 1;
  int y = box.getY() - 1;
  int w = int(box.getWidth());
  int h = int(box.getHeight());

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

  for (int ty{0}; ty < h; ty++)
  {
    int ypos = y + ty;

    for (int tx{0}; tx < w; tx++)
    {
      int xpos = x + tx;
      auto tc = &vterm->text[ypos * vterm->width + xpos];  // terminal character
      auto sc = generateCharacter(FPoint(xpos, ypos));  // shown character
      std::memcpy (tc, &sc, sizeof(*tc));
    }

    if ( int(vterm->changes[ypos].xmin) > x )
      vterm->changes[ypos].xmin = uInt(x);

    if ( int(vterm->changes[ypos].xmax) < x + w - 1 )
      vterm->changes[ypos].xmax = uInt(x + w - 1);
  }

  vterm->has_changes = true;
}

//----------------------------------------------------------------------
FVTerm::covered_state FVTerm::isCovered ( const FPoint& pos
                                        , term_area* area )
{
  // Determines the covered state for the given position

  if ( ! area )
    return non_covered;

  bool found = bool(area == vdesktop);
  auto is_covered = non_covered;

  if ( FWidget::getWindowList() && ! FWidget::getWindowList()->empty() )
  {
    for (auto& win_obj : *FWidget::getWindowList())
    {
      auto win = win_obj->getVWin();

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

      if ( found && geometry.contains(pos) )
      {
        int width = win->width + win->right_shadow;
        int x = pos.getX();
        int y = pos.getY();
        auto tmp = &win->text[(y - win_y) * width + (x - win_x)];

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
                                   , const FPoint& area_pos
                                   , const FPoint& terminal_pos )
{
  // Add the overlapping color to this character

  int x = area_pos.getX();
  int y = area_pos.getY();
  int tx = terminal_pos.getX();
  int ty = terminal_pos.getY();
  int width = area->width + area->right_shadow;
  // Area character
  auto ac = &area->text[y * width + x];
  // Terminal character
  auto tc = &vterm->text[ty * vterm->width + tx];
  // New character
  charData nc{};
  std::memcpy (&nc, ac, sizeof(nc));
  // Overlapped character
  auto oc = getOverlappedCharacter (terminal_pos + FPoint(1, 1), area->widget);
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
void FVTerm::updateOverlappedCharacter ( term_area* area
                                       , const FPoint& terminal_pos )
{
  // Restore one character on vterm

  // Terminal character
  int  tx = terminal_pos.getX();
  int  ty = terminal_pos.getY();
  auto tc = &vterm->text[ty * vterm->width + tx];
  // Overlapped character
  auto oc = getCoveredCharacter (terminal_pos + FPoint(1, 1), area->widget);
  oc.attr.bit.no_changes = bool(tc->attr.bit.printed && *tc == oc);
  std::memcpy (tc, &oc, sizeof(*tc));
}

//----------------------------------------------------------------------
void FVTerm::updateShadedCharacter ( term_area* area
                                   , const FPoint& area_pos
                                   , const FPoint& terminal_pos )
{
  // Get covered character + add the current color

  int x = area_pos.getX();
  int y = area_pos.getY();
  int tx = terminal_pos.getX();
  int ty = terminal_pos.getY();
  int width = area->width + area->right_shadow;
  // Area character
  auto ac = &area->text[y * width + x];
  // Terminal character
  auto tc = &vterm->text[ty * vterm->width + tx];
  // Overlapped character
  auto oc = getCoveredCharacter (terminal_pos + FPoint(1, 1), area->widget);
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
                                     , const FPoint& area_pos
                                     , const FPoint& terminal_pos )
{
  // Add the covered background to this character

  int x = area_pos.getX();
  int y = area_pos.getY();
  int tx = terminal_pos.getX();
  int ty = terminal_pos.getY();
  int width = area->width + area->right_shadow;
  // Area character
  auto ac = &area->text[y * width + x];
  // Terminal character
  auto tc = &vterm->text[ty * vterm->width + tx];
  // New character
  charData nc{};
  std::memcpy (&nc, ac, sizeof(nc));
  // Covered character
  auto cc = getCoveredCharacter (terminal_pos + FPoint(1, 1), area->widget);
  nc.bg_color = cc.bg_color;
  nc.attr.bit.no_changes = bool(tc->attr.bit.printed && *tc == nc);
  std::memcpy (tc, &nc, sizeof(*tc));
}

//----------------------------------------------------------------------
void FVTerm::updateCharacter ( term_area* area
                             , const FPoint& area_pos
                             , const FPoint& terminal_pos )
{
  // Copy a area character to the virtual terminal

  int x = area_pos.getX();
  int y = area_pos.getY();
  int tx = terminal_pos.getX();
  int ty = terminal_pos.getY();
  int width = area->width + area->right_shadow;
  // Area character
  auto ac = &area->text[y * width + x];
  // Terminal character
  auto tc = &vterm->text[ty * vterm->width + tx];
  std::memcpy (tc, ac, sizeof(*tc));

  if ( tc->attr.bit.printed && *tc == *ac )
    tc->attr.bit.no_changes = true;
  else
    tc->attr.bit.no_changes = false;
}

//----------------------------------------------------------------------
bool FVTerm::updateVTermCharacter ( term_area* area
                                  , const FPoint& area_pos
                                  , const FPoint& terminal_pos )
{
  int x = area_pos.getX();
  int y = area_pos.getY();
  int width = area->width + area->right_shadow;
  // Area character
  auto ac = &area->text[y * width + x];

  // Get covered state
  auto is_covered = isCovered(terminal_pos, area);

  if ( is_covered == fully_covered )
    return false;

  if ( is_covered == half_covered )
  {
    updateOverlappedColor(area, area_pos, terminal_pos);
  }
  else if ( ac->attr.bit.transparent )   // Transparent
  {
    updateOverlappedCharacter(area, terminal_pos);
  }
  else  // Not transparent
  {
    if ( ac->attr.bit.trans_shadow )  // Transparent shadow
    {
      updateShadedCharacter (area, area_pos, terminal_pos);
    }
    else if ( ac->attr.bit.inherit_bg )
    {
      updateInheritBackground (area, area_pos, terminal_pos);
    }
    else  // Default
    {
      updateCharacter (area, area_pos, terminal_pos);
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
    auto iter = area->preprocessing_call.begin();
    auto end = area->preprocessing_call.end();

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

  if ( ! widget->getWindowList() || widget->getWindowList()->empty() )
    return;

  auto iter = widget->getWindowList()->begin();
  auto end  = widget->getWindowList()->end();

  for (; iter != end; ++iter)
  {
    auto win = (*iter)->getVWin();

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
      auto iter2 = win->preprocessing_call.begin();
      auto end2 = win->preprocessing_call.end();

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

  int ax  = area->offset_left;
  int ay  = area->offset_top;
  int width = area->width + area->right_shadow;
  int height = area->height + area->bottom_shadow;
  int ol  = 0;  // Outside left
  int y_end;

  // Call the processing handler methods
  callPreprocessingHandler(area);

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  if ( height + ay > vterm->height )
    y_end = vterm->height - ay;
  else
    y_end = height;

  for (int y{0}; y < y_end; y++)  // Line loop
  {
    int _xmin, _xmax;
    bool modified = false;
    int line_xmin = int(area->changes[y].xmin);
    int line_xmax = int(area->changes[y].xmax);

    if ( line_xmin > line_xmax )
      continue;

    if ( ax == 0 )
      line_xmin = ol;

    if ( width + ax - ol >= vterm->width )
      line_xmax = vterm->width + ol - ax - 1;

    if ( ax + line_xmin >= vterm->width )
      continue;

    for (int x = line_xmin; x <= line_xmax; x++)  // Column loop
    {
      // Global terminal positions
      int tx = ax + x;
      int ty = ay + y;

      if ( tx < 0 || ty < 0 )
        continue;

      tx -= ol;

      if ( updateVTermCharacter(area, FPoint(x, y), FPoint(tx, ty)) )
        modified = true;

      if ( ! modified )
        line_xmin++;  // Don't update covered character
    }

    _xmin = ax + line_xmin - ol;
    _xmax = ax + line_xmax;

    if ( _xmin < int(vterm->changes[ay + y].xmin) )
      vterm->changes[ay + y].xmin = uInt(_xmin);

    if ( _xmax >= vterm->width )
      _xmax = vterm->width - 1;

    if ( _xmax > int(vterm->changes[ay + y].xmax) )
      vterm->changes[ay + y].xmax = uInt(_xmax);

    area->changes[y].xmin = uInt(width);
    area->changes[y].xmax = 0;
  }

  vterm->has_changes = true;
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
    // area offset
    int ax  = area->offset_left;
    int ay  = area->offset_top;
    // area cursor position
    int cx = area->input_cursor_x;
    int cy = area->input_cursor_y;
    // terminal position
    int x  = ax + cx;
    int y  = ay + cy;

    if ( isInsideArea (FPoint(cx, cy), area)
      && isInsideTerminal (FPoint(x, y))
      && isCovered (FPoint(x, y), area) == non_covered )
    {
      vterm->input_cursor_x = x;
      vterm->input_cursor_y = y;
      vterm->input_cursor_visible = true;
      vterm->has_changes = true;
      return true;
    }
  }

  vterm->input_cursor_visible = false;
  return false;
}

//----------------------------------------------------------------------
bool FVTerm::isInsideArea (const FPoint& pos, term_area* area)
{
  // Check whether the coordinates are within the area

  auto aw = std::size_t(area->width);
  auto ah = std::size_t(area->height);
  FRect area_geometry(0, 0, aw, ah);

  if ( area_geometry.contains(pos) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
void FVTerm::setAreaCursor ( const FPoint& pos
                           , bool visible
                           , term_area* area )
{
  if ( ! area )
    return;

  area->input_cursor_x = pos.getX() - 1;
  area->input_cursor_y = pos.getY() - 1;
  area->input_cursor_visible = visible;
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FPoint& pos, term_area* area)
{
  // Copies a block from the virtual terminal position to the given area

  if ( ! area )
    return;

  int y_end;
  int length;
  int ax = pos.getX() - 1;
  int ay = pos.getY() - 1;

  if ( area->height + ay > vterm->height )
    y_end = area->height - ay;
  else
    y_end = area->height;

  if ( area->width + ax > vterm->width )
    length = vterm->width - ax;
  else
    length = area->width;

  for (int y{0}; y < y_end; y++)  // line loop
  {
    auto tc = &vterm->text[(ay + y) * vterm->width + ax];  // terminal character
    auto ac = &area->text[y * area->width];  // area character
    std::memcpy (ac, tc, sizeof(*ac) * unsigned(length));

    if ( int(area->changes[y].xmin) > 0 )
      area->changes[y].xmin = 0;

    if ( int(area->changes[y].xmax) < length - 1 )
      area->changes[y].xmax = uInt(length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FRect& box, term_area* area)
{
  // Copies a block from the virtual terminal rectangle to the given area

  if ( ! area )
    return;

  int x = box.getX();
  int y = box.getY();
  int w = int(box.getWidth());
  int h = int(box.getHeight());
  int dx = x - area->offset_left + 1;
  int dy = y - area->offset_top + 1;
  int y_end{}, length{};

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
    int line_len = area->width + area->right_shadow;
    auto tc = &vterm->text[(y + _y - 1) * vterm->width + x - 1];  // terminal character
    auto ac = &area->text[(dy + _y) * line_len + dx];  // area character
    std::memcpy (ac, tc, sizeof(*ac) * unsigned(length));

    if ( int(area->changes[dy + _y].xmin) > dx )
      area->changes[dy + _y].xmin = uInt(dx);

    if ( int(area->changes[dy + _y].xmax) < dx + length - 1 )
      area->changes[dy + _y].xmax = uInt(dx + length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::putArea (const FPoint& pos, term_area* area)
{
  // Copies the given area block to the virtual terminal position

  charData* tc{};  // terminal character
  charData* ac{};  // area character

  if ( ! area || ! area->visible )
    return;

  int ax = pos.getX() - 1;
  int ay = pos.getY() - 1;
  int width = area->width + area->right_shadow;
  int height = area->height + area->bottom_shadow;
  int ol = 0;  // outside left
  int y_end{}, length{};

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  if ( ay + height > vterm->height )
    y_end = vterm->height - ay;
  else
    y_end = height;

  if ( width - ol + ax > vterm->width )
    length = vterm->width - ax;
  else
    length = width - ol;

  if ( length < 1 )
    return;

  for (int y{0}; y < y_end; y++)  // line loop
  {
    if ( area->changes[y].trans_count == 0 )
    {
      // Line has only covered characters
      ac = &area->text[y * width + ol];
      tc = &vterm->text[(ay + y) * vterm->width + ax];
      putAreaLine (ac, tc, length);
    }
    else
    {
      // Line has one or more transparent characters
      for (int x{0}; x < length; x++)  // column loop
      {
        int cx = ax + x;
        int cy = ay + y;
        ac = &area->text[y * width + ol + x];
        tc = &vterm->text[cy * vterm->width + cx];
        putAreaCharacter (FPoint(cx + 1, cy + 1), area->widget, ac, tc);
      }
    }

    if ( ax < int(vterm->changes[ay + y].xmin) )
      vterm->changes[ay + y].xmin = uInt(ax);

    if ( ax + length - 1 > int(vterm->changes[ay + y].xmax) )
      vterm->changes[ay + y].xmax = uInt(ax + length - 1);
  }

  vterm->has_changes = true;
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaForward (term_area* area)
{
  // Scrolls the entire area up line down
  charData  nc{};  // next character
  charData* lc{};  // last character
  charData* dc{};  // destination character

  if ( ! area )
    return;

  if ( area->height <= 1 )
    return;

  int length = area->width;
  int total_width = area->width + area->right_shadow;
  int y_max = area->height - 1;

  for (int y{0}; y < y_max; y++)
  {
    int pos1 = y * total_width;
    int pos2 = (y + 1) * total_width;
    auto sc = &area->text[pos2];  // source character
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
      putArea (FPoint(1, 1), vdesktop);

      // avoid update lines from 0 to (y_max - 1)
      for (int y{0}; y < y_max; y++)
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

  charData  nc{};  // next character
  charData* lc{};  // last character
  charData* dc{};  // destination character

  if ( ! area )
    return;

  if ( area->height <= 1 )
    return;

  int length = area->width;
  int total_width = area->width + area->right_shadow;
  int y_max = area->height - 1;

  for (int y = y_max; y > 0; y--)
  {
    int pos1 = (y - 1) * total_width;
    int pos2 = y * total_width;
    auto sc = &area->text[pos1];  // source character
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
      putArea (FPoint(1, 1), vdesktop);

      // avoid update lines from 1 to y_max
      for (int y{1}; y <= y_max; y++)
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

  charData nc{};  // next character

  // Current attributes with a space character
  std::memcpy (&nc, &next_attribute, sizeof(nc));
  nc.code = fillchar;

  if ( ! (area && area->text) )
  {
    clearTerm (fillchar);
    return;
  }

  uInt w = uInt(area->width + area->right_shadow);

  if ( area->right_shadow == 0 )
  {
    if ( clearFullArea(area, nc) )
      return;
  }
  else
    clearAreaWithShadow(area, nc);

  for (int i{0}; i < area->height; i++)
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

  for (int i{0}; i < area->bottom_shadow; i++)
  {
    int y = area->height + i;
    area->changes[y].xmin = 0;
    area->changes[y].xmax = w - 1;
    area->changes[y].trans_count = w;
  }

  area->has_changes = true;
}

//----------------------------------------------------------------------
charData FVTerm::generateCharacter (const FPoint& pos)
{
  // Generates characters for a given position considering all areas

  int x = pos.getX();
  int y = pos.getY();
  auto sc = &vdesktop->text[y * vdesktop->width + x];  // shown character

  if ( ! FWidget::getWindowList() || FWidget::getWindowList()->empty() )
    return *sc;

  for (auto& win_obj : *FWidget::getWindowList())
  {
    auto win = win_obj->getVWin();

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
      int line_len = win->width + win->right_shadow;
      auto tmp = &win->text[(y - win_y) * line_len + (x - win_x)];

      if ( ! tmp->attr.bit.transparent )   // Current character not transparent
      {
        if ( tmp->attr.bit.trans_shadow )  // Transparent shadow
        {
          // Keep the current vterm character
          if ( sc != &s_ch )
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
charData FVTerm::getCharacter ( character_type char_type
                              , const FPoint& pos
                              , FVTerm* obj )
{
  // Gets the overlapped or the covered character for a given position

  int x = pos.getX() - 1;
  int y = pos.getY() - 1;
  int xx = x;
  int yy = y;

  if ( xx < 0 )
    xx = 0;

  if ( yy < 0 )
    yy = 0;

  if ( xx >= vterm->width )
    xx = vterm->width - 1;

  if ( yy >= vterm->height )
    yy = vterm->height - 1;

  auto cc = &vdesktop->text[yy * vdesktop->width + xx];  // covered character

  if ( ! FWidget::getWindowList() || FWidget::getWindowList()->empty() )
    return *cc;

  // Get the window layer of this object
  auto w = static_cast<FWidget*>(obj);
  int layer = FWindow::getWindowLayer(w);

  for (auto&& win_obj : *FWidget::getWindowList())
  {
    bool significant_char;

    // char_type can be "overlapped_character"
    // or "covered_character"
    if ( char_type == covered_character )
      significant_char = bool(layer >= FWindow::getWindowLayer(win_obj));
    else
      significant_char = bool(layer < FWindow::getWindowLayer(win_obj));

    if ( obj && win_obj != obj && significant_char )
    {
      auto win = win_obj->getVWin();

      if ( ! win || ! win->visible )
        continue;

      FRect geometry ( win->offset_left
                     , win->offset_top
                     , std::size_t(win->width + win->right_shadow)
                     , std::size_t(win->height + win->bottom_shadow) );

      // Window visible and contains current character
      if ( geometry.contains(x, y) )
        getAreaCharacter (FPoint(x, y), win, cc);
    }
    else if ( char_type == covered_character )
      break;
  }

  return *cc;
}

//----------------------------------------------------------------------
charData FVTerm::getCoveredCharacter (const FPoint& pos, FVTerm* obj)
{
  // Gets the covered character for a given position
  return getCharacter (covered_character, pos, obj);
}

//----------------------------------------------------------------------
charData FVTerm::getOverlappedCharacter (const FPoint& pos, FVTerm* obj)
{
  // Gets the overlapped character for a given position
  return getCharacter (overlapped_character, pos, obj);
}

//----------------------------------------------------------------------
void FVTerm::processTerminalUpdate()
{
  // Retains terminal updates if there are unprocessed inputs
  static constexpr int max_skip = 8;

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
  vterm       = nullptr;
  vdesktop    = nullptr;
  fsystem     = FTerm::getFSystem();

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

  // term_attribute stores the current state of the terminal
  term_attribute.code         = '\0';
  term_attribute.fg_color     = fc::Default;
  term_attribute.bg_color     = fc::Default;
  term_attribute.attr.byte[0] = 0;
  term_attribute.attr.byte[0] = 0;
  term_attribute.attr.byte[0] = 0;

  // next_attribute contains the state of the next printed character
  std::memcpy (&next_attribute, &term_attribute, sizeof(next_attribute));

  // Create virtual terminal
  FRect term_geometry (0, 0, getColumnNumber(), getLineNumber());
  createVTerm (term_geometry.getSize());

  // Create virtual desktop area
  FSize shadow_size(0, 0);
  createArea (term_geometry, shadow_size, vdesktop);
  vdesktop->visible = true;
  active_area = vdesktop;

  // Get FKeyboard object
  keyboard = FTerm::getFKeyboard();

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
void FVTerm::putAreaCharacter ( const FPoint& pos, FVTerm* obj
                              , charData* ac
                              , charData* tc )
{
  if ( ac->attr.bit.transparent )  // Transparent
  {
    // Restore one character on vterm
    charData ch = getCoveredCharacter (pos, obj);
    std::memcpy (tc, &ch, sizeof(*tc));
  }
  else  // Mot transparent
  {
    if ( ac->attr.bit.trans_shadow )  // Transparent shadow
    {
      // Get covered character + add the current color
      charData ch = getCoveredCharacter (pos, obj);
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
      charData ch{};
      std::memcpy (&ch, ac, sizeof(ch));
      charData cc = getCoveredCharacter (pos, obj);
      ch.bg_color = cc.bg_color;
      std::memcpy (tc, &ch, sizeof(*tc));
    }
    else  // Default
      std::memcpy (tc, ac, sizeof(*tc));
  }
}

//----------------------------------------------------------------------
void FVTerm::getAreaCharacter ( const FPoint& pos, term_area* area
                              , charData*& cc )
{
  int area_x = area->offset_left;
  int area_y = area->offset_top;
  int line_len = area->width + area->right_shadow;
  int x = pos.getX();
  int y = pos.getY();
  auto tmp = &area->text[(y - area_y) * line_len + (x - area_x)];

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

  auto& cl = TCAP(fc::t_clear_screen);
  auto& cd = TCAP(fc::t_clr_eos);
  auto& cb = TCAP(fc::t_clr_eol);
  bool ut = FTermcap::background_color_erase;
  auto next = &next_attribute;
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

    for (int i{0}; i < int(getLineNumber()); i++)
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
    for (int i{0}; i < vdesktop->height; i++)
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

  for (int y{0}; y < area->height; y++)
  {
    int pos = y * total_width;
    // Clear area
    std::fill_n (&area->text[pos], total_width, nc);
    // Make right shadow transparent
    std::fill_n (&area->text[pos + area->width], area->right_shadow, t_char);
  }

  // Make bottom shadow transparent
  for (int y{0}; y < area->bottom_shadow; y++)
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
  auto& ce = TCAP(fc::t_clr_eol);
  auto min_char = &vt->text[y * uInt(vt->width) + xmin];

  if ( ce && min_char->code == ' ' )
  {
    uInt beginning_whitespace = 1;
    bool normal = FTerm::isNormal(min_char);
    bool& ut = FTermcap::background_color_erase;

    for (uInt x = xmin + 1; x < uInt(vt->width); x++)
    {
      auto ch = &vt->text[y * uInt(vt->width) + x];

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
  auto& cb = TCAP(fc::t_clr_bol);
  auto first_char = &vt->text[y * uInt(vt->width)];

  if ( cb && first_char->code == ' ' )
  {
    uInt leading_whitespace = 1;
    bool normal = FTerm::isNormal(first_char);
    bool& ut = FTermcap::background_color_erase;

    for (uInt x{1}; x < uInt(vt->width); x++)
    {
      auto ch = &vt->text[y * uInt(vt->width) + x];

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
  auto& ce = TCAP(fc::t_clr_eol);
  auto last_char = &vt->text[(y + 1) * uInt(vt->width) - 1];

  if ( ce && last_char->code == ' ' )
  {
    uInt trailing_whitespace = 1;
    bool normal = FTerm::isNormal(last_char);
    bool& ut = FTermcap::background_color_erase;

    for (uInt x = uInt(vt->width) - 1; x >  0 ; x--)
    {
      auto ch = &vt->text[y * uInt(vt->width) + x];

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
  auto print_char = &vt->text[y * uInt(vt->width) + x];
  print_char->attr.bit.printed = true;

  if ( print_char->attr.bit.no_changes )
  {
    uInt count{1};

    for (uInt i = x + 1; i <= xmax; i++)
    {
      auto ch = &vt->text[y * uInt(vt->width) + i];

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
    term_area*& vt = vterm;
    auto& ec = TCAP(fc::t_erase_chars);
    auto& rp = TCAP(fc::t_repeat_char);
    auto print_char = &vt->text[y * uInt(vt->width) + x];
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
  auto& ec = TCAP(fc::t_erase_chars);
  auto print_char = &vt->text[y * uInt(vt->width) + x];

  if ( ! ec || print_char->code != ' ' )
    return not_used;

  uInt whitespace = 1;
  bool normal = FTerm::isNormal(print_char);

  for (uInt i = x + 1; i <= xmax; i++)
  {
    auto ch = &vt->text[y * uInt(vt->width) + i];

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

      for (uInt i{0}; i < whitespace; i++, x++)
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
  auto& rp = TCAP(fc::t_repeat_char);
  auto print_char = &vt->text[y * uInt(vt->width) + x];

  if ( ! rp )
    return not_used;

  uInt repetitions{1};

  for (uInt i = x + 1; i <= xmax; i++)
  {
    auto ch = &vt->text[y * uInt(vt->width) + i];

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
      term_pos->x_ref() += int(repetitions);
      x = x + repetitions - 1;
    }
    else
    {
      x--;

      for (uInt i{0}; i < repetitions; i++, x++)
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
  bool end_of_area{false};
  int width  = area->width
    , height = area->height
    , rsh    = area->right_shadow
    , bsh    = area->bottom_shadow;

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

  if ( xmin <= xmax )  // Line has changes
  {
    bool draw_leading_ws = false;
    bool draw_trailing_ws = false;
    auto& ce = TCAP(fc::t_clr_eol);
    auto first_char = &vt->text[y * uInt(vt->width)];
    auto last_char  = &vt->text[(y + 1) * uInt(vt->width) - 1];
    auto min_char   = &vt->text[y * uInt(vt->width) + xmin];

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
        auto& cb = TCAP(fc::t_clr_bol);
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

    if ( isInsideTerminal(FPoint(x, y)) )
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
bool FVTerm::isInsideTerminal (const FPoint& pos)
{
  // Check whether the coordinates are within the virtual terminal

  FRect term_geometry (0, 0, getColumnNumber(), getLineNumber());

  if ( term_geometry.contains(pos) )
    return true;
  else
    return false;
}

//----------------------------------------------------------------------
inline bool FVTerm::isTermSizeChanged()
{
  auto data = getFTerm().getFTermData();

  if ( ! data )
    return false;

  auto old_term_geometry = data->getTermGeometry();
  getFTerm().detectTermSize();
  auto term_geometry = data->getTermGeometry();
  term_geometry.move (-1, -1);

  if ( old_term_geometry.getSize() != term_geometry.getSize() )
    return true;

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
        // Show in reverse video
        next_char->attr.bit.reverse = true;
        break;

      default:
        break;
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::charsetChanges (charData*& next_char)
{
  wchar_t& code = next_char->code;
  next_char->encoded_code = code;

  if ( getEncoding() == fc::UTF8 )
    return;

  wchar_t ch_enc = FTerm::charEncode(code);

  if ( ch_enc == code )
    return;

  if ( ch_enc == 0 )
  {
    next_char->encoded_code = wchar_t(FTerm::charEncode(code, fc::ASCII));
    return;
  }

  next_char->encoded_code = ch_enc;

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
        next_char->encoded_code = int(FTerm::charEncode(code, fc::ASCII));
      else
      {
        next_char->encoded_code += 0x5f;
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
  characterFilter (next_char);
  appendOutputBuffer (next_char->encoded_code);
}

//----------------------------------------------------------------------
inline void FVTerm::appendAttributes (charData*& next_attr)
{
  auto term_attr = &term_attribute;

  // generate attribute string for the next character
  char* attr_str = FTerm::changeAttribute (term_attr, next_attr);

  if ( attr_str )
    appendOutputBuffer (attr_str);
}

//----------------------------------------------------------------------
int FVTerm::appendLowerRight (charData*& screen_char)
{
  auto& SA = TCAP(fc::t_enter_am_mode);
  auto& RA = TCAP(fc::t_exit_am_mode);

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
    auto& IC = TCAP(fc::t_parm_ich);
    auto& im = TCAP(fc::t_enter_insert_mode);
    auto& ei = TCAP(fc::t_exit_insert_mode);
    auto& ip = TCAP(fc::t_insert_padding);
    auto& ic = TCAP(fc::t_insert_character);

    int x = int(getColumnNumber()) - 2;
    int y = int(getLineNumber()) - 1;
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
inline void FVTerm::characterFilter (charData*& next_char)
{
  charSubstitution& sub_map = fterm->getCharSubstitutionMap();

  if ( sub_map.find(next_char->encoded_code) != sub_map.end() )
    next_char->encoded_code = sub_map[next_char->encoded_code];
}

//----------------------------------------------------------------------
inline void FVTerm::appendOutputBuffer (const std::string& s)
{
  const char* const& c_string = s.c_str();
  fsystem->tputs (c_string, 1, appendOutputBuffer);
}

//----------------------------------------------------------------------
inline void FVTerm::appendOutputBuffer (const char s[])
{
  fsystem->tputs (s, 1, appendOutputBuffer);
}

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
