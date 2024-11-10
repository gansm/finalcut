/***********************************************************************
* flistview.h - Widget FListView and FListViewItem                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2024 Markus Gans                                      *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏           ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏           ▕ FObject ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏           ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲                     ▲
 *            │                     │
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏1     1▕▔▔▔▔▔▔▔▏
 *      ▕ FListView ▏- - - -▕ FListViewItem ▏- - - -▕ FData ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▏
 */

#ifndef FLISTVIEW_H
#define FLISTVIEW_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <iterator>
#include <memory>
#include <stack>
#include <unordered_map>
#include <utility>
#include <vector>

#include "final/ftypes.h"
#include "final/fwidget.h"
#include "final/util/fdata.h"
#include "final/vterm/fvtermbuffer.h"
#include "final/widget/fscrollbar.h"

namespace finalcut
{

// class forward declaration
class FListView;
class FScrollbar;
class FString;

//----------------------------------------------------------------------
// class FListViewItem
//----------------------------------------------------------------------

class FListViewItem : public FObject
{
  public:
    // Constructor
    explicit FListViewItem (iterator);
    template <typename DT>
    FListViewItem (const FStringList&, DT&&, iterator);

    // copy constructor
    FListViewItem (const FListViewItem&);

    // Destructor
    ~FListViewItem() override;

    // copy assignment operator (=)
    auto operator = (const FListViewItem&) -> FListViewItem&;

    // Accessors
    auto getClassName() const -> FString override;
    auto getColumnCount() const -> uInt;
    auto getSortColumn() const -> int;
    auto getText (int) const -> FString;
    template <typename DT>
    auto getData() const -> clean_fdata_t<DT>&;
    auto getDepth() const -> uInt;

    // Mutators
    void setText (int, const FString&);
    template <typename DT>
    void setData (DT&&);
    void setCheckable (bool = true);
    void setChecked (bool = true);

    // Inquiry
    auto isChecked() const -> bool;
    auto isExpand() const -> bool;

    // Methods
    auto insert (FListViewItem*) -> iterator;
    auto insert (FListViewItem*, iterator) const -> iterator;
    void remove (FListViewItem*) const;
    void expand();
    void collapse();

  private:
    // Using-declaration
    using FDataAccessPtr = std::shared_ptr<FDataAccess>;

    // Inquiry
    auto isExpandable() const -> bool;
    auto isCheckable() const -> bool;

    // Methods
    template <typename Compare>
    void sort (Compare);
    auto appendItem (FListViewItem*) -> iterator;
    void replaceControlCodes();
    auto getVisibleLines() -> std::size_t;
    void resetVisibleLineCounter();

    // Data members
    FStringList     column_list{};
    FDataAccessPtr  data_pointer{};
    iterator        root{};
    std::size_t     visible_lines{1};
    bool            expandable{false};
    bool            is_expand{false};
    bool            checkable{false};
    bool            is_checked{false};

    // Friend class
    friend class FListView;
    friend class FListViewIterator;
};


// FListViewItem inline functions
//----------------------------------------------------------------------
template <typename DT>
inline FListViewItem::FListViewItem ( const FStringList& cols
                                    , DT&& data
                                    , iterator parent_iter )
  : FObject{nullptr}
  , column_list{cols}
  , data_pointer{makeFData(std::forward<DT>(data))}
{
  if ( cols.empty() )
    return;

  replaceControlCodes();
  insert (this, parent_iter);
}

//----------------------------------------------------------------------
inline auto FListViewItem::getClassName() const -> FString
{ return "FListViewItem"; }

//----------------------------------------------------------------------
inline auto FListViewItem::getColumnCount() const -> uInt
{ return static_cast<uInt>(column_list.size()); }

//----------------------------------------------------------------------
template <typename DT>
inline auto FListViewItem::getData() const -> clean_fdata_t<DT>&
{
  return static_cast<FData<clean_fdata_t<DT>>&>(*data_pointer).get();
}

//----------------------------------------------------------------------
template <typename DT>
inline void FListViewItem::setData (DT&& data)
{
  const auto data_obj = makeFData(std::forward<DT>(data));
  data_pointer = data_obj;
}

//----------------------------------------------------------------------
inline void FListViewItem::setChecked (bool checked)
{ is_checked = checked; }

//----------------------------------------------------------------------
inline auto FListViewItem::isChecked() const -> bool
{ return is_checked; }

//----------------------------------------------------------------------
inline auto FListViewItem::isExpand() const -> bool
{ return is_expand; }

//----------------------------------------------------------------------
inline auto FListViewItem::isExpandable() const -> bool
{ return expandable; }

//----------------------------------------------------------------------
inline auto FListViewItem::isCheckable() const -> bool
{ return checkable; }


//----------------------------------------------------------------------
// class FListViewIterator
//----------------------------------------------------------------------

class FListViewIterator
{
  public:
    // Using-declarations
    using FObjectList   = std::vector<FObject*>;
    using Iterator      = FObjectList::iterator;
    using IteratorStack = std::stack<Iterator>;

