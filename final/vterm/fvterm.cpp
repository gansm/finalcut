/***********************************************************************
* fvterm.cpp - Virtual terminal implementation                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2025 Markus Gans                                      *
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

#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_set>
#include <vector>

#include "final/fapplication.h"
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
#include "final/vterm/fvterm.h"

namespace finalcut
{

namespace internal
{

constexpr auto getByte1TransparentMask() -> uInt8
{
  FCharAttribute mask{};
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  return getFAttributeByte(mask, 1);
}

constexpr auto getByte1Transparent() -> uInt8
{
  FCharAttribute mask{};
  mask.transparent = true;
  return getFAttributeByte(mask, 1);
}

constexpr auto getByte1ColorOverlay() -> uInt8
{
  FCharAttribute mask{};
  mask.color_overlay = true;
  return getFAttributeByte(mask, 1);
}

constexpr auto getByte1InheritBackground() -> uInt8
{
  FCharAttribute mask{};
  mask.inherit_background = true;
  return getFAttributeByte(mask, 1);
}

constexpr auto getByte1PrintTransparentMask() -> uInt8
{
  FCharAttribute mask{};
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  mask.no_changes = true;
  mask.printed = true;
  return getFAttributeByte(mask, 1);
}

constexpr auto getResetMask() -> uInt32
{
  FCharAttribute mask{};
  mask.no_changes = true;
  mask.printed = true;
  return FCharAttribute_to_uInt32(mask);
}

constexpr auto getColorOverlayMask() -> uInt32
{
  FCharAttribute mask{};
  mask.color_overlay = true;
  mask.reverse = true;
  mask.standout = true;
  mask.no_changes = true;
  mask.printed = true;
  return FCharAttribute_to_uInt32(mask);
}

struct var
{
  static bool fvterm_initialized;  // Global init state
  static constexpr auto b1_transparent_mask = getByte1TransparentMask();
  static constexpr auto b1_transparent = getByte1Transparent();
  static constexpr auto b1_color_overlay = getByte1ColorOverlay();
  static constexpr auto b1_inherit_background = getByte1InheritBackground();
  static constexpr auto b1_print_transparent_mask = getByte1PrintTransparentMask();
  static constexpr auto print_reset_mask = getResetMask();
  static constexpr auto color_overlay_mask = getColorOverlayMask();
};

bool             var::fvterm_initialized{false};
constexpr uInt8  var::b1_transparent_mask;
constexpr uInt8  var::b1_transparent;
constexpr uInt8  var::b1_print_transparent_mask;
constexpr uInt32 var::print_reset_mask;
constexpr uInt32 var::color_overlay_mask;

}  // namespace internal

// static class attributes
bool                 FVTerm::draw_completed{false};
bool                 FVTerm::skip_one_vterm_update{false};
bool                 FVTerm::no_terminal_updates{false};
bool                 FVTerm::force_terminal_update{false};
FVTerm::FTermArea*   FVTerm::active_area{nullptr};
int                  FVTerm::tabstop{8};

using TransparentInvisibleLookupMap = std::unordered_set<wchar_t>;

//----------------------------------------------------------------------
static auto getTransparentInvisibleLookupMap() -> TransparentInvisibleLookupMap
{
  // Encapsulate global unordered_map object
  static const auto& trans_inv_lookup = std::make_unique<TransparentInvisibleLookupMap>
  (
    std::initializer_list<wchar_t>
    ({
      wchar_t(UniChar::LowerHalfBlock),
      wchar_t(UniChar::UpperHalfBlock),
      wchar_t(UniChar::LeftHalfBlock),
      wchar_t(UniChar::RightHalfBlock),
      wchar_t(UniChar::MediumShade),
      wchar_t(UniChar::FullBlock)
    })
  );

  return *trans_inv_lookup;
}

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
auto FVTerm::operator << (FVTermBuffer& buffer) noexcept -> FVTerm&
{
  print (buffer);
  return *this;
}

//----------------------------------------------------------------------
auto FVTerm::operator << (const FVTermBuffer& buffer) noexcept -> FVTerm&
{
  print (buffer);
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
  const auto* win = getPrintArea();

  if ( win )
    return { win->position.x + win->cursor.x
           , win->position.y + win->cursor.y };

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
  if ( auto* win = getPrintArea() )
  {
    win->setCursorPos ( pos.getX() - win->position.x
                      , pos.getY() - win->position.y );
  }
}

//----------------------------------------------------------------------
void FVTerm::setNonBlockingRead (bool enable)
{
  static const auto& init_object = getGlobalFVTermInstance();
  init_object->foutput->setNonBlockingRead (enable);
}

//----------------------------------------------------------------------
auto FVTerm::hasPreprocessingHandler (const FVTerm* instance) noexcept -> bool
{
  if ( ! print_area )
    getPrintArea();

  if ( ! instance || ! print_area || print_area->preproc_list.empty() )
    return false;

  auto iter = print_area->preproc_list.cbegin();

  while ( iter != print_area->preproc_list.cend() )
  {
    if ( iter->get()->instance.get() == instance )
      return true;

    ++iter;
  }

  return false;
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
  vterm_old = createArea(box);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (const FSize& size) const noexcept
{
  // Resize virtual terminal

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  resizeArea (box, vterm.get());
  resizeArea (box, vterm_old.get());
}

//----------------------------------------------------------------------
void FVTerm::putVTerm() const
{
  // Update the entire terminal screen

  auto* vterm_changes = &vterm->changes[unsigned(0)];
  std::for_each ( vterm_changes
                , vterm_changes + unsigned(vterm->size.height)
                , [this] (auto& vterm_changes_line)
                  {
                    vterm_changes_line.xmin = 0;
                    vterm_changes_line.xmax = uInt(vterm->size.width - 1);
                  } );
  updateTerminal();
}

//----------------------------------------------------------------------
auto FVTerm::updateTerminal() const -> bool
{
  // Update terminal screen when modified

  auto terminal_updated = canUpdateTerminalNow()
                        ? foutput->updateTerminal()
                        : false;

  if ( terminal_updated )
    saveCurrentVTerm();

  return terminal_updated;
}

//----------------------------------------------------------------------
void FVTerm::reduceTerminalLineUpdates (uInt y)
{
  static const auto& init_object = getGlobalFVTermInstance();
  static const auto& vterm = init_object->vterm;
  static const auto& vterm_old = init_object->vterm_old;
  auto& vterm_changes = vterm->changes[unsigned(y)];
  uInt& xmin = vterm_changes.xmin;
  uInt& xmax = vterm_changes.xmax;

  if ( xmin > xmax )  // No changes
    return;

  const auto* first = &vterm->getFChar(int(xmin), int(y));
  const auto* first_old = &vterm_old->getFChar(int(xmin), int(y));
  auto* last = &vterm->getFChar(int(xmax), int(y));
  const auto* last_old = &vterm_old->getFChar(int(xmax), int(y));

  while ( xmin < xmax && *first == *first_old )
  {
    xmin++;
    first++;
    first_old++;
  }

  while ( last >= first && *last == *last_old )
  {
    xmax--;
    last--;
    last_old--;
  }

  while ( last > first )
  {
    if ( *last == *last_old )
      last->attr.bit.no_changes = true;

    last--;
    last_old--;
  }
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
      area->cursor.y++;
      // fall through
    case L'\r':
      area->cursor.x = 1;
      break;

    case L'\t':
      area->cursor.x += tabstop - ((area->cursor.x - 1) % tabstop);
      break;

    case L'\b':
      area->cursor.x--;
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

  vterm_buffer.print(string);
  return print (vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, const FString& string) noexcept -> int
{
  if ( ! area || string.isEmpty() )
    return -1;

  vterm_buffer.print(string);
  return print (area, vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (const std::vector<FChar>& term_string) noexcept -> int
{
  if ( term_string.empty() )
    return -1;

  vterm_buffer.assign(term_string.cbegin(), term_string.cend());
  return print (vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, const std::vector<FChar>& term_string) noexcept -> int
{
  if ( ! area || term_string.empty() )
    return -1;

  vterm_buffer.assign(term_string.cbegin(), term_string.cend());
  return print (area, vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (FVTermBuffer& buffer) noexcept -> int
{
  if ( buffer.isEmpty() )
    return -1;

  auto* area = getPrintArea();
  return area ? print (area, buffer) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (const FVTermBuffer& buffer) noexcept -> int
{
  if ( buffer.isEmpty() )
    return -1;

  auto* area = getPrintArea();
  return area ? print (area, buffer) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, FVTermBuffer& buffer) const noexcept -> int
{
  auto len = print (area, static_cast<const FVTermBuffer&>(buffer));
  buffer.clear();  // Clear FVTermBuffer after printing
  return len;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, const FVTermBuffer& buffer) const noexcept -> int
{
  int len{0};

  if ( ! area || buffer.isEmpty() )
    return -1;

  for (const auto& fchar : buffer)
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
  auto* area = getPrintArea();
  return area ? print (area, c) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, wchar_t c) noexcept -> int
{
  if ( ! area )
    return -1;

  static const auto& next_attr = getAttribute();
  nc.color.data = next_attr.color.data;
  nc.attr.byte[0] = next_attr.attr.byte[0];
  nc.attr.byte[1] = next_attr.attr.byte[1];
  nc.attr.byte[2] = 0;
  nc.attr.byte[3] = 0;
  nc.ch[0] = c;
  nc.ch[1] = L'\0';
  return print (area, nc);
}

//----------------------------------------------------------------------
auto FVTerm::print (const FChar& term_char) noexcept -> int
{
  auto* area = getPrintArea();
  return area ? print (area, term_char) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermArea* area, const FChar& term_char) const noexcept -> int
{
  if ( ! area )
    return -1;  // No area

  if ( interpretControlCodes(area, term_char) )
    return 0;  // No printable character

  if ( ! area->isPrintPositionInsideArea() || printWrap(area) )
    return -1;  // Cursor position out of range or end of area reached

  // Printing term_char on area at the current cursor position
  auto char_width = printCharacterOnCoordinate (area, term_char);

  if ( char_width == 0 && ! term_char.attr.bit.fullwidth_padding )
    return 0;

  area->cursor.x++;
  area->has_changes = true;

  // Line break at right margin
  if ( area->cursor.x > getFullAreaWidth(area) )
  {
    area->cursor.x = 1;
    area->cursor.y++;
  }
  else if ( char_width == 2 )
    printPaddingCharacter (area, term_char);

  // Prevent up scrolling
  if ( area->cursor.y > getFullAreaHeight(area) )
    area->cursor.y--;

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
auto FVTerm::createArea (const FShadowBox& shadowbox) -> std::unique_ptr<FTermArea>
{
  // initialize virtual window

  auto area = std::make_unique<FTermArea>();
  area->setOwner<FVTerm*>(this);
  area->encoding = foutput->getEncoding();
  resizeArea (shadowbox, area.get());
  return area;
}

//----------------------------------------------------------------------
auto FVTerm::createArea (const FRect& box) -> std::unique_ptr<FTermArea>
{
  // initialize virtual window

  const auto no_shadow = FSize(0, 0);
  return createArea ({box, no_shadow});
}

//----------------------------------------------------------------------
void FVTerm::resizeArea ( const FShadowBox& shadowbox
                        , FTermArea* area ) const
{
  // Resize the virtual window to a new size

  assert ( isAreaValid(shadowbox) );

  if ( ! area )
    return;

  if ( isSizeEqual(area, shadowbox) )
  {
    area->position.x = shadowbox.box.getX();
    area->position.y = shadowbox.box.getY();
    return;  // Move only
  }

  const auto full_width  = shadowbox.box.getWidth()
                         + shadowbox.shadow.getWidth();
  const auto full_height = shadowbox.box.getHeight()
                         + shadowbox.shadow.getHeight();

  if ( ! tryResizeArea(area, full_width, full_height) )
    return;

  updateAreaProperties (area, shadowbox);
  // Set default FChar in area
  resetTextAreaToDefault (area, { FSize{full_width, full_height} });
}

//----------------------------------------------------------------------
void FVTerm::resizeArea (const FRect& box, FTermArea* area) const
{
  // Resize the virtual window to a new size

  const auto no_shadow = FSize(0, 0);
  resizeArea ({box, no_shadow}, area);
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (const FRect& box) const noexcept
{
  if ( ! vterm || box.getWidth() == 0 || box.getHeight() == 0 )
    return;

  const auto* vterm_win_list = getWindowList();

  if ( ! vterm_win_list || vterm_win_list->empty() )
    return;

  const FSize vterm_size{ static_cast<std::size_t>(vterm->size.width)
                        , static_cast<std::size_t>(vterm->size.height) };

  if ( vdesktop && vdesktop->reprint(box, vterm_size) )
    addLayer(vdesktop.get());

  for (const auto& vterm_obj : *vterm_win_list)
  {
    if ( ! vterm_obj )
      continue;

    auto* win = vterm_obj->getVWin();

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
    const int cx = area->input_cursor.x;
    const int cy = area->input_cursor.y;
    // terminal position = area position + area cursor position
    const int x  = area->position.x + cx;
    const int y  = area->position.y + cy;

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

  const int y_end  = std::min(vterm->size.height - ay, area->size.height);
  const int length = std::min(vterm->size.width - ax, area->size.width);
  auto* line_changes = &area->changes[0];

  for (auto y{0}; y < y_end; y++)  // line loop
  {
    const auto& tc = vterm->getFChar(ax, ay + y);  // terminal character
    auto& ac = area->getFChar(0, y);  // area character
    putAreaLine (tc, ac, unsigned(length));
    line_changes->xmin = 0;
    line_changes->xmax = uInt(length - 1);
    std::advance(line_changes, 1);
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
  int dx = x - area->position.x;
  int dy = y - area->position.y;
  auto w = std::min(area->size.width - dx, int(box.getWidth()));
  auto h = std::min(area->size.height - dy, int(box.getHeight()));

  if ( x < 0 || y < 0 )
    return;

  if ( dx < 0 ) { w += dx; x -= dx; dx = 0; }
  if ( dy < 0 ) { h += dy; y -= dy; dy = 0; }
  const int y_end = std::min(vterm->size.height - y, h);
  const int length = std::min(vterm->size.width - x, w);

  if ( length < 1 )
    return;

  auto* line_changes = &area->changes[unsigned(dy)];

  for (auto line{0}; line < y_end; line++)  // line loop
  {
    const auto& tc = vterm->getFChar(x, y + line);  // terminal character
    auto& ac = area->getFChar(dx, dy + line);  // area character
    putAreaLine (tc, ac, unsigned(length));
    line_changes->xmin = std::min(line_changes->xmin, uInt(dx));
    line_changes->xmax = std::max(line_changes->xmax, uInt(dx + length - 1));
    std::advance(line_changes, 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::addLayer (FTermArea* area) const noexcept
{
  // Transmit changes in area to the virtual terminal (add layer to vterm)

  if ( ! area || ! area->visible )
    return;

  const int area_x = area->position.x;
  const int ax = std::max(area_x, 0);
  const int ay = area->position.y;
  const int ol = std::max(0, -area_x);  // Outside left
  const int vterm_width = vterm->size.width;
  const int width = getFullAreaWidth(area);
  const int height = area->minimized ? area->min_size.height : getFullAreaHeight(area);
  const int y_end = std::min(vterm->size.height - ay, height);
  const int xmax_inside_vterm = vterm_width - area_x - 1;

  // Call the preprocessing handler methods (child area change handling)
  callPreprocessingHandler(area);
  auto* line_changes = &area->changes[0];

  for (auto y{0}; y < y_end; y++)  // Line loop
  {
    const auto line_xmin = std::max(int(line_changes->xmin), ol);
    const auto line_xmax = std::min(int(line_changes->xmax), xmax_inside_vterm);

    if ( line_xmin > line_xmax )
    {
      std::advance(line_changes, 1);
      continue;
    }

    const int tx = area_x + line_xmin;  // Global terminal x-position
    const int ty = ay + y;              // Global terminal y-position

    if ( tx < 0 || tx >= vterm_width || ty < 0 )
    {
      std::advance(line_changes, 1);
      continue;
    }

    const std::size_t length = unsigned(line_xmax - line_xmin + 1);
    const auto& ac = area->getFChar(line_xmin, y);  // Area character
    auto& tc = vterm->getFChar(tx, ty);  // Terminal character

    if ( line_changes->trans_count > 0 )
    {
      // Line with hidden and transparent characters
      addAreaLineWithTransparency (&ac, &tc, length);
    }
    else
    {
      // Line has only covered characters
      putAreaLine (ac, tc, length);
    }

    auto& vterm_changes = vterm->changes[unsigned(ty)];
    vterm_changes.xmin = std::min(vterm_changes.xmin, uInt(tx));
    vterm_changes.xmax = std::max ( vterm_changes.xmax
                                  , uInt(std::min(ax + line_xmax, vterm_width - 1)));
    line_changes->xmin = uInt(width);
    line_changes->xmax = 0;
    std::advance(line_changes, 1);
  }

  vterm->has_changes = true;
  updateVTermCursor(area);
}

//----------------------------------------------------------------------
void FVTerm::putArea (const FPoint& pos, FTermArea* area) const noexcept
{
  // Copies the given area block to the virtual terminal position

  if ( ! area || ! area->visible )
    return;

  copyArea (vterm.get(), pos, area);
}

//----------------------------------------------------------------------
void FVTerm::copyArea (FTermArea* dst, const FPoint& pos, FTermArea* src) const noexcept
{
  // Copies the src area to the dst area position

  if ( ! dst || ! src )
    return;

  if ( dst == vterm.get() )
    skip_one_vterm_update = true;

  const int src_width = getFullAreaWidth(src);
  const int src_height = src->minimized ? src->min_size.height : getFullAreaHeight(src);
  const int ax = std::max(0, pos.getX() - 1);
  const int ay = std::max(0, pos.getY() - 1);
  const int ol = std::max(0, 1 - pos.getX());  // outside left
  const int ot = std::max(0, 1 - pos.getY());  // outside top
  const int y_end = std::min(dst->size.height - ay, src_height - ot);
  const int length = std::min(dst->size.width - ax, src_width - ol);

  if ( length < 1 || y_end < 1 )
    return;

  auto* src_changes = &src->changes[0];
  auto* dst_changes = &dst->changes[unsigned(ay)];

  if ( skip_one_vterm_update )  // dst is the virtual terminal
    determineCoveredAreas(src);

  for (int y{0}; y < y_end; y++)  // line loop
  {
    const int cy = ay + y;
    const auto* sc = &src->getFChar(ol, ot + y);  // src character ptr
    auto* dc = &dst->getFChar(ax, cy);  // dst character ptr

    if ( skip_one_vterm_update && src_changes->trans_count > 0 )
    {
      // Line with hidden and transparent characters
      putAreaLineWithTransparency (sc, dc, length, {ax, cy}, src_changes->covered);
    }
    else
    {
      // Line has only covered characters
      putAreaLine (*sc, *dc, unsigned(length));
    }

    dst_changes->xmin = std::min(uInt(ax), dst_changes->xmin);
    dst_changes->xmax = std::max(uInt(ax + length - 1), dst_changes->xmax);
    std::advance(src_changes, 1);
    std::advance(dst_changes, 1);
  }

  dst->has_changes = true;
}

//----------------------------------------------------------------------
void FVTerm::determineWindowLayers() noexcept
{
  // Determination of the window layer for all virtual windows

  const auto* vterm_win_list = getWindowList();

  if ( ! vterm_win_list || vterm_win_list->empty() )
    return;

  const auto& begin = vterm_win_list->cbegin();
  const auto& end = vterm_win_list->cend();
  auto iter = begin;

  while ( iter != end )
  {
    (*iter)->getPrintArea()->layer = int(std::distance(begin, iter) + 1);
    ++iter;
  }
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaForward (FTermArea* area)
{
  // Scrolls the entire area on line up

  if ( ! area || area->size.height <= 1 )
    return;

  const int y_max = area->size.height - 1;
  const int x_max = area->size.width - 1;
  auto* line_changes = &area->changes[0];

  for (auto y{0}; y < y_max; y++)
  {
    auto& dc = area->getFChar(0, y);  // destination character
    const auto& sc = area->getFChar(0, y + 1);  // source character
    putAreaLine (sc, dc, unsigned(area->size.width));
    line_changes->xmin = 0;
    line_changes->xmax = uInt(x_max);
    std::advance(line_changes, 1);
  }

  // insert a new line below
  const auto& lc = area->getFChar(x_max, area->size.height - 2);  // last character
  nc.color.data = lc.color.data;
  nc.attr = lc.attr;
  nc.ch[0] = L' ';
  nc.ch[1] = L'\0';
  auto& dc = area->getFChar(0, y_max);  // destination character
  std::fill (&dc, &dc + area->size.width, nc);
  auto& new_line_changes = area->changes[unsigned(y_max)];
  new_line_changes.xmin = 0;
  new_line_changes.xmax = uInt(x_max);
  area->has_changes = true;

  if ( area == vdesktop.get() )
    scrollTerminalForward();  // Scrolls the terminal up one line
}

//----------------------------------------------------------------------
void FVTerm::scrollAreaReverse (FTermArea* area)
{
  // Scrolls the entire area one line down

  if ( ! area || area->size.height <= 1 )
    return;

  const int y_max = area->size.height - 1;
  const int x_max = area->size.width - 1;
  auto* line_changes = &area->changes[0];

  for (auto y = y_max; y > 0; y--)
  {
    auto& dc = area->getFChar(0, y);  // destination character
    const auto& sc = area->getFChar(0, y - 1);  // source character
    putAreaLine (sc, dc, unsigned(area->size.width));
    line_changes->xmin = 0;
    line_changes->xmax = uInt(x_max);
    std::advance(line_changes, 1);
  }

  // insert a new line above
  const auto& lc = area->getFChar(0, 1);  // last character
  nc.color.data = lc.color.data;
  nc.attr = lc.attr;
  nc.ch[0] = L' ';
  nc.ch[1] = L'\0';
  auto& dc = area->getFChar(0, 0);  // destination character
  std::fill (&dc, &dc + area->size.width, nc);
  auto& new_line_changes = area->changes[unsigned(y_max)];
  new_line_changes.xmin = 0;
  new_line_changes.xmax = uInt(x_max);
  area->has_changes = true;

  if ( area == vdesktop.get() )
    scrollTerminalReverse();  // Scrolls the terminal down one line
}

//----------------------------------------------------------------------
void FVTerm::clearArea (FTermArea* area, wchar_t fillchar) noexcept
{
  // Clear the area with the current attributes

  static const auto& next_attr = getAttribute();
  nc.color.data = next_attr.color.data;
  nc.attr = next_attr.attr;
  nc.ch[0] = fillchar;  // Current attributes with the fill character
  nc.ch[1] = L'\0';
  nc.attr.bit.char_width = getColumnWidth(nc.ch[0]) & 0x03;

  if ( ! area || area->data.empty() )
  {
    foutput->clearTerminal (fillchar);
    return;
  }

  const auto width = uInt(getFullAreaWidth(area));

  if ( area->shadow.width == 0 )
  {
    if ( clearFullArea(area, nc) )
      return;
  }
  else
    clearAreaWithShadow(area, nc);

  auto* line_changes = &area->changes[0];

  for (auto i{0}; i < area->size.height; i++)
  {
    line_changes->xmin = 0;
    line_changes->xmax = width - 1;

    if ( nc.attr.bit.transparent
      || nc.attr.bit.color_overlay
      || nc.attr.bit.inherit_background )
      line_changes->trans_count = width;
    else if ( area->shadow.width != 0 )
      line_changes->trans_count = uInt(area->shadow.width);
    else
      line_changes->trans_count = 0;

    std::advance(line_changes, 1);
  }

  line_changes = &area->changes[unsigned(area->size.height)];

  for (auto i{0}; i < area->shadow.height; i++)
  {
    line_changes->xmin = 0;
    line_changes->xmax = width - 1;
    line_changes->trans_count = width;
    std::advance(line_changes, 1);
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

  for (auto&& vterm_obj : *vterm_window_list)
  {
    auto v_win = vterm_obj->getVWin();

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
  constexpr FChar default_char
  {
    { { L' ',  L'\0', L'\0', L'\0', L'\0' } },
    { { L'\0', L'\0', L'\0', L'\0', L'\0' } },
    { { FColor::Default, FColor::Default } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3 (byte 2 = 0x08 = char_width 1)
  };
  std::fill (area->data.begin(), area->data.end(), default_char);

  const FTermArea::FLineChanges unchanged { uInt(size.getWidth())
                                          , 0, 0, false };
  std::fill (area->changes.begin(), area->changes.end(), unchanged);
}

//----------------------------------------------------------------------
inline auto FVTerm::resizeTextArea ( FTermArea* area
                                   , std::size_t height
                                   , std::size_t size ) const -> bool
{
  // Set the number of lines for changes to "height"
  // and resize the text area to "size" elements

  area->changes.resize(height);
  resizeTextArea (area, size);
  return true;
}

//----------------------------------------------------------------------
inline auto FVTerm::resizeTextArea (FTermArea* area, std::size_t size) const -> bool
{
  // Resize text area to "size" FChar elements

  area->data.resize(size);
  return true;
}

//----------------------------------------------------------------------
auto FVTerm::isCovered (const FPoint& pos, const FTermArea* area) const noexcept -> CoveredState
{
  // Determines the covered state for the given position

  const auto* vterm_win_list{getWindowList()};

  if ( ! area || ! vterm_win_list || vterm_win_list->empty()
    || vterm_win_list->back()->getVWin() == area )
    return CoveredState::None;

  auto is_covered{CoveredState::None};  // Initial state: no coverage
  bool found{ area == vdesktop.get() };
  const auto pos_x = pos.getX();
  const auto pos_y = pos.getY();

  for (const auto& vterm_obj : *vterm_win_list)  // Iteration through window list
  {
    const auto* win = vterm_obj->getVWin();

    if ( ! win || ! win->visible )
      continue;

    if ( found && win->contains(pos_x, pos_y) )  // is covered
    {
      // Reduce repetitive attribute access
      const auto delta_x = pos_x - win->position.x;
      const auto delta_y = pos_y - win->position.y;
      const auto& tmp = win->getFChar(delta_x, delta_y);

      if ( tmp.attr.bit.color_overlay )
      {
        is_covered = CoveredState::Half;  // Mark as partially covered
      }
      else if ( ! tmp.attr.bit.transparent )
      {
        return CoveredState::Full;  // Fully covered
      }
    }

    if ( area == win )
      found = true;
  }

  return is_covered;
}

//----------------------------------------------------------------------
inline auto FVTerm::isAreaValid (const FShadowBox& shadowbox) const -> bool
{
  return shadowbox.box.getY() >= 0
      && shadowbox.box.getWidth() > 0
      && shadowbox.box.getHeight() > 0;
}

//----------------------------------------------------------------------
inline auto FVTerm::isSizeEqual (const FTermArea* area, const FShadowBox& shadowbox) const -> bool
{
  return area->size.width == int(shadowbox.box.getWidth())
      && area->size.height == int(shadowbox.box.getHeight())
      && area->shadow.width == int(shadowbox.shadow.getWidth())
      && area->shadow.height == int(shadowbox.shadow.getHeight());
}

//----------------------------------------------------------------------
constexpr auto FVTerm::needsHeightResize ( const FTermArea* area
                                         , const std::size_t full_height ) const noexcept -> bool
{
  return getFullAreaHeight(area) != int(full_height);
}

//----------------------------------------------------------------------
constexpr auto FVTerm::needsWidthResize ( const FTermArea* area
                                        , const std::size_t full_width ) const noexcept -> bool
{
  return getFullAreaWidth(area) != int(full_width);
}

//----------------------------------------------------------------------
inline auto FVTerm::tryResizeArea ( FTermArea* area
                                  , const std::size_t width
                                  , const std::size_t height ) const -> bool
{
  if ( needsHeightResize(area, height) )
    return resizeTextArea(area, height, width * height);

  if ( needsWidthResize(area, width) )
    return resizeTextArea(area, width * height);

  return false;
}

//----------------------------------------------------------------------
inline void FVTerm::updateAreaProperties (FTermArea* area, const FShadowBox& shadowbox) const
{
  area->position.x      = shadowbox.box.getX();
  area->position.y      = shadowbox.box.getY();
  area->size.width      = int(shadowbox.box.getWidth());
  area->size.height     = int(shadowbox.box.getHeight());
  area->min_size.width  = area->size.width;
  area->min_size.height = DEFAULT_MINIMIZED_HEIGHT;
  area->shadow.width    = int(shadowbox.shadow.getWidth());
  area->shadow.height   = int(shadowbox.shadow.getHeight());
  area->has_changes     = false;
}

//----------------------------------------------------------------------
constexpr auto FVTerm::getFullAreaWidth (const FTermArea* area) const noexcept -> int
{
  return area->size.width + area->shadow.width;
}

//----------------------------------------------------------------------
constexpr auto FVTerm::getFullAreaHeight (const FTermArea* area) const noexcept -> int
{
  return area->size.height + area->shadow.height;
}

//----------------------------------------------------------------------
void FVTerm::passChangesToOverlap (const FTermArea* area) const
{
  const auto* vterm_win_list = getWindowList();

  if ( ! area || ! vterm_win_list || vterm_win_list->empty() )
    return;

  processOverlappingWindows (area, *vterm_win_list);
}

//----------------------------------------------------------------------
inline void FVTerm::processOverlappingWindows ( const FTermArea* area
                                              , const FVTermList& vterm_win_list ) const
{
  bool found{false};

  if ( ! area )
    return;

  for (auto&& vterm_obj : vterm_win_list)
  {
    auto* win = vterm_obj->getVWin();

    if ( found && win && win->visible && win->isOverlapped(area) )
    {
      // Pass changes to the found window
      passChangesToOverlappingWindow (win, area);
    }

    if ( area == win )
      found = true;
  }
}

//----------------------------------------------------------------------
inline void FVTerm::passChangesToOverlappingWindow (FTermArea* win, const FTermArea* area) const
{
  win->has_changes = true;

  // Boundaries
  const int win_y_min = win->position.y;
  const int win_height = win->minimized ? win->min_size.height : getFullAreaHeight(win);
  const int win_y_max = win_y_min + win_height - 1;
  const int area_y_min = area->position.y;
  const int area_height = area->minimized ? area->min_size.height : getFullAreaHeight(area);
  const int area_y_max = area_y_min + area_height - 1;
  const int vterm_y_max = vterm->size.height - 1;

  // Calculate start and end line
  const int y_start = calculateStartCoordinate (area_y_min, win_y_min);
  const int y_end = calculateEndCoordinate (vterm_y_max, area_y_max, win_y_min, win_y_max);

  for (auto y{y_start}; y <= y_end; y++)  // Line loop
  {
    passChangesToOverlappingWindowLine (win, y, area);
  }
}

//----------------------------------------------------------------------
inline void FVTerm::passChangesToOverlappingWindowLine (FTermArea* win, int y, const FTermArea* area) const
{
  // Boundaries
  const int win_x_min = win->position.x;
  const int win_x_max = win_x_min + win->size.width + win->shadow.width - 1;
  const int area_x_min = area->position.x;
  const int area_x_max = area_x_min + area->size.width + area->shadow.width - 1;
  const int vterm_x_max = vterm->size.width - 1;

  // Calculate start and end position
  const int x_start = calculateStartCoordinate (area_x_min, win_x_min);
  const int x_end = calculateEndCoordinate (vterm_x_max, area_x_max, win_x_min, win_x_max);

  // Sets the new change boundaries
  auto& line_changes = win->changes[unsigned(y)];
  line_changes.xmin = uInt(std::min(int(line_changes.xmin), x_start));
  line_changes.xmax = uInt(std::max(int(line_changes.xmax), x_end));
}

//----------------------------------------------------------------------
inline auto FVTerm::calculateStartCoordinate (int area_min, int win_min) const noexcept -> int
{
  return std::max({0, area_min, win_min}) - win_min;
}

//----------------------------------------------------------------------
inline auto FVTerm::calculateEndCoordinate (int vterm_max, int area_max, int win_min, int win_max) const noexcept -> int
{
  return std::min({vterm_max, area_max, win_max}) - win_min;
}

//----------------------------------------------------------------------
void FVTerm::updateVTerm() const
{
  // Updates the character data from all areas to VTerm

  updateVTermDesktop();

  if ( ! vterm_window_list || vterm_window_list->empty() )
    return;

  for (auto&& vterm_obj : *vterm_window_list)  // List from bottom to top
  {
    auto v_win = vterm_obj->getVWin();

    if ( ! (v_win && v_win->visible && v_win->layer > 0) )
      continue;

    updateVTermWindow(v_win);
  }
}

//----------------------------------------------------------------------
inline void FVTerm::updateVTermDesktop() const
{
  if ( ! hasPendingUpdates(vdesktop.get()) )
    return;

  addLayer(vdesktop.get());  // Add vdesktop changes to vterm
  vdesktop->has_changes = false;
}

//----------------------------------------------------------------------
inline void FVTerm::updateVTermWindow (FTermArea* v_win) const
{
  if ( hasPendingUpdates(v_win) )
  {
    passChangesToOverlap(v_win);
    addLayer(v_win);  // Add v_win changes to vterm
    v_win->has_changes = false;
  }
  else if ( hasChildAreaChanges(v_win) )
  {
    passChangesToOverlap(v_win);
    addLayer(v_win);  // and call the child area processing handler there
    clearChildAreaChanges(v_win);
  }
}

//----------------------------------------------------------------------
inline void FVTerm::scrollTerminalForward() const
{
  // Scrolls the terminal up one line

  foutput->flush();  // Empty buffer before scrolling

  if ( ! foutput->scrollTerminalForward() )
    return;

  // Avoid update lines from 0 to (height - 2)
  auto* vdesktop_changes = &vdesktop->changes[0];
  const auto* vdesktop_changes_end = vdesktop_changes
                                   + unsigned(vdesktop->size.height - 1);

  while ( vdesktop_changes < vdesktop_changes_end )
  {
    vdesktop_changes->xmin = uInt(vdesktop->size.width - 1);
    vdesktop_changes->xmax = 0;
    std::advance(vdesktop_changes, 1);
  }

  putArea (FPoint{1, 1}, vdesktop.get());
  saveCurrentVTerm();  // Ensure that the current terminal is comparable
  forceTerminalUpdate();
}

//----------------------------------------------------------------------
inline void FVTerm::scrollTerminalReverse() const
{
  // Scrolls the terminal down one line

  foutput->flush();  // Empty buffer before scrolling

  if ( ! foutput->scrollTerminalReverse() )
    return;

  // avoid update lines from 1 to (height - 1)
  auto* vdesktop_changes = &vdesktop->changes[1];
  const auto* vdesktop_changes_end = vdesktop_changes
                                   + unsigned(vdesktop->size.height);

  while ( vdesktop_changes < vdesktop_changes_end )
  {
    vdesktop_changes->xmin = uInt(vdesktop->size.width - 1);
    vdesktop_changes->xmax = 0;
    std::advance(vdesktop_changes, 1);
  }

  putArea (FPoint{1, 1}, vdesktop.get());
  saveCurrentVTerm();  // Ensure that the current terminal is comparable
  forceTerminalUpdate();
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
    const auto& preprocessingHandler = pcall->function;
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
  return x >= 0 && x < area->size.width
      && y >= 0 && y < area->size.height;
}

//----------------------------------------------------------------------
inline auto FVTerm::isFCharTransparent (const FChar& fchar) const -> bool
{
  return (fchar.attr.byte[1] & internal::var::b1_transparent_mask) != 0;
}

//----------------------------------------------------------------------
inline auto FVTerm::isTransparentInvisible (const FChar& fchar) const -> bool
{
  static const auto& trans_inv_chars = getTransparentInvisibleLookupMap();
  const auto& fist_char = fchar.ch[0];
  return trans_inv_chars.find(fist_char) != trans_inv_chars.end();
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
  // Reset the FINAL CUT terminal settings and
  // return to the normal terminal mode
  setNormal();
  foutput->finishTerminal();
  forceTerminalUpdate();
  internal::var::fvterm_initialized = false;
  setGlobalFVTermInstance(nullptr);
}

//----------------------------------------------------------------------
inline void FVTerm::saveCurrentVTerm() const
{
  // Save the content of the virtual terminal
  std::memcpy(vterm_old->data.data(), vterm->data.data(), vterm->data.size() * sizeof(FChar));
}

//----------------------------------------------------------------------
inline void FVTerm::putAreaLine ( const FChar& src_char
                                , FChar& dst_char
                                , const std::size_t length ) const
{
  // copy "length" characters from area to terminal

  std::memcpy (&dst_char, &src_char, length * sizeof(dst_char));
}

//----------------------------------------------------------------------
inline void FVTerm::applyColorOverlay (const FChar& src_char, FChar& dst_char) const
{
  dst_char.ch = src_char.ch;  // Get covered character
  dst_char.attr.data &= ~internal::var::color_overlay_mask;

  if ( isTransparentInvisible(dst_char) )
    dst_char.ch[0] = L' ';
}

//----------------------------------------------------------------------
inline void FVTerm::inheritBackground (const FChar& src_char, FChar& dst_char) const
{
  // Add the covered background to this character
  dst_char.color.pair.bg = src_char.color.pair.bg;
}

//----------------------------------------------------------------------
inline void FVTerm::putMultiLayerAreaLine ( FChar* dst_char
                                           , const std::size_t length
                                           , FPoint& pos ) const noexcept
{
  if ( length == 0 )
    return;

  if ( overlay_search_buffer.size() < length )  // Resize only if needed
    overlay_search_buffer.resize(length);

  auto* search_buffer = overlay_search_buffer.data();
  std::fill_n(search_buffer, length, SearchState::start);
  const auto term_x = pos.getX();
  const auto term_y = pos.getY();

  for (const auto* win : covered_areas_buffer)
  {
    // Precalculate window boundaries
    const int current_height = win->minimized ? win->min_size.height
                                              : win->size.height + win->shadow.height;
    const int x_min = win->position.x;
    const int x_max = x_min + win->size.width + win->shadow.width;
    const int y_min = win->position.y;
    const int y_max = y_min + current_height;

    // Early rejection if the line is completely outside the window
    if ( term_y < y_min
      || term_y >= y_max
      || term_x >= x_max
      || term_x + int(length) <= x_min )
      continue;

    // Precalculate array indexing values for getFChar
    const auto x = term_x - win->position.x;
    const auto y = term_y - win->position.y;
    const auto width = win->size.width + win->shadow.width;
    const auto index = unsigned(y) * unsigned(width) + unsigned(x);

    // Calculate the intersection of the line with the window
    const auto start_i = std::size_t(std::max(0, x_min - term_x));
    const auto end_i = std::size_t(std::min(int(length), x_max - term_x));

    // Cache frequently accessed data pointer
    const auto* const win_data = win->data.data();

    // Process only the intersecting part of the line
    for (std::size_t i{start_i}; i < end_i; i++)
    {
      auto& char_search = search_buffer[i];

      if ( char_search == SearchState::ready )
        continue;

      const auto& win_char = win_data[index + unsigned(i)];
      auto& dst = dst_char[i];
      const auto transparency = win_char.attr.byte[1]
                              & internal::var::b1_transparent_mask;

      switch ( transparency )
      {
        case internal::var::b1_transparent:  // Transparent
          if ( char_search == SearchState::start )
          {
            char_search = SearchState::printable;
            continue;
          }
          break;

        case internal::var::b1_color_overlay:  // Color overlay
          if ( char_search != SearchState::overlay )
          {
            if ( char_search == SearchState::background )
            {
              inheritBackground(win_char, dst);
              char_search = SearchState::ready;
              continue;
            }

            std::memcpy (&dst, &win_char, sizeof(FChar));
            char_search = SearchState::overlay;
            continue;
          }
          break;

        case internal::var::b1_inherit_background:  // Inherit background color
          if ( char_search != SearchState::background )
          {
            if ( char_search == SearchState::overlay )
            {
              applyColorOverlay(win_char, dst);
              char_search = SearchState::ready;
              continue;
            }

            std::memcpy (&dst, &win_char, sizeof(FChar));
            char_search = SearchState::background;
            continue;
          }
          break;

        default:
          // Color overlay
          if ( char_search == SearchState::overlay )
          {
            applyColorOverlay(win_char, dst);
            char_search = SearchState::ready;
            continue;
          }

          // Inherit background color
          if ( char_search == SearchState::background )
          {
            inheritBackground(win_char, dst);
            char_search = SearchState::ready;
            continue;
          }

          // Non-transparent
          std::memcpy (&dst, &win_char, sizeof(FChar));
          char_search = SearchState::ready;
      }
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::putAreaLineWithTransparency ( const FChar* src_char
                                                , FChar* dst_char
                                                , const int length
                                                , FPoint pos
                                                , bool line_covered) const
{
  if ( length < 1 )
    return;

  const auto handle_region = \
      [this, &pos, &dst_char, &line_covered] ( bool transparent
                                             , std::size_t count
                                             , const FChar* start )
  {
    if ( transparent || line_covered )
      putMultiLayerAreaLine (dst_char, count, pos);
    else
      putAreaLine (*start, *dst_char, count);

    pos.x_ref() += count;
    std::advance(dst_char, count);
  };

  const auto* end_char{src_char + length};
  const auto* region_start{src_char};
  std::size_t region_count{1};
  bool is_region_transparent{isFCharTransparent(*src_char)};

  for (const auto* current_char = src_char + 1; current_char < end_char; ++current_char)
  {
    const bool is_current_char_transparent = isFCharTransparent(*current_char);

    if ( is_current_char_transparent == is_region_transparent )
      ++region_count;
    else
    {
      handle_region (is_region_transparent, region_count, region_start);
      // Reset for the next region
      is_region_transparent = is_current_char_transparent;
      region_start = current_char;
      region_count = 1;
    }
  }

  // Handle the final region
  handle_region (is_region_transparent, region_count, region_start);
}

//----------------------------------------------------------------------
inline void FVTerm::addAreaLineWithTransparency ( const FChar* src_char
                                                , FChar* dst_char
                                                , const std::size_t length ) const
{
  if ( length == 0 )
    return;

  const auto handle_region = \
      [this, &dst_char] ( bool transparent
                        , std::size_t count
                        , const FChar* start )
  {
    if ( transparent )
      addTransparent (start, dst_char, count);
    else
      putNonTransparent (start, dst_char, count);
  };

  const auto* end_char{src_char + length};
  const auto* region_start{src_char};
  std::size_t region_count{1};
  bool is_region_transparent{isFCharTransparent(*src_char)};

  for (const auto* current_char = src_char + 1; current_char < end_char; ++current_char)
  {
    const bool is_current_char_transparent = isFCharTransparent(*current_char);

    if ( is_current_char_transparent == is_region_transparent )
      ++region_count;
    else
    {
      handle_region (is_region_transparent, region_count, region_start);
      // Reset for the next region
      is_region_transparent = is_current_char_transparent;
      region_start = current_char;
      region_count = 1;
    }
  }

  // Handle the final region
  handle_region (is_region_transparent, region_count, region_start);
}

//----------------------------------------------------------------------
inline void FVTerm::addTransparentAreaLine ( const FChar& src_char
                                           , FChar& dst_char
                                           , const std::size_t length ) const
{
  const auto* src = &src_char;
  auto* dst = &dst_char;
  const auto* end = src + length;

  while ( src < end )  // column loop
  {
    addTransparentAreaChar (*src, *dst);
    std::advance(src, 1);  // src character
    std::advance(dst, 1);  // dst character
  }
}

//----------------------------------------------------------------------
inline void FVTerm::addTransparentAreaChar (const FChar& src_char, FChar& dst_char) const
{
  if ( src_char.attr.bit.transparent )  // Transparent
    return;  // Leave character on vterm untouched

  if ( src_char.attr.bit.color_overlay )  // Color overlay
  {
    // Get covered character + add the current color
    dst_char.color.data = src_char.color.data;
    dst_char.attr.data = src_char.attr.data & ~internal::var::color_overlay_mask;

    if ( isTransparentInvisible(dst_char) )
      dst_char.ch[0] = L' ';

    return;
  }

  if ( src_char.attr.bit.inherit_background )
  {
    // Add the covered background to this character
    auto bg_color = dst_char.color.pair.bg;
    dst_char = src_char;
    dst_char.color.pair.bg = bg_color;
    dst_char.attr.data &= ~internal::var::print_reset_mask;
    return;
  }

  // Default
  dst_char = src_char;
}

//----------------------------------------------------------------------
auto FVTerm::clearFullArea (FTermArea* area, FChar& fillchar) const -> bool
{
  // Clear area
  std::fill (area->data.begin(), area->data.end(), fillchar);

  if ( area != vdesktop.get() )  // Is the area identical to the desktop?
    return false;

  // Try to clear the terminal rapidly with a control sequence
  if ( foutput->clearTerminal (fillchar.ch[0]) )
  {
    fillchar.attr.bit.printed = true;
    std::fill (vterm->data.begin(), vterm->data.end(), fillchar);
    saveCurrentVTerm();
  }
  else
  {
    auto* vdesktop_changes = &vdesktop->changes[0];
    const auto* vdesktop_changes_end = vdesktop_changes
                                     + unsigned(vdesktop->size.height);

    while ( vdesktop_changes < vdesktop_changes_end )
    {
      vdesktop_changes->xmin = 0;
      vdesktop_changes->xmax = uInt(vdesktop->size.width) - 1;
      vdesktop_changes->trans_count = 0;
      std::advance(vdesktop_changes, 1);
    }

    vdesktop->has_changes = true;
  }

  return true;
}

//----------------------------------------------------------------------
void FVTerm::clearAreaWithShadow (FTermArea* area, const FChar& fillchar) const noexcept
{
  FChar t_char = fillchar;
  t_char.ch[0] = L'\0';
  t_char.attr.bit.transparent = true;
  t_char.attr.bit.char_width = 0;
  const int total_width = getFullAreaWidth(area);

  for (auto y{0}; y < area->size.height; y++)
  {
    // Clear area
    auto* area_pos = &area->getFChar(0, y);
    std::fill (area_pos, area_pos + area->size.width, fillchar);
    // Make right shadow transparent
    auto* shadow_begin = &area->getFChar(area->size.width, y);
    std::fill (shadow_begin, shadow_begin + area->shadow.width, t_char);
  }

  // Make bottom shadow transparent
  for (auto y{0}; y < area->shadow.height; y++)
  {
    auto* area_pos = &area->getFChar(0, area->size.height + y);
    std::fill (area_pos, area_pos + total_width, t_char);
  }
}

//----------------------------------------------------------------------
inline auto FVTerm::printWrap (FTermArea* area) const -> bool
{
  bool end_of_area{false};
  const int& width  = area->size.width;
  const int& height = area->size.height;
  const int& rsh    = area->shadow.width;
  const int& bsh    = area->shadow.height;

  // Line break at right margin
  if ( area->cursor.x > width + rsh )
  {
    area->cursor.x = 1;
    area->cursor.y++;
  }

  // Prevent up scrolling
  if ( area->cursor.y > height + bsh )
  {
    area->cursor.y--;
    end_of_area = true;
  }

  return end_of_area;
}

//----------------------------------------------------------------------
inline auto FVTerm::changedToTransparency (const FChar& from, const FChar& to) const -> bool
{
  return ( uInt8(~ from.attr.byte[1] & internal::var::b1_print_transparent_mask)
         & uInt8(    to.attr.byte[1] & internal::var::b1_print_transparent_mask) ) != 0;
}

//----------------------------------------------------------------------
inline auto FVTerm::changedFromTransparency (const FChar& from, const FChar& to) const -> bool
{
  return changedToTransparency(to, from);
}

//----------------------------------------------------------------------
inline auto FVTerm::printCharacterOnCoordinate ( FTermArea* area
                                               , const FChar& ch ) const noexcept -> std::size_t
{
  const int ax = area->cursor.x - 1;
  const int ay = area->cursor.y - 1;
  auto* ac = &area->getFChar(ax, ay);  // area character

  if ( *ac == ch )  // compare with an overloaded operator
    return ac->attr.bit.char_width;

  auto& line_changes = area->changes[unsigned(ay)];

  if ( changedToTransparency(*ac, ch) )
  {
    // add one transparent character form line
    line_changes.trans_count++;
  }

  if ( changedFromTransparency(*ac, ch) )
  {
    // remove one transparent character from line
    line_changes.trans_count--;
  }

  // copy character to area
  *ac = ch;

  if ( ac->attr.bit.char_width == 0 )
  {
    const auto new_char_width = getColumnWidth(ac->ch[0]);

    if ( new_char_width == 0 )
      return 0;

    addColumnWidth(*ac, new_char_width);  // Add column width
  }

  const auto padding = unsigned(ac->attr.bit.char_width == 2);

  if ( uInt(ax) < line_changes.xmin )
    line_changes.xmin = uInt(ax);

  if ( uInt(ax) + padding > line_changes.xmax )
    line_changes.xmax = uInt(ax) + padding;

  return ac->attr.bit.char_width;
}

//----------------------------------------------------------------------
inline void FVTerm::printPaddingCharacter ( FTermArea* area
                                          , const FChar& term_char ) const
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
inline void FVTerm::putNonTransparent ( const FChar* start_char
                                      , FChar*& dst_char
                                      , std::size_t& non_trans_count ) const
{
  if ( non_trans_count == 0 )
    return;

  putAreaLine(*start_char, *dst_char, non_trans_count);
  dst_char += non_trans_count;  // dst character
  non_trans_count = 0;
}

//----------------------------------------------------------------------
inline void FVTerm::addTransparent ( const FChar* start_char
                                   , FChar*& dst_char
                                   , std::size_t& trans_count ) const
{
  if ( trans_count == 0 )
    return;

  addTransparentAreaLine (*start_char, *dst_char, trans_count);
  dst_char += trans_count;  // dst character
  trans_count = 0;
}

//----------------------------------------------------------------------
inline void FVTerm::addVDesktopToListIfExists (FTermAreaList& list) const
{
  // Adds the virtual desktop area, if it exists

  if ( vdesktop )
    list.push_back(vdesktop.get());
}

//----------------------------------------------------------------------
inline void FVTerm::determineCoveredAreas (FTermArea* src) const
{
  const auto* vterm_win_list = getWindowList();
  covered_areas_buffer.clear();
  resetLineCoveredState(src);

  if ( ! vterm_win_list || vterm_win_list->empty() )
  {
    addVDesktopToListIfExists(covered_areas_buffer);
    return;
  }

  const auto end = vterm_win_list->crend();

  // Areas from top to bottom
  for (auto iter = vterm_win_list->crbegin(); iter < end; ++iter)
  {
    const auto& vterm_obj = *iter;

    if ( ! vterm_obj )
      continue;

    const auto* win = vterm_obj->getVWin();

    if ( ! win || ! win->visible || win->layer < 1
      || ! win->isOverlapped(src) )
      continue;

    covered_areas_buffer.push_back(win);

    if ( win->layer > src->layer )
      determineLineCoveredState(win, src);
  }

  addVDesktopToListIfExists(covered_areas_buffer);
}

//----------------------------------------------------------------------
inline void FVTerm::resetLineCoveredState (FTermArea* src) const
{
  std::for_each ( std::begin(src->changes)
                , std::end(src->changes)
                , [] (auto& changes) { changes.covered = false; } );
}

//----------------------------------------------------------------------
inline void FVTerm::determineLineCoveredState ( const FTermArea* const win
                                              , FTermArea* src ) const
{
  // Requirements: The two areas have been checked for overlap

  const int win_current_height = win->minimized ? win->min_size.height
                                                : win->size.height + win->shadow.height;
  const int src_current_height = src->minimized ? src->min_size.height
                                                : src->size.height + src->shadow.height;
  const int win_y_min = win->position.y;
  const int win_y_max = win_y_min + win_current_height;
  const int src_y_min = src->position.y;
  const int src_y_max = src_y_min + src_current_height;
  const auto start = std::size_t(std::max(win_y_min, src_y_min) - src->position.y);
  const auto end = std::size_t(std::min(win_y_max, src_y_max) - src->position.y);
  auto* changes = src->changes.data() + start;
  const auto* const changes_end = src->changes.data() + end;

  for (; changes < changes_end; ++changes)
    changes->covered = true;
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
auto FVTerm::canUpdateTerminalNow() const -> bool
{
  // Check if terminal updates were stopped, application is stopping,
  // VTerm has no changes, or the drawing is not completed

  return  ! FVTerm::areTerminalUpdatesPaused()
       && ! FApplication::isQuit()
       && ( foutput->isFlushTimeout() || FVTerm::isTerminalUpdateForced() )
       && FVTerm::hasPendingTerminalUpdates()
       && FVTerm::isDrawingFinished();
}

//----------------------------------------------------------------------
auto FVTerm::hasPendingUpdates (const FTermArea* area) noexcept -> bool
{
  return (area && area->has_changes);
}

}  // namespace finalcut
