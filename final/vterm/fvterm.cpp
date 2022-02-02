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
#include <queue>
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
  if ( ! init_object )
    init<FTermOutput>();
  else
  {
    foutput = std::shared_ptr<FOutput>(init_object->foutput);
    window_list = std::shared_ptr<FVTermList>(init_object->window_list);
  }
}

//----------------------------------------------------------------------
FVTerm::~FVTerm()  // destructor
{
  if ( init_object == this )
    finish();
}

//----------------------------------------------------------------------
FVTerm& FVTerm::operator << (const FVTermBuffer& vterm_buffer)
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
FPoint FVTerm::getPrintCursor()
{
  const auto& win = getPrintArea();

  if ( win )
    return { win->offset_left + win->cursor_x
           , win->offset_top + win->cursor_y };

  return {0, 0};
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
void FVTerm::setCursor (const FPoint& pos)
{
  if ( auto win = getPrintArea() )
  {
    win->cursor_x = pos.getX() - win->offset_left;
    win->cursor_y = pos.getY() - win->offset_top;
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
void FVTerm::createVTerm (const FSize& size)
{
  // initialize virtual terminal

  const FRect box{0, 0, size.getWidth(), size.getHeight()};
  const FSize shadow{0, 0};
  createArea (box, shadow, vterm);
}

//----------------------------------------------------------------------
void FVTerm::resizeVTerm (const FSize& size) const
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
bool FVTerm::updateTerminal() const
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
inline bool FVTerm::interpretControlCodes ( FTermArea* area
                                          , const FChar& term_char ) const
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
int FVTerm::print (const FString& string)
{
  if ( string.isEmpty() )
    return 0;

  FVTermBuffer vterm_buffer{};
  vterm_buffer.print(string);
  return print (vterm_buffer);
}

//----------------------------------------------------------------------
int FVTerm::print (FTermArea* area, const FString& string)
{
  if ( ! area || string.isEmpty() )
    return -1;

  FVTermBuffer vterm_buffer{};
  vterm_buffer.print(string);
  return print (area, vterm_buffer);
}

//----------------------------------------------------------------------
int FVTerm::print (const std::vector<FChar>& term_string)
{
  if ( term_string.empty() )
    return -1;

  FVTermBuffer vterm_buffer{term_string.cbegin(), term_string.cend()};
  return print (vterm_buffer);
}

//----------------------------------------------------------------------
int FVTerm::print (FTermArea* area, const std::vector<FChar>& term_string)
{
  if ( ! area || term_string.empty() )
    return -1;

  FVTermBuffer vterm_buffer{term_string.cbegin(), term_string.cend()};
  return print (area, vterm_buffer);
}

//----------------------------------------------------------------------
int FVTerm::print (const FVTermBuffer& vterm_buffer)
{
  if ( vterm_buffer.isEmpty() )
    return -1;

  auto area = getPrintArea();
  return area ? print (area, vterm_buffer) : -1;
}

//----------------------------------------------------------------------
int FVTerm::print (FTermArea* area, const FVTermBuffer& vterm_buffer)
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
int FVTerm::print (wchar_t c)
{
  auto area = getPrintArea();
  return area ? print (area, c) : -1;
}

//----------------------------------------------------------------------
int FVTerm::print (FTermArea* area, wchar_t c)
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
int FVTerm::print (FChar& term_char)
{
  auto area = getPrintArea();
  return area ? print (area, term_char) : -1;
}

//----------------------------------------------------------------------
int FVTerm::print (FTermArea* area, const FChar& term_char)
{
  auto fchar = term_char;
  return print (area, fchar);
}

//----------------------------------------------------------------------
int FVTerm::print (FTermArea* area, FChar& term_char)
{
  if ( ! area )
    return -1;  // No area

  if ( interpretControlCodes(area, term_char) && printWrap(area) )
    return -1;  // End of area reached

  const int ax = area->cursor_x - 1;
  const int ay = area->cursor_y - 1;

  if ( term_char.attr.bit.char_width == 0 )
    addColumnWidth(term_char);  // Add column width

  auto char_width = term_char.attr.bit.char_width;

  if ( char_width == 0 && ! term_char.attr.bit.fullwidth_padding )
    return 0;

  // Print term_char on area at position (ax, ay)
  printCharacterOnCoordinate (area, ax, ay, term_char);
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
  {
    area->cursor_y--;
    return -1;
  }

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
FVTerm::FTermArea* FVTerm::getPrintArea()
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
void FVTerm::resizeArea ( const FRect& box
                        , const FSize& shadow
                        , FTermArea* area ) const
{
  // Resize the virtual window to a new size.

  const int offset_left = box.getX();
  const int offset_top  = box.getY();
  const auto width = int(box.getWidth());
  const auto height = int(box.getHeight());
  const auto rsw = int(shadow.getWidth());
  const auto bsh = int(shadow.getHeight());

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
  resetTextAreaToDefault (area, size);
}

//----------------------------------------------------------------------
void FVTerm::removeArea (FTermArea*& area)
{
  // remove the virtual window

  if ( area == nullptr )
    return;

  delete[] area->changes;
  area->changes = nullptr;
  delete[] area->data;
  area->data = nullptr;
  delete area;
  area = nullptr;
}

//----------------------------------------------------------------------
void FVTerm::restoreVTerm (const FRect& box)
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
      auto& tc = vterm->data[ypos * vterm->width + xpos];  // terminal character
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
bool FVTerm::updateVTermCursor (const FTermArea* area) const
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
bool FVTerm::isCursorHideable() const
{
  return foutput->isCursorHideable();
}

//----------------------------------------------------------------------
void FVTerm::setAreaCursor ( const FPoint& pos
                           , bool visible
                           , FTermArea* area )
{
  if ( ! area )
    return;

  area->input_cursor_x = pos.getX() - 1;
  area->input_cursor_y = pos.getY() - 1;
  area->input_cursor_visible = visible;
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FPoint& pos, const FTermArea* area)
{
  // Copies a block from the virtual terminal position to the given area

  if ( ! area )
    return;

  const int ax = pos.getX() - 1;
  const int ay = pos.getY() - 1;
  int y_end{};
  int length{};

  if ( area->height + ay > vterm->height )
    y_end = area->height - ay;
  else
    y_end = area->height;

  if ( area->width + ax > vterm->width )
    length = vterm->width - ax;
  else
    length = area->width;

  for (auto y{0}; y < y_end; y++)  // line loop
  {
    const auto& tc = vterm->data[(ay + y) * vterm->width + ax];  // terminal character
    auto& ac = area->data[y * area->width];  // area character
    std::memcpy (&ac, &tc, sizeof(ac) * unsigned(length));
    auto& area_changes = area->changes[y];

    if ( int(area_changes.xmin) > 0 )
      area_changes.xmin = 0;

    if ( int(area_changes.xmax) < length - 1 )
      area_changes.xmax = uInt(length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::getArea (const FRect& box, const FTermArea* area)
{
  // Copies a block from the virtual terminal rectangle to the given area

  if ( ! area )
    return;

  const int x = box.getX();
  const int y = box.getY();
  const auto w = int(box.getWidth());
  const auto h = int(box.getHeight());
  const int dx = x - area->offset_left + 1;
  const int dy = y - area->offset_top + 1;
  int y_end{};
  int length{};

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

  for (auto line = 0; line < y_end; line++)  // line loop
  {
    const int line_len = getFullAreaWidth(area);
    const auto& tc = vterm->data[(y + line - 1) * vterm->width + x - 1];  // terminal character
    auto& ac = area->data[(dy + line) * line_len + dx];  // area character
    std::memcpy (&ac, &tc, sizeof(ac) * unsigned(length));
    auto area_changes = area->changes[dy + line];

    if ( int(area_changes.xmin) > dx )
      area_changes.xmin = uInt(dx);

    if ( int(area_changes.xmax) < dx + length - 1 )
      area_changes.xmax = uInt(dx + length - 1);
  }
}

//----------------------------------------------------------------------
void FVTerm::putArea (const FTermArea* area) const
{
  // Transmit changes in the area to the virtual terminal

  if ( ! area || ! area->visible )
    return;

  int ax  = area->offset_left;
  const int ay  = area->offset_top;
  const int width = getFullAreaWidth(area);
  const int height = area->minimized ? area->min_height : getFullAreaHeight(area);
  int ol{0};  // Outside left
  int y_end{};

  // Call the preprocessing handler methods
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

  for (auto y{0}; y < y_end; y++)  // Line loop
  {
    bool modified{false};
    auto& area_changes = area->changes[y];
    auto line_xmin = int(area_changes.xmin);
    auto line_xmax = int(area_changes.xmax);

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

    area_changes.xmin = uInt(width);
    area_changes.xmax = 0;
  }

  vterm->has_changes = true;
  updateVTermCursor(area);
}

//----------------------------------------------------------------------
void FVTerm::putArea (const FPoint& pos, const FTermArea* area)
{
  // Copies the given area block to the virtual terminal position

  if ( ! area || ! area->visible )
    return;

  int ax = pos.getX() - 1;
  const int ay = pos.getY() - 1;
  const int width = getFullAreaWidth(area);
  const int height = area->minimized ? area->min_height : getFullAreaHeight(area);
  int ol{0};  // outside left
  int y_end{};
  int length{};

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

  for (auto y{0}; y < y_end; y++)  // line loop
  {
    if ( area->changes[y].trans_count == 0 )
    {
      // Line has only covered characters
      const auto& ac = area->data[y * width + ol];           // area character
      auto& tc = vterm->data[(ay + y) * vterm->width + ax];  // terminal character
      putAreaLine (ac, tc, std::size_t(length));
    }
    else
    {
      // Line has one or more transparent characters
      for (auto x{0}; x < length; x++)  // column loop
      {
        const int cx = ax + x;
        const int cy = ay + y;
        const auto& ac = area->data[y * width + ol + x];  // area character
        auto& tc = vterm->data[cy * vterm->width + cx];   // terminal character
        putAreaCharacter (FPoint{cx, cy}, area, ac, tc);
      }
    }

    auto& vterm_changes = vterm->changes[ay + y];

    if ( ax < int(vterm_changes.xmin) )
      vterm_changes.xmin = uInt(ax);

    if ( ax + length - 1 > int(vterm_changes.xmax) )
      vterm_changes.xmax = uInt(ax + length - 1);
  }

  vterm->has_changes = true;
}

//----------------------------------------------------------------------
int FVTerm::getLayer (FVTerm& obj)
{
  // returns the layer from the FVTerm object

  return obj.FVTerm::getPrintArea()->layer;
}

//----------------------------------------------------------------------
void FVTerm::determineWindowLayers()
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

  const int length = area->width;
  const int total_width = getFullAreaWidth(area);
  const int y_max = area->height - 1;

  for (auto y{0}; y < y_max; y++)
  {
    const int pos1 = y * total_width;
    const int pos2 = (y + 1) * total_width;
    const auto& sc = area->data[pos2];  // source character
    auto& dc = area->data[pos1];  // destination character
    std::memcpy (&dc, &sc, sizeof(dc) * unsigned(length));
    auto& area_changes = area->changes[y];
    area_changes.xmin = 0;
    area_changes.xmax = uInt(area->width - 1);
  }

  // insert a new line below
  FChar nc{};  // next character
  auto bottom_right = std::size_t((y_max * total_width) - area->right_shadow - 1);
  const auto& lc = area->data[bottom_right];  // last character
  std::memcpy (&nc, &lc, sizeof(nc));
  nc.ch[0] = L' ';
  auto& dc = area->data[y_max * total_width];  // destination character
  std::fill_n (&dc, area->width, nc);
  auto& area_changes_nl = area->changes[y_max];
  area_changes_nl.xmin = 0;
  area_changes_nl.xmax = uInt(area->width - 1);
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

  const int length = area->width;
  const int total_width = getFullAreaWidth(area);
  const int y_max = area->height - 1;

  for (auto y = y_max; y > 0; y--)
  {
    const int pos1 = (y - 1) * total_width;
    const int pos2 = y * total_width;
    const auto& sc = area->data[pos1];  // source character
    auto& dc = area->data[pos2];  // destination character
    std::memcpy (&dc, &sc, sizeof(dc) * unsigned(length));
    auto& area_changes = area->changes[y];
    area_changes.xmin = 0;
    area_changes.xmax = uInt(area->width - 1);
  }

  // insert a new line above
  FChar nc{};  // next character
  const auto& lc = area->data[total_width];  // last character
  std::memcpy (&nc, &lc, sizeof(nc));
  nc.ch[0] = L' ';
  auto& dc = area->data[0];  // destination character
  std::fill_n (&dc, area->width, nc);
  auto& area_changes_nl = area->changes[y_max];
  area_changes_nl.xmin = 0;
  area_changes_nl.xmax = uInt(area->width - 1);
  area->has_changes = true;

  if ( area == vdesktop )
    scrollTerminalReverse();  // Scrolls the terminal down one line
}

//----------------------------------------------------------------------
void FVTerm::clearArea (FTermArea* area, wchar_t fillchar) const
{
  // Clear the area with the current attributes

  FChar nc{getAttribute()};  // next character
  nc.ch[0] = fillchar;  // Current attributes with the fill character

  if ( ! (area && area->data) )
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
    auto& area_changes = area->changes[i];
    area_changes.xmin = 0;
    area_changes.xmax = width - 1;

    if ( nc.attr.bit.transparent
      || nc.attr.bit.color_overlay
      || nc.attr.bit.inherit_background )
      area_changes.trans_count = width;
    else if ( area->right_shadow != 0 )
      area_changes.trans_count = uInt(area->right_shadow);
    else
      area_changes.trans_count = 0;
  }

  for (auto i{0}; i < area->bottom_shadow; i++)
  {
    const int y = area->height + i;
    auto& area_changes = area->changes[y];
    area_changes.xmin = 0;
    area_changes.xmax = width - 1;
    area_changes.trans_count = width;
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
bool FVTerm::processTerminalUpdate() const
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
void FVTerm::startDrawing()
{
  // Pauses the terminal updates for the printing phase
  draw_completed = false;
}

//----------------------------------------------------------------------
void FVTerm::finishDrawing()
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
inline void FVTerm::resetTextAreaToDefault ( const FTermArea* area
                                           , const FSize& size ) const
{
  FChar default_char;
  FLineChanges unchanged{};

  default_char.ch[0]        = L' ';
  default_char.fg_color     = FColor::Default;
  default_char.bg_color     = FColor::Default;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;
  default_char.attr.byte[2] = 0;
  default_char.attr.byte[3] = 0;

  std::fill_n (area->data, size.getArea(), default_char);

  unchanged.xmin = uInt(size.getWidth());
  unchanged.xmax = 0;
  unchanged.trans_count = 0;

  std::fill_n (area->changes, size.getHeight(), unchanged);
}

//----------------------------------------------------------------------
inline bool FVTerm::reallocateTextArea ( FTermArea* area
                                       , std::size_t height
                                       , std::size_t size )
{
  // Reallocate "height" lines for changes
  // and "size" bytes for the text area

  if ( area->changes )
    delete[] area->changes;

  if ( area->data )
    delete[] area->data;

  try
  {
    area->changes = new FLineChanges[height];
    area->data    = new FChar[size];
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FLineChanges[height] or FChar[size]");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
inline bool FVTerm::reallocateTextArea (FTermArea* area, std::size_t size)
{
  // Reallocate "size" bytes for the text area

  if ( area->data )
    delete[] area->data;

  try
  {
    area->data = new FChar[size];
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FChar[size]");
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
FVTerm::CoveredState FVTerm::isCovered ( const FPoint& pos
                                       , const FTermArea* area )
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
      const int width  = getFullAreaWidth(win);
      const int& win_x = win->offset_left;
      const int& win_y = win->offset_top;
      const auto& tmp = &win->data[(y - win_y) * width + (x - win_x)];

      if ( tmp->attr.bit.color_overlay )
      {
        is_covered = CoveredState::Half;
      }
      else if ( ! tmp->attr.bit.transparent )
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
constexpr int FVTerm::getFullAreaWidth (const FTermArea* area)
{
  return area->width + area->right_shadow;
}

//----------------------------------------------------------------------
constexpr int FVTerm::getFullAreaHeight (const FTermArea* area)
{
  return area->height + area->bottom_shadow;
}

//----------------------------------------------------------------------
inline void FVTerm::updateOverlappedColor ( const FChar& area_char
                                          , const FChar& over_char
                                          , FChar& vterm_char )
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
                                              , FChar& vterm_char )
{
  // Restore one character on vterm

  cover_char.attr.bit.no_changes = \
      bool(vterm_char.attr.bit.printed && vterm_char == cover_char);
  std::memcpy (&vterm_char, &cover_char, sizeof(vterm_char));
}

//----------------------------------------------------------------------
inline void FVTerm::updateShadedCharacter ( const FChar& area_char
                                          , FChar& cover_char
                                          , FChar& vterm_char )
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
                                            , FChar& vterm_char )
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
inline void FVTerm::updateCharacter (const FChar& area_char, FChar& vterm_char)
{
  // Copy a area character to the virtual terminal

  std::memcpy (&vterm_char, &area_char, sizeof(vterm_char));

  if ( vterm_char.attr.bit.printed && vterm_char == area_char )
    vterm_char.attr.bit.no_changes = true;
  else
    vterm_char.attr.bit.no_changes = false;
}

//----------------------------------------------------------------------
bool FVTerm::updateVTermCharacter ( const FTermArea* area
                                  , const FPoint& area_pos
                                  , const FPoint& terminal_pos )
{
  // Area character
  const int width = getFullAreaWidth(area);
  const int area_index = area_pos.getY() * width + area_pos.getX();
  const auto& ac = area->data[area_index];
  // Terminal character
  const int terminal_index = terminal_pos.getY() * vterm->width
                           + terminal_pos.getX();
  auto& tc = vterm->data[terminal_index];

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
bool FVTerm::hasChildAreaChanges (const FTermArea* area) const
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
bool FVTerm::isInsideArea (const FPoint& pos, const FTermArea* area)
{
  // Check whether the coordinates are within the area

  const int x = pos.getX();
  const int y = pos.getY();
  return x >= 0 && x < area->width
      && y >= 0 && y < area->height;
}

//----------------------------------------------------------------------
bool FVTerm::isTransparentInvisible (const FChar& fchar)
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
FChar FVTerm::generateCharacter (const FPoint& pos)
{
  // Generates characters for a given position considering all areas

  const int x = pos.getX();
  const int y = pos.getY();
  auto sc = &vdesktop->data[y * vdesktop->width + x];  // shown character
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
      const int line_len = win->width + win->right_shadow;
      const int win_x = win->offset_left;
      const int win_y = win->offset_top;
      auto tmp = &win->data[(y - win_y) * line_len + (x - win_x)];

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
FChar FVTerm::getCharacter ( CharacterType char_type
                           , const FPoint& pos
                           , const FTermArea* area )
{
  // Gets the overlapped or the covered character for a given position

  int xx = std::max(pos.getX(), 0);
  int yy = std::max(pos.getY(), 0);

  if ( xx >= vterm->width )
    xx = vterm->width - 1;

  if ( yy >= vterm->height )
    yy = vterm->height - 1;

  auto cc = &vdesktop->data[yy * vdesktop->width + xx];  // covered character
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
inline FChar FVTerm::getCoveredCharacter (const FPoint& pos, const FTermArea* area)
{
  // Gets the covered character for a given position
  return getCharacter (CharacterType::Covered, pos, area);
}

//----------------------------------------------------------------------
inline FChar FVTerm::getOverlappedCharacter (const FPoint& pos, const FTermArea* area)
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
inline void FVTerm::putAreaLine (const FChar& area_char, FChar& vterm_char, std::size_t length)
{
  // copy "length" characters from area to terminal

  std::memcpy (&vterm_char, &area_char, sizeof(vterm_char) * length);
}

//----------------------------------------------------------------------
void FVTerm::putAreaCharacter ( const FPoint& pos, const FTermArea* area
                              , const FChar& area_char, FChar& vterm_char )
{
  if ( area_char.attr.bit.transparent )  // Transparent
  {
    // Restore one character on vterm
    const FChar& ch = getCoveredCharacter (pos, area);
    std::memcpy (&vterm_char, &ch, sizeof(vterm_char));
  }
  else  // Mot transparent
  {
    if ( area_char.attr.bit.color_overlay )  // Transparent shadow
    {
      // Get covered character + add the current color
      FChar ch = getCoveredCharacter (pos, area);
      ch.fg_color = area_char.fg_color;
      ch.bg_color = area_char.bg_color;
      ch.attr.bit.reverse  = false;
      ch.attr.bit.standout = false;

      if ( isTransparentInvisible(ch) )
        ch.ch[0] = L' ';

      std::memcpy (&vterm_char, &ch, sizeof(vterm_char));
    }
    else if ( area_char.attr.bit.inherit_background )
    {
      // Add the covered background to this character
      FChar ch{};
      std::memcpy (&ch, &area_char, sizeof(ch));
      FChar cc = getCoveredCharacter (pos, area);
      ch.bg_color = cc.bg_color;
      std::memcpy (&vterm_char, &ch, sizeof(vterm_char));
    }
    else  // Default
      std::memcpy (&vterm_char, &area_char, sizeof(vterm_char));
  }
}

//----------------------------------------------------------------------
void FVTerm::getAreaCharacter ( const FPoint& pos, const FTermArea* area
                              , FChar*& cc )
{
  const int area_x = area->offset_left;
  const int area_y = area->offset_top;
  const int line_len = getFullAreaWidth(area);
  const int x = pos.getX();
  const int y = pos.getY();
  auto& tmp = area->data[(y - area_y) * line_len + (x - area_x)];

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
bool FVTerm::clearFullArea (const FTermArea* area, FChar& nc) const
{
  // Clear area
  const int area_size = area->width * area->height;
  std::fill_n (area->data, area_size, nc);

  if ( area != vdesktop )  // Is the area identical to the desktop?
    return false;

  // Try to clear the terminal rapidly with a control sequence
  if ( foutput->clearTerminal (nc.ch[0]) )
  {
    nc.attr.bit.printed = true;
    std::fill_n (vterm->data, area_size, nc);
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
void FVTerm::clearAreaWithShadow (const FTermArea* area, const FChar& nc)
{
  FChar t_char = nc;
  t_char.ch[0] = L'\0';
  t_char.attr.bit.transparent = true;
  const int total_width = getFullAreaWidth(area);

  for (auto y{0}; y < area->height; y++)
  {
    const int pos = y * total_width;
    // Clear area
    std::fill_n (&area->data[pos], total_width, nc);
    // Make right shadow transparent
    std::fill_n (&area->data[pos + area->width], area->right_shadow, t_char);
  }

  // Make bottom shadow transparent
  for (auto y{0}; y < area->bottom_shadow; y++)
  {
    const int pos = total_width * (y + area->height);
    std::fill_n (&area->data[pos], total_width, t_char);
  }
}

//----------------------------------------------------------------------
bool FVTerm::printWrap (FTermArea* area) const
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
uInt8 FVTerm::getByte1TransMask()
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
inline bool FVTerm::changedToTransparency (const FChar& from, const FChar& to) const
{
  return ( uInt8(~ from.attr.byte[1] & b1_trans_mask)
         & uInt8(    to.attr.byte[1] & b1_trans_mask) ) != 0;
}

//----------------------------------------------------------------------
inline bool FVTerm::changedFromTransparency (const FChar& from, const FChar& to) const
{
  return changedToTransparency(to, from);
}

//----------------------------------------------------------------------
inline void FVTerm::printCharacterOnCoordinate ( FTermArea* area
                                               , const int& ax
                                               , const int& ay
                                               , const FChar& ch) const
{
  if ( area->cursor_x <= 0
    || area->cursor_y <= 0
    || ax >= getFullAreaWidth(area)
    || ay >= getFullAreaHeight(area) )
    return;

  const int line_len = getFullAreaWidth(area);
  auto& ac = area->data[ay * line_len + ax];  // area character

  if ( ac == ch )  // compare with an overloaded operator
    return;

  auto& area_changes = area->changes[ay];

  if ( changedToTransparency(ac, ch) )
  {
    // add one transparent character form line
    area_changes.trans_count++;
  }

  if ( changedFromTransparency(ac, ch) )
  {
    // remove one transparent character from line
    area_changes.trans_count--;
  }

  // copy character to area
  std::memcpy (&ac, &ch, sizeof(ac));

  if ( ax < int(area_changes.xmin) )
    area_changes.xmin = uInt(ax);

  if ( ax > int(area_changes.xmax) )
    area_changes.xmax = uInt(ax);
}

//----------------------------------------------------------------------
void FVTerm::printPaddingCharacter (FTermArea* area, const FChar& term_char)
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
bool FVTerm::isInsideTerminal (const FPoint& pos) const noexcept
{
  // Check whether the coordinates are within the virtual terminal

  const auto x = pos.getX();
  const auto y = pos.getY();
  return x >= 0 && x < int(foutput->getColumnNumber())
      && y >= 0 && y < int(foutput->getLineNumber());
}

//----------------------------------------------------------------------
bool FVTerm::hasPendingUpdates (const FTermArea* area) noexcept
{
  return (area && area->has_changes);
}


//----------------------------------------------------------------------
// struct FVTerm::FTermArea
//----------------------------------------------------------------------

bool FVTerm::FTermArea::contains (const FPoint& pos) const noexcept
{
  // Is the terminal position (pos) located on my area?

  const int area_height = minimized ? min_height : height + bottom_shadow;
  const int x = pos.getX();
  const int y = pos.getY();
  return x >= offset_left
      && x <= offset_left + width + right_shadow - 1
      && y >= offset_top
      && y <= offset_top + area_height - 1;
}

}  // namespace finalcut
