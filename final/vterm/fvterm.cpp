/***********************************************************************
* fvterm.cpp - Virtual terminal implementation                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2016-2026 Markus Gans                                      *
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

constexpr auto getTransparentMask() noexcept -> uInt32
{
  FCharAttribute mask{};
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  return FCharAttribute_to_uInt32(mask);
}

constexpr auto getPrintTransparentMask() noexcept -> uInt32
{
  FCharAttribute mask{};
  mask.transparent = true;
  mask.color_overlay = true;
  mask.inherit_background = true;
  mask.no_changes = true;
  mask.printed = true;
  return FCharAttribute_to_uInt32(mask);
}

constexpr auto getResetMask() noexcept -> uInt32
{
  FCharAttribute mask{};
  mask.no_changes = true;
  mask.printed = true;
  return ~FCharAttribute_to_uInt32(mask);
}

constexpr auto getColorOverlayResetMask() noexcept -> uInt32
{
  FCharAttribute mask{};
  mask.color_overlay = true;
  mask.reverse = true;
  mask.standout = true;
  mask.no_changes = true;
  mask.printed = true;
  return ~FCharAttribute_to_uInt32(mask);
}

struct var
{
  static bool fvterm_initialized;  // Global init state
  static constexpr auto transparent_mask = getTransparentMask();
  static constexpr auto print_transparent_mask = getPrintTransparentMask();
  static constexpr auto print_reset_mask = getResetMask();
  static constexpr auto color_overlay_reset_mask = getColorOverlayResetMask();
};

bool             var::fvterm_initialized{false};
constexpr uInt32 var::transparent_mask;
constexpr uInt32 var::print_transparent_mask;
constexpr uInt32 var::print_reset_mask;
constexpr uInt32 var::color_overlay_reset_mask;

}  // namespace internal

// static class attributes
bool                 FVTerm::draw_completed{false};
bool                 FVTerm::skip_one_vterm_update{false};
bool                 FVTerm::no_terminal_updates{false};
bool                 FVTerm::force_terminal_update{false};
FVTerm::FTermRegion* FVTerm::active_region{nullptr};
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
  const auto* win = getPrintRegion();

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
  if ( auto* win = getPrintRegion() )
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
  if ( ! print_region )
    getPrintRegion();

  if ( ! instance || ! print_region || print_region->preproc_list.empty() )
    return false;

  auto iter = print_region->preproc_list.cbegin();

  while ( iter != print_region->preproc_list.cend() )
  {
    if ( iter->get()->instance.get() == instance )
      return true;

    ++iter;
  }

  return false;
}

//----------------------------------------------------------------------
void FVTerm::clearRegion (wchar_t fillchar)
{
  clearRegion (vwin.get(), fillchar);
}

//----------------------------------------------------------------------
void FVTerm::createVDesktop (const FSize& size) noexcept
{
  // Initialize virtual desktop region

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  vdesktop = createRegion(box);
  vdesktop->visible = true;
}

//----------------------------------------------------------------------
void FVTerm::createVTerm (const FSize& size) noexcept
{
  // Initialize virtual terminal

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  vterm = createRegion(box);
  vterm_old = createRegion(box);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (const FSize& size) const noexcept
{
  // Resize virtual terminal

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  resizeRegion (box, vterm.get());
  resizeRegion (box, vterm_old.get());
}

//----------------------------------------------------------------------
void FVTerm::putVTerm() const
{
  // Update the entire terminal screen

  auto xmax = uInt(vterm->size.width - 1);
  auto ymax = uInt(vterm->size.height - 1);
  auto vterm_changes = vterm->changes_in_line.begin();
  std::for_each ( vterm_changes
                , vterm_changes + vterm->size.height
                , [&xmax] (auto& vterm_changes_line) noexcept
                  {
                    vterm_changes_line.xmin = 0;
                    vterm_changes_line.xmax = xmax;
                  } );
  vterm->changes_in_row = {0, ymax};
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
  auto& vterm_changes = vterm->changes_in_line[unsigned(y)];
  uInt& xmin = vterm_changes.xmin;
  uInt& xmax = vterm_changes.xmax;

  if ( xmin > xmax )  // No changes
    return;

  auto first = vterm->getFCharIterator(int(xmin), int(y));
  auto first_old = vterm_old->getFCharIterator(int(xmin), int(y));
  auto last = vterm->getFCharIterator(int(xmax), int(y));
  auto last_old = vterm_old->getFCharIterator(int(xmax), int(y));

  while ( xmin < xmax && *first == *first_old )
  {
    xmin++;
    ++first;
    ++first_old;
  }

  while ( last >= first && *last == *last_old )
  {
    xmax--;
    --last;
    --last_old;
  }

  while ( last > first )
  {
    if ( *last == *last_old )
      last->setBit(FAttribute::set::no_changes);

    --last;
    --last_old;
  }
}

//----------------------------------------------------------------------
void FVTerm::addPreprocessingHandler ( const FVTerm* instance
                                     , FPreprocessingFunction&& function )
{
  if ( ! print_region )
    getPrintRegion();

  if ( ! instance || ! print_region )
    return;

  delPreprocessingHandler (instance);
  auto obj = std::make_unique<FVTermPreprocessing> \
      (instance, std::move(function));
  print_region->preproc_list.emplace_back(std::move(obj));
}

//----------------------------------------------------------------------
void FVTerm::delPreprocessingHandler (const FVTerm* instance)
{
  if ( ! print_region )
    getPrintRegion();

  if ( ! instance || ! print_region || print_region->preproc_list.empty() )
    return;

  auto iter = print_region->preproc_list.cbegin();

  while ( iter != print_region->preproc_list.cend() )
  {
    if ( iter->get()->instance.get() == instance )
      iter = print_region->preproc_list.erase(iter);
    else
      ++iter;
  }
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
auto FVTerm::print (FTermRegion* region, const FString& string) noexcept -> int
{
  if ( ! region || string.isEmpty() )
    return -1;

  vterm_buffer.print(string);
  return print (region, vterm_buffer);
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
auto FVTerm::print (FTermRegion* region, const std::vector<FChar>& term_string) noexcept -> int
{
  if ( ! region || term_string.empty() )
    return -1;

  vterm_buffer.assign(term_string.cbegin(), term_string.cend());
  return print (region, vterm_buffer);
}

//----------------------------------------------------------------------
auto FVTerm::print (FVTermBuffer& buffer) noexcept -> int
{
  if ( buffer.isEmpty() )
    return -1;

  auto* region = getPrintRegion();
  return region ? print (region, buffer) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (const FVTermBuffer& buffer) noexcept -> int
{
  if ( buffer.isEmpty() )
    return -1;

  auto* region = getPrintRegion();
  return region ? print (region, buffer) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermRegion* region, FVTermBuffer& buffer) const noexcept -> int
{
  auto len = print (region, static_cast<const FVTermBuffer&>(buffer));
  buffer.clear();  // Clear FVTermBuffer after printing
  return len;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermRegion* region, const FVTermBuffer& buffer) const noexcept -> int
{
  int len{0};

  if ( ! region || buffer.isEmpty() )
    return -1;

  auto ac = region->getFCharIterator( region->cursor.x - 1
                                    , region->cursor.y - 1 );  // Region character iterator

  for (const auto& fchar : buffer)
  {
    if ( printCharacter(region, ac, fchar) == -1 )  // Print next character
      break;  // No region or end of region reached

    len++;
  }

  return len;
}

//----------------------------------------------------------------------
auto FVTerm::print (wchar_t c) noexcept -> int
{
  auto* region = getPrintRegion();
  return region ? print (region, c) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermRegion* region, wchar_t c) noexcept -> int
{
  if ( ! region )
    return -1;

  static const auto& next_attr = getAttribute();
  nc.color.data = next_attr.color.data;
  static constexpr auto mask = 0x0000ffffU;
  nc.attr.data = next_attr.attr.data & mask;
  nc.ch[0] = c;
  nc.ch[1] = L'\0';
  return print (region, nc);
}

//----------------------------------------------------------------------
auto FVTerm::print (const FChar& term_char) noexcept -> int
{
  auto* region = getPrintRegion();
  return region ? print (region, term_char) : -1;
}

//----------------------------------------------------------------------
auto FVTerm::print (FTermRegion* region, const FChar& term_char) const noexcept -> int
{
  if ( ! region )
    return -1;  // No region

  auto ac = region->getFCharIterator( region->cursor.x - 1
                                    , region->cursor.y - 1 );  // Region character iterator
  return printCharacter(region, ac, term_char);
}

//----------------------------------------------------------------------
void FVTerm::flush() const
{
  foutput->flush();
}


// protected methods of FVTerm
//----------------------------------------------------------------------
auto FVTerm::getPrintRegion() -> FTermRegion*
{
  // returns the print region of this object

  if ( print_region )
    return print_region;

  if ( vwin )
  {
    print_region = vwin.get();
    return print_region;
  }

  if ( child_print_region )
  {
    print_region = child_print_region;
    return print_region;
  }

  return vdesktop.get();
}

//----------------------------------------------------------------------
auto FVTerm::createRegion (const FShadowBox& shadowbox) -> std::unique_ptr<FTermRegion>
{
  // initialize virtual window

  auto region = std::make_unique<FTermRegion>();
  region->setOwner<FVTerm*>(this);
  region->encoding = foutput->getEncoding();
  resizeRegion (shadowbox, region.get());
  return region;
}

//----------------------------------------------------------------------
auto FVTerm::createRegion (const FRect& box) -> std::unique_ptr<FTermRegion>
{
  // initialize virtual window

  const auto no_shadow = FSize(0, 0);
  return createRegion ({box, no_shadow});
}

//----------------------------------------------------------------------
void FVTerm::resizeRegion ( const FShadowBox& shadowbox
                          , FTermRegion* region ) const
{
  // Resize the virtual window to a new size

  assert ( isRegionValid(shadowbox) );

  if ( ! region )
    return;

  if ( isSizeEqual(region, shadowbox) )
  {
    region->position.x = shadowbox.box.getX();
    region->position.y = shadowbox.box.getY();
    return;  // Move only
  }

  const auto full_width  = shadowbox.box.getWidth()
                         + shadowbox.shadow.getWidth();
  const auto full_height = shadowbox.box.getHeight()
                         + shadowbox.shadow.getHeight();

  if ( ! tryResizeRegion(region, full_width, full_height) )
    return;

  updateRegionProperties (region, shadowbox);
  // Set default FChar in region
  resetTextRegionToDefault (region, { FSize{full_width, full_height} });
}

//----------------------------------------------------------------------
void FVTerm::resizeRegion (const FRect& box, FTermRegion* region) const
{
  // Resize the virtual window to a new size

  const auto no_shadow = FSize(0, 0);
  resizeRegion ({box, no_shadow}, region);
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
auto FVTerm::updateVTermCursor (const FTermRegion* region) const noexcept -> bool
{
  if ( ! (region && isActive(region) && region->visible) )
    return false;

  if ( region->input_cursor_visible )
  {
    // Region cursor position
    const int cx = region->input_cursor.x;
    const int cy = region->input_cursor.y;
    // Terminal position = region position + region cursor position
    const int x  = region->position.x + cx;
    const int y  = region->position.y + cy;

    if ( isInsideRegion (FPoint{cx, cy}, region)
      && isInsideTerminal (FPoint{x, y})
      && isCovered (FPoint{x, y}, region) == CoveredState::None )
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
auto FVTerm::isCursorHideable() const noexcept -> bool
{
  return foutput->isCursorHideable();
}

//----------------------------------------------------------------------
void FVTerm::setRegionCursor ( const FPoint& pos
                             , bool visible
                             , FTermRegion* region ) const noexcept
{
  if ( ! region )
    return;

  region->setInputCursorPos (pos.getX() - 1, pos.getY() - 1);
  region->input_cursor_visible = visible;
}

//----------------------------------------------------------------------
void FVTerm::getRegion (const FPoint& pos, FTermRegion* region) const noexcept
{
  // Copies a block from the virtual terminal position to the given region

  if ( ! region )
    return;

  const int ax = pos.getX() - 1;
  const int ay = pos.getY() - 1;

  if ( ax < 0 || ay < 0 )
    return;

  const int y_end  = std::min(vterm->size.height - ay, region->size.height);
  const int length = std::min(vterm->size.width - ax, region->size.width);
  const auto region_width = getFullRegionWidth(region);
  const auto vterm_width = getFullRegionWidth(vterm.get());
  auto line_changes = region->changes_in_line.begin();
  auto ac = region->data.begin();  // Region character
  auto tc = vterm->data.cbegin() + (ay * vterm_width);  // Terminal character

  for (auto y{0}; y < y_end; y++)  // line loop
  {
    putRegionLine (*(tc + unsigned(ax)), *ac, length);
    line_changes->xmin = 0;
    line_changes->xmax = uInt(length - 1);
    ++line_changes;
    ac += region_width;
    tc += vterm_width;
  }

  auto& changes_in_row = region->changes_in_row;
  changes_in_row.ymin = 0;
  changes_in_row.ymax = std::max(changes_in_row.ymax, uInt(y_end - 1));
}

//----------------------------------------------------------------------
void FVTerm::getRegion (const FRect& box, FTermRegion* region) const noexcept
{
  // Copies a block from the virtual terminal rectangle to the given region

  if ( ! region )
    return;

  int x = box.getX() - 1;
  int y = box.getY() - 1;
  int dx = x - region->position.x;
  int dy = y - region->position.y;
  auto w = std::min(region->size.width - dx, int(box.getWidth()));
  auto h = std::min(region->size.height - dy, int(box.getHeight()));

  if ( x < 0 || y < 0 )
    return;

  if ( dx < 0 ) { w += dx; x -= dx; dx = 0; }
  if ( dy < 0 ) { h += dy; y -= dy; dy = 0; }
  const int y_end = std::min(vterm->size.height - y, h);
  const int length = std::min(vterm->size.width - x, w);
  const auto region_width = getFullRegionWidth(region);
  const auto vterm_width = getFullRegionWidth(vterm.get());
  auto ac = region->data.begin() + (dy * region_width);  // Region character
  auto tc = vterm->data.cbegin() + (y * vterm_width);  // Terminal character

  if ( length < 1 )
    return;

  auto line_changes = region->changes_in_line.begin() + dy;

  for (auto line{0}; line < y_end; line++)  // line loop
  {
    putRegionLine (*(tc + x), *(ac + dx), length);
    line_changes->xmin = std::min(line_changes->xmin, uInt(dx));
    line_changes->xmax = std::max(line_changes->xmax, uInt(dx + length - 1));
    ++line_changes;
    ac += region_width;
    tc += vterm_width;
  }

  auto& changes_in_row = region->changes_in_row;
  changes_in_row.ymin = 0;
  changes_in_row.ymax = std::max(changes_in_row.ymax, uInt(y_end - 1));
}

//----------------------------------------------------------------------
void FVTerm::addLayer (FTermRegion* region) const noexcept
{
  // Transmit changes in region to the virtual terminal (add layer to vterm)

  if ( ! region || ! region->visible )
    return;

  // Compute geometry and decide early whether to render
  LayerGeometry geometry = computeLayerGeometry(region);

  // Early exit if region is completely outside vterm
  if ( ! isLayerOutsideVTerm(geometry) )
    return;

  // Call the preprocessing handler methods (child region change handling)
  callPreprocessingHandler(region);

  // Create a stack by combining identical rows in sequence
  line_changes_batch.clear();  // Clear buffer
  buildLineChangeBatch(region, geometry);

  if ( line_changes_batch.empty() )
    return;

  // Apply the batches to vterm data
  applyLineBatch(region, geometry);

  // Update the range of changed rows and the cursor on vterm
  updateVTermChangesFromBatch(geometry);
  updateVTermCursor(region);
}

//----------------------------------------------------------------------
void FVTerm::putRegion (const FPoint& pos, FTermRegion* region) const noexcept
{
  // Copies the given region block to the virtual terminal position

  if ( ! region || ! region->visible )
    return;

  copyRegion (vterm.get(), pos, region);
}

//----------------------------------------------------------------------
void FVTerm::copyRegion (FTermRegion* dst, const FPoint& pos, FTermRegion* src) const noexcept
{
  // Copies the src region to the dst region position

  if ( ! dst || ! src )
    return;

  if ( dst == vterm.get() )
    skip_one_vterm_update = true;

  const int src_width = getFullRegionWidth(src);
  const int dst_width = getFullRegionWidth(dst);
  const int src_height = src->minimized ? src->min_size.height : getFullRegionHeight(src);
  const int ax = std::max(0, pos.getX() - 1);
  const int ay = std::max(0, pos.getY() - 1);
  const int ol = std::max(0, 1 - pos.getX());  // outside left
  const int ot = std::max(0, 1 - pos.getY());  // outside top
  const int y_end = std::min(dst->size.height - ay, src_height - ot);
  const int length = std::min(dst->size.width - ax, src_width - ol);

  if ( length < 1 || y_end < 1 )
    return;

  auto src_changes = src->changes_in_line.cbegin();
  auto dst_changes = dst->changes_in_line.begin() + ay;
  auto sc = src->data.cbegin() + (src_width * ot) + ol;  // src character ptr
  auto dc = dst->data.begin() + (dst_width * ay) + ax;  // dst character ptr

  if ( skip_one_vterm_update )  // dst is the virtual terminal
    determineCoveredRegions(src);

  for (int y{0}; y < y_end; y++)  // line loop
  {
    if ( skip_one_vterm_update && src_changes->trans_count > 0 )
    {
      // Line with hidden and transparent characters
      putRegionLineWithTransparency (sc, dc, length, {ax, ay + y}, src_changes->covered);
    }
    else
    {
      // Line has only covered characters
      putRegionLine (*sc, *dc, length);
    }

    dst_changes->xmin = std::min(dst_changes->xmin, uInt(ax));
    dst_changes->xmax = std::max(dst_changes->xmax, uInt(ax + length - 1));
    ++src_changes;
    ++dst_changes;
    sc += src_width;
    dc += dst_width;
  }

  dst->changes_in_row.ymin = std::min(dst->changes_in_row.ymin, uInt(ay));
  dst->changes_in_row.ymax = std::max(dst->changes_in_row.ymax, uInt(ay + y_end - 1));
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
    (*iter)->getPrintRegion()->layer = int(std::distance(begin, iter) + 1);
    ++iter;
  }
}

//----------------------------------------------------------------------
void FVTerm::scrollRegionForward (FTermRegion* region)
{
  // Scrolls the entire region on line up

  if ( ! region || region->size.height <= 1 )
    return;

  const int y_max = region->size.height - 1;
  const int x_max = region->size.width - 1;
  auto line_changes = region->changes_in_line.begin();
  const auto region_width = getFullRegionWidth(region);
  auto sc = region->data.cbegin() + region_width;  // source character
  auto dc = region->data.begin();  // destination character

  for (auto y{0}; y < y_max; y++)
  {
    putRegionLine (*sc, *dc, region->size.width);
    line_changes->xmin = 0;
    line_changes->xmax = uInt(x_max);
    ++line_changes;
    sc += region_width;
    dc += region_width;
  }

  // insert a new line below
  const auto& lc = region->getFChar(x_max, region->size.height - 2);  // last character
  nc.color.data = lc.color.data;
  nc.attr = lc.attr;
  nc.ch[0] = L' ';
  nc.ch[1] = L'\0';
  dc = region->data.begin() + (region_width * y_max);  // destination character
  std::fill (dc, dc + region->size.width, nc);
  auto new_line_changes = region->changes_in_line.begin() + y_max;
  new_line_changes->xmin = 0;
  new_line_changes->xmax = uInt(x_max);
  region->changes_in_row = {0, uInt(y_max)};
  region->has_changes = true;

  if ( region == vdesktop.get() )
    scrollTerminalForward();  // Scrolls the terminal up one line
}

//----------------------------------------------------------------------
void FVTerm::scrollRegionReverse (FTermRegion* region)
{
  // Scrolls the entire region one line down

  if ( ! region || region->size.height <= 1 )
    return;

  const int y_max = region->size.height - 1;
  const int x_max = region->size.width - 1;
  auto line_changes = region->changes_in_line.begin() + y_max;
  const auto region_width = getFullRegionWidth(region);
  auto sc = region->data.cbegin() + (region_width * (y_max - 1));  // source character
  auto dc = region->data.begin() + (region_width * y_max);  // destination character

  for (auto y = y_max; y > 0; y--)
  {
    putRegionLine (*sc, *dc, region->size.width);
    line_changes->xmin = 0;
    line_changes->xmax = uInt(x_max);
    --line_changes;
    sc -= region_width;
    dc -= region_width;
  }

  // insert a new line above
  const auto& lc = region->getFChar(0, 1);  // last character
  nc.color.data = lc.color.data;
  nc.attr = lc.attr;
  nc.ch[0] = L' ';
  nc.ch[1] = L'\0';
  dc = region->data.begin();  // destination character
  std::fill (dc, dc + region->size.width, nc);
  auto new_line_changes = region->changes_in_line.begin() + y_max;
  new_line_changes->xmin = 0;
  new_line_changes->xmax = uInt(x_max);
  region->changes_in_row = {0, uInt(y_max)};
  region->has_changes = true;

  if ( region == vdesktop.get() )
    scrollTerminalReverse();  // Scrolls the terminal down one line
}

//----------------------------------------------------------------------
void FVTerm::clearRegion (FTermRegion* region, wchar_t fillchar) noexcept
{
  // Clear the region with the current attributes

  static const auto& next_attr = getAttribute();
  nc.color.data = next_attr.color.data;
  nc.attr = next_attr.attr;
  nc.ch[0] = fillchar;  // Current attributes with the fill character
  nc.ch[1] = L'\0';
  nc.setCharWidth(uInt32(getColumnWidth(nc.ch[0])));

  if ( ! region || region->data.empty() )
  {
    foutput->clearTerminal (fillchar);
    return;
  }

  const auto width = uInt(getFullRegionWidth(region));

  if ( region->shadow.width == 0 )
  {
    if ( clearFullRegion(region, nc) )
      return;
  }
  else
    clearRegionWithShadow(region, nc);

  auto line_changes = region->changes_in_line.begin();

  for (auto i{0}; i < region->size.height; i++)
  {
    line_changes->xmin = 0;
    line_changes->xmax = width - 1;

    if ( nc.isBitSet(FAttribute::set::transparent)
      || nc.isBitSet(FAttribute::set::color_overlay)
      || nc.isBitSet(FAttribute::set::inherit_background) )
      line_changes->trans_count = width;
    else if ( region->shadow.width != 0 )
      line_changes->trans_count = uInt(region->shadow.width);
    else
      line_changes->trans_count = 0;

    ++line_changes;
  }

  line_changes = region->changes_in_line.begin() + region->size.height;

  for (auto i{0}; i < region->shadow.height; i++)
  {
    line_changes->xmin = 0;
    line_changes->xmax = width - 1;
    line_changes->trans_count = width;
    ++line_changes;
  }

  region->changes_in_row = {0, uInt(getFullRegionHeight(region) - 1)};
  region->has_changes = true;
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
  resetRegionEncoding();
}


// private methods of FVTerm
//----------------------------------------------------------------------
void FVTerm::setGlobalFVTermInstance (FVTerm* ptr) noexcept
{
  getGlobalFVTermInstance() = ptr;
}

//----------------------------------------------------------------------
auto FVTerm::getGlobalFVTermInstance() noexcept -> FVTerm*&
{
  static FVTerm* init_object{nullptr};
  return init_object;
}

//----------------------------------------------------------------------
auto FVTerm::isInitialized() noexcept -> bool
{
  return internal::var::fvterm_initialized;
}

//----------------------------------------------------------------------
void FVTerm::resetRegionEncoding() const
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
      if ( pcall->instance && pcall->instance->child_print_region )
        pcall->instance->child_print_region->encoding = encoding;
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::resetTextRegionToDefault ( FTermRegion* region
                                             , const FSize& size ) const noexcept
{
  static constexpr FChar default_char
  {
    { L' ',  L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    default_color_pair,
    { 0x00080000U }  // char_width = 1
  };
  std::fill (region->data.begin(), region->data.end(), default_char);

  const FTermRegion::FLineChanges unchanged { uInt(size.getWidth())
                                          , 0, 0, false };
  std::fill (region->changes_in_line.begin(), region->changes_in_line.end(), unchanged);
  region->changes_in_row = { uInt(size.getHeight()), 0};
}

//----------------------------------------------------------------------
inline auto FVTerm::resizeTextRegion ( FTermRegion* region
                                     , std::size_t height
                                     , std::size_t size ) const -> bool
{
  // Set the number of lines for changes to "height"
  // and resize the text region to "size" elements

  region->changes_in_line.resize(height);
  resizeTextRegion (region, size);
  return true;
}

//----------------------------------------------------------------------
inline auto FVTerm::resizeTextRegion (FTermRegion* region, std::size_t size) const -> bool
{
  // Resize text region to "size" FChar elements

  region->data.resize(size);
  return true;
}

//----------------------------------------------------------------------
auto FVTerm::isCovered (const FPoint& pos, const FTermRegion* region) const noexcept -> CoveredState
{
  // Determines the covered state for the given position

  const auto* vterm_win_list{getWindowList()};

  if ( ! region || ! vterm_win_list || vterm_win_list->empty()
    || vterm_win_list->back()->getVWin() == region )  // Top window can't be covered
    return CoveredState::None;

  const auto pos_x = pos.getX();
  const auto pos_y = pos.getY();

  auto is_covered{CoveredState::None};  // Initial state: no coverage
  bool found{ region == vdesktop.get() };

  for (const auto& vterm_obj : *vterm_win_list)  // Iteration through window list
  {
    const auto* win = vterm_obj->getVWin();

    if ( ! win || ! win->visible )
      continue;

    if ( ! found )  // Check if we've found the target region yet
    {
      found = ( region == win );
      continue;
    }

    const int win_x_min = win->position.x;
    const int win_y_min = win->position.y;

    if ( pos_x < win_x_min || pos_y < win_y_min )
      continue;

    const int current_height = win->minimized ? win->min_size.height
                                              : getFullRegionHeight(win);
    const int win_x_max = win_x_min + getFullRegionWidth(win);
    const int win_y_max = win_y_min + current_height;

    if ( pos_x >= win_x_max || pos_y >= win_y_max )
      continue;

    // Position is covered by this window - check for transparency
    const auto delta_x = pos_x - win->position.x;
    const auto delta_y = pos_y - win->position.y;
    const auto width = getFullRegionWidth(win);
    const auto index = unsigned(delta_y) * unsigned(width) + unsigned(delta_x);
    const auto& character = win->data[index];

    if ( character.isBitSet(FAttribute::set::transparent) )
      continue;

    if ( character.isBitSet(FAttribute::set::color_overlay) )  // Color overlay = half covered
      is_covered = CoveredState::Half;  // Mark as partially covered
    else
      return CoveredState::Full;  // Fully covered
  }

  return is_covered;
}

//----------------------------------------------------------------------
inline auto FVTerm::isRegionValid (const FShadowBox& shadowbox) const noexcept -> bool
{
  return shadowbox.box.getY() >= 0
      && shadowbox.box.getWidth() > 0
      && shadowbox.box.getHeight() > 0;
}

//----------------------------------------------------------------------
inline auto FVTerm::isSizeEqual (const FTermRegion* region, const FShadowBox& shadowbox) const noexcept -> bool
{
  return region->size.width == int(shadowbox.box.getWidth())
      && region->size.height == int(shadowbox.box.getHeight())
      && region->shadow.width == int(shadowbox.shadow.getWidth())
      && region->shadow.height == int(shadowbox.shadow.getHeight());
}

//----------------------------------------------------------------------
constexpr auto FVTerm::needsHeightResize ( const FTermRegion* region
                                         , const std::size_t full_height ) const noexcept -> bool
{
  return getFullRegionHeight(region) != int(full_height);
}

//----------------------------------------------------------------------
constexpr auto FVTerm::needsWidthResize ( const FTermRegion* region
                                        , const std::size_t full_width ) const noexcept -> bool
{
  return getFullRegionWidth(region) != int(full_width);
}

//----------------------------------------------------------------------
inline auto FVTerm::tryResizeRegion ( FTermRegion* region
                                    , const std::size_t width
                                    , const std::size_t height ) const -> bool
{
  if ( needsHeightResize(region, height) )
    return resizeTextRegion(region, height, width * height);

  if ( needsWidthResize(region, width) )
    return resizeTextRegion(region, width * height);

  return false;
}

//----------------------------------------------------------------------
inline void FVTerm::updateRegionProperties ( FTermRegion* region
                                           , const FShadowBox& shadowbox ) const noexcept
{
  region->position.x      = shadowbox.box.getX();
  region->position.y      = shadowbox.box.getY();
  region->size.width      = int(shadowbox.box.getWidth());
  region->size.height     = int(shadowbox.box.getHeight());
  region->min_size.width  = region->size.width;
  region->min_size.height = DEFAULT_MINIMIZED_HEIGHT;
  region->shadow.width    = int(shadowbox.shadow.getWidth());
  region->shadow.height   = int(shadowbox.shadow.getHeight());
  region->has_changes     = false;
}

//----------------------------------------------------------------------
constexpr auto FVTerm::getFullRegionWidth (const FTermRegion* region) const noexcept -> int
{
  return region->size.width + region->shadow.width;
}

//----------------------------------------------------------------------
constexpr auto FVTerm::getFullRegionHeight (const FTermRegion* region) const noexcept -> int
{
  return region->size.height + region->shadow.height;
}

//----------------------------------------------------------------------
void FVTerm::passChangesToOverlap (const FTermRegion* region) const
{
  const auto* vterm_win_list = getWindowList();

  if ( ! region || ! vterm_win_list || vterm_win_list->empty() )
    return;

  processOverlappingWindows (region, *vterm_win_list);
}

//----------------------------------------------------------------------
inline void FVTerm::processOverlappingWindows ( const FTermRegion* region
                                              , const FVTermList& vterm_win_list ) const
{
  bool found{false};

  if ( ! region )
    return;

  for (const auto& vterm_obj : vterm_win_list)
  {
    auto* win = vterm_obj->getVWin();

    if ( found && win && win->visible && win->isOverlapped(region) )
    {
      // Pass changes to the found window
      passChangesToOverlappingWindow (win, region);
    }

    if ( region == win )
      found = true;
  }
}

//----------------------------------------------------------------------
inline void FVTerm::passChangesToOverlappingWindow (FTermRegion* win, const FTermRegion* region) const noexcept
{
  win->has_changes = true;

  // Boundaries
  const int win_y_min = win->position.y;
  const int win_height = win->minimized ? win->min_size.height : getFullRegionHeight(win);
  const int win_y_max = win_y_min + win_height - 1;
  const int region_y_min = region->position.y;
  const int region_height = region->minimized ? region->min_size.height : getFullRegionHeight(region);
  const int region_y_max = region_y_min + region_height - 1;
  const int vterm_y_max = vterm->size.height - 1;

  // Calculate start and end line
  const int y_start = calculateStartCoordinate (region_y_min, win_y_min);
  const int y_end = calculateEndCoordinate (vterm_y_max, region_y_max, win_y_min, win_y_max);

  for (auto y{y_start}; y <= y_end; y++)  // Line loop
  {
    passChangesToOverlappingWindowLine (win, y, region);
  }

  win->changes_in_row = {uInt(y_start), uInt(y_end)};
}

//----------------------------------------------------------------------
inline void FVTerm::passChangesToOverlappingWindowLine ( FTermRegion* win
                                                       , int y
                                                       , const FTermRegion* region ) const noexcept
{
  // Boundaries
  const int win_x_min = win->position.x;
  const int win_x_max = win_x_min + getFullRegionWidth(win) - 1;
  const int region_x_min = region->position.x;
  const int region_x_max = region_x_min + getFullRegionWidth(region) - 1;
  const int vterm_x_max = vterm->size.width - 1;

  // Calculate start and end position
  const int x_start = calculateStartCoordinate (region_x_min, win_x_min);
  const int x_end = calculateEndCoordinate (vterm_x_max, region_x_max, win_x_min, win_x_max);

  // Sets the new change boundaries
  auto& line_changes = win->changes_in_line[unsigned(y)];
  line_changes.xmin = uInt(std::min(int(line_changes.xmin), x_start));
  line_changes.xmax = uInt(std::max(int(line_changes.xmax), x_end));
}

//----------------------------------------------------------------------
inline auto FVTerm::calculateStartCoordinate (int region_min, int win_min) const noexcept -> int
{
  return std::max({0, region_min, win_min}) - win_min;
}

//----------------------------------------------------------------------
inline auto FVTerm::calculateEndCoordinate (int vterm_max, int region_max, int win_min, int win_max) const noexcept -> int
{
  return std::min({vterm_max, region_max, win_max}) - win_min;
}

//----------------------------------------------------------------------
void FVTerm::updateVTerm() const
{
  // Updates the character data from all regions to VTerm

  updateVTermDesktop();

  if ( ! vterm_window_list || vterm_window_list->empty() )
    return;

  for (const auto& vterm_obj : *vterm_window_list)  // List from bottom to top
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
inline void FVTerm::updateVTermWindow (FTermRegion* v_win) const
{
  if ( hasPendingUpdates(v_win) )
  {
    passChangesToOverlap(v_win);
    addLayer(v_win);  // Add v_win changes to vterm
    v_win->has_changes = false;
  }
  else if ( hasChildRegionChanges(v_win) )
  {
    passChangesToOverlap(v_win);
    addLayer(v_win);  // and call the child region processing handler there
    clearChildRegionChanges(v_win);
  }
}
//----------------------------------------------------------------------
inline auto FVTerm::computeLayerGeometry (const FTermRegion* region) const noexcept -> LayerGeometry
{
  // Compute the geometry values used throughout the rendering process

  const auto region_x = region->position.x;
  const auto region_y = region->position.y;
  const auto vterm_width = vterm->size.width;
  const auto vterm_height = vterm->size.height;
  const auto height = region->minimized ? region->min_size.height : getFullRegionHeight(region);

  return LayerGeometry
  {
    region_x,
    region_y,
    std::max(0, -region_x),
    vterm_width,
    vterm_height,
    std::max(region_x, 0),
    getFullRegionWidth(region),
    height,
    vterm_width - region_x - 1,
    // Calculate actual rendering range (clipped to vterm)
    std::max(0, -region_y),
    std::min(vterm_height - region_y, height)
  };
}

//----------------------------------------------------------------------
inline auto FVTerm::isLayerOutsideVTerm (const LayerGeometry& geo) const noexcept -> bool
{
  if ( geo.region_y >= geo.vterm_height )
    return false;

  if ( geo.ax >= geo.vterm_width )
    return false;

  if ( geo.region_y + geo.height <= 0 )
    return false;

  return true;
}

//----------------------------------------------------------------------
inline void FVTerm::buildLineChangeBatch ( const FTermRegion* region
                                         , const LayerGeometry& geo ) const noexcept
{
  int prev_xmin{-1};
  int prev_xmax{-1};
  NoTrans prev_has_no_trans{NoTrans::Undefined};

  auto line_changes = region->changes_in_line.begin() + geo.y_start;

  for (auto y{geo.y_start}; y < geo.y_end; y++)  // Line loop
  {
    const auto line_xmin_raw = int(line_changes->xmin);
    const auto line_xmax_raw = int(line_changes->xmax);

    if ( line_xmin_raw > line_xmax_raw )
    {
      ++line_changes;
      prev_xmin = -1;
      prev_xmax = -1;
      prev_has_no_trans = NoTrans::Undefined;
      continue;
    }

    const auto line_xmin = std::max(line_xmin_raw, geo.ol);
    const auto line_xmax = std::min(line_xmax_raw, geo.xmax_inside_vterm);

    if ( line_xmin > line_xmax )
    {
      ++line_changes;
      prev_xmin = -1;
      prev_xmax = -1;
      prev_has_no_trans = NoTrans::Undefined;
      continue;
    }

    const NoTrans has_no_trans = line_changes->trans_count == 0 ? NoTrans::Set : NoTrans::Unset;
    ++line_changes;

    if ( prev_xmin == line_xmin
      && prev_xmax == line_xmax
      && prev_has_no_trans == has_no_trans )
    {
      line_changes_batch.back().count++;
      continue;
    }

    line_changes_batch.push_back({1, y, line_xmin, line_xmax, has_no_trans});
    prev_xmin = line_xmin;
    prev_xmax = line_xmax;
    prev_has_no_trans = has_no_trans;
  }
}

//----------------------------------------------------------------------
inline void FVTerm::applyLineBatch ( FTermRegion* region
                                   , const LayerGeometry& geo ) const noexcept
{
  for (const auto& line : line_changes_batch)
  {
    const auto line_xmin = line.xmin;
    const auto line_xmax = line.xmax;
    const auto has_no_trans = line.has_no_transparency == NoTrans::Set;
    const auto tx = geo.region_x + line_xmin;  // Global terminal x-position
    const int length = line_xmax - line_xmin + 1;

    // Process all lines in batch with same operation
    for (int i = 0; i < line.count; ++i)
    {
      const auto y = line.ypos + i;
      auto line_changes = region->changes_in_line.begin() + y;
      const auto ty = geo.region_y + y;  // Global terminal y-position
      const auto region_line_offset = y * geo.width + line_xmin;
      const auto vterm_line_offset = ty * geo.vterm_width + tx;
      auto ac = region->data.begin() + region_line_offset;    // Region character
      auto tc = vterm->data.begin() + vterm_line_offset;  // Terminal character

      if ( has_no_trans )
      {
        // Line has only covered characters
        putRegionLine (*ac, *tc, length);
      }
      else
      {
        // Line with hidden and transparent characters
        addRegionLineWithTransparency (ac, tc, length);
      }

      auto vterm_changes = vterm->changes_in_line.begin() + ty;
      const auto tx_start = uInt(tx);
      const auto tx_end   = uInt(std::min( geo.ax + line_xmax
                                         , geo.vterm_width - 1 ));
      vterm_changes->xmin = std::min(vterm_changes->xmin, tx_start);
      vterm_changes->xmax = std::max(vterm_changes->xmax, tx_end);

      line_changes->xmin = uInt(geo.width);
      line_changes->xmax = 0;
      ++line_changes;
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::updateVTermChangesFromBatch (const LayerGeometry& geo) const noexcept
{
  const auto& first = line_changes_batch.front();
  const auto& last  = line_changes_batch.back();
  const auto begin = uInt(geo.region_y + first.ypos - geo.y_start);
  const auto end = uInt(geo.region_y + last.ypos + last.count - 1 - geo.y_start);

  auto& changes_in_row = vterm->changes_in_row;
  changes_in_row.ymin = std::min(changes_in_row.ymin, begin);
  changes_in_row.ymax = std::max(changes_in_row.ymax, end);
  vterm->has_changes = true;
}

//----------------------------------------------------------------------
inline void FVTerm::scrollTerminalForward() const
{
  // Scrolls the terminal up one line

  foutput->flush();  // Empty buffer before scrolling

  if ( ! foutput->scrollTerminalForward() )
    return;

  // Avoid update lines from 0 to (height - 2)
  auto vdesktop_changes = vdesktop->changes_in_line.begin();
  const auto vdesktop_changes_end = vdesktop_changes
                                  + vdesktop->size.height - 1;

  while ( vdesktop_changes < vdesktop_changes_end )
  {
    vdesktop_changes->xmin = uInt(vdesktop->size.width - 1);
    vdesktop_changes->xmax = 0;
    ++vdesktop_changes;
  }

  vdesktop->changes_in_row = {0, uInt(vdesktop->size.height - 2)};
  putRegion (FPoint{1, 1}, vdesktop.get());
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
  auto vdesktop_changes = vdesktop->changes_in_line.begin() + 1;
  const auto vdesktop_changes_end = vdesktop_changes
                                  + vdesktop->size.height;

  while ( vdesktop_changes < vdesktop_changes_end )
  {
    vdesktop_changes->xmin = uInt(vdesktop->size.width - 1);
    vdesktop_changes->xmax = 0;
    ++vdesktop_changes;
  }

  vdesktop->changes_in_row = {1, uInt(vdesktop->size.height - 1)};
  putRegion (FPoint{1, 1}, vdesktop.get());
  saveCurrentVTerm();  // Ensure that the current terminal is comparable
  forceTerminalUpdate();
}

//--------------------------------------------------------------------setTermAttributes--
void FVTerm::callPreprocessingHandler (const FTermRegion* region) const
{
  // Call preprocessing handler

  if ( ! region || region->preproc_list.empty() )
    return;

  for (const auto& pcall : region->preproc_list)
  {
    // call the preprocessing handler
    const auto& preprocessingHandler = pcall->function;
    preprocessingHandler();
  }
}

//----------------------------------------------------------------------
auto FVTerm::hasChildRegionChanges (const FTermRegion* region) const -> bool
{
  if ( ! region || region->preproc_list.empty() )
    return false;

  return std::any_of ( region->preproc_list.cbegin()
                     , region->preproc_list.cend()
                     , [] (const auto& pcall) noexcept
                       {
                         return pcall->instance
                             && pcall->instance->child_print_region
                             && pcall->instance->child_print_region->has_changes;
                       }
                     );
}

//----------------------------------------------------------------------
void FVTerm::clearChildRegionChanges (const FTermRegion* region) const
{
  if ( ! region || region->preproc_list.empty() )
    return;

  for (const auto& pcall : region->preproc_list)
  {
    if ( pcall->instance && pcall->instance->child_print_region )
      pcall->instance->child_print_region->has_changes = false;
  }
}

//----------------------------------------------------------------------
constexpr auto FVTerm::isInsideRegion (const FPoint& pos, const FTermRegion* region) noexcept -> bool
{
  // Check whether the coordinates are within the region

  const int x = pos.getX();
  const int y = pos.getY();
  return x >= 0 && x < region->size.width
      && y >= 0 && y < region->size.height;
}

//----------------------------------------------------------------------
inline auto FVTerm::isInsideTerminal (const FPoint& pos) const noexcept -> bool
{
  // Check whether the coordinates are within the virtual terminal

  const auto x = pos.getX();
  const auto y = pos.getY();
  return x >= 0 && x < int(foutput->getColumnNumber())
      && y >= 0 && y < int(foutput->getLineNumber());
}

//----------------------------------------------------------------------
constexpr auto FVTerm::isFCharTransparent (const FChar& fchar) noexcept -> bool
{
  return fchar.isBitSet(internal::var::transparent_mask);
}

//----------------------------------------------------------------------
inline auto FVTerm::isTransparentInvisible (const FChar& fchar) const noexcept -> bool
{
  static const auto& trans_inv_chars = getTransparentInvisibleLookupMap();
  const auto& fist_char = fchar.ch.unicode_data[0];
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

  // Create virtual desktop region
  createVDesktop (term_size);
  active_region = vdesktop.get();

  // Reserving a typical number of changes
  line_changes_batch.reserve(32);

  // Reservation of capacity for at least 16 regions
  covered_regions_buffer.reserve(16);

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
inline void FVTerm::saveCurrentVTerm() const noexcept
{
  // Save the content of the virtual terminal
  std::memcpy(vterm_old->data.data(), vterm->data.data(), vterm->data.size() * sizeof(FChar));
}

//----------------------------------------------------------------------
inline void FVTerm::putRegionLine ( const FChar& src_char
                                  , FChar& dst_char
                                  , const int length ) const noexcept
{
  // copy "length" characters from region to terminal

  std::memcpy (&dst_char, &src_char, std::size_t(length) * sizeof(dst_char));
}

//----------------------------------------------------------------------
inline void FVTerm::applyColorOverlay (const FChar& src_char, FChar& dst_char) const
{
  dst_char.ch = src_char.ch;  // Get covered character
  dst_char.unsetBit(internal::var::color_overlay_reset_mask);

  if ( isTransparentInvisible(dst_char) )
    dst_char.ch[0] = L' ';
}

//----------------------------------------------------------------------
inline void FVTerm::inheritBackground (const FChar& src_char, FChar& dst_char) const
{
  // Add the covered background to this character
  copyBgColor(src_char.color, dst_char.color);
}

//----------------------------------------------------------------------
inline void FVTerm::putMultiLayerRegionLine ( FChar_iterator dst_char
                                            , const int length
                                            , const FPoint& pos ) const noexcept
{
  if ( length == 0 )
    return;

  if ( overlay_search_buffer.size() < std::size_t(length) )  // Resize only if needed
    overlay_search_buffer.resize(std::size_t(length));

  auto search_buffer = overlay_search_buffer.begin();
  std::fill_n(search_buffer, length, SearchState::start);
  const auto term_x = pos.getX();
  const auto term_y = pos.getY();

  overlay_line_buffer.clear();  // Clear buffer

  for (const auto* win : covered_regions_buffer)
  {
    // Precalculate window boundaries
    const int current_height = win->minimized ? win->min_size.height
                                              : getFullRegionHeight(win);
    const int x_min = win->position.x;
    const int x_max = x_min + getFullRegionWidth(win);
    const int y_min = win->position.y;
    const int y_max = y_min + current_height;

    // Early rejection if the line is completely outside the window
    if ( term_y < y_min
      || term_y >= y_max
      || term_x >= x_max
      || term_x + length <= x_min )
      continue;

    // Precalculate array indexing values for getFChar
    const auto x = term_x - x_min;
    const auto y = term_y - y_min;
    const auto width = getFullRegionWidth(win);
    const auto index = y * width + x;

    // Calculate the intersection of the line with the window
    const auto start_idx = std::max(0, x_min - term_x);
    const auto end_idx = std::min(length, x_max - term_x);

    // Store pre-calculated region line data
    overlay_line_buffer.push_back
    (
      { win->data.begin(), index, start_idx, end_idx }
    );
  }

  for (const auto& line : overlay_line_buffer)
  {
    auto offset = line.start_idx + line.offset;
    auto win_char = line.iter + offset;
    auto dst = dst_char + line.start_idx;

    // Process only the intersecting part of the line
    for (std::ptrdiff_t idx{line.start_idx}; idx < line.end_idx; ++idx, ++win_char, ++dst)
    {
      auto& char_search = *(search_buffer + idx);

      if ( char_search == SearchState::ready )
        continue;

      const auto transparency = win_char->attr.data
                              & internal::var::transparent_mask;

      if ( transparency == FAttribute::set::transparent )  // Transparent
      {
        if ( char_search == SearchState::start )
          char_search = SearchState::printable;

        continue;
      }

      if ( transparency == FAttribute::set::color_overlay )  // Color overlay
      {
        if ( char_search == SearchState::background )
        {
          inheritBackground(*win_char, *dst);
          char_search = SearchState::ready;
        }
        else if ( char_search != SearchState::overlay )
        {
          std::memcpy (&dst[0], &win_char[0], sizeof(FChar));
          char_search = SearchState::overlay;
        }

        continue;
      }

      if ( transparency == FAttribute::set::inherit_background )  // Inherit background color
      {
        if ( char_search == SearchState::overlay )
        {
          applyColorOverlay(*win_char, *dst);
          char_search = SearchState::ready;
        }
        else if ( char_search != SearchState::background )
        {
          std::memcpy (&dst[0], &win_char[0], sizeof(FChar));
          char_search = SearchState::background;
        }

        continue;
      }

      // transparency = 0
      if ( char_search == SearchState::overlay )  // Color overlay
      {
        applyColorOverlay(*win_char, *dst);
      }
      else if ( char_search == SearchState::background )  // Inherit background color
      {
        inheritBackground(*win_char, *dst);
      }
      else  // Non-transparent
      {
        std::memcpy (&dst[0], &win_char[0], sizeof(FChar));
      }

      char_search = SearchState::ready;
    }
  }
}

//----------------------------------------------------------------------
inline void FVTerm::putRegionLineWithTransparency ( FChar_const_iterator src_char
                                                  , FChar_iterator dst_char
                                                  , const int length
                                                  , FPoint pos
                                                  , bool line_covered) const
{
  if ( length < 1 )
    return;

  bool is_region_transparent = isFCharTransparent(*src_char);
  int region_count{1};
  const auto end_char = src_char + length;
  auto region_start = src_char;
  auto current_char = src_char + 1;
  const auto unroll_end = end_char - 3;

  while ( current_char < unroll_end )  // Main unrolled loop
  {
    const std::array<bool, 4> trans_data
    {{
      isFCharTransparent(*current_char),
      isFCharTransparent(*(current_char + 1)),
      isFCharTransparent(*(current_char + 2)),
      isFCharTransparent(*(current_char + 3))
    }};

    if ( trans_data[0] == is_region_transparent
      && trans_data[1] == is_region_transparent
      && trans_data[2] == is_region_transparent
      && trans_data[3] == is_region_transparent )
    {
      region_count += 4;
      current_char += 4;
      continue;
    }

    for (uInt i = 0; i < 4; ++i)
    {
      const bool is_current_char_transparent = trans_data[i];

      if ( is_current_char_transparent == is_region_transparent )
      {
        ++region_count;
      }
      else
      {
        // Process completed region
        if ( is_region_transparent || line_covered )
          putMultiLayerRegionLine (dst_char, region_count, pos);
        else
          putRegionLine (*region_start, *dst_char, region_count);

        pos.x_ref() += region_count;
        dst_char += region_count;

        // Reset for the next region
        is_region_transparent = is_current_char_transparent;
        region_start = current_char;
        region_count = 1;
      }

      ++current_char;
    }
  }

  while ( current_char < end_char )  // Process remaining characters (tail)
  {
    const bool is_current_transparent = isFCharTransparent(*current_char);

    if ( is_current_transparent == is_region_transparent )
    {
      ++region_count;
    }
    else
    {
      // Process completed region
      if ( is_region_transparent || line_covered )
        putMultiLayerRegionLine (dst_char, region_count, pos);
      else
        putRegionLine (*region_start, *dst_char, region_count);

      pos.x_ref() += region_count;
      dst_char += region_count;

      // Start new region
      is_region_transparent = is_current_transparent;
      region_start = current_char;
      region_count = 1;
    }

    ++current_char;
  }

  // Handle the final region
  if ( is_region_transparent || line_covered )
    putMultiLayerRegionLine (dst_char, region_count, pos);
  else
    putRegionLine (*region_start, *dst_char, region_count);
}

//----------------------------------------------------------------------
inline void FVTerm::addRegionLineWithTransparency ( FChar_const_iterator src_char
                                                  , FChar_iterator dst_char
                                                  , const int length ) const
{
  if ( length == 0 )
    return;

  const auto handle_region = \
      [this, &dst_char] ( bool transparent
                        , int count
                        , FChar_const_iterator start )
  {
    if ( transparent )
      addTransparent (start, dst_char, count);
    else
      putNonTransparent (start, dst_char, count);
  };

  const auto end_char = src_char + length;
  auto region_start = src_char;
  int region_count{1};
  bool is_region_transparent{isFCharTransparent(*src_char)};

  for (auto current_char = src_char + 1; current_char < end_char; ++current_char)
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
inline void FVTerm::addTransparentRegionLine ( const FChar_const_iterator& src_char
                                             , const FChar_iterator& dst_char
                                             , const int length ) const
{
  auto src = src_char;
  auto dst = dst_char;
  auto end = src + length;

  while ( src < end )  // column loop
  {
    addTransparentRegionChar (*src, *dst);
    ++src;  // src character
    ++dst;  // dst character
  }
}

//----------------------------------------------------------------------
inline void FVTerm::addTransparentRegionChar (const FChar& src_char, FChar& dst_char) const
{
  if ( src_char.isBitSet(FAttribute::set::transparent) )  // Transparent
    return;  // Leave character on vterm untouched

  if ( src_char.isBitSet(FAttribute::set::color_overlay) )  // Color overlay
  {
    // Get covered character + add the current color
    dst_char.color.data = src_char.color.data;
    dst_char.attr.data = src_char.attr.data & internal::var::color_overlay_reset_mask;

    if ( isTransparentInvisible(dst_char) )
      dst_char.ch[0] = L' ';

    return;
  }

  if ( src_char.isBitSet(FAttribute::set::inherit_background) )
  {
    // Add the covered background to this character
    auto bg_color = dst_char.color.getBgColor();
    dst_char = src_char;
    dst_char.color.setBgColor(bg_color);
    dst_char.unsetBit(internal::var::print_reset_mask);
    return;
  }

  // Default
  dst_char = src_char;
}

//----------------------------------------------------------------------
auto FVTerm::clearFullRegion (FTermRegion* region, FChar& fillchar) const -> bool
{
  // Clear region
  std::fill (region->data.begin(), region->data.end(), fillchar);

  if ( region != vdesktop.get() )  // Is the region identical to the desktop?
    return false;

  // Try to clear the terminal rapidly with a control sequence
  if ( foutput->clearTerminal (fillchar.ch[0]) )
  {
    fillchar.setBit(FAttribute::set::printed);
    std::fill (vterm->data.begin(), vterm->data.end(), fillchar);
    saveCurrentVTerm();
  }
  else
  {
    auto vdesktop_changes = vdesktop->changes_in_line.begin();
    const auto vdesktop_changes_end = vdesktop_changes
                                    + vdesktop->size.height;

    while ( vdesktop_changes < vdesktop_changes_end )
    {
      vdesktop_changes->xmin = 0;
      vdesktop_changes->xmax = uInt(vdesktop->size.width) - 1;
      vdesktop_changes->trans_count = 0;
      ++vdesktop_changes;
    }

    vdesktop->changes_in_row = {0, uInt(vdesktop->size.height - 1)};
    vdesktop->has_changes = true;
  }

  return true;
}

//----------------------------------------------------------------------
void FVTerm::clearRegionWithShadow (FTermRegion* region, const FChar& fillchar) const noexcept
{
  FChar t_char = fillchar;
  t_char.ch[0] = L'\0';
  t_char.setBit(FAttribute::set::transparent);
  t_char.setCharWidth(0);
  const auto total_width = getFullRegionWidth(region);
  auto region_pos = region->data.begin();
  auto shadow_begin = region->data.begin() + region->size.width;

  for (auto y{0}; y < region->size.height; y++)
  {
    // Clear region
    std::fill (region_pos, region_pos + region->size.width, fillchar);
    // Make right shadow transparent
    std::fill (shadow_begin, shadow_begin + region->shadow.width, t_char);
    region_pos += total_width;
    shadow_begin += total_width;
  }

  // Make bottom shadow transparent
  region_pos = region->data.begin() + (total_width * region->size.height);

  for (auto y{0}; y < region->shadow.height; y++)
  {
    std::fill (region_pos, region_pos + total_width, t_char);
    region_pos += total_width;
  }
}

//----------------------------------------------------------------------
inline auto FVTerm::printWrap (FTermRegion* region, FChar_iterator& ac) const -> bool
{
  bool end_of_region{false};
  const int& width  = region->size.width;
  const int& height = region->size.height;
  const int& rsh    = region->shadow.width;
  const int& bsh    = region->shadow.height;

  // Line break at right margin
  if ( region->cursor.x > width + rsh )
  {
    region->cursor.x = 1;
    region->cursor.y++;
    ac = region->getFCharIterator(region->cursor.x - 1, region->cursor.y - 1);
  }

  // Prevent up scrolling
  if ( region->cursor.y > height + bsh )
  {
    region->cursor.y--;
    ac = region->getFCharIterator(region->cursor.x - 1, region->cursor.y - 1);
    end_of_region = true;
  }

  return end_of_region;
}

//----------------------------------------------------------------------
inline auto FVTerm::interpretControlCodes ( FTermRegion* region
                                          , FChar_iterator& ac
                                          , const FChar& term_char ) const noexcept -> bool
{
  switch ( term_char.ch.unicode_data[0] )
  {
    case L'\n':
      region->cursor.y++;
      // fall through
    case L'\r':
      region->cursor.x = 1;
      break;

    case L'\t':
      region->cursor.x += tabstop - ((region->cursor.x - 1) % tabstop);
      break;

    case L'\b':
      region->cursor.x--;
      break;

    case L'\a':
      foutput->beep();
      break;

    default:
      return false;
  }

  ac = region->getFCharIterator(region->cursor.x - 1, region->cursor.y - 1);
  return true;
}

//----------------------------------------------------------------------
inline auto FVTerm::printCharacter ( FTermRegion* region
                                   , FChar_iterator& ac
                                   , const FChar& term_char ) const noexcept -> int
{
  if ( ! region )
    return -1;  // No region

  if ( interpretControlCodes(region, ac, term_char) )
    return 0;  // No printable character

  if ( ! region->isPrintPositionInsideRegion() || printWrap(region, ac) )
    return -1;  // Cursor position out of range or end of region reached

  // Printing term_char on region at the current cursor position
  auto char_width = printCharacterOnCoordinate (region, ac, term_char);

  if ( char_width == 0
    && ! term_char.isBitSet(FAttribute::set::fullwidth_padding) )
    return 0;

  region->cursor.x++;
  ++ac;
  region->has_changes = true;

  // Line break at right margin
  if ( region->cursor.x > getFullRegionWidth(region) )
  {
    region->cursor.x = 1;
    region->cursor.y++;
  }
  else if ( char_width == 2 )
  {
    printPaddingCharacter (region, term_char);
    ++ac;
  }

  // Prevent up scrolling
  if ( region->cursor.y > getFullRegionHeight(region) )
    region->cursor.y--;

  return 1;
}

//----------------------------------------------------------------------
inline auto FVTerm::printCharacterOnCoordinate ( FTermRegion* region
                                               , const FChar_iterator& ac
                                               , const FChar& ch ) const noexcept -> std::size_t
{
  const auto ax = unsigned(region->cursor.x - 1);
  const auto ay = unsigned(region->cursor.y - 1);

  if ( *ac == ch )  // compare with an overloaded operator
    return ac->getCharWidth();

  const bool trans_old = ac->isBitSet(internal::var::print_transparent_mask);
  const bool trans_new = ch.isBitSet(internal::var::print_transparent_mask);
  const auto trans_changed = int(trans_new) - int(trans_old);

  if ( trans_changed != 0 )
    region->changes_in_line[ay].trans_count += uInt(trans_changed);

  // copy character to region
  *ac = ch;

  auto current_width = uInt8(ac->getCharWidth());

  if ( current_width == 0 )
  {
    const auto new_char_width = getColumnWidth(ac->ch[0]);

    if ( new_char_width == 0 )
      return 0;

    addColumnWidth(*ac, new_char_width);  // Add column width
    current_width = uInt8(new_char_width);
  }

  region->updateRegionChanges (ax, ay, current_width);
  return current_width;
}

//----------------------------------------------------------------------
inline void FVTerm::printPaddingCharacter ( FTermRegion* region
                                          , const FChar& term_char ) const
{
  // Creates a padding-character from the current character (term_char)
  // and prints it. It is a placeholder for the column after
  // a full-width character.

  FChar pc{term_char};  // Copy character to padding character

  if ( region->encoding == Encoding::UTF8 )
  {
    pc.ch[0] = L'\0';
    pc.setBit(FAttribute::set::fullwidth_padding);
    pc.setCharWidth(0);
  }
  else
  {
    pc.ch[0] = L'.';
    pc.ch[1] = L'\0';
    pc.setCharWidth(1);
  }

  // Print the padding-character
  print (region, pc);
}

//----------------------------------------------------------------------
inline void FVTerm::putNonTransparent ( FChar_const_iterator start_char
                                      , FChar_iterator& dst_char
                                      , int& non_trans_count ) const
{
  if ( non_trans_count == 0 )
    return;

  putRegionLine(*start_char, *dst_char, non_trans_count);
  dst_char += non_trans_count;  // dst character
  non_trans_count = 0;
}

//----------------------------------------------------------------------
inline void FVTerm::addTransparent ( FChar_const_iterator start_char
                                   , FChar_iterator& dst_char
                                   , int& trans_count ) const
{
  if ( trans_count == 0 )
    return;

  addTransparentRegionLine (start_char, dst_char, trans_count);
  dst_char += trans_count;  // dst character
  trans_count = 0;
}

//----------------------------------------------------------------------
inline void FVTerm::addVDesktopToListIfExists (FTermRegionList& list) const
{
  // Adds the virtual desktop region, if it exists

  if ( vdesktop )
    list.push_back(vdesktop.get());
}

//----------------------------------------------------------------------
inline void FVTerm::determineCoveredRegions (FTermRegion* src) const
{
  const auto* vterm_win_list = getWindowList();
  covered_regions_buffer.clear();
  resetLineCoveredState(src);

  if ( ! vterm_win_list || vterm_win_list->empty() )
  {
    addVDesktopToListIfExists(covered_regions_buffer);
    return;
  }

  const auto end = vterm_win_list->crend();
  const auto src_layer = src->layer;

  // Regions from top to bottom
  for (auto iter = vterm_win_list->crbegin(); iter < end; ++iter)
  {
    const auto& vterm_obj = *iter;

    if ( ! vterm_obj )
      continue;

    const auto* win = vterm_obj->getVWin();

    if ( ! win || ! win->visible || win->layer < 1
      || ! win->isOverlapped(src) )
      continue;

    covered_regions_buffer.push_back(win);

    if ( win->layer <= src_layer )
      continue;

    determineLineCoveredState(win, src);
  }

  addVDesktopToListIfExists(covered_regions_buffer);
}

//----------------------------------------------------------------------
inline void FVTerm::resetLineCoveredState (FTermRegion* src) const
{
  auto changes = src->changes_in_line.begin();
  const auto changes_end = src->changes_in_line.end();

  // Unrolled loop for better performance
  while ( changes + 4 < changes_end )
  {
    changes[0].covered = false;
    changes[1].covered = false;
    changes[2].covered = false;
    changes[3].covered = false;
    changes += 4;
  }

  while ( changes < changes_end )  // Handle remaining lines
  {
    changes->covered = false;
    ++changes;
  }
}

//----------------------------------------------------------------------
inline void FVTerm::determineLineCoveredState ( const FTermRegion* const win
                                              , FTermRegion* src ) const noexcept
{
  // Requirements: The two regions have been checked for overlap

  const int win_height = win->minimized ? win->min_size.height
                                        : getFullRegionHeight(win);
  const int src_height = src->minimized ? src->min_size.height
                                        : getFullRegionHeight(src);
  const int win_y_min = win->position.y;
  const int win_y_max = win_y_min + win_height;
  const int src_y_min = src->position.y;
  const int src_y_max = src_y_min + src_height;
  const int overlap_start = std::max(win_y_min, src_y_min);
  const int overlap_end = std::min(win_y_max, src_y_max);
  const auto start = overlap_start - src_y_min;
  const auto end = overlap_end - src_y_min;
  auto changes_base = src->changes_in_line.begin();
  auto changes = changes_base + start;
  const auto changes_end = changes_base + end;

  // Unrolled loop for better performance
  while ( changes + 4 <= changes_end )
  {
    changes[0].covered = true;
    changes[1].covered = true;
    changes[2].covered = true;
    changes[3].covered = true;
    changes += 4;
  }

  while ( changes < changes_end )  // Handle remaining lines
  {
    changes->covered = true;
    ++changes;
  }
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
auto FVTerm::hasPendingUpdates (const FTermRegion* region) noexcept -> bool
{
  return (region && region->has_changes);
}

}  // namespace finalcut