    // Constructor
    FListViewIterator () = default;
    ~FListViewIterator () = default;
    explicit FListViewIterator (Iterator);
    FListViewIterator (const FListViewIterator&) = default;
    FListViewIterator (FListViewIterator&& i) noexcept
      : iter_path{std::move(i.iter_path)}
      , node{i.node}
      , position{i.position}
    { }

    // Overloaded operators
    auto operator = (const FListViewIterator&) -> FListViewIterator& = default;
    auto operator = (FListViewIterator&&) noexcept -> FListViewIterator& = default;
    auto operator = (Iterator iter) -> FListViewIterator&;
    auto operator ++ () -> FListViewIterator&;    // prefix
    auto operator ++ (int) -> FListViewIterator;  // postfix
    auto operator -- () -> FListViewIterator&;    // prefix
    auto operator -- (int) -> FListViewIterator;  // postfix
    auto operator += (int) -> FListViewIterator&;
    auto operator -= (int) -> FListViewIterator&;
    auto operator * () const -> FObject*&;
    auto operator -> () const -> FObject*;

    friend inline auto operator == ( const FListViewIterator& lhs
                                   , const FListViewIterator& rhs ) -> bool
    {
      return lhs.node == rhs.node;
    }

    friend inline auto operator != ( const FListViewIterator& lhs
                                   , const FListViewIterator& rhs ) -> bool
    {
      return lhs.node != rhs.node;
    }

    friend inline auto operator == ( const FListViewIterator& listview_iter
                                   , const Iterator& iter) -> bool
    {
      return listview_iter.node == iter;
    }

    friend inline auto operator != ( const FListViewIterator& listview_iter
                                   , const Iterator& iter) -> bool
    {
      return listview_iter.node != iter;
    }

    // Accessor
    auto getClassName() const -> FString;
    auto getPosition() -> int&;

    // Methods
    void parentElement();

    // Friend Non-member operator functions
    friend auto operator + (const FListViewIterator& lhs, int n) -> FListViewIterator
    {
      auto tmp = lhs;

      for (int i = n; i > 0 ; i--)
        tmp.nextElement(tmp.node);

      return tmp;
    }

    friend auto operator - (const FListViewIterator& lhs, int n) -> FListViewIterator
    {
      auto tmp = lhs;

      for (int i = n; i > 0 ; i--)
        tmp.prevElement(tmp.node);

      return tmp;
    }

  private:
    // Methods
    void nextElement (Iterator&);
    void prevElement (Iterator&);

    // Data members
    IteratorStack  iter_path{};
    Iterator       node{};
    int            position{0};
};


// FListViewIterator inline functions
//----------------------------------------------------------------------
inline auto FListViewIterator::operator * () const -> FObject*&
{ return *node; }

//----------------------------------------------------------------------
inline auto FListViewIterator::operator -> () const -> FObject*
{ return *node; }

//----------------------------------------------------------------------
inline auto FListViewIterator::getClassName() const -> FString
{ return "FListViewIterator"; }

//----------------------------------------------------------------------
inline auto FListViewIterator::getPosition() -> int&
{ return position; }


//----------------------------------------------------------------------
// class FListView
//----------------------------------------------------------------------

class FListView : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;
    using FListViewItems = std::vector<FListViewItem*>;

    // Disable copy constructor
    FListView (const FListView&) = delete;

    // Disable move constructor
    FListView (FListView&&) noexcept = delete;

    // Constructor
    explicit FListView (FWidget* = nullptr);

    // Destructor
    ~FListView() override;

