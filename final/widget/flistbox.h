/***********************************************************************
* flistbox.h - Widget FListBox and FListBoxItem                        *
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
 *       ▕▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1     1▕▔▔▔▔▔▔▔▏
 *       ▕ FListBox ▏- - - -▕ FListBoxItem ▏- - - -▕ FData ▏
 *       ▕▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▏
 *
 */

#ifndef FLISTBOX_H
#define FLISTBOX_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "final/fwidget.h"
#include "final/util/fdata.h"
#include "final/widget/fscrollbar.h"

namespace finalcut
{

// class forward declaration
class FScrollbar;
class FString;

//----------------------------------------------------------------------
// class FListBoxItem
//----------------------------------------------------------------------

class FListBoxItem
{
  public:
    // Constructors
    template <typename DT = std::nullptr_t>
    explicit FListBoxItem (const FString& = FString{}, DT&& = DT() );

    // Accessors
    auto getClassName() const -> FString;
    auto getText() const -> FString;
    template <typename DT>
    auto getData() const -> clean_fdata_t<DT>&;

    // Mutators
    void setText (const FString&);
    template <typename DT>
    void setData (DT&&);

    // Inquiries
    auto isSelected() const -> bool;

    // Methods
    void clear();

  private:
    // Using-declaration
    using FDataAccessPtr = std::shared_ptr<FDataAccess>;

    // Methods
    auto stringFilter (const FString&) const -> FString;

    // Data members
    FString         text{};
    FDataAccessPtr  data_pointer{};
    BracketType     brackets{BracketType::None};
    bool            selected{false};

    // Friend classes
    friend class FListBox;
};


// FListBoxItem inline functions
//----------------------------------------------------------------------
template <typename DT>
inline FListBoxItem::FListBoxItem (const FString& txt, DT&& data)
  : text{stringFilter(txt)}
  , data_pointer{makeFData(std::forward<DT>(data))}
{ }

//----------------------------------------------------------------------
inline auto FListBoxItem::getClassName() const -> FString
{ return "FListBoxItem"; }

//----------------------------------------------------------------------
inline auto FListBoxItem::getText() const -> FString
{ return text; }

//----------------------------------------------------------------------
template <typename DT>
inline auto FListBoxItem::getData() const -> clean_fdata_t<DT>&
{
  return static_cast<FData<clean_fdata_t<DT>>&>(*data_pointer).get();
}

//----------------------------------------------------------------------
inline void FListBoxItem::setText (const FString& txt)
{
  text.setString(stringFilter(txt));
}

//----------------------------------------------------------------------
template <typename DT>
inline void FListBoxItem::setData (DT&& data)
{
  const auto data_obj = makeFData(std::forward<DT>(data));
  data_pointer.reset(data_obj);
}

//----------------------------------------------------------------------
inline auto FListBoxItem::isSelected() const -> bool
{ return selected; }

//----------------------------------------------------------------------
inline void FListBoxItem::clear()
{ text.clear(); }

//----------------------------------------------------------------------
inline auto FListBoxItem::stringFilter (const FString& txt) const -> FString
{
  return txt.rtrim()
            .expandTabs(FVTerm::getFOutput()->getTabstop())
            .removeBackspaces()
            .removeDel()
            .replaceControlCodes();
}


//----------------------------------------------------------------------
// class FListBox
//----------------------------------------------------------------------

class FListBox : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;
    using FListBoxItems = std::vector<FListBoxItem>;

    // Constructor
    explicit FListBox (FWidget* = nullptr);
    template <typename Iterator
            , typename InsertConverter>
    FListBox (Iterator, Iterator, InsertConverter, FWidget* = nullptr);
    template <typename Container
            , typename LazyConverter>
    FListBox (Container, LazyConverter&&, FWidget* = nullptr);

    // Disable copy constructor
    FListBox (const FListBox&) = delete;

    // Disable move constructor
    FListBox (FListBox&&) noexcept = delete;

    // Destructor
    ~FListBox() override;

    // Disable copy assignment operator (=)
    auto operator = (const FListBox&) -> FListBox& = delete;

    // Disable move assignment operator (=)
    auto operator = (FListBox&&) noexcept -> FListBox& = delete;

