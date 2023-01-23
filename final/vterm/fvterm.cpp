/***********************************************************************
* fvterm.cpp - Virtual terminal implementation                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2023 Markus Gans                                      *
*                                                                      *
* FINAL CUT is free software; you can redistribute it and/or modify    *
* it under the terms of the GNU Lesser General Public License as       *
* published by the Free Software Foundation; either version 3 of       *
* the License, or (at your option) any later version.                  *
*                                                                      *
* FINAL CUT is distributed in the hope that it will be useful, but     *
* WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include <numeric>
#include <string>
#include <vector>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/output/tty/ftermoutput.h"
#include "final/util/flog.h"
#include "final/util/fpoint.h"
#include "final/util/frect.h"
#include "final/util/fsize.h"
#include "final/util/fsystem.h"
#include "final/vterm/fcolorpair.h"
#include "final/vterm/fstyle.h"
#include "final/vterm/fvtermbuffer.h"
#include "final/vterm/fvterm.h"

namespace finalcut
{

namespace internal
{

struct var
{
  static bool  fvterm_initialized;  // Global init state
  static uInt8 b1_transparent_mask;
};

bool  var::fvterm_initialized{false};
uInt8 var::b1_transparent_mask{};

}  // namespace internal

// static class attributes
bool                 FVTerm::draw_completed{false};
bool                 FVTerm::skip_one_vterm_update{false};
bool                 FVTerm::no_terminal_updates{false};
bool                 FVTerm::force_terminal_update{false};
FVTerm::FTermArea*   FVTerm::active_area{nullptr};
FChar                FVTerm::s_ch{};
FChar                FVTerm::i_ch{};
uInt8                FVTerm::b1_print_trans_mask{};
int                  FVTerm::tabstop{8};


//----------------------------------------------------------------------
// class FVTerm
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FVTerm::FVTerm()
{
  init<FTermOutput>();
}

//----------------------------------------------------------------------
FVTerm::~FVTerm()  // destructor
{
  if ( getGlobalFVTermInstance() == this )
    finish();
}

//----------------------------------------------------------------------
auto FVTerm::operator << (const FVTermBuffer& vterm_buffer) noexcept -> FVTerm&
{
  print (vterm_buffer);
  return *this;
}


// public methods of FVTerm
//----------------------------------------------------------------------
auto FVTerm::getFOutput() -> std::shared_ptr<FOutput>
{
  static const auto& init_object = getGlobalFVTermInstance();
  return init_object->foutput;
}

//----------------------------------------------------------------------
auto FVTerm::getPrintCursor() -> FPoint
{
  const auto& win = getPrintArea();

  if ( win )
    return { win->offset_left + win->cursor_x
           , win->offset_top + win->cursor_y };

  return {0, 0};  // Fallback coordinates
}

//----------------------------------------------------------------------
void FVTerm::setTerminalUpdates (TerminalUpdate refresh_state) const
{
  if ( refresh_state == TerminalUpdate::Stop )
  {
    no_terminal_updates = true;
  }
  else if ( refresh_state == TerminalUpdate::Continue
         || refresh_state == TerminalUpdate::Start )
  {
    no_terminal_updates = false;
  }

  if ( refresh_state == TerminalUpdate::Start )
    updateTerminal();
}

//----------------------------------------------------------------------
void FVTerm::setCursor (const FPoint& pos) noexcept
{
  if ( auto win = getPrintArea() )
  {
    win->setCursorPos ( pos.getX() - win->offset_left
                      , pos.getY() - win->offset_top );
  }
}

//----------------------------------------------------------------------
void FVTerm::setNonBlockingRead (bool enable)
{
  static const auto& init_object = getGlobalFVTermInstance();
  init_object->foutput->setNonBlockingRead (enable);
}

//----------------------------------------------------------------------
void FVTerm::clearArea (wchar_t fillchar)
{
  clearArea (vwin.get(), fillchar);
}

//----------------------------------------------------------------------
void FVTerm::createVDesktop (const FSize& size) noexcept
{
  // Initialize virtual desktop area

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  vdesktop = createArea(box);
  vdesktop->visible = true;
}

//----------------------------------------------------------------------
void FVTerm::createVTerm (const FSize& size) noexcept
{
  // Initialize virtual terminal

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  vterm = createArea(box);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (const FSize& size) const noexcept
{
  // Resize virtual terminal

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  resizeArea (box, vterm.get());
}

//----------------------------------------------------------------------
void FVTerm::putVTerm() const
{
  for (auto i{0}; i < vterm->height; i++)
  {
    auto& vterm_changes = vterm->changes[std::size_t(i)];
    vterm_changes.xmin = 0;
    vterm_changes.xmax = uInt(vterm->width - 1);
  }

  updateTerminal();
}

//----------------------------------------------------------------------
auto FVTerm::updateTerminal() const -> bool
{
  return foutput->updateTerminal();
}

//----------------------------------------------------------------------
void FVTerm::addPreprocessingHandler ( const FVTerm* instance
                                     , FPreprocessingFunction&& function )
{
  if ( ! print_area )
    getPrintArea();

  if ( ! instance || ! print_area )
    return;

  delPreprocessingHandler (instance);
  auto obj = std::make_unique<FVTermPreprocessing> \
      (instance, std::move(function));
  print_area->preproc_list.emplace_back(std::move(obj));
}

//----------------------------------------------------------------------
void FVTerm::delPreprocessingHandler (const FVTerm* instance)
{
  if ( ! print_area )
    getPrintArea();

  if ( ! instance || ! print_area || print_area->preproc_list.empty() )
    return;

  auto iter = print_area->preproc_list.cbegin();

  while ( iter != print_area->preproc_list.cend() )
  {
    if ( iter->get()->instance.get() == instance )
      iter = print_area->preproc_list.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
inline auto FVTerm::interpretControlCodes ( FTermArea* area
                                          , const FChar& term_char ) const noexcept -> bool
{
  switch ( term_char.ch[0] )
  {
    case L'\n':
      area->cursor_y++;
      // fall through
    case L'\r':
      area->cursor_x = 1;
      break;

    case L'\t':
      area->cursor_x += tabstop - ((area->cursor_x - 1) % tabstop);
      break;

    case L'\b':
      area->cursor_x--;
      break;

    case L'\a':
      foutput->beep();
      break;

    default:
      return false;
  }

  return true;
}

//----------------------------------------------------------------------
auto FVTerm::print (const FString& string) noexcept -> int
{
  if ( string.isEmpty() )
    return 0;

  FVTermBuffer vterm_buffer{};
  vterm_buffer.print(string);
  return print (vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, const FString& string) const noexcept -> int
{
  if ( ! area || string.isEmpty() )
    return -1;

  FVTermBuffer vterm_buffer{};
  vterm_buffer.print(string);
  return print (area, vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (const std::vector<FChar>& term_string) noexcept -> int
{
  if ( term_string.empty() )
    return -1;

  FVTermBuffer vterm_buffer{term_string.cbegin(), term_string.cend()};
  return print (vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, const std::vector<FChar>& term_string) const noexcept -> int
{
  if ( ! area || term_string.empty() )
    return -1;

  FVTermBuffer vterm_buffer{term_string.cbegin(), term_string.cend()};
  return print (area, vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (const FVTermBuffer& vterm_buffer) noexcept -> int
{
  if ( vterm_buffer.isEmpty() )
    return -1;

  auto area = getPrintArea();
  return area ? print (area, vterm_buffer) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, const FVTermBuffer& vterm_buffer) const noexcept -> int
{
  int len{0};

  if ( ! area || vterm_buffer.isEmpty() )
    return -1;

  for (auto&& fchar : vterm_buffer)
  {
    if ( print(area, fchar) == -1 )  // Print next character
      break;  // No area or end of area reached

    len++;
  }

  return len;
}

//----------------------------------------------------------------------
auto FVTerm::print (wchar_t c) noexcept -> int
{
  auto area = getPrintArea();
  return area ? print (area, c) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, wchar_t c) const noexcept -> int
{
  if ( ! area )
    return -1;

  FChar nc{getAttribute()};  // next character
  nc.ch[0] = c;
  nc.attr.byte[2] = 0;
  nc.attr.byte[3] = 0;
  return print (area, nc);
}

//----------------------------------------------------------------------
auto FVTerm::print (const FChar& term_char) noexcept -> int
{
  auto area = getPrintArea();
  return area ? print (area, term_char) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, const FChar& term_char) const noexcept -> int
{
  if ( ! area )
    return -1;  // No area

  if ( interpretControlCodes(area, term_char) )
    return 0;  // No printable character

  if ( ! area->checkPrintPos() || printWrap(area) )
    return -1;  // Cursor position out of range or end of area reached

  // Printing term_char on area at the current cursor position
  auto char_width = printCharacterOnCoordinate (area, term_char);

  if ( char_width == 0 && ! term_char.attr.bit.fullwidth_padding )
    return 0;

  area->cursor_x++;
  area->has_changes = true;

  // Line break at right margin
  if ( area->cursor_x > getFullAreaWidth(area) )
  {
    area->cursor_x = 1;
    area->cursor_y++;
  }
  else if ( char_width == 2 )
    printPaddingCharacter (area, term_char);

  // Prevent up scrolling
  if ( area->cursor_y > getFullAreaHeight(area) )
    area->cursor_y--;

  return 1;
}

//----------------------------------------------------------------------
void FVTerm::flush() const
{
  foutput->flush();
}


// protected methods of FVTerm
//----------------------------------------------------------------------
auto FVTerm::getPrintArea() -> FTermArea*
{
  // returns the print area of this object

  if ( print_area )
    return print_area;

  if ( vwin )
  {
    print_area = vwin.get();
    return print_area;
  }

  if ( child_print_area )
  {
    print_area = child_print_area;
    return print_area;
  }

  return vdesktop.get();
}

//----------------------------------------------------------------------
auto FVTerm::createArea (const FRect& box, const FSize& shadow) -> std::unique_ptr<FTermArea>
{
  // initialize virtual window

  auto area = std::make_unique<FTermArea>();
  area->setOwner<FVTerm*>(this);
  area->encoding = foutput->getEncoding();
  resizeArea (box, shadow, area.get());
  return area;
}

//----------------------------------------------------------------------
auto FVTerm::createArea (const FRect& box) -> std::unique_ptr<FTermArea>
{
  // initialize virtual window

  const auto no_shadow = FSize(0, 0);
  return createArea (box, no_shadow);
}

//----------------------------------------------------------------------
void FVTerm::resizeArea ( const FRect& box
                        , const FSize& shadow
                        , FTermArea* area ) const
{
  // Resize the virtual window to a new size

  const auto offset_left = box.getX();
  const auto offset_top  = box.getY();
  const auto width = int(box.getWidth());
  const auto height = int(box.getHeight());
  const auto rsw = int(shadow.getWidth());
  const auto bsh = int(shadow.getHeight());

  assert ( offset_top >= 0 && width > 0 && height > 0
        && rsw >= 0 && bsh >= 0 );

  if ( ! area )
    return;

  if ( width == area->width
    && height == area->height
    && rsw == area->right_shadow
    && bsh == area->bottom_shadow )
  {
    area->offset_left = offset_left;
    area->offset_top = offset_top;
    return;  // Move only
  }

  bool realloc_success{false};
  const std::size_t full_width = std::size_t(width) + std::size_t(rsw);
  const std::size_t full_height = std::size_t(height) + std::size_t(bsh);
  const std::size_t area_size = full_width * full_height;

  if ( getFullAreaHeight(area) != int(full_height) )
  {
    realloc_success = resizeTextArea (area, full_height, area_size);
  }
  else if ( getFullAreaWidth(area) != int(full_width) )
  {
    realloc_success = resizeTextArea (area, area_size);
  }
  else
    return;

  if ( ! realloc_success )
    return;

  area->offset_left   = offset_left;
  area->offset_top    = offset_top;
  area->width         = width;
  area->height        = height;
  area->min_width     = width;
  area->min_height    = DEFAULT_MINIMIZED_HEIGHT;
  area->right_shadow  = rsw;
  area->bottom_shadow = bsh;
  area->has_changes   = false;

  const FSize size{full_width, full_height};
  resetTextAreaToDefault (area, size);  // Set default FChar in area
}

//----------------------------------------------------------------------
void FVTerm::resizeArea (const FRect& box, FTermArea* area) const
{
  // Resize the virtual window to a new size

  const auto no_shadow = FSize(0, 0);
  resizeArea (box, no_shadow, area);
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (const FRect& box) const noexcept
{
  const auto& win_list = getWindowList();

  if ( ! vterm || ! win_list || win_list->empty()
    || box.getWidth() == 0 || box.getHeight() == 0 )
    return;

  const auto vterm_size = FSize{ std::size_t(vterm->width)
                               , std::size_t(vterm->height) };

  if ( vdesktop && vdesktop->reprint(box, vterm_size) )
    addLayer(vdesktop.get());

  for (auto&& win_obj : *win_list)
  {
    const auto& win = win_obj->getVWin();

    if ( win && win->visible && win->layer > 0 && win->reprint(box, vterm_size) )
      addLayer(win);
  }
}

//----------------------------------------------------------------------
auto FVTerm::updateVTermCursor (const FTermArea* area) const noexcept -> bool
{
  if ( ! (area && isActive(area) && area->visible) )
    return false;

  if ( area->input_cursor_visible )
  {
    // area cursor position
    const int cx = area->input_cursor_x;
    const int cy = area->input_cursor_y;
    // terminal position = area offset + area cursor position
    const int x  = area->offset_left + cx;
    const int y  = area->offset_top + cy;

    if ( isInsideArea (FPoint{cx, cy}, area)
      && isInsideTerminal (FPoint{x, y})
      && isCovered (FPoint{x, y}, area) == CoveredState::None )
    {
      vterm->setInputCursorPos(x, y);
      vterm->input_cursor_visible = true;
      vterm->has_changes = true;
      return true;
    }
  }

  vterm->input_cursor_visible = false;
  return false;
}

//----------------------------------------------------------------------
auto FVTerm::isCursorHideable() const -> bool
{
  return foutput->isCursorHideable();
}

//----------------------------------------------------------------------
void FVTerm::setAreaCursor ( const FPoint& pos
                           , bool visible
                           , FTermArea* area ) const noexcept
{
  if ( ! area )
    return;

  area->setInputCursorPos (pos.getX() - 1, pos.getY() - 1);
  area->input_cursor_visible = visible;
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FPoint& pos, FTermArea* area) const noexcept
{
  // Copies a block from the virtual terminal position to the given area

  if ( ! area )
    return;

  const int ax = pos.getX() - 1;
  const int ay = pos.getY() - 1;

  if ( ax < 0 || ay < 0 )
    return;

  int y_end  = std::min(vterm->height - ay, area->height);
  int length = std::min(vterm->width - ax, area->width);

  for (auto y{0}; y < y_end; y++)  // line loop
  {
    const auto& tc = vterm->getFChar(ax, ay + y);  // terminal character
    auto& ac = area->getFChar(0, y);  // area character
    putAreaLine (tc, ac, unsigned(length));
    auto& line_changes = area->changes[std::size_t(y)];
    line_changes.xmin = 0;
    line_changes.xmax = uInt(length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FRect& box, FTermArea* area) const noexcept
{
  // Copies a block from the virtual terminal rectangle to the given area

  if ( ! area )
    return;

  int x = box.getX() - 1;
  int y = box.getY() - 1;
  int dx = x - area->offset_left;
  int dy = y - area->offset_top;
  auto w = std::min(area->width - dx, int(box.getWidth()));
  auto h = std::min(area->height - dy, int(box.getHeight()));

  if ( x < 0 || y < 0 )
    return;

  if ( dx < 0 ) { w += dx; x -= dx; dx = 0; }
  if ( dy < 0 ) { h += dy; y -= dy; dy = 0; }
  const int y_end = std::min(vterm->height - y, h);
  const int length = std::min(vterm->width - x, w);

  if ( length < 1 )
    return;

  for (auto line{0}; line < y_end; line++)  // line loop
  {
    const auto& tc = vterm->getFChar(x, y + line);  // terminal character
    auto& ac = area->getFChar(dx, dy + line);  // area character
    putAreaLine (tc, ac, unsigned(length));
    auto& line_changes = area->changes[std::size_t(dy + line)];
    line_changes.xmin = std::min(line_changes.xmin, uInt(dx));
    line_changes.xmax = std::max(line_changes.xmax, uInt(dx + length - 1));
  }
}

//----------------------------------------------------------------------
void FVTerm::addLayer (FTermArea* area) const noexcept
{
  // Transmit changes in area to the virtual terminal

  if ( ! area || ! area->visible )
    return;

  const int ax = std::max(area->offset_left, 0);
  const int ol = std::max(0, -area->offset_left);  // Outside left
  const int ay = area->offset_top;
  const int width = getFullAreaWidth(area);
  const int height = area->minimized ? area->min_height : getFullAreaHeight(area);
  const int y_end = std::min(vterm->height - ay, height);

  // Call the preprocessing handler methods (child area change handling)
  callPreprocessingHandler(area);

  for (auto y{0}; y < y_end; y++)  // Line loop
  {
    auto& line_changes = area->changes[std::size_t(y)];
    auto line_xmin = int(line_changes.xmin);
    auto line_xmax = int(line_changes.xmax);

    if ( line_xmin > line_xmax )
      continue;

    line_xmin = std::max(line_xmin, ol);
    line_xmax = std::min(line_xmax, vterm->width + ol - ax - 1);
    const std::size_t length = unsigned(line_xmax - line_xmin + 1);
    const int tx = ax - ol;  // Global terminal positions for x
    const int ty = ay + y;  // Global terminal positions for y

    if ( ax + line_xmin >= vterm->width || tx + line_xmin + ol < 0 || ty < 0 )
      continue;

    // Area character
    const auto& ac = area->getFChar(line_xmin, y);

    // Terminal character
    auto& tc = vterm->getFChar(tx + line_xmin, ty);

    if ( line_changes.trans_count > 0 )
    {
      // Line with hidden and transparent characters
      addTransparentAreaLine (&ac, &tc, length);
    }
    else
    {
      // Line has only covered characters
      putAreaLine (ac, tc, length);
    }

    int new_xmin = ax + line_xmin - ol;
    int new_xmax = ax + line_xmax;
    auto& vterm_changes = vterm->changes[std::size_t(ty)];
    vterm_changes.xmin = std::min(vterm_changes.xmin, uInt(new_xmin));
    new_xmax = std::min(new_xmax, vterm->width - 1);
    vterm_changes.xmax = std::max (vterm_changes.xmax, uInt(new_xmax));
    line_changes.xmin = uInt(width);
    line_changes.xmax = 0;
  }

  vterm->has_changes = true;
  updateVTermCursor(area);
}

//----------------------------------------------------------------------
void FVTerm::putArea (const FPoint& pos, const FTermArea* area) const noexcept
{
  // Copies the given area block to the virtual terminal position

  if ( ! area || ! area->visible )
    return;

  copyArea (vterm.get(), pos, area);
  restoreOverlaidWindows(area);
}

//----------------------------------------------------------------------
void FVTerm::copyArea (FTermArea* dst, const FPoint& pos, const FTermArea* const src) const noexcept
{
  // Copies the src area to the dst area position

  if ( ! dst || ! src )
    return;

  if ( dst == vterm.get() )
    skip_one_vterm_update = true;

  const int src_width = getFullAreaWidth(src);
  const int src_height = src->minimized ? src->min_height : getFullAreaHeight(src);
  int ax = pos.getX() - 1;
  int ay = pos.getY() - 1;
  int ol = std::max(0, -ax);  // outside left
  int ot = std::max(0, -ay);  // outside top
  ax = std::max(0, ax);
  ay = std::max(0, ay);
  const int y_end = std::min(dst->height - ay, src_height - ot);
  const int length = std::min(dst->width - ax, src_width - ol);

  if ( length < 1 )
    return;

  for (int y{0}; y < y_end; y++)  // line loop
  {
    const int cy = ay + y;
    auto& dst_changes = dst->changes[std::size_t(cy)];
    const auto* sc = &src->getFChar(ol, ot + y);  // src character
    auto* dc = &dst->getFChar(ax, cy);  // dst character

    if ( src->changes[std::size_t(y)].trans_count > 0 )
    {
      // Line with hidden and transparent characters
      putAreaLineWithTransparency ( sc, dc, length
                                  , src, FPoint{ax, cy} );
    }
    else
    {
      // Line has only covered characters
      putAreaLine (*sc, *dc, std::size_t(length));
    }

    dst_changes.xmin = std::min(uInt(ax), dst_changes.xmin);
    dst_changes.xmax = std::max(uInt(ax + length - 1), dst_changes.xmax);
  }

  dst->has_changes = true;
}

//----------------------------------------------------------------------
void FVTerm::determineWindowLayers() noexcept
{
  // Determination of the window layer for all virtual windows

  const auto& win_list = getWindowList();

  if ( ! win_list || win_list->empty() )
    return;

  const auto& begin = win_list->cbegin();
  const auto& end = win_list->cend();
  auto iter = begin;

  while ( iter != end )
  {
    (*iter)->getPrintArea()->layer = int(std::distance(begin, iter) + 1);
    ++iter;
  }
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaForward (FTermArea* area) const
{
  // Scrolls the entire area on line up

  if ( ! area || area->height <= 1 )
    return;

  const int y_max = area->height - 1;
  const int x_max = area->width - 1;

  for (auto y{0}; y < y_max; y++)
  {
    auto& dc = area->getFChar(0, y);  // destination character
    const auto& sc = area->getFChar(0, y + 1);  // source character
    putAreaLine (sc, dc, unsigned(area->width));
    auto& line_changes = area->changes[std::size_t(y)];
    line_changes.xmin = 0;
    line_changes.xmax = uInt(x_max);
  }

  // insert a new line below
  FChar nc{};  // next character
  const auto& lc = area->getFChar(x_max, area->height - 2);  // last character
  nc = lc;
  nc.ch[0] = L' ';
  auto& dc = area->getFChar(0, y_max);  // destination character
  std::fill_n (&dc, area->width, nc);
  auto& new_line_changes = area->changes[std::size_t(y_max)];
  new_line_changes.xmin = 0;
  new_line_changes.xmax = uInt(x_max);
  area->has_changes = true;

  if ( area == vdesktop.get() )
    scrollTerminalForward();  // Scrolls the terminal up one line
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaReverse (FTermArea* area) const
{
  // Scrolls the entire area one line down

  if ( ! area || area->height <= 1 )
    return;

  const int y_max = area->height - 1;
  const int x_max = area->width - 1;

  for (auto y = y_max; y > 0; y--)
  {
    auto& dc = area->getFChar(0, y);  // destination character
    const auto& sc = area->getFChar(0, y - 1);  // source character
    putAreaLine (sc, dc, unsigned(area->width));
    auto& line_changes = area->changes[std::size_t(y)];
    line_changes.xmin = 0;
    line_changes.xmax = uInt(x_max);
  }

  // insert a new line above
  FChar nc{};  // next character
  const auto& lc = area->getFChar(0, 1);  // last character
  nc = lc;
  nc.ch[0] = L' ';
  auto& dc = area->getFChar(0, 0);  // destination character
  std::fill_n (&dc, area->width, nc);
  auto& new_line_changes = area->changes[std::size_t(y_max)];
  new_line_changes.xmin = 0;
  new_line_changes.xmax = uInt(x_max);
  area->has_changes = true;

  if ( area == vdesktop.get() )
    scrollTerminalReverse();  // Scrolls the terminal down one line
}

//----------------------------------------------------------------------
void FVTerm::clearArea (FTermArea* area, wchar_t fillchar) const noexcept
{
  // Clear the area with the current attributes

  FChar nc{getAttribute()};  // next character
  nc.ch[0] = fillchar;  // Current attributes with the fill character
  nc.attr.bit.char_width = getColumnWidth(nc.ch[0]) & 0x03;

  if ( ! area || area->data.empty() )
  {
    foutput->clearTerminal (fillchar);
    return;
  }

  const auto width = uInt(getFullAreaWidth(area));

  if ( area->right_shadow == 0 )
  {
    if ( clearFullArea(area, nc) )
      return;
  }
  else
    clearAreaWithShadow(area, nc);

  for (auto i{0}; i < area->height; i++)
  {
    auto& line_changes = area->changes[std::size_t(i)];
    line_changes.xmin = 0;
    line_changes.xmax = width - 1;

    if ( nc.attr.bit.transparent
      || nc.attr.bit.color_overlay
      || nc.attr.bit.inherit_background )
      line_changes.trans_count = width;
    else if ( area->right_shadow != 0 )
      line_changes.trans_count = uInt(area->right_shadow);
    else
      line_changes.trans_count = 0;
  }

  for (auto i{0}; i < area->bottom_shadow; i++)
  {
    const int y = area->height + i;
    auto& line_changes = area->changes[std::size_t(y)];
    line_changes.xmin = 0;
    line_changes.xmax = width - 1;
    line_changes.trans_count = width;
  }

  area->has_changes = true;
}

//----------------------------------------------------------------------
void FVTerm::forceTerminalUpdate() const
{
  force_terminal_update = true;
  processTerminalUpdate();
  flush();
  force_terminal_update = false;
}

//----------------------------------------------------------------------
auto FVTerm::processTerminalUpdate() const -> bool
{
  // Checks if the resizing of the terminal is not finished

  if ( foutput->hasTerminalResized() )
    return false;

  // Update data on VTerm
  if ( skip_one_vterm_update )
    skip_one_vterm_update = false;
  else
    updateVTerm();

  // Update the visible terminal
  return updateTerminal();
}

//----------------------------------------------------------------------
void FVTerm::startDrawing() noexcept
{
  // Pauses the terminal updates for the printing phase
  draw_completed = false;
}

//----------------------------------------------------------------------
void FVTerm::finishDrawing() noexcept
{
  // After the printing phase is completed, the terminal will be updated
  draw_completed = true;
}

//----------------------------------------------------------------------
void FVTerm::initTerminal()
{
  foutput->initTerminal(vterm.get());
  tabstop = foutput->getTabstop();
  resetAreaEncoding();
}


// private methods of FVTerm
//----------------------------------------------------------------------
void FVTerm::setGlobalFVTermInstance (FVTerm* ptr)
{
  getGlobalFVTermInstance() = ptr;
}

//----------------------------------------------------------------------
auto FVTerm::getGlobalFVTermInstance() -> FVTerm*&
{
  static FVTerm* init_object{nullptr};
  return init_object;
}

//----------------------------------------------------------------------
auto FVTerm::isInitialized() -> bool
{
  return internal::var::fvterm_initialized;
}

//----------------------------------------------------------------------
void FVTerm::resetAreaEncoding() const
{
  auto encoding = foutput->getEncoding();
  vdesktop->encoding = encoding;
  vterm->encoding = encoding;

  for (auto&& window : *window_list)
  {
    auto v_win = window->getVWin();

    if ( ! v_win )
      continue;

    v_win->encoding = encoding;

    for (const auto& pcall : v_win->preproc_list)
    {
      if ( pcall->instance && pcall->instance->child_print_area )
        pcall->instance->child_print_area->encoding = encoding;
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::resetTextAreaToDefault ( FTermArea* area
                                           , const FSize& size ) const noexcept
{
  FChar default_char;
  FLineChanges unchanged{};

  default_char.ch[0]        = L' ';
  default_char.fg_color     = FColor::Default;
  default_char.bg_color     = FColor::Default;
  memset (&default_char.attr.byte, 0, sizeof(default_char.attr.byte));
  default_char.attr.bit.char_width = 1;

  std::fill_n (area->data.begin(), size.getArea(), default_char);

  unchanged.xmin = uInt(size.getWidth());
  unchanged.xmax = 0;
  unchanged.trans_count = 0;

  std::fill_n (area->changes.begin(), size.getHeight(), unchanged);
}

//----------------------------------------------------------------------
inline auto FVTerm::resizeTextArea ( FTermArea* area
                                   , std::size_t height
                                   , std::size_t size ) const -> bool
{
  // Set the number of lines for changes to "height"
  // and resize the text area to "size" elements

  area->changes.resize(height);
  area->changes.shrink_to_fit();
  resizeTextArea (area, size);
  return true;
}

//----------------------------------------------------------------------
inline auto FVTerm::resizeTextArea (FTermArea* area, std::size_t size) const -> bool
{
  // Resize text area to "size" FChar elements

  area->data.resize(size);
  area->data.shrink_to_fit();
  return true;
}

//----------------------------------------------------------------------
auto FVTerm::isCovered (const FPoint& pos, const FTermArea* area) const noexcept -> CoveredState
{
  // Determines the covered state for the given position

  const auto& win_list = getWindowList();
  auto is_covered = CoveredState::None;
  bool found{ area == vdesktop.get() };

  if ( ! area || ! win_list || win_list->empty()
    || win_list->back()->getVWin() == area )
    return CoveredState::None;

  for (const auto& win_obj : *win_list)
  {
    const auto& win = win_obj->getVWin();

    if ( ! (win && win->visible) )
      continue;

    if ( found && win->contains(pos) )  // is covered
    {
      const auto& tmp = win->getFChar( pos.getX() - win->offset_left
                                     , pos.getY() - win->offset_top );

      if ( tmp.attr.bit.color_overlay )
      {
        is_covered = CoveredState::Half;
      }
      else if ( ! tmp.attr.bit.transparent )
      {
        return CoveredState::Full;
      }
    }

    if ( area == win )
      found = true;
  }

  return is_covered;
}

//----------------------------------------------------------------------
constexpr auto FVTerm::getFullAreaWidth (const FTermArea* area) const noexcept -> int
{
  return area->width + area->right_shadow;
}

//----------------------------------------------------------------------
constexpr auto FVTerm::getFullAreaHeight (const FTermArea* area) const noexcept -> int
{
  return area->height + area->bottom_shadow;
}

//----------------------------------------------------------------------
void FVTerm::passChangesToOverlap (const FTermArea* area) const
{
  const auto& win_list = getWindowList();
  bool found{false};

  if ( ! area || ! win_list || win_list->empty() )
    return;

  for (auto&& win_obj : *win_list)
  {
    const auto& win = win_obj->getVWin();

    if ( found && win && win->visible && win->isOverlapped(area) )
    {
      // Pass the changes to the overlapping window

      win->has_changes = true;
      const int win_offset_top = win->offset_top;
      const int y_start = std::max(0, std::max(area->offset_top, win_offset_top)) - win_offset_top;
      const int area_height = area->minimized ? area->min_height : getFullAreaHeight(area);
      const int win_height = win->minimized ? win->min_height : getFullAreaHeight(win);
      const int area_y2 = area->offset_top + area_height - 1;
      const int win_y2 = win_offset_top + win_height - 1;
      const int y_end = std::min(vterm->height - 1, std::min(area_y2, win_y2)) - win_offset_top;

      for (auto y{y_start}; y <= y_end; y++)  // Line loop
      {
        const int win_offset_left = win->offset_left;
        const int x_start = std::max(0, std::max(area->offset_left, win_offset_left)) - win_offset_left;
        const int area_x2 = area->offset_left + area->width + area->right_shadow - 1;
        const int win_x2 = win_offset_left + win->width + win->right_shadow - 1;
        const int x_end = std::min(vterm->width - 1, std::min(area_x2, win_x2)) - win_offset_left;
        auto& line_changes = win->changes[std::size_t(y)];
        line_changes.xmin = std::min(int(line_changes.xmin), x_start);
        line_changes.xmax = std::max(int(line_changes.xmax), x_end);
      }
    }

    if ( win == area )
      found = true;
  }
}

//----------------------------------------------------------------------
void FVTerm::restoreOverlaidWindows (const FTermArea* area) const noexcept
{
  // Restoring overlaid windows

  const auto& win_list = getWindowList();

  if ( ! win_list || win_list->empty() )
    return;

  bool overlaid{false};

  for (auto&& win_obj : *win_list)
  {
    const auto& win = win_obj->getVWin();

    if ( overlaid && win && win->visible && win->isOverlapped(area) )
      copyArea (vterm.get(), FPoint{win->offset_left + 1, win->offset_top + 1}, win);
    else if ( getVWin() == win )
      overlaid = true;
  }
}

//----------------------------------------------------------------------
void FVTerm::updateVTerm() const
{
  // Updates the character data from all areas to VTerm

  if ( hasPendingUpdates(vdesktop.get()) )
  {
    addLayer(vdesktop.get());
    vdesktop->has_changes = false;
  }

  if ( ! window_list || window_list->empty() )
    return;

  for (auto&& window : *window_list)  // List from bottom to top
  {
    auto v_win = window->getVWin();

    if ( ! (v_win && v_win->visible && v_win->layer > 0) )
      continue;

    if ( hasPendingUpdates(v_win) )
    {
      passChangesToOverlap(v_win);
      addLayer(v_win);
      v_win->has_changes = false;
    }
    else if ( hasChildAreaChanges(v_win) )
    {
      passChangesToOverlap(v_win);
      addLayer(v_win);  // and call the child area processing handler there
      clearChildAreaChanges(v_win);
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::scrollTerminalForward() const
{
  // Scrolls the terminal up one line

  forceTerminalUpdate();  // Empty buffer before scrolling

  if ( ! foutput->scrollTerminalForward() )
    return;

  const int y_max = vdesktop->height - 1;

  // avoid update lines from 0 to (y_max - 1)
  for (auto y{0}; y < y_max; y++)
  {
    auto& vdesktop_changes = vdesktop->changes[std::size_t(y)];
    vdesktop_changes.xmin = uInt(vdesktop->width - 1);
    vdesktop_changes.xmax = 0;
  }

  putArea (FPoint{1, 1}, vdesktop.get());
}

//----------------------------------------------------------------------
inline void FVTerm::scrollTerminalReverse() const
{
  // Scrolls the terminal down one line

  forceTerminalUpdate();  // Empty buffer before scrolling

  if ( ! foutput->scrollTerminalReverse() )
    return;

  const int y_max = vdesktop->height - 1;

  // avoid update lines from 1 to y_max
  for (auto y{0}; y < y_max; y++)
  {
    auto& vdesktop_changes = vdesktop->changes[std::size_t(y + 1)];
    vdesktop_changes.xmin = uInt(vdesktop->width - 1);
    vdesktop_changes.xmax = 0;
  }

  putArea (FPoint{1, 1}, vdesktop.get());
}

//----------------------------------------------------------------------
void FVTerm::callPreprocessingHandler (const FTermArea* area) const
{
  // Call preprocessing handler

  if ( ! area || area->preproc_list.empty() )
    return;

  for (auto&& pcall : area->preproc_list)
  {
    // call the preprocessing handler
    auto preprocessingHandler = pcall->function;
    preprocessingHandler();
  }
}

//----------------------------------------------------------------------
auto FVTerm::hasChildAreaChanges (const FTermArea* area) const -> bool
{
  if ( ! area || area->preproc_list.empty() )
    return false;

  return std::any_of ( area->preproc_list.cbegin()
                     , area->preproc_list.cend()
                     , [] (const auto& pcall)
                       {
                         return pcall->instance
                             && pcall->instance->child_print_area
                             && pcall->instance->child_print_area->has_changes;
                       }
                     );
}

//----------------------------------------------------------------------
void FVTerm::clearChildAreaChanges (const FTermArea* area) const
{
  if ( ! area || area->preproc_list.empty() )
    return;

  for (auto&& pcall : area->preproc_list)
  {
    if ( pcall->instance && pcall->instance->child_print_area )
      pcall->instance->child_print_area->has_changes = false;
  }
}

//----------------------------------------------------------------------
auto FVTerm::isInsideArea (const FPoint& pos, const FTermArea* area) const -> bool
{
  // Check whether the coordinates are within the area

  const int x = pos.getX();
  const int y = pos.getY();
  return x >= 0 && x < area->width
      && y >= 0 && y < area->height;
}

//----------------------------------------------------------------------
inline auto FVTerm::isTransparentInvisible (const FChar& fchar) const -> bool
{
  const auto& fist_char = fchar.ch[0];
  return ( fist_char == UniChar::LowerHalfBlock
        || fist_char == UniChar::UpperHalfBlock
        || fist_char == UniChar::LeftHalfBlock
        || fist_char == UniChar::RightHalfBlock
        || fist_char == UniChar::MediumShade
        || fist_char == UniChar::FullBlock );
}

//----------------------------------------------------------------------
auto FVTerm::getCharacter ( CharacterType char_type
                          , const FPoint& pos
                          , const FTermArea* area ) const -> FChar&
{
  // Gets the overlapped or the covered character for a given position

  int xx = std::max(pos.getX(), 0);
  int yy = std::max(pos.getY(), 0);
  xx = std::min(xx, vterm->width - 1);
  yy = std::min(yy, vterm->height - 1);
  auto cc = &vdesktop->getFChar(xx, yy);  // covered character
  const auto& win_list = getWindowList();

  if ( ! area || ! win_list || win_list->empty() )
    return *cc;

  // Get the window layer of this widget object
  const auto has_an_owner = area->hasOwner();
  auto& area_owner = *area->getOwner<FVTerm*>();
  const int layer = has_an_owner ? getLayer(area_owner) : 0;

  for (auto&& win_obj : *win_list)
  {
    // char_type can be "overlapped_character" or "covered_character"
    bool significant_char = (char_type == CharacterType::Covered)
                          ? (layer >= getLayer(*win_obj))
                          : (layer < getLayer(*win_obj));

    if ( significant_char && has_an_owner && win_obj != &area_owner )
    {
      const auto& win = win_obj->getVWin();

      // Window visible and contains current character
      if ( win && win->visible && win->contains(pos) )
        getAreaCharacter (pos, win, cc);
    }
    else if ( char_type == CharacterType::Covered )
      break;
  }

  return *cc;
}

//----------------------------------------------------------------------
inline auto FVTerm::getCoveredCharacter (const FPoint& pos, const FTermArea* area) const -> FChar&
{
  // Gets the covered character for a given position
  return getCharacter (CharacterType::Covered, pos, area);
}

//----------------------------------------------------------------------
void FVTerm::defineByte1TransparentMask()
{
  FAttribute mask{};
  mask.bit.transparent = true;
  mask.bit.color_overlay = true;
  mask.bit.inherit_background = true;
  internal::var::b1_transparent_mask = mask.byte[1];
}

//----------------------------------------------------------------------
void FVTerm::initSettings()
{
  // Presetting of the current locale for full-width character support.
  // The final setting is made later in FTerm::init_locale().
  std::setlocale (LC_ALL, "");

  // Initialize the printing attribute
  FVTermAttribute::initAttribute();

  // Create virtual terminal
  FSize term_size {foutput->getColumnNumber(), foutput->getLineNumber()};
  createVTerm (term_size);

  // Create virtual desktop area
  createVDesktop (term_size);
  active_area = vdesktop.get();

  // fvterm is now initialized
  internal::var::fvterm_initialized = true;
}

//----------------------------------------------------------------------
void FVTerm::finish() const
{
  // Resetting the terminal
  setNormal();
  foutput->finishTerminal();
  forceTerminalUpdate();
  internal::var::fvterm_initialized = false;
  setGlobalFVTermInstance(nullptr);
}

//----------------------------------------------------------------------
inline void FVTerm::putAreaLine (const FChar& src_char, FChar& dst_char, const std::size_t length) const
{
  // copy "length" characters from area to terminal

  std::memcpy (&dst_char, &src_char, length * sizeof(dst_char));
}

//----------------------------------------------------------------------
inline void FVTerm::addTransparentAreaLine ( const FChar* src_char
                                           , FChar* dst_char
                                           , const std::size_t length ) const
{
  const auto end_char = src_char + length;
  const auto last_char = end_char - 1;
  const FChar* start_char{nullptr};
  std::size_t non_trans_count{0};

  for (; src_char < end_char; src_char++)  // column loop
  {
    const auto isTransparent = \
        bool((src_char->attr.byte[1] & internal::var::b1_transparent_mask) != 0);

    if ( ! isTransparent )
    {
      if ( non_trans_count == 0 )
        start_char = src_char;

      non_trans_count++;
    }
    else if ( non_trans_count != 0 )
    {
      putAreaLine (*start_char, *dst_char, non_trans_count);
      dst_char += non_trans_count;  // dst character
      non_trans_count = 0;
      addTransparentAreaChar (*src_char, *dst_char);
      ++dst_char;  // dst character
    }
    else
    {
      addTransparentAreaChar (*src_char, *dst_char);
      ++dst_char;  // dst character
    }

    if ( src_char == last_char && non_trans_count != 0 )
    {
      putAreaLine (*start_char, *dst_char, non_trans_count);
      break;
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::addTransparentAreaChar (const FChar& src_char, FChar& dst_char) const
{
  if ( src_char.attr.bit.transparent )  // Transparent
  {
    // Leave character on vterm untouched
    dst_char.attr.bit.printed = false;
  }
  else if ( src_char.attr.bit.color_overlay )  // Transparent shadow
  {
    // Get covered character + add the current color
    dst_char.fg_color = src_char.fg_color;
    dst_char.bg_color = src_char.bg_color;
    std::memcpy ( &dst_char.attr.byte[0]
                , &src_char.attr.byte[0]
                , sizeof(src_char.attr.byte) );
    dst_char.attr.bit.color_overlay  = false;
    dst_char.attr.bit.reverse  = false;
    dst_char.attr.bit.standout = false;

    if ( isTransparentInvisible(dst_char) )
      dst_char.ch[0] = L' ';
  }
  else if ( src_char.attr.bit.inherit_background )
  {
    // Add the covered background to this character
    auto bg_color = dst_char.bg_color;
    dst_char = src_char;
    dst_char.bg_color = bg_color;
  }
  else  // Default
    dst_char = src_char;
}

//----------------------------------------------------------------------
inline void FVTerm::putTransparentAreaLine ( const FChar& src_char
                                           , FChar& dst_char
                                           , const std::size_t length
                                           , const FTermArea* src_area
                                           , FPoint start_pos ) const
{
  auto src = &src_char;
  auto dst = &dst_char;
  const auto end = src + length;
  start_pos.x_ref() -= int(length);

  for (; src < end; ++src)  // column loop
  {
    putAreaCharacter (start_pos, src_area, *src, *dst);
    start_pos.x_ref()++;
    ++dst;  // dst character
  }
}

//----------------------------------------------------------------------
inline void FVTerm::putAreaLineWithTransparency ( const FChar* src_char
                                                , FChar* dst_char
                                                , const int length
                                                , const FTermArea* src_area
                                                , FPoint&& pos ) const
{
  const int end_char = pos.getX() + length;
  const FChar* start_char{nullptr};
  std::size_t trans_count{0};
  std::size_t non_trans_count{0};

  // Line has one or more transparent characters
  while ( pos.getX() < end_char )  // column loop
  {
    const auto isTransparent = \
        bool((src_char->attr.byte[1] & internal::var::b1_transparent_mask) != 0);

    if ( isTransparent && non_trans_count != 0 )
    {
      putAreaLine (*start_char, *dst_char, non_trans_count);
      dst_char += non_trans_count;  // dst character
      non_trans_count = 0;
    }
    else if ( ! isTransparent && trans_count != 0 )
    {
      putTransparentAreaLine (*start_char, *dst_char, trans_count, src_area, pos);
      dst_char += trans_count;  // dst character
      trans_count = 0;
    }

    if ( isTransparent )
    {
      if ( trans_count == 0 )
        start_char = src_char;

      trans_count++;
    }
    else
    {
      if ( non_trans_count == 0 )
        start_char = src_char;

      non_trans_count++;
    }

    pos.x_ref()++;

    if ( pos.getX() == end_char )
    {
      if ( trans_count != 0 )
        putTransparentAreaLine (*start_char, *dst_char, trans_count , src_area, pos);
      else if ( non_trans_count != 0 )
        putAreaLine (*start_char, *dst_char, non_trans_count);

      break;
    }

    ++src_char;  // src character
  }
}

//----------------------------------------------------------------------
void FVTerm::putAreaCharacter ( const FPoint& pos, const FTermArea* area
                              , const FChar& src_char, FChar& dst_char ) const
{
  if ( src_char.attr.bit.transparent )  // Transparent
  {
    // Restore one character on vterm
    dst_char = getCoveredCharacter (pos, area);
  }
  else if ( src_char.attr.bit.color_overlay )  // Transparent shadow
  {
    // Get covered character + add the current color
    dst_char = getCoveredCharacter (pos, area);
    dst_char.fg_color = src_char.fg_color;
    dst_char.bg_color = src_char.bg_color;
    dst_char.attr.bit.reverse  = false;
    dst_char.attr.bit.standout = false;

    if ( isTransparentInvisible(dst_char) )
      dst_char.ch[0] = L' ';
  }
  else if ( src_char.attr.bit.inherit_background )
  {
    // Add the covered background to this character
    dst_char = src_char;
    dst_char.bg_color = getCoveredCharacter(pos, area).bg_color;
  }
  else  // Default
    dst_char = src_char;
}

//----------------------------------------------------------------------
void FVTerm::getAreaCharacter ( const FPoint& pos, FTermArea* area
                              , FChar*& cc ) const
{
  const int area_x = area->offset_left;
  const int area_y = area->offset_top;
  const int x = pos.getX();
  const int y = pos.getY();
  auto& tmp = area->getFChar(x - area_x, y - area_y);

  // Current character not transparent
  if ( tmp.attr.bit.transparent )
    return;

  if ( tmp.attr.bit.color_overlay )  // transparent shadow
  {
    // Keep the current vterm character
    s_ch = *cc;
    s_ch.fg_color = tmp.fg_color;
    s_ch.bg_color = tmp.bg_color;
    s_ch.attr.bit.reverse  = false;
    s_ch.attr.bit.standout = false;
    cc = &s_ch;
  }
  else if ( tmp.attr.bit.inherit_background )
  {
    // Add the covered background to this character
    i_ch = tmp;
    i_ch.bg_color = cc->bg_color;  // last background color
    cc = &i_ch;
  }
  else  // default
    cc = &tmp;
}

//----------------------------------------------------------------------
auto FVTerm::clearFullArea (FTermArea* area, FChar& nc) const -> bool
{
  // Clear area
  const int area_size = area->width * area->height;
  std::fill_n (area->data.begin(), area_size, nc);

  if ( area != vdesktop.get() )  // Is the area identical to the desktop?
    return false;

  // Try to clear the terminal rapidly with a control sequence
  if ( foutput->clearTerminal (nc.ch[0]) )
  {
    nc.attr.bit.printed = true;
    std::fill_n (vterm->data.begin(), area_size, nc);
  }
  else
  {
    for (auto i{0}; i < vdesktop->height; i++)
    {
      auto& vdesktop_changes = vdesktop->changes[std::size_t(i)];
      vdesktop_changes.xmin = 0;
      vdesktop_changes.xmax = uInt(vdesktop->width) - 1;
      vdesktop_changes.trans_count = 0;
    }

    vdesktop->has_changes = true;
  }

  return true;
}

//----------------------------------------------------------------------
void FVTerm::clearAreaWithShadow (FTermArea* area, const FChar& nc) const noexcept
{
  FChar t_char = nc;
  t_char.ch[0] = L'\0';
  t_char.attr.bit.transparent = true;
  t_char.attr.bit.char_width = 0;
  const int total_width = getFullAreaWidth(area);

  for (auto y{0}; y < area->height; y++)
  {
    // Clear area
    std::fill_n (&area->getFChar(0, y), total_width, nc);
    // Make right shadow transparent
    std::fill_n (&area->getFChar(area->width, y), area->right_shadow, t_char);
  }

  // Make bottom shadow transparent
  for (auto y{0}; y < area->bottom_shadow; y++)
  {
    std::fill_n (&area->getFChar(0, y + area->height), total_width, t_char);
  }
}

//----------------------------------------------------------------------
auto FVTerm::printWrap (FTermArea* area) const -> bool
{
  bool end_of_area{false};
  const int width  = area->width;
  const int height = area->height;
  const int rsh    = area->right_shadow;
  const int bsh    = area->bottom_shadow;

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
auto FVTerm::getByte1PrintTransMask() const -> uInt8
{
  // Set bits that must not be reset
  FAttribute mask{};
  mask.bit.transparent = true;
  mask.bit.color_overlay = true;
  mask.bit.inherit_background = true;
  mask.bit.no_changes = true;
  mask.bit.printed = true;
  return mask.byte[1];
}

//----------------------------------------------------------------------
inline auto FVTerm::changedToTransparency (const FChar& from, const FChar& to) const -> bool
{
  return ( uInt8(~ from.attr.byte[1] & b1_print_trans_mask)
         & uInt8(    to.attr.byte[1] & b1_print_trans_mask) ) != 0;
}

//----------------------------------------------------------------------
inline auto FVTerm::changedFromTransparency (const FChar& from, const FChar& to) const -> bool
{
  return changedToTransparency(to, from);
}

//----------------------------------------------------------------------
inline auto FVTerm::printCharacterOnCoordinate ( FTermArea* area
                                               , const FChar& ch) const noexcept -> std::size_t
{
  const int ax = area->cursor_x - 1;
  const int ay = area->cursor_y - 1;
  auto& ac = area->getFChar(ax, ay);  // area character

  if ( ac == ch )  // compare with an overloaded operator
    return ac.attr.bit.char_width;

  auto& line_changes = area->changes[std::size_t(ay)];

  if ( changedToTransparency(ac, ch) )
  {
    // add one transparent character form line
    line_changes.trans_count++;
  }

  if ( changedFromTransparency(ac, ch) )
  {
    // remove one transparent character from line
    line_changes.trans_count--;
  }

  // copy character to area
  ac = ch;

  if ( ac.attr.bit.char_width == 0 )
  {
    const auto new_char_width = getColumnWidth(ac.ch[0]);

    if ( new_char_width == 0 )
      return 0;

    addColumnWidth(ac, new_char_width);  // Add column width
  }

  const auto padding = int(ac.attr.bit.char_width == 2);

  if ( ax < int(line_changes.xmin) )
    line_changes.xmin = uInt(ax);

  if ( ax + padding > int(line_changes.xmax) )
    line_changes.xmax = uInt(ax + padding);

  return ac.attr.bit.char_width;
}

//----------------------------------------------------------------------
inline void FVTerm::printPaddingCharacter (FTermArea* area, const FChar& term_char) const
{
  // Creates a padding-character from the current character (term_char)
  // and prints it. It is a placeholder for the column after
  // a full-width character.

  FChar pc{term_char};  // Copy character to padding character

  if ( area->encoding == Encoding::UTF8 )
  {
    pc.ch = {{ L'\0' }};
    pc.attr.bit.fullwidth_padding = true;
    pc.attr.bit.char_width = 0;
  }
  else
  {
    pc.ch[0] = L'.';
    pc.ch[1] = L'\0';
    pc.attr.bit.char_width = 1;
  }

  // Print the padding-character
  print (area, pc);
}

//----------------------------------------------------------------------
auto FVTerm::isInsideTerminal (const FPoint& pos) const noexcept -> bool
{
  // Check whether the coordinates are within the virtual terminal

  const auto x = pos.getX();
  const auto y = pos.getY();
  return x >= 0 && x < int(foutput->getColumnNumber())
      && y >= 0 && y < int(foutput->getLineNumber());
}

//----------------------------------------------------------------------
auto FVTerm::hasPendingUpdates (const FTermArea* area) noexcept -> bool
{
  return (area && area->has_changes);
}

}  // namespace finalcut