    // Disable copy assignment operator (=)
    auto operator = (const FListView&) -> FListView& = delete;

    // Disable move assignment operator (=)
    auto operator = (FListView&&) noexcept -> FListView& = delete;

    // Accessors
    auto getClassName() const -> FString override;
    auto getCount() const -> std::size_t;
    auto getColumnCount() const -> std::size_t;
    auto getColumnAlignment (int) const -> Align;
    auto getColumnText (int) const -> FString;
    auto getColumnSortType (int) const -> SortType;
    auto getSortOrder() const -> SortOrder;
    auto getSortColumn() const -> int;
    auto getCurrentItem() -> FListViewItem*;

    // Mutators
    void setSize (const FSize&, bool = true) override;
    void setGeometry ( const FPoint&, const FSize&
                     , bool = true ) override;
    void setColumnAlignment (int, Align);
    void setColumnText (int, const FString&);
    void setColumnSortType (int, SortType = SortType::Name);
    void setColumnSort (int, SortOrder = SortOrder::Ascending);
    template <typename Compare>
    void setUserAscendingCompare (Compare);
    template <typename Compare>
    void setUserDescendingCompare (Compare);
    void hideSortIndicator (bool = true);
    void showColumn (int);
    void hideColumn (int);
    void setTreeView (bool = true);
    void unsetTreeView();

    // Inquiries
    auto isColumnHidden (int) const -> bool;

    // Methods
    virtual auto addColumn (const FString&, int = USE_MAX_SIZE) -> int;
    virtual auto removeColumn (int) -> int;
    void removeAllColumns();
    void hide() override;
    auto insert (FListViewItem*) -> iterator;
    auto insert (FListViewItem*, iterator) -> iterator;
    template <typename DT = std::nullptr_t>
    auto insert (const FStringList&, DT&& = DT()) -> iterator;
    auto insert (const FStringList&, iterator) -> iterator;
    template <typename DT>
    auto insert (const FStringList&, DT&&, iterator) -> iterator;
    template <typename T
            , typename DT = std::nullptr_t>
    auto insert (const std::initializer_list<T>&, DT&& = DT()) -> iterator;
    template <typename T>
    auto insert (const std::initializer_list<T>&, iterator) -> iterator;
    template <typename T
            , typename DT>
    auto insert (const std::initializer_list<T>&, DT&&, iterator) -> iterator;
    template <typename ColT
            , typename DT = std::nullptr_t>
    auto insert (const std::vector<ColT>&, DT&& = DT()) -> iterator;
    template <typename ColT>
    auto insert (const std::vector<ColT>&, iterator) -> iterator;
    template <typename ColT
            , typename DT>
    auto insert (const std::vector<ColT>&, DT&&, iterator) -> iterator;
    void remove (FListViewItem*);
    void clear();
    auto getData() & -> FListViewItems&;
    auto getData() const & -> const FListViewItems&;

