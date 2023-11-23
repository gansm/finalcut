/***********************************************************************
* ftextview.h - Widget FTextView (a multiline text viewer)             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2023 Markus Gans                                      *
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
        auto wc = getColorTheme();
        attributes.fg_color = wc->dialog.fg;
        attributes.bg_color = wc->dialog.bg;
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
    auto getLine (FTextViewList::size_type) -> FTextViewLine&;
    auto getLines() const & -> const FTextViewList&;

    // Mutators
    void setSize (const FSize&, bool = true) override;
    void setGeometry (const FPoint&, const FSize&, bool = true) override;
    void resetColors() override;
    void setText (const FString&);
    void addHighlight (std::size_t, const FTextHighlight&);
    void resetHighlight (std::size_t);
    void scrollToX (int);
    void scrollToY (int);
    void scrollTo (const FPoint&);
    void scrollTo (int, int);
    void scrollToBegin();
    void scrollToEnd();
    void scrollBy (int, int);

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
    void onWheel (FWheelEvent*) override;

  protected:
    // Method
    void initLayout() override;
    void adjustSize() override;

  private:
    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;

    // Accessors
    auto getTextHeight() const -> std::size_t;
    auto getTextWidth() const -> std::size_t;

    // Inquiry
    auto isHorizontallyScrollable() const -> bool;
    auto isVerticallyScrollable() const -> bool;

    // Methods
    void init();
    void mapKeyFunctions();
    void draw() override;
    void drawBorder() override;
    void drawScrollbars() const;
    void drawText();
    void printHighlighted ( FVTermBuffer&
                          , const std::vector<FTextHighlight>& );
    auto useFDialogBorder() const -> bool;
    auto isPrintable (wchar_t) const -> bool;
    auto splitTextLines (const FString&) const -> FStringList;
    void processLine (FString&&, int);
    void updateVerticalScrollBar();
    void updateHorizontalScrollBar (std::size_t);
    void processChanged() const;
    void changeOnResize() const;

    // Callback methods
    void cb_vbarChange (const FWidget*);
    void cb_hbarChange (const FWidget*);

    // Data members
    FTextViewList  data{};
    FScrollbarPtr  vbar{nullptr};
    FScrollbarPtr  hbar{nullptr};
    KeyMap         key_map{};
    bool           update_scrollbar{true};
    int            xoffset{0};
    int            yoffset{0};
    int            nf_offset{0};
    std::size_t    max_line_width{0};
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
{ return std::size_t(data.size()); }

//----------------------------------------------------------------------
inline auto FTextView::getScrollPos() const -> FPoint
{ return {xoffset, yoffset}; }

//----------------------------------------------------------------------
inline auto FTextView::getTextVisibleSize() const -> FSize
{ return {getTextWidth(), getTextHeight()}; }

//----------------------------------------------------------------------
inline auto FTextView::getLine (FTextViewList::size_type line) -> FTextViewLine&
{ return data.at(line); }

//----------------------------------------------------------------------
inline auto FTextView::getLines() const & -> const FTextViewList&
{ return data; }

//----------------------------------------------------------------------
inline void FTextView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

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

}  // namespace finalcut

#endif  // FTEXTVIEW_H
