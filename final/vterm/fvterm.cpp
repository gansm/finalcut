/***********************************************************************
* fvterm.cpp - Virtual terminal implementation                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2022 Markus Gans                                      *
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

// static class attributes
bool                 FVTerm::draw_completed{false};
bool                 FVTerm::no_terminal_updates{false};
bool                 FVTerm::force_terminal_update{false};
const FVTerm*        FVTerm::init_object{nullptr};
FVTerm::FTermArea*   FVTerm::vterm{nullptr};
FVTerm::FTermArea*   FVTerm::vdesktop{nullptr};
FVTerm::FTermArea*   FVTerm::active_area{nullptr};
FChar                FVTerm::s_ch{};
FChar                FVTerm::i_ch{};
uInt8                FVTerm::b1_trans_mask{};
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
  if ( init_object == this )
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
  init_object->foutput->setNonBlockingRead (enable);
}

//----------------------------------------------------------------------
void FVTerm::clearArea (wchar_t fillchar)
{
  clearArea (vwin, fillchar);
}

//----------------------------------------------------------------------
void FVTerm::createVTerm (const FSize& size) noexcept
{
  // initialize virtual terminal

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  const FSize shadow{0, 0};
  createArea (box, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (const FSize& size) const noexcept
{
  // resize virtual terminal

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  const FSize shadow{0, 0};
  resizeArea (box, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::putVTerm() const
{
  for (auto i{0}; i < vterm->height; i++)
  {
    auto& vterm_changes = vterm->changes[i];
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
auto FVTerm::print (FTermArea* area, const FString& string) noexcept -> int
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
auto FVTerm::print (FTermArea* area, const std::vector<FChar>& term_string) noexcept -> int
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
auto FVTerm::print (FTermArea* area, const FVTermBuffer& vterm_buffer) noexcept -> int
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
auto FVTerm::print (FTermArea* area, wchar_t c) noexcept -> int
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
auto FVTerm::print (FTermArea* area, const FChar& term_char) noexcept -> int
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
void FVTerm::print (const FPoint& p)
{
  setCursor (p);
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
    print_area = vwin;
    return print_area;
  }

  if ( child_print_area )
  {
    print_area = child_print_area;
    return print_area;
  }

  return vdesktop;
}

//----------------------------------------------------------------------
void FVTerm::createArea ( const FRect& box
                        , const FSize& shadow
                        , FTermArea*& area )
{
  // initialize virtual window

  try
  {
    area = new FTermArea;
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FTermArea");
    return;
  }

  area->setOwner<FVTerm*>(this);
  resizeArea (box, shadow, area);
}

//----------------------------------------------------------------------
void FVTerm::createArea (const FRect& box, FTermArea*& area)
{
  // initialize virtual window

  const auto no_shadow = FSize(0, 0);
  createArea (box, no_shadow, area);
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
    if ( offset_left != area->offset_left )
      area->offset_left = offset_left;

    if ( offset_top != area->offset_top )
      area->offset_top = offset_top;

    return;  // Move only
  }

  bool realloc_success{false};
  const std::size_t full_width = std::size_t(width) + std::size_t(rsw);
  const std::size_t full_height = std::size_t(height) + std::size_t(bsh);
  const std::size_t area_size = full_width * full_height;

  if ( getFullAreaHeight(area) != int(full_height) )
  {
    realloc_success = reallocateTextArea ( area
                                         , full_height
                                         , area_size );
  }
  else if ( getFullAreaWidth(area) != int(full_width) )
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
void FVTerm::removeArea (FTermArea*& area)
{
  // remove the virtual window

  if ( area == nullptr )
    return;

  delete area;
  area = nullptr;
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (const FRect& box) noexcept
{
  if ( ! vterm )
    return;

  int x = box.getX() - 1;
  int y = box.getY() - 1;
  auto w = int(box.getWidth());
  auto h = int(box.getHeight());

  if ( x < 0 )
    x = 0;

  if ( y < 0 )
    y = 0;

  if ( x + w > vterm->width )
    w = vterm->width - x;

  if ( y + h > vterm->height )
    h = vterm->height - y;

  if ( w < 0 || h < 0 )
    return;

  for (auto ty{0}; ty < h; ty++)
  {
    const int ypos = y + ty;

    for (auto tx{0}; tx < w; tx++)
    {
      const int xpos = x + tx;
      auto& tc = vterm->getFChar(xpos, ypos);  // terminal character
      auto sc = generateCharacter(FPoint{xpos, ypos});  // shown character
      std::memcpy (&tc, &sc, sizeof(tc));
    }

    auto& vterm_changes = vterm->changes[ypos];

    if ( int(vterm_changes.xmin) > x )
      vterm_changes.xmin = uInt(x);

    if ( int(vterm_changes.xmax) < x + w - 1 )
      vterm_changes.xmax = uInt(x + w - 1);
  }

  vterm->has_changes = true;
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
                           , FTermArea* area ) noexcept
{
  if ( ! area )
    return;

  area->setInputCursorPos (pos.getX() - 1, pos.getY() - 1);
  area->input_cursor_visible = visible;
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FPoint& pos, FTermArea* area) noexcept
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
    std::memcpy (&ac, &tc, sizeof(ac) * unsigned(length));
    auto& line_changes = area->changes[y];

    if ( int(line_changes.xmin) > 0 )
      line_changes.xmin = 0;

    if ( int(line_changes.xmax) < length - 1 )
      line_changes.xmax = uInt(length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FRect& box, FTermArea* area) noexcept
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

  for (auto line = 0; line < y_end; line++)  // line loop
  {
    const auto& tc = vterm->getFChar(x, y + line);  // terminal character
    auto& ac = area->getFChar(dx, dy + line);  // area character
    std::memcpy (&ac, &tc, sizeof(ac) * unsigned(length));
    auto line_changes = area->changes[dy + line];

    if ( int(line_changes.xmin) > dx )
      line_changes.xmin = uInt(dx);

    if ( int(line_changes.xmax) < dx + length - 1 )
      line_changes.xmax = uInt(dx + length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::putArea (FTermArea* area) const noexcept
{
  // Transmit changes in the area to the virtual terminal

  if ( ! area || ! area->visible )
    return;

  int ax  = area->offset_left;
  const int ay  = area->offset_top;
  const int width = getFullAreaWidth(area);
  const int height = area->minimized ? area->min_height : getFullAreaHeight(area);
  int ol{0};  // Outside left


  // Call the preprocessing handler methods
  callPreprocessingHandler(area);

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  int y_end = std::min(vterm->height - ay, height);

  for (auto y{0}; y < y_end; y++)  // Line loop
  {
    bool modified{false};
    auto& line_changes = area->changes[y];
    auto line_xmin = int(line_changes.xmin);
    auto line_xmax = int(line_changes.xmax);

    if ( line_xmin > line_xmax )
      continue;

    if ( ax == 0 )
      line_xmin = ol;

    if ( width + ax - ol >= vterm->width )
      line_xmax = vterm->width + ol - ax - 1;

    if ( ax + line_xmin >= vterm->width )
      continue;

    for (auto x = line_xmin; x <= line_xmax; x++)  // Column loop
    {
      // Global terminal positions
      int tx = ax + x;
      const int ty = ay + y;

      if ( tx < 0 || ty < 0 )
        continue;

      tx -= ol;
      bool update = updateVTermCharacter(area, FPoint{x, y}, FPoint{tx, ty});

      if ( ! modified && ! update )
        line_xmin++;  // Don't update covered character

      if ( update )
        modified = true;
    }

    int new_xmin = ax + line_xmin - ol;
    int new_xmax = ax + line_xmax;
    auto& vterm_changes = vterm->changes[ay + y];

    if ( new_xmin < int(vterm_changes.xmin) )
      vterm_changes.xmin = uInt(new_xmin);

    if ( new_xmax >= vterm->width )
      new_xmax = vterm->width - 1;

    if ( new_xmax > int(vterm_changes.xmax) )
      vterm_changes.xmax = uInt(new_xmax);

    line_changes.xmin = uInt(width);
    line_changes.xmax = 0;
  }

  vterm->has_changes = true;
  updateVTermCursor(area);
}

//----------------------------------------------------------------------
void FVTerm::putArea (const FPoint& pos, const FTermArea* area) noexcept
{
  // Copies the given area block to the virtual terminal position

  if ( area && area->visible )
    copyArea (vterm, pos, area);
}

//----------------------------------------------------------------------
void FVTerm::copyArea (FTermArea* dst, const FPoint& pos, const FTermArea* src) noexcept
{
  // Copies the src area to the dst area position

  if ( ! dst || ! src )
    return;

  int ax = pos.getX() - 1;
  int ay = pos.getY() - 1;
  const int width = getFullAreaWidth(src);
  const int height = src->minimized ? src->min_height : getFullAreaHeight(src);
  int ol{0};  // outside left
  int ot{0};  // outside top

  if ( ax < 0 )
  {
    ol = std::abs(ax);
    ax = 0;
  }

  if ( ay < 0 )
  {
    ot = std::abs(ay);
    ay = 0;
  }

  int y_end = std::min(dst->height - ay, height - ot);
  int length = std::min(dst->width - ax, width - ol);

  if ( length < 1 )
    return;

  for (auto y{0}; y < y_end; y++)  // line loop
  {
    if ( src->changes[y].trans_count == 0 )
    {
      // Line has only covered characters
      const auto& sc = src->getFChar(ol, ot + y);  // src character
      auto& dc = dst->getFChar(ax, ay + y);  // dst character
      putAreaLine (sc, dc, std::size_t(length));
    }
    else
    {
      // Line has one or more transparent characters
      for (auto x{0}; x < length; x++)  // column loop
      {
        const int cx = ax + x;
        const int cy = ay + y;
        const auto& sc = src->getFChar(ol + x, ot + y);  // src character
        auto& dc = dst->getFChar(cx, cy);  // dst character
        putAreaCharacter (FPoint{cx, cy}, src, sc, dc);
      }
    }

    auto& dst_changes = dst->changes[ay + y];

    if ( ax < int(dst_changes.xmin) )
      dst_changes.xmin = uInt(ax);

    if ( ax + length - 1 > int(dst_changes.xmax) )
      dst_changes.xmax = uInt(ax + length - 1);
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
    std::memcpy (&dc, &sc, sizeof(dc) * unsigned(area->width));
    auto& line_changes = area->changes[y];
    line_changes.xmin = 0;
    line_changes.xmax = uInt(x_max);
  }

  // insert a new line below
  FChar nc{};  // next character
  const auto& lc = area->getFChar(x_max, area->height - 2);  // last character
  std::memcpy (&nc, &lc, sizeof(nc));
  nc.ch[0] = L' ';
  auto& dc = area->getFChar(0, y_max);  // destination character
  std::fill_n (&dc, area->width, nc);
  auto& new_line_changes = area->changes[y_max];
  new_line_changes.xmin = 0;
  new_line_changes.xmax = uInt(x_max);
  area->has_changes = true;

  if ( area == vdesktop )
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
    std::memcpy (&dc, &sc, sizeof(dc) * unsigned(area->width));
    auto& line_changes = area->changes[y];
    line_changes.xmin = 0;
    line_changes.xmax = uInt(x_max);
  }

  // insert a new line above
  FChar nc{};  // next character
  const auto& lc = area->getFChar(0, 1);  // last character
  std::memcpy (&nc, &lc, sizeof(nc));
  nc.ch[0] = L' ';
  auto& dc = area->getFChar(0, 0);  // destination character
  std::fill_n (&dc, area->width, nc);
  auto& new_line_changes = area->changes[y_max];
  new_line_changes.xmin = 0;
  new_line_changes.xmax = uInt(x_max);
  area->has_changes = true;

  if ( area == vdesktop )
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
    auto& line_changes = area->changes[i];
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
    auto& line_changes = area->changes[y];
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
  foutput->initTerminal(vterm);
  tabstop = foutput->getTabstop();
}


// private methods of FVTerm
//----------------------------------------------------------------------
inline void FVTerm::resetTextAreaToDefault ( FTermArea* area
                                           , const FSize& size ) const noexcept
{
  FChar default_char;
  FLineChanges unchanged{};

  default_char.ch[0]        = L' ';
  default_char.fg_color     = FColor::Default;
  default_char.bg_color     = FColor::Default;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;
  default_char.attr.byte[2] = 8;  // char_width = 1
  default_char.attr.byte[3] = 0;

  std::fill_n (area->data.begin(), size.getArea(), default_char);

  unchanged.xmin = uInt(size.getWidth());
  unchanged.xmax = 0;
  unchanged.trans_count = 0;

  std::fill_n (area->changes.begin(), size.getHeight(), unchanged);
}

//----------------------------------------------------------------------
inline auto FVTerm::reallocateTextArea ( FTermArea* area
                                       , std::size_t height
                                       , std::size_t size ) -> bool
{
  // Reallocate "height" lines for changes
  // and "size" bytes for the text area

  try
  {
    area->changes.resize(height);
    area->data.resize(size);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FLineChanges[height] or FChar[size]");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
inline auto FVTerm::reallocateTextArea (FTermArea* area, std::size_t size) -> bool
{
  // Reallocate "size" bytes for the text area

  try
  {
    area->data.resize(size);
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FChar[size]");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
auto FVTerm::isCovered (const FPoint& pos, const FTermArea* area) noexcept -> CoveredState
{
  // Determines the covered state for the given position

  const auto& win_list = getWindowList();
  auto is_covered = CoveredState::None;
  bool found{ area == vdesktop };

  if ( ! area || ! win_list || win_list->empty()
    || win_list->back()->getVWin() == area )
    return CoveredState::None;

  for (auto& win_obj : *win_list)
  {
    const auto& win = win_obj->getVWin();

    if ( ! (win && win->visible) )
      continue;

    if ( found && win->contains(pos) )  // is covered
    {
      const int& x = pos.getX();
      const int& y = pos.getY();
      const int& win_x = win->offset_left;
      const int& win_y = win->offset_top;
      const auto& tmp = win->getFChar(x - win_x, y - win_y);

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
constexpr auto FVTerm::getFullAreaWidth (const FTermArea* area) noexcept -> int
{
  return area->width + area->right_shadow;
}

//----------------------------------------------------------------------
constexpr auto FVTerm::getFullAreaHeight (const FTermArea* area) noexcept -> int
{
  return area->height + area->bottom_shadow;
}

//----------------------------------------------------------------------
inline void FVTerm::updateOverlappedColor ( const FChar& area_char
                                          , const FChar& over_char
                                          , FChar& vterm_char ) noexcept
{
  // Add the overlapping color to this character

  // New character
  FChar nc{};
  std::memcpy (&nc, &area_char, sizeof(nc));
  nc.fg_color = over_char.fg_color;
  nc.bg_color = over_char.bg_color;
  nc.attr.bit.reverse  = false;
  nc.attr.bit.standout = false;

  if ( isTransparentInvisible(nc) )
    nc.ch[0] = L' ';

  nc.attr.bit.no_changes = bool(vterm_char.attr.bit.printed && vterm_char == nc);
  std::memcpy (&vterm_char, &nc, sizeof(vterm_char));
}

//----------------------------------------------------------------------
inline void FVTerm::updateOverlappedCharacter ( FChar& cover_char
                                              , FChar& vterm_char ) noexcept
{
  // Restore one character on vterm

  cover_char.attr.bit.no_changes = \
      bool(vterm_char.attr.bit.printed && vterm_char == cover_char);
  std::memcpy (&vterm_char, &cover_char, sizeof(vterm_char));
}

//----------------------------------------------------------------------
inline void FVTerm::updateShadedCharacter ( const FChar& area_char
                                          , FChar& cover_char
                                          , FChar& vterm_char ) noexcept
{
  // Get covered character + add the current color

  cover_char.fg_color = area_char.fg_color;
  cover_char.bg_color = area_char.bg_color;
  cover_char.attr.bit.reverse  = false;
  cover_char.attr.bit.standout = false;

  if ( isTransparentInvisible(cover_char) )
    cover_char.ch[0] = L' ';

  cover_char.attr.bit.no_changes = \
      bool(vterm_char.attr.bit.printed && vterm_char == cover_char);
  std::memcpy (&vterm_char, &cover_char, sizeof(vterm_char));
}

//----------------------------------------------------------------------
inline void FVTerm::updateInheritBackground ( const FChar& area_char
                                            , const FChar& cover_char
                                            , FChar& vterm_char ) noexcept
{
  // Add the covered background to this character

  // New character
  FChar nc{};
  std::memcpy (&nc, &area_char, sizeof(nc));
  nc.bg_color = cover_char.bg_color;
  nc.attr.bit.no_changes = \
      bool(vterm_char.attr.bit.printed && vterm_char == nc);
  std::memcpy (&vterm_char, &nc, sizeof(vterm_char));
}

//----------------------------------------------------------------------
inline void FVTerm::updateCharacter (const FChar& area_char, FChar& vterm_char) noexcept
{
  // Copy a area character to the virtual terminal

  std::memcpy (&vterm_char, &area_char, sizeof(vterm_char));

  if ( vterm_char.attr.bit.printed && vterm_char == area_char )
    vterm_char.attr.bit.no_changes = true;
  else
    vterm_char.attr.bit.no_changes = false;
}

//----------------------------------------------------------------------
auto FVTerm::updateVTermCharacter ( const FTermArea* area
                                  , const FPoint& area_pos
                                  , const FPoint& terminal_pos ) noexcept -> bool
{
  // Area character
  const auto& ac = area->getFChar(area_pos.getX(), area_pos.getY());

  // Terminal character
  auto& tc = vterm->getFChar(terminal_pos.getX(), terminal_pos.getY());

  // Get covered state
  const auto is_covered = isCovered(terminal_pos, area);

  if ( is_covered == CoveredState::Full )
    return false;

  if ( is_covered == CoveredState::Half )
  {
    // Overlapped character
    auto oc = getOverlappedCharacter (terminal_pos, area);
    updateOverlappedColor (ac, oc, tc);
  }
  else if ( ac.attr.bit.transparent )   // Transparent
  {
    // Covered character
    auto cc = getCoveredCharacter (terminal_pos, area);
    updateOverlappedCharacter (cc, tc);
  }
  else  // Not transparent
  {
    if ( ac.attr.bit.color_overlay )  // Color overlay (transparent shadow)
    {
      // Covered character
      auto cc = getCoveredCharacter (terminal_pos, area);
      updateShadedCharacter (ac, cc, tc);
    }
    else if ( ac.attr.bit.inherit_background )
    {
      // Covered character
      auto cc = getCoveredCharacter (terminal_pos, area);
      updateInheritBackground (ac, cc, tc);
    }
    else  // Default
    {
      updateCharacter (ac, tc);
    }
  }

  return true;
}

//----------------------------------------------------------------------
void FVTerm::updateVTerm() const
{
  // Updates the character data from all areas to VTerm

  if ( hasPendingUpdates(vdesktop) )
  {
    putArea(vdesktop);
    vdesktop->has_changes = false;
  }

  if ( ! window_list || window_list->empty() )
    return;

  for (auto&& window : *window_list)
  {
    auto v_win = window->getVWin();

    if ( ! (v_win && v_win->visible) )
      continue;

    if ( hasPendingUpdates(v_win) )
    {
      putArea(v_win);
      v_win->has_changes = false;
    }
    else if ( hasChildAreaChanges(v_win) )
    {
      putArea(v_win);  // and call the child area processing handler there
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
    auto& vdesktop_changes = vdesktop->changes[y];
    vdesktop_changes.xmin = uInt(vdesktop->width - 1);
    vdesktop_changes.xmax = 0;
  }

  putArea (FPoint{1, 1}, vdesktop);
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
    auto& vdesktop_changes = vdesktop->changes[y + 1];
    vdesktop_changes.xmin = uInt(vdesktop->width - 1);
    vdesktop_changes.xmax = 0;
  }

  putArea (FPoint{1, 1}, vdesktop);
}

//----------------------------------------------------------------------
void FVTerm::callPreprocessingHandler (const FTermArea* area)
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
auto FVTerm::isInsideArea (const FPoint& pos, const FTermArea* area) -> bool
{
  // Check whether the coordinates are within the area

  const int x = pos.getX();
  const int y = pos.getY();
  return x >= 0 && x < area->width
      && y >= 0 && y < area->height;
}

//----------------------------------------------------------------------
auto FVTerm::isTransparentInvisible (const FChar& fchar) -> bool
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
auto FVTerm::generateCharacter (const FPoint& pos) -> FChar
{
  // Generates characters for a given position considering all areas

  auto sc = &vdesktop->getFChar(pos);  // shown character
  const auto& win_list = getWindowList();

  if ( ! win_list || win_list->empty() )
    return *sc;

  for (auto& win_obj : *win_list)
  {
    const auto& win = win_obj->getVWin();

    if ( ! win || ! win->visible )
      continue;

    // Window is visible and contains current character
    if ( win->contains(pos) )
    {
      const int win_x = win->offset_left;
      const int win_y = win->offset_top;
      auto tmp = &win->getFChar(pos.getX() - win_x, pos.getY() - win_y);

      if ( ! tmp->attr.bit.transparent )   // Current character not transparent
      {
        if ( tmp->attr.bit.color_overlay )  // Transparent shadow
        {
          // Keep the current vterm character
          if ( sc != &s_ch )
            std::memcpy (&s_ch, sc, sizeof(s_ch));

          s_ch.fg_color = tmp->fg_color;
          s_ch.bg_color = tmp->bg_color;
          s_ch.attr.bit.reverse  = false;
          s_ch.attr.bit.standout = false;

          if ( isTransparentInvisible(s_ch) )
            s_ch.ch[0] = L' ';

          sc = &s_ch;
        }
        else if ( tmp->attr.bit.inherit_background )
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
auto FVTerm::getCharacter ( CharacterType char_type
                          , const FPoint& pos
                          , const FTermArea* area ) -> FChar
{
  // Gets the overlapped or the covered character for a given position

  int xx = std::max(pos.getX(), 0);
  int yy = std::max(pos.getY(), 0);

  if ( xx >= vterm->width )
    xx = vterm->width - 1;

  if ( yy >= vterm->height )
    yy = vterm->height - 1;

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
    bool significant_char{false};

    // char_type can be "overlapped_character"
    // or "covered_character"
    if ( char_type == CharacterType::Covered )
      significant_char = bool(layer >= getLayer(*win_obj));
    else
      significant_char = bool(layer < getLayer(*win_obj));

    if ( has_an_owner && win_obj != &area_owner && significant_char )
    {
      const auto& win = win_obj->getVWin();

      if ( ! win || ! win->visible )
        continue;

      // Window visible and contains current character
      if ( win->contains(pos) )
        getAreaCharacter (pos, win, cc);
    }
    else if ( char_type == CharacterType::Covered )
      break;
  }

  return *cc;
}

//----------------------------------------------------------------------
inline auto FVTerm::getCoveredCharacter (const FPoint& pos, const FTermArea* area) -> FChar
{
  // Gets the covered character for a given position
  return getCharacter (CharacterType::Covered, pos, area);
}

//----------------------------------------------------------------------
inline auto FVTerm::getOverlappedCharacter (const FPoint& pos, const FTermArea* area) -> FChar
{
  // Gets the overlapped character for a given position
  return getCharacter (CharacterType::Overlapped, pos, area);
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
  FRect term_geometry {0, 0, foutput->getColumnNumber(), foutput->getLineNumber()};
  createVTerm (term_geometry.getSize());

  // Create virtual desktop area
  FSize shadow_size{0, 0};
  createArea (term_geometry, shadow_size, vdesktop);
  vdesktop->visible = true;
  active_area = vdesktop;
}

//----------------------------------------------------------------------
void FVTerm::finish() const
{
  // Resetting the terminal
  setNormal();
  foutput->finishTerminal();
  forceTerminalUpdate();

  // remove virtual terminal + virtual desktop area
  removeArea (vdesktop);
  removeArea (vterm);

  init_object = nullptr;
}

//----------------------------------------------------------------------
inline void FVTerm::putAreaLine (const FChar& src_char, FChar& dst_char, std::size_t length)
{
  // copy "length" characters from area to terminal

  std::memcpy (&dst_char, &src_char, sizeof(dst_char) * length);
}

//----------------------------------------------------------------------
void FVTerm::putAreaCharacter ( const FPoint& pos, const FTermArea* area
                              , const FChar& src_char, FChar& dst_char )
{
  if ( src_char.attr.bit.transparent )  // Transparent
  {
    // Restore one character on vterm
    const FChar& ch = getCoveredCharacter (pos, area);
    std::memcpy (&dst_char, &ch, sizeof(dst_char));
  }
  else  // Not transparent
  {
    if ( src_char.attr.bit.color_overlay )  // Transparent shadow
    {
      // Get covered character + add the current color
      FChar ch = getCoveredCharacter (pos, area);
      ch.fg_color = src_char.fg_color;
      ch.bg_color = src_char.bg_color;
      ch.attr.bit.reverse  = false;
      ch.attr.bit.standout = false;

      if ( isTransparentInvisible(ch) )
        ch.ch[0] = L' ';

      std::memcpy (&dst_char, &ch, sizeof(dst_char));
    }
    else if ( src_char.attr.bit.inherit_background )
    {
      // Add the covered background to this character
      FChar ch{};
      std::memcpy (&ch, &src_char, sizeof(ch));
      FChar cc = getCoveredCharacter (pos, area);
      ch.bg_color = cc.bg_color;
      std::memcpy (&dst_char, &ch, sizeof(dst_char));
    }
    else  // Default
      std::memcpy (&dst_char, &src_char, sizeof(dst_char));
  }
}

//----------------------------------------------------------------------
void FVTerm::getAreaCharacter ( const FPoint& pos, FTermArea* area
                              , FChar*& cc )
{
  const int area_x = area->offset_left;
  const int area_y = area->offset_top;
  const int x = pos.getX();
  const int y = pos.getY();
  auto& tmp = area->getFChar(x - area_x, y - area_y);

  // Current character not transparent
  if ( ! tmp.attr.bit.transparent )
  {
    if ( tmp.attr.bit.color_overlay )  // transparent shadow
    {
      // Keep the current vterm character
      std::memcpy (&s_ch, cc, sizeof(s_ch));
      s_ch.fg_color = tmp.fg_color;
      s_ch.bg_color = tmp.bg_color;
      s_ch.attr.bit.reverse  = false;
      s_ch.attr.bit.standout = false;
      cc = &s_ch;
    }
    else if ( tmp.attr.bit.inherit_background )
    {
      // Add the covered background to this character
      std::memcpy (&i_ch, &tmp, sizeof(i_ch));
      i_ch.bg_color = cc->bg_color;  // last background color
      cc = &i_ch;
    }
    else  // default
      cc = &tmp;
  }
}

//----------------------------------------------------------------------
auto FVTerm::clearFullArea (FTermArea* area, FChar& nc) const -> bool
{
  // Clear area
  const int area_size = area->width * area->height;
  std::fill_n (area->data.begin(), area_size, nc);

  if ( area != vdesktop )  // Is the area identical to the desktop?
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
      auto& vdesktop_changes = vdesktop->changes[i];
      vdesktop_changes.xmin = 0;
      vdesktop_changes.xmax = uInt(vdesktop->width) - 1;
      vdesktop_changes.trans_count = 0;
    }

    vdesktop->has_changes = true;
  }

  return true;
}

//----------------------------------------------------------------------
void FVTerm::clearAreaWithShadow (FTermArea* area, const FChar& nc) noexcept
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
auto FVTerm::getByte1TransMask() -> uInt8
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
  return ( uInt8(~ from.attr.byte[1] & b1_trans_mask)
         & uInt8(    to.attr.byte[1] & b1_trans_mask) ) != 0;
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

  auto& line_changes = area->changes[ay];

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
  std::memcpy (&ac, &ch, sizeof(ac));

  if ( ac.attr.bit.char_width == 0 )
  {
    const auto char_width = getColumnWidth(ac.ch[0]);

    if ( char_width == 0 )
      return 0;

    addColumnWidth(ac, char_width);  // Add column width
  }

  if ( ax < int(line_changes.xmin) )
    line_changes.xmin = uInt(ax);

  if ( ax > int(line_changes.xmax) )
    line_changes.xmax = uInt(ax);

  return ac.attr.bit.char_width;
}

//----------------------------------------------------------------------
inline void FVTerm::printPaddingCharacter (FTermArea* area, const FChar& term_char)
{
  // Creates a padding-character from the current character (term_char)
  // and prints it. It is a placeholder for the column after
  // a full-width character.

  FChar pc;  // padding character

  // Copy character to padding character
  std::memcpy (&pc, &term_char, sizeof(pc));

  if ( foutput->getEncoding() == Encoding::UTF8 )
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
