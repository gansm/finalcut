/***********************************************************************
* ftextview.h - Widget FTextView (a multiline text viewer)             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTerm  ▏ ▕ FObject ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FTextView ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTEXTVIEW_H
#define FTEXTVIEW_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <limits>
#include <limits>
#include <memory>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <utility>
#include <vector>

#include "final/fwidgetcolors.h"
#include "final/fwidget.h"
#include "final/util/fstring.h"
#include "final/util/fstringstream.h"
#include "final/vterm/fcolorpair.h"
#include "final/vterm/fstyle.h"

namespace finalcut
{

// class forward declaration
class FScrollbar;

// Global using-declaration
using FScrollbarPtr = std::shared_ptr<FScrollbar>;

//----------------------------------------------------------------------
// class FTextView
//----------------------------------------------------------------------

class FTextView : public FWidget
{
  public:
    struct FTextHighlight
    {
      // Constants
      static constexpr std::size_t EOL = std::numeric_limits<std::size_t>::max();

      FTextHighlight (std::size_t i, std::size_t l, const FChar& fchar) noexcept
        : index{i}
        , length{l}
        , attributes{fchar}
      { }

      FTextHighlight (std::size_t i, const FChar& fchar) noexcept
        : FTextHighlight{i, EOL, fchar}
      { }

      FTextHighlight (std::size_t i, std::size_t l, const FStyle& s) noexcept
        : index{i}
        , length{l}
      {
        auto wc_dialog = getColorTheme()->dialog;
        attributes.fg_color = wc_dialog.fg;
        attributes.bg_color = wc_dialog.bg;
        attributes.attr = s.toFAttribute();
      }

      FTextHighlight (std::size_t i, const FStyle& s) noexcept
        : FTextHighlight{i, EOL, s}
      { }

      FTextHighlight (std::size_t i, std::size_t l, FColor c, const FStyle& s = FStyle()) noexcept
        : index{i}
        , length{l}
      {
        attributes.fg_color = c;
        attributes.bg_color = getColorTheme()->dialog.bg;
        attributes.attr = s.toFAttribute();
      }

      FTextHighlight (std::size_t i, FColor c, const FStyle& s = FStyle()) noexcept
        : FTextHighlight{i, EOL, c, s}
      { }

      FTextHighlight (std::size_t i, std::size_t l, const FColorPair& cpair, const FStyle& s = FStyle()) noexcept
        : index{i}
        , length{l}
      {
        attributes.fg_color = cpair.getForegroundColor();
        attributes.bg_color = cpair.getBackgroundColor();
        attributes.attr = s.toFAttribute();
      }

      FTextHighlight (std::size_t i, const FColorPair& cpair, const FStyle& s = FStyle()) noexcept
        : FTextHighlight{i, EOL, cpair, s}
      { }

      // Data members
      std::size_t index{};
      std::size_t length{};
      FChar       attributes{};
    };

    struct FTextViewLine
    {
      explicit FTextViewLine (FString&& s, std::vector<FTextHighlight>&& v = {}) noexcept
        : text{std::move(s)}
        , highlight{std::move(v)}
      { }

      FString text{};
      std::vector<FTextHighlight> highlight{};
    };

    // Using-declarations
    using FTextViewList = std::vector<FTextViewLine>;
    using FWidget::setGeometry;

    struct FTextPosition
    {
      FTextViewList::size_type row{UNINITIALIZED_ROW};
      FString::size_type       column{UNINITIALIZED_COLUMN};
    };

    // Constructor
    explicit FTextView (FWidget* = nullptr);

    // Destructor
    ~FTextView() noexcept override;

    // Overloaded operators
    auto operator = (const FString&) -> FTextView&;
    template <typename typeT>
    auto operator << (const typeT&) -> FTextView&;
    auto operator << (const UniChar&) -> FTextView&;
    auto operator << (const std::string&) -> FTextView&;

    // Accessors
    auto getClassName() const -> FString override;
    auto getColumns() const noexcept -> std::size_t;
    auto getRows() const -> std::size_t;
    auto getScrollPos() const -> FPoint;
    auto getTextVisibleSize() const -> FSize;
    auto getText() const -> FString;
    auto getSelectedText() const -> FString;
    auto getSelectionStart() const -> FTextPosition;
    auto getSelectionEnd() const -> FTextPosition;
    auto getLine (FTextViewList::size_type) -> FTextViewLine&;
    auto getLine (FTextViewList::size_type) const -> const FTextViewLine&;
    auto getLines() const & -> const FTextViewList&;

    // Mutators
    void setSize (const FSize&, bool = true) override;
    void setGeometry (const FPoint&, const FSize&, bool = true) override;
    void resetColors() override;
    void setText (const FString&);
    void addHighlight (std::size_t, const FTextHighlight&);
    void resetHighlight (std::size_t);
    void setSelectionStart (const FTextViewList::size_type, const FString::size_type);
    void setSelectionEnd (const FTextViewList::size_type, const FString::size_type);
    void resetSelection();
    template <typename T>
    void setLines (T&&);
    void setSelectable (bool = true);
    void unsetSelectable();
    void scrollToX (int);
    void scrollToY (int);
    void scrollTo (const FPoint&);
    void scrollTo (int, int);
    void scrollToBegin();
    void scrollToEnd();
    void scrollBy (int, int);

    // Inquiry
    auto hasSelectedText() const -> bool;
    auto isSelectable() const -> bool;

    // Methods
    void hide() override;
    void clear();
    template <typename T>
    void append (const std::initializer_list<T>&);
    void append (const FString&);
    template <typename T>
    void insert (const std::initializer_list<T>&, int);
    void insert (const FString&, int);
    void replaceRange (const FString&, int, int);
    void deleteRange (int, int);
    void deleteLine (int);

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onMouseDoubleClick (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onTimer (FTimerEvent*) override;

  protected:
    // Method
    void initLayout() override;
    void adjustSize() override;

    // Inquiry
    auto isHorizontallyScrollable() const -> bool;
    auto isVerticallyScrollable() const -> bool;

    // Accessors
    auto getTextHeight() const -> std::size_t;
    auto getTextWidth() const -> std::size_t;

  private:
    // Constants
    static constexpr auto UNINITIALIZED_ROW = static_cast<FTextViewList::size_type>(-1);
    static constexpr auto UNINITIALIZED_COLUMN = static_cast<FString::size_type>(-1);

    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;

    // Inquiry
    auto isWithinTextBounds (const FPoint&) const -> bool;
    auto isLowerRightResizeCorner (const FPoint&) const -> bool;
    auto hasWrongSelectionOrder() const -> bool;

    // Methods
    void init();
    void mapKeyFunctions();
    void draw() override;
    void drawBorder() override;
    void drawScrollbars() const;
    void drawText();
    auto canSkipDrawing() const -> bool;
    void printLine (std::size_t);
    void addHighlighting ( FVTermBuffer&
                         , const std::vector<FTextHighlight>& ) const;
    void addSelection (FVTermBuffer&, std::size_t) const;
    auto useFDialogBorder() const -> bool;
    auto isPrintable (wchar_t) const -> bool;
    auto splitTextLines (const FString&) const -> FStringList;
    void processLine (FString&&, int);
    template<typename T1, typename T2>
    void setSelectionStartInt (T1&&, T2&&);
    template<typename T1, typename T2>
    void setSelectionEndInt (T1&&, T2&&);
    auto getScrollBarMaxHorizontal() const noexcept -> int;
    auto getScrollBarMaxVertical() const noexcept -> int;
    void updateVerticalScrollBar() const;
    void updateHorizontalScrollBar (std::size_t);
    auto convertMouse2TextPos (const FPoint&) const -> FPoint;
    void handleMouseWithinListBounds (const FPoint&);
    void handleMouseDragging (const FMouseEvent*);
    void handleLeftDragScroll();
    void handleRightDragScroll();
    void handleUpDragScroll();
    void handleDownDragScroll();
    void dragLeft();
    void dragRight();
    void dragUp();
    void dragDown();
    void stopDragScroll();
    void processChanged() const;
    void changeOnResize() const;
    auto shouldUpdateScrollbar (FScrollbar::ScrollType) const -> bool;
    auto getVerticalScrollDistance (const FScrollbar::ScrollType) const -> int;
    auto getHorizontalScrollDistance (const FScrollbar::ScrollType) const -> int;

    // Callback methods
    void cb_vbarChange (const FWidget*);
    void cb_hbarChange (const FWidget*);

    // Data members
    FTextViewList   data{};
    FScrollbarPtr   vbar{nullptr};
    FScrollbarPtr   hbar{nullptr};
    FTextPosition   selection_start{};
    FTextPosition   selection_end{};
    FPoint          select_click_pos{-1, -1};
    KeyMap          key_map{};
    DragScrollMode  drag_scroll{DragScrollMode::None};
    bool            update_scrollbar{true};
    bool            pass_to_dialog{false};
    bool            selectable{false};
    int             scroll_repeat{100};
    int             xoffset{0};
    int             yoffset{0};
    int             nf_offset{0};
    std::size_t     max_line_width{0};
};

// FListBox inline functions
//----------------------------------------------------------------------
inline auto FTextView::operator = (const FString& s) -> FTextView&
{
  setText(s);
  return *this;
}

//----------------------------------------------------------------------
template <typename typeT>
inline auto FTextView::operator << (const typeT& s) -> FTextView&
{
  FStringStream outstream{std::ios_base::out};
  outstream << s;

  if ( ! outstream.str().isEmpty() )
    append (outstream.str());

  return *this;
}

//----------------------------------------------------------------------
inline auto FTextView::operator << (const UniChar& c) -> FTextView&
{
  append (static_cast<wchar_t>(c));  // Required under Solaris
  return *this;
}

//----------------------------------------------------------------------
inline auto FTextView::operator << (const std::string& string) -> FTextView&
{
  append (string);
  return *this;
}

//----------------------------------------------------------------------
inline auto FTextView::getClassName() const -> FString
{ return "FTextView"; }

//----------------------------------------------------------------------
inline auto FTextView::getColumns() const noexcept -> std::size_t
{ return max_line_width; }

//----------------------------------------------------------------------
inline auto FTextView::getRows() const -> std::size_t
{ return data.size(); }

//----------------------------------------------------------------------
inline auto FTextView::getScrollPos() const -> FPoint
{ return {xoffset, yoffset}; }

//----------------------------------------------------------------------
inline auto FTextView::getTextVisibleSize() const -> FSize
{ return { FSize{getTextWidth(), getTextHeight()} }; }

//----------------------------------------------------------------------
inline auto FTextView::getSelectionStart() const -> FTextPosition
{ return selection_start; }

//----------------------------------------------------------------------
inline auto FTextView::getSelectionEnd() const -> FTextPosition
{ return selection_end; }

//----------------------------------------------------------------------
inline auto FTextView::getLine (FTextViewList::size_type line) -> FTextViewLine&
{ return data.at(line); }

//----------------------------------------------------------------------
inline auto FTextView::getLine (FTextViewList::size_type line) const -> const FTextViewLine&
{ return data.at(line); }

//----------------------------------------------------------------------
inline auto FTextView::getLines() const & -> const FTextViewList&
{ return data; }

//----------------------------------------------------------------------
inline void FTextView::setSelectionStart ( const FTextViewList::size_type row
                                         , const FString::size_type col )
{ selection_start = {row, col}; }

//----------------------------------------------------------------------
inline void FTextView::setSelectionEnd ( const FTextViewList::size_type row
                                       , const FString::size_type col )
{ selection_end = {row, col}; }

//----------------------------------------------------------------------
inline void FTextView::resetSelection()
{
  selection_start = {UNINITIALIZED_ROW, UNINITIALIZED_COLUMN};
  selection_end = {UNINITIALIZED_ROW, UNINITIALIZED_COLUMN};
}

//----------------------------------------------------------------------
template <typename T>
inline void FTextView::setLines (T&& list)
{
  clear();
  data = std::forward<T>(list);
  updateVerticalScrollBar();
  processChanged();
}

//----------------------------------------------------------------------
inline void FTextView::setSelectable (bool enable)
{ selectable = enable; }

//----------------------------------------------------------------------
inline void FTextView::unsetSelectable()
{ selectable = false; }

//----------------------------------------------------------------------
inline void FTextView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

//---------------------------------------------------------------
inline auto FTextView::hasSelectedText() const -> bool
{
  return selection_start.row != UNINITIALIZED_ROW
      && selection_end.row != UNINITIALIZED_ROW
      && selection_start.column != UNINITIALIZED_COLUMN
      && selection_end.column != UNINITIALIZED_COLUMN;
}

//----------------------------------------------------------------------
inline auto FTextView::isSelectable() const -> bool
{ return selectable; }

//----------------------------------------------------------------------
template <typename T>
void FTextView::append (const std::initializer_list<T>& list)
{
  for (const auto& str : list)
    insert(str, -1);
}

//----------------------------------------------------------------------
template <typename T>
void FTextView::insert (const std::initializer_list<T>& list, int pos)
{
  for (const auto& str : list)
  {
    insert(str, pos);
    pos++;
  }
}

//----------------------------------------------------------------------
inline void FTextView::deleteLine (int pos)
{ deleteRange (pos, pos); }

//----------------------------------------------------------------------
inline auto FTextView::isHorizontallyScrollable() const -> bool
{ return max_line_width > getTextWidth(); }

//----------------------------------------------------------------------
inline auto FTextView::isVerticallyScrollable() const -> bool
{ return getRows() > getTextHeight(); }

//----------------------------------------------------------------------
template<typename T1, typename T2>
inline void FTextView::setSelectionStartInt (T1&& row, T2&& col)
{
  selection_start = { static_cast<const FTextViewList::size_type>(std::forward<T1>(row))
                    , static_cast<const FString::size_type>(std::forward<T2>(col)) };
}

//----------------------------------------------------------------------
template<typename T1, typename T2>
inline void FTextView::setSelectionEndInt (T1&& row, T2&& col)
{
  selection_end = { static_cast<const FTextViewList::size_type>(std::forward<T1>(row))
                  , static_cast<const FString::size_type>(std::forward<T2>(col)) };
}

}  // namespace finalcut

#endif  // FTEXTVIEW_H