    // Accessors
    auto getClassName() const -> FString override;
    auto getCount() const -> std::size_t;
    auto getItem (std::size_t) & -> FListBoxItem&;
    auto getItem (std::size_t) const & -> const FListBoxItem&;
    auto getItem (FListBoxItems::iterator) & -> FListBoxItem&;
    auto getItem (FListBoxItems::const_iterator) const & -> const FListBoxItem&;
    auto currentItem() const noexcept -> std::size_t;
    auto getData() & -> FListBoxItems&;
    auto getData() const & -> const FListBoxItems&;
    auto getText() & -> FString&;

    // Mutators
    void setCurrentItem (std::size_t);
    void setCurrentItem (FListBoxItems::iterator);
    void selectItem (std::size_t);
    void selectItem (FListBoxItems::iterator) const;
    void unselectItem (std::size_t);
    void unselectItem (FListBoxItems::iterator) const;
    void showInsideBrackets (const std::size_t, BracketType);
    void showNoBrackets (std::size_t);
    void showNoBrackets (FListBoxItems::iterator) const;
    void setSize (const FSize&, bool = true) override;
    void setGeometry (const FPoint&, const FSize&, bool = true) override;
    void setMultiSelection (bool = true);
    void unsetMultiSelection ();
    void setDisable() override;
    void setText (const FString&);

    // Inquiries
    auto isSelected (std::size_t) const -> bool;
    auto isSelected (FListBoxItems::iterator) const -> bool;
    auto isMultiSelection() const -> bool;
    auto hasBrackets (std::size_t) const -> bool;
    auto hasBrackets (FListBoxItems::iterator) const -> bool;