    virtual void sort();

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onMouseDoubleClick (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onTimer (FTimerEvent*) override;
    void onFocusOut (FFocusEvent*) override;

  protected:
    // Methods
    void initLayout() override;
    void adjustViewport (const int);
    void adjustScrollbars (const std::size_t) const;
    void adjustSize() override;

  private:
    struct Header;  // forward declaration

    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;
    using KeyMapResult = std::unordered_map<FKey, std::function<bool()>, EnumHash<FKey>>;
    using HeaderItems = std::vector<Header>;
    using SortTypes = std::vector<SortType>;

    struct ListViewData
    {
      iterator      root{};
      FObjectList   selflist{};
      FObjectList   itemlist{};
      HeaderItems   header;  // GitHub issues #122
      FVTermBuffer  headerline{};
      KeyMap        key_map{};
      KeyMapResult  key_map_result{};
    };

    struct SelectionState
    {
      FListViewIterator     current_iter{};
      const FListViewItem*  clicked_checkbox_item{nullptr};
      FPoint                clicked_expander_pos{-1, -1};
      FPoint                clicked_header_pos{-1, -1};
    };

    struct SortState
    {
      int        column{-1};
      SortTypes  type{};
      SortOrder  order{SortOrder::Unsorted};
      bool       hide_sort_indicator{false};
    };

    struct ScrollingState
    {
      FScrollbarPtr      vbar{nullptr};
      FScrollbarPtr      hbar{nullptr};
      FListViewIterator  first_visible_line{};
      FListViewIterator  last_visible_line{};
      int                first_line_position_before{-1};
      int                xoffset{0};
      bool               timer{false};
      int                repeat{100};
      int                distance{1};
    };

    // Constants
    static constexpr std::size_t checkbox_space = 4;

    // Constants
    static constexpr int USE_MAX_SIZE = -1;

    // Accessors
    static auto getNullIterator() -> iterator&;

    // Mutators
    static void setNullIterator (const iterator&);

    // Inquiry
    auto isHorizontallyScrollable() const -> bool;
    auto isVerticallyScrollable() const -> bool;
    auto canSkipListDrawing() const -> bool;
    auto canSkipDragScrolling() -> bool;

    // Methods
    void init();
    void mapKeyFunctions();
    void processKeyAction (FKeyEvent*);
    template <typename Compare>
    void sort (Compare);
    auto getAlignOffset ( const Align
                        , const std::size_t
                        , const std::size_t ) const -> std::size_t;
    auto getListEnd (const FListViewItem*) -> iterator;
    void draw() override;
    void drawBorder() override;
    void drawScrollbars() const;
    void drawHeadlines();
    void drawList();
    void setInputCursor (const FListViewItem*, int, bool);
    void finalizeListDrawing (int);
    void adjustWidthForTreeView (std::size_t&, std::size_t, bool) const;
    void drawListLine (const FListViewItem*, bool, bool);
    auto createColumnsString (const FListViewItem*) -> FString;
    void printColumnsString (FString&);
    void clearList();
    void setLineAttributes (bool, bool) const;
    auto getCheckBox (const FListViewItem* item) const -> FString;
    auto getLinePrefix (const FListViewItem*, std::size_t) const -> FString;
    void drawSortIndicator (std::size_t&, std::size_t);
    void drawHeadlineLabel (const HeaderItems::const_iterator&);
    void drawHeaderBorder (std::size_t);
    auto findHeaderStartPos (bool&) -> FVTermBuffer::iterator;
    auto findHeaderEndPos (FVTermBuffer::iterator, bool, bool&) -> FVTermBuffer::iterator;
    void drawBufferedHeadline();
    void drawColumnEllipsis ( const HeaderItems::const_iterator&
                            , const FString& );
    void updateLayout();
    void updateDrawing (bool, bool);
    auto determineLineWidth (FListViewItem*) -> std::size_t;
    void beforeInsertion (FListViewItem*);
    void afterInsertion();
    void adjustListBeforeRemoval (const FListViewItem*);
    void removeItemFromParent (FListViewItem*);
    void updateListAfterRemoval();
    void recalculateHorizontalBar (std::size_t);
    void recalculateVerticalBar (std::size_t) const;
    void mouseHeaderClicked();
    auto getHeaderClickWidth (const Header&, int) const -> int;
    auto isPositionWithinHeader (int, int, int) const -> bool;
    void handleColumnSort (int);
    void handleTreeExpanderClick (const FMouseEvent*);
    void handleCheckboxClick (const FMouseEvent*);
    void wheelUp (int);
    void wheelDown (int);
    void wheelLeft (int);
    void wheelRight (int);
    auto dragScrollUp (int) -> bool;
    auto dragScrollDown (int) -> bool;
    void dragUp (MouseButton);
    void dragDown (MouseButton);
    void stopDragScroll();
    void toggleItemExpandState (FListViewItem*) const;
    void toggleItemCheckState (FListViewItem*) const;
    auto isCheckboxClicked (int, int) const -> bool;
    void resetClickedPositions();
    auto isWithinHeaderBounds (const FPoint&) const -> bool;
    auto isWithinListBounds (const FPoint&) const -> bool;
    auto appendItem (FListViewItem*) -> iterator;
    void handleListEvent (const FMouseEvent*);
    void handleTreeViewEvents (const FMouseEvent*, const FListViewItem*);
    void handleCheckableItemsEvents (const FMouseEvent*, const FListViewItem*);
    void processClick() const;
    void processRowChanged() const;
    void processChanged() const;
    void changeOnResize() const;
    void toggleCheckbox();
    void collapseAndScrollLeft();
    void jumpToParentElement (const FListViewItem*);
    void expandAndScrollRight();
    void firstPos();
    void lastPos();
    auto expandSubtree() -> bool;
    auto collapseSubtree() -> bool;
    void setRelativePosition (int);
    void stepForward();
    void stepBackward();
    void stepForward (int);
    void stepBackward (int);
    void scrollToX (int);
    void scrollToY (int);
    void scrollTo (const FPoint&);
    void scrollTo (int, int);
    void scrollBy (int, int);
    auto isItemListEmpty() const -> bool;
    auto isTreeView() const -> bool;
    auto isColumnIndexInvalid (int) const -> bool;
    auto hasCheckableItems() const -> bool;
    auto getScrollBarMaxHorizontal() const noexcept -> int;
    auto getScrollBarMaxVertical (const std::size_t) const noexcept -> int;
    void updateViewAfterVBarChange (const FScrollbar::ScrollType);
    void updateViewAfterHBarChange (const FScrollbar::ScrollType, const int);
    auto getVerticalScrollDistance (const FScrollbar::ScrollType) const -> int;
    auto getHorizontalScrollDistance (const FScrollbar::ScrollType) const -> int;

    // Callback methods
    void cb_vbarChange (const FWidget*);
    void cb_hbarChange (const FWidget*);

    // Data members
    std::size_t     nf_offset{0};
    std::size_t     max_line_width{1};
    bool            tree_view{false};
    bool            has_checkable_items{false};
    ListViewData    data{};
    SortState       sorting{};
    ScrollingState  scroll{};
    SelectionState  selection{};
    DragScrollMode  drag_scroll{DragScrollMode::None};

    // Function Pointer
    bool (*user_defined_ascending) (const FObject*, const FObject*){nullptr};
    bool (*user_defined_descending) (const FObject*, const FObject*){nullptr};

    // Friend class
    friend class FListViewItem;
};


//----------------------------------------------------------------------
// struct FListView::Header
//----------------------------------------------------------------------

struct FListView::Header
{
  public:
    Header() = default;