    // Methods
    void hide() override;
    template <typename Iterator
            , typename InsertConverter>
    void insert (Iterator, Iterator, const InsertConverter&);
    template <typename Container
            , typename LazyConverter>
    void insert (const Container&, LazyConverter&&);
    template <typename Container
            , typename LazyConverter>
    void insert (Container*, LazyConverter&&);
    void insert (const FListBoxItem&);
    template <typename T
            , typename DT = std::nullptr_t>
    void insert ( const std::initializer_list<T>& list
                , BracketType = BracketType::None
                , bool = false
                , DT&& = DT() );
    template <typename ItemT
            , typename DT = std::nullptr_t>
    void insert ( const ItemT&
                , BracketType = BracketType::None
                , bool = false
                , DT&& = DT() );
    void remove (std::size_t);
    auto findItem (const FString&) -> FListBoxItems::iterator;
    void reserve (std::size_t);
    void clear();

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onMouseDoubleClick (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onTimer (FTimerEvent*) override;
    void onFocusIn (FFocusEvent*) override;
    void onFocusOut (FFocusEvent*) override;

  protected:
    // Methods
    void initLayout() override;
    void adjustYOffset (std::size_t);
    void adjustSize() override;

  private:
    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;
    using KeyMapResult = std::unordered_map<FKey, std::function<bool()>, EnumHash<FKey>>;
    using LazyInsert = std::function<void(FListBoxItem&, FDataAccess*, std::size_t)>;
    using FWidgetColorsptr = std::shared_ptr<FWidgetColors>;

    struct ListBoxData
    {
      FListBoxItems  itemlist{};
      FDataAccess*   source_container{nullptr};
      FString        text{};
      FString        inc_search{};
      KeyMap         key_map{};
      KeyMapResult   key_map_result{};
    };

    struct SelectionState
    {
      std::size_t  current{0};
      int          last_current{-1};
      int          select_from_item{-1};
      bool         multi_select{false};
      bool         mouse_select{false};
      bool         click_on_list{false};
    };

    struct ScrollingState
    {
      FScrollbarPtr  vbar{nullptr};
      FScrollbarPtr  hbar{nullptr};
      int            xoffset{0};
      int            yoffset{0};
      int            last_yoffset{-1};
      int            repeat{100};
      int            distance{1};
      bool           timer{false};
    };

    struct ElementData
    {
      bool&        search_mark;
      std::size_t  bracket_space{0};
      std::size_t  inc_len{0};
      FString      text{};
      std::size_t  text_width{0};
      std::size_t  column_width{0};
    };

    // Enumeration
    enum class ConvertType
    {
      None   = 0,
      Direct = 1,
      Lazy   = 2
    };

    // Accessors
    static auto getString (FListBoxItems::iterator) -> FString;

    // Inquiry
    auto isHorizontallyScrollable() const -> bool;
    auto isVerticallyScrollable() const -> bool;
    auto isCurrentLine (int) const -> bool;
    auto isDragSelect() const -> bool;
    auto canSkipDragScrolling() -> bool;

    // Methods
    void init();
    void mapKeyFunctions();
    void processKeyAction (FKeyEvent*);
    void draw() override;
    void drawBorder() override;
    void drawScrollbars() const;
    void drawHeadline();
    auto canSkipDrawing() const -> bool;
    auto calculateNumberItemsToDraw() const -> std::size_t;
    auto canRedrawPartialList() const -> bool;
    void updateRedrawParameters (std::size_t&, std::size_t&) const;
    void finalizeDrawing();
    void drawList();
    void drawListLine (int, FListBoxItems::iterator, bool);
    void printLeftBracket (BracketType);
    template<class UnaryPred>
    void printLeftBracket_if (BracketType, UnaryPred);
    void printRightBracket (BracketType);
    template<class UnaryPred>
    void printRightBracket_if (BracketType, ElementData&, UnaryPred);
    void setCurrentFocusedElementColor() const;
    void drawListBracketsLine (int, FListBoxItems::iterator, bool);
    void printElement (const ElementData&);
    auto shouldPrintRightBracket (const ElementData&, std::size_t) const -> bool;
    void printLeftCurrentLineArrow (int);
    void printRightCurrentLineArrow (int);
    void printRemainingSpacesFromPos (std::size_t);
    void setInitialLineAttributes (bool) const;
    void setCurrentLineAttributes (int, bool, bool, bool&);
    void setSelectedCurrentLineAttributes (int);
    void setUnselectedCurrentLineAttributes (int, bool, bool&);
    void setLineAttributes (int, bool, bool, bool&);
    auto getMaxWidth() const ->  std::size_t;
    void unsetAttributes() const;
    void updateDrawing (bool, bool);
    void recalculateHorizontalBar (std::size_t, bool);
    void recalculateVerticalBar (std::size_t) const;
    void multiSelection (std::size_t);
    void multiSelectionUpTo (std::size_t);
    void wheelUp (int);
    void wheelDown (int);
    void wheelLeft (int);
    void wheelRight (int);
    auto dragScrollUp() -> bool;
    auto dragScrollDown() -> bool;
    void dragUp (MouseButton);
    void dragDown (MouseButton);
    void stopDragScroll();
    void prevListItem (int);
    void nextListItem (int);
    void scrollToX (int);
    void scrollToY (int);
    void scrollLeft (int);
    void scrollRight (int);
    void scrollLeft();
    void scrollRight();
    void onePosUp();
    void onePosDown();
    void onePageUp();
    void onePageDown();
    void firstPos();
    void lastPos();
    auto isWithinListBounds (const FPoint&) const -> bool;
    auto skipIncrementalSearch() -> bool;
    auto isNonSelectMouseButtonPressed (const FMouseEvent*) const -> bool;
    template <typename MultiSelectionFunction>
    void handleMouseWithinListBounds (const FMouseEvent*, const MultiSelectionFunction&);
    void handleMouseDragging (const FMouseEvent*);
    void acceptSelection();
    auto spacebarProcessing() -> bool;
    auto changeSelectionAndPosition() -> bool;
    auto deletePreviousCharacter() -> bool;
    auto keyIncSearchInput (FKey) -> bool;
    void processClick() const;
    void processSelect() const;
    void processRowChanged() const;
    void processChanged() const;
    void changeOnResize() const;
    void updateScrollBarAfterRemoval (std::size_t);
    auto getScrollBarMaxHorizontal() const noexcept -> int;
    auto getScrollBarMaxVertical() const noexcept -> int;
    void recalculateMaximumLineWidth();
    void lazyConvert (FListBoxItems::iterator, std::size_t);
    auto index2iterator (std::size_t) -> FListBoxItems::iterator;
    auto index2iterator (std::size_t index) const -> FListBoxItems::const_iterator;
    void handleSelectionChange (const std::size_t);
    void handleXOffsetChange (const int);
    void handleVerticalScrollBarUpdate (const FScrollbar::ScrollType, const int) const;
    void handleHorizontalScrollBarUpdate (const FScrollbar::ScrollType, const int) const;
    auto getVerticalScrollDistance (const FScrollbar::ScrollType) const -> int;
    auto getHorizontalScrollDistance (const FScrollbar::ScrollType) const -> int;

    // Callback methods
    void cb_vbarChange (const FWidget*);
    void cb_hbarChange (const FWidget*);

    // Function Pointer
    LazyInsert      lazy_inserter{};

    // Data members
    std::size_t     nf_offset{0};
    std::size_t     max_line_width{0};
    ListBoxData     data{};
    ScrollingState  scroll{};
    SelectionState  selection{};
    ConvertType     conv_type{ConvertType::None};
    DragScrollMode  drag_scroll{DragScrollMode::None};
};

// non-member function
//----------------------------------------------------------------------
namespace flistboxhelper
{

template <typename Container>
constexpr auto getContainer(FDataAccess* container) -> clean_fdata_t<Container>&
{
  return static_cast<FData<clean_fdata_t<Container>>&>(*container).get();
}

}  // namespace flistboxhelper

// FListBox inline functions
//----------------------------------------------------------------------
template <typename Iterator
        , typename InsertConverter>
inline FListBox::FListBox ( Iterator first
                          , Iterator last
                          , InsertConverter convert
                          , FWidget* parent )
  : FWidget{parent}
{
  init();

  while ( first != last )
  {
    insert(convert(first), BracketType::None, false, &(*first));
    ++first;
  }
}

//----------------------------------------------------------------------
template <typename Container
        , typename LazyConverter>
inline FListBox::FListBox ( Container container
                          , LazyConverter&& convert
                          , FWidget* parent )
  : FWidget{parent}
{
  init();
  insert (container, std::forward<LazyConverter>(convert));
}

//----------------------------------------------------------------------
inline auto FListBox::getClassName() const -> FString
{ return "FListBox"; }

//----------------------------------------------------------------------
inline auto FListBox::getCount() const -> std::size_t
{ return data.itemlist.size(); }

//----------------------------------------------------------------------
inline auto FListBox::getItem (std::size_t index) & -> FListBoxItem&
{
  auto iter = index2iterator(index - 1);
  return *iter;
}

//----------------------------------------------------------------------
inline auto FListBox::getItem (std::size_t index) const & -> const FListBoxItem&
{
  auto iter = index2iterator(index - 1);
  return *iter;
}

//----------------------------------------------------------------------
inline auto FListBox::getItem (FListBoxItems::iterator iter) & -> FListBoxItem&
{ return *iter; }

//----------------------------------------------------------------------
inline auto FListBox::getItem (FListBoxItems::const_iterator iter) const & -> const FListBoxItem&
{ return *iter; }

//----------------------------------------------------------------------
inline auto FListBox::currentItem() const noexcept -> std::size_t
{ return selection.current; }

//----------------------------------------------------------------------
inline auto FListBox::getData() & -> FListBoxItems&
{ return data.itemlist; }

//----------------------------------------------------------------------
inline auto FListBox::getData() const & -> const FListBoxItems&
{ return data.itemlist; }

//----------------------------------------------------------------------
inline auto FListBox::getText() & -> FString&
{ return data.text; }

//----------------------------------------------------------------------
inline void FListBox::selectItem (std::size_t index)
{ index2iterator(index - 1)->selected = true; }

//----------------------------------------------------------------------
inline void FListBox::selectItem (FListBoxItems::iterator iter) const
{ iter->selected = true; }

//----------------------------------------------------------------------
inline void FListBox::unselectItem (std::size_t index)
{ index2iterator(index - 1)->selected = false; }

//----------------------------------------------------------------------
inline void FListBox::unselectItem (FListBoxItems::iterator iter) const
{ iter->selected = false; }

//----------------------------------------------------------------------
inline void FListBox::showNoBrackets (std::size_t index)
{ index2iterator(index - 1)->brackets = BracketType::None; }

//----------------------------------------------------------------------
inline void FListBox::showNoBrackets (FListBoxItems::iterator iter) const
{ iter->brackets = BracketType::None; }

//----------------------------------------------------------------------
inline void FListBox::setMultiSelection (bool enable)
{ selection.multi_select = enable; }

//----------------------------------------------------------------------
inline void FListBox::unsetMultiSelection()
{ setMultiSelection(false); }

//----------------------------------------------------------------------
inline void FListBox::setDisable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline auto FListBox::isSelected (std::size_t index) const -> bool
{ return index2iterator(index - 1)->selected; }

//----------------------------------------------------------------------
inline auto FListBox::isSelected (FListBoxItems::iterator iter) const -> bool
{ return iter->selected; }

//----------------------------------------------------------------------
inline auto FListBox::isMultiSelection() const -> bool
{ return selection.multi_select; }

//----------------------------------------------------------------------
inline auto FListBox::hasBrackets(std::size_t index) const -> bool
{ return index2iterator(index - 1)->brackets != BracketType::None; }

//----------------------------------------------------------------------
inline auto FListBox::hasBrackets(FListBoxItems::iterator iter) const -> bool
{ return iter->brackets != BracketType::None; }

//----------------------------------------------------------------------
inline void FListBox::reserve (std::size_t new_cap)
{ data.itemlist.reserve(new_cap); }

//----------------------------------------------------------------------
template <typename Iterator
        , typename InsertConverter>
inline void FListBox::insert ( Iterator first
                             , Iterator last
                             , const InsertConverter& convert )
{
  conv_type = ConvertType::Direct;

  while ( first != last )
  {
    insert (convert(first), BracketType::None, false, &(*first));
    ++first;
  }
}

//----------------------------------------------------------------------
template <typename Container
        , typename LazyConverter>
void FListBox::insert (const Container& container, LazyConverter&& converter)
{
  conv_type = ConvertType::Lazy;
  data.source_container = makeFData(container);
  lazy_inserter = std::forward<LazyConverter>(converter);
  const std::size_t size = container.size();

  if ( size > 0 )
    data.itemlist.resize(size);

  recalculateVerticalBar(size);
}

//----------------------------------------------------------------------
template <typename Container
        , typename LazyConverter>
void FListBox::insert (Container* container, LazyConverter&& converter)
{
  insert (*container, std::forward<LazyConverter>(converter));
}

//----------------------------------------------------------------------
template <typename T
        , typename DT>
void FListBox::insert ( const std::initializer_list<T>& list
                      , BracketType b
                      , bool s
                      , DT&& d )
{
  for (const auto& item : list)
  {
    FListBoxItem listItem (FString() << item, std::forward<DT>(d));
    listItem.brackets = b;
    listItem.selected = s;
    insert (listItem);
  }
}

//----------------------------------------------------------------------
template <typename ItemT
        , typename DT>
void FListBox::insert ( const ItemT& item
                      , BracketType b
                      , bool s
                      , DT&& d )
{
  FListBoxItem listItem (FString() << item, std::forward<DT>(d));
  listItem.brackets = b;
  listItem.selected = s;
  insert (listItem);
}

//----------------------------------------------------------------------
inline auto FListBox::isHorizontallyScrollable() const -> bool
{ return max_line_width + 1 >= getClientWidth(); }

//----------------------------------------------------------------------
inline auto FListBox::isVerticallyScrollable() const -> bool
{ return getCount() > getClientHeight(); }

//----------------------------------------------------------------------
inline auto FListBox::isCurrentLine (int y) const -> bool
{ return y + scroll.yoffset + 1 == int(selection.current); }

//----------------------------------------------------------------------
template<class UnaryPred>
inline void FListBox::printLeftBracket_if ( BracketType bracket_type
                                          , UnaryPred p )
{
  if ( p )
    printLeftBracket(bracket_type);
}

//----------------------------------------------------------------------
template<class UnaryPred>
inline void FListBox::printRightBracket_if ( BracketType bracket_type
                                           , ElementData& elem
                                           , UnaryPred p )
{
  if ( ! p )
    return;

  if ( elem.search_mark && elem.text.getLength() == elem.inc_len )
    setCurrentFocusedElementColor();

  printRightBracket (bracket_type);
  elem.column_width++;
}

//----------------------------------------------------------------------
inline auto FListBox::getMaxWidth() const ->  std::size_t
{ return getWidth() - nf_offset - 4; }

//----------------------------------------------------------------------
inline auto \
    FListBox::index2iterator (std::size_t index) -> FListBoxItems::iterator
{
  auto iter = data.itemlist.begin();
  using distance_type = FListBoxItems::difference_type;
  std::advance (iter, distance_type(index));
  return iter;
}

//----------------------------------------------------------------------
inline auto \
    FListBox::index2iterator (std::size_t index) const -> FListBoxItems::const_iterator
{
  auto iter = data.itemlist.begin();
  using distance_type = FListBoxItems::difference_type;
  std::advance (iter, distance_type(index));
  return iter;
}

}  // namespace finalcut

#endif  // FLISTBOX_H