    FString name{};
    Align   alignment{Align::Left};
    int     width{0};
    bool    fixed_width{false};
    bool    visible{true};
};


// FListView inline functions
//----------------------------------------------------------------------
inline auto FListView::getClassName() const -> FString
{ return "FListView"; }

//----------------------------------------------------------------------
inline auto FListView::getSortOrder() const -> SortOrder
{ return sorting.order; }

//----------------------------------------------------------------------
inline auto FListView::getSortColumn() const -> int
{ return sorting.column; }

//----------------------------------------------------------------------
inline auto FListView::getCurrentItem() -> FListViewItem*
{ return static_cast<FListViewItem*>(*selection.current_iter); }

//----------------------------------------------------------------------
template <typename Compare>
inline void FListView::setUserAscendingCompare (Compare cmp)
{ user_defined_ascending = cmp; }

//----------------------------------------------------------------------
template <typename Compare>
inline void FListView::setUserDescendingCompare (Compare cmp)
{ user_defined_descending = cmp; }

//----------------------------------------------------------------------
inline void FListView::hideSortIndicator (bool hide)
{ sorting.hide_sort_indicator = hide; }

//----------------------------------------------------------------------
inline void FListView::setTreeView (bool enable)
{ tree_view = enable; }

//----------------------------------------------------------------------
inline void FListView::unsetTreeView()
{ setTreeView(false); }

//----------------------------------------------------------------------
inline auto FListView::insert (FListViewItem* item) -> FObject::iterator
{ return insert (item, data.root); }

//----------------------------------------------------------------------
template <typename DT>
inline auto
    FListView::insert (const FStringList& cols, DT&& d) -> FObject::iterator
{ return insert (cols, std::forward<DT>(d), data.root); }

//----------------------------------------------------------------------
inline auto
    FListView::insert ( const FStringList& cols
                      , iterator parent_iter ) -> FObject::iterator
{ return insert (cols, nullptr, parent_iter); }

//----------------------------------------------------------------------
template <typename DT>
inline auto FListView::insert ( const FStringList& cols
                              , DT&& d
                              , iterator parent_iter ) -> FObject::iterator
{
  FListViewItem* item;

  if ( cols.empty() || parent_iter == getNullIterator() )
    return getNullIterator();

  if ( ! *parent_iter )
    parent_iter = data.root;

  try
  {
    item = new FListViewItem (cols, std::forward<DT>(d), getNullIterator());
  }
  catch (const std::bad_alloc&)
  {
    badAllocOutput ("FListViewItem");
    return getNullIterator();
  }

  item->replaceControlCodes();
  return insert(item, parent_iter);
}

//----------------------------------------------------------------------
template <typename T
        , typename DT>
inline auto
    FListView::insert (const std::initializer_list<T>& list, DT&& d) -> FObject::iterator
{ return insert (list, std::forward<DT>(d), data.root); }

//----------------------------------------------------------------------
template <typename T>
inline auto
    FListView::insert ( const std::initializer_list<T>& list
                      , iterator parent_iter ) -> FObject::iterator
{ return insert (list, 0, parent_iter); }

//----------------------------------------------------------------------
template <typename T
        , typename DT>
auto FListView::insert ( const std::initializer_list<T>& list
                       , DT&& d
                       , iterator parent_iter ) -> FObject::iterator
{
  FStringList str_cols;

  std::transform ( std::begin(list)
                 , std::end(list)
                 , std::back_inserter(str_cols)
                 , [] (const auto& col)
                   {
                     const FString s(FString() << col);
                     return s;
                   }
                 );

  auto item_iter = insert (str_cols, std::forward<DT>(d), parent_iter);
  return item_iter;
}

//----------------------------------------------------------------------
template <typename ColT
        , typename DT>
inline auto
    FListView::insert (const std::vector<ColT>& cols, DT&& d) -> FObject::iterator
{ return insert (cols, std::forward<DT>(d), data.root); }

//----------------------------------------------------------------------
template <typename ColT>
inline auto
    FListView::insert ( const std::vector<ColT>& cols
                      , iterator parent_iter ) -> FObject::iterator
{ return insert (cols, 0, parent_iter); }

//----------------------------------------------------------------------
template <typename ColT
        , typename DT>
auto
    FListView::insert ( const std::vector<ColT>& cols
                      , DT&& d
                      , iterator parent_iter ) -> FObject::iterator
{
  FStringList str_cols;

  std::transform ( std::begin(cols)
                 , std::end(cols)
                 , std::back_inserter(str_cols)
                 , [] (const auto& col)
                   {
                     const FString s(FString() << col);
                     return s;
                   }
                 );

  auto item_iter = insert (str_cols, std::forward<DT>(d), parent_iter);
  return item_iter;
}

//----------------------------------------------------------------------
inline auto FListView::getData() & -> FListViewItems&
{
  FObjectList* ptr = &data.itemlist;
  return *static_cast<FListViewItems*>(static_cast<void*>(ptr));
}

//----------------------------------------------------------------------
inline auto FListView::getData() const & -> const FListViewItems&
{
  const FObjectList* ptr = &data.itemlist;
  return *static_cast<const FListViewItems*>(static_cast<const void*>(ptr));
}

//----------------------------------------------------------------------
inline auto FListView::isHorizontallyScrollable() const -> bool
{ return max_line_width > getClientWidth(); }

//----------------------------------------------------------------------
inline auto FListView::isVerticallyScrollable() const -> bool
{ return getCount() > getClientHeight(); }

//----------------------------------------------------------------------
inline auto FListView::canSkipListDrawing() const -> bool
{ return isItemListEmpty() || getHeight() <= 2 || getWidth() <= 4; }

//----------------------------------------------------------------------
inline auto FListView::getColumnCount() const -> std::size_t
{ return data.header.size(); }

//----------------------------------------------------------------------
inline void FListView::toggleItemCheckState (FListViewItem* item) const
{ item->setChecked(! item->isChecked()); }

//----------------------------------------------------------------------
inline void FListView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline auto FListView::isItemListEmpty() const -> bool
{ return data.itemlist.empty(); }

//----------------------------------------------------------------------
inline auto FListView::isTreeView() const -> bool
{ return tree_view; }

//----------------------------------------------------------------------
inline auto FListView::isColumnIndexInvalid (int column) const -> bool
{
  return column < 1 || data.header.empty() || column > int(data.header.size());
}

//----------------------------------------------------------------------
inline auto FListView::hasCheckableItems() const -> bool
{ return has_checkable_items; }

}  // namespace finalcut

#endif  // FLISTVIEW_H
