/***********************************************************************
* flistview.h - Widget FListView and FListViewItem                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2021 Markus Gans                                      *
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
    FListViewItem& operator = (const FListViewItem&);

    // Accessors
    FString             getClassName() const override;
    uInt                getColumnCount() const;
    int                 getSortColumn() const;
    FString             getText (int) const;
    template <typename DT>
    clean_fdata_t<DT>&  getData() const;
    uInt                getDepth() const;

    // Mutators
    void                setText (int, const FString&);
    template <typename DT>
    void                setData (DT&&);
    void                setCheckable (bool = true);
    void                setChecked (bool = true);

    // Inquiry
    bool                isChecked() const;
    bool                isExpand() const;

    // Methods
    iterator            insert (FListViewItem*);
    iterator            insert (FListViewItem*, iterator) const;
    void                remove (FListViewItem*) const;
    void                expand();
    void                collapse();

  private:
    // Using-declaration
    using FDataAccessPtr = std::shared_ptr<FDataAccess>;

    // Inquiry
    bool                isExpandable() const;
    bool                isCheckable() const;

    // Methods
    template <typename Compare>
    void                sort (Compare);
    iterator            appendItem (FListViewItem*);
    void                replaceControlCodes();
    std::size_t         getVisibleLines();
    void                resetVisibleLineCounter();

    // Data members
    FStringList         column_list{};
    FDataAccessPtr      data_pointer{};
    iterator            root{};
    std::size_t         visible_lines{1};
    bool                expandable{false};
    bool                is_expand{false};
    bool                checkable{false};
    bool                is_checked{false};

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
inline FString FListViewItem::getClassName() const
{ return "FListViewItem"; }

//----------------------------------------------------------------------
inline uInt FListViewItem::getColumnCount() const
{ return static_cast<uInt>(column_list.size()); }

//----------------------------------------------------------------------
template <typename DT>
inline clean_fdata_t<DT>& FListViewItem::getData() const
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
inline bool FListViewItem::isChecked() const
{ return is_checked; }

//----------------------------------------------------------------------
inline bool FListViewItem::isExpand() const
{ return is_expand; }

//----------------------------------------------------------------------
inline bool FListViewItem::isExpandable() const
{ return expandable; }

//----------------------------------------------------------------------
inline bool FListViewItem::isCheckable() const
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
    FListViewIterator (Iterator);
    FListViewIterator (const FListViewIterator&) = default;
    FListViewIterator (FListViewIterator&& i) noexcept
      : iter_path{std::move(i.iter_path)}
      , node{i.node}
      , position{i.position}
    { }

    // Overloaded operators
    FListViewIterator& operator = (const FListViewIterator&) = default;
    FListViewIterator& operator = (FListViewIterator&&) noexcept = default;
    FListViewIterator& operator ++ ();     // prefix
    FListViewIterator  operator ++ (int);  // postfix
    FListViewIterator& operator -- ();     // prefix
    FListViewIterator  operator -- (int);  // postfix
    FListViewIterator& operator += (int);
    FListViewIterator& operator -= (int);
    FObject*&          operator * () const;
    FObject*           operator -> () const;
    bool               operator == (const FListViewIterator&) const;
    bool               operator != (const FListViewIterator&) const;

    // Accessor
    FString            getClassName() const;
    int&               getPosition();

    // Methods
    void               parentElement();

  private:
    // Methods
    void               nextElement (Iterator&);
    void               prevElement (Iterator&);

    // Data members
    IteratorStack      iter_path{};
    Iterator           node{};
    int                position{0};
};


// FListViewIterator inline functions
//----------------------------------------------------------------------
inline FObject*& FListViewIterator::operator * () const
{ return *node; }

//----------------------------------------------------------------------
inline FObject* FListViewIterator::operator -> () const
{ return *node; }

//----------------------------------------------------------------------
inline bool FListViewIterator::operator == (const FListViewIterator& rhs) const
{ return node == rhs.node; }

//----------------------------------------------------------------------
inline bool FListViewIterator::operator != (const FListViewIterator& rhs) const
{ return node != rhs.node; }

//----------------------------------------------------------------------
inline FString FListViewIterator::getClassName() const
{ return "FListViewIterator"; }

//----------------------------------------------------------------------
inline int& FListViewIterator::getPosition()
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
    FListView& operator = (const FListView&) = delete;

    // Disable move assignment operator (=)
    FListView& operator = (FListView&&) noexcept = delete;

    // Accessors
    FString               getClassName() const override;
    std::size_t           getCount() const;
    Align                 getColumnAlignment (int) const;
    FString               getColumnText (int) const;
    SortType              getColumnSortType (int) const;
    SortOrder             getSortOrder() const;
    int                   getSortColumn() const;
    FListViewItem*        getCurrentItem();

    // Mutators
    void                  setSize (const FSize&, bool = true) override;
    void                  setGeometry ( const FPoint&, const FSize&
                                      , bool = true ) override;
    void                  setColumnAlignment (int, Align);
    void                  setColumnText (int, const FString&);
    void                  setColumnSortType (int, SortType = SortType::Name);
    void                  setColumnSort (int, SortOrder = SortOrder::Ascending);
    template <typename Compare>
    void                  setUserAscendingCompare (Compare);
    template <typename Compare>
    void                  setUserDescendingCompare (Compare);
    void                  hideSortIndicator (bool = true);
    bool                  setTreeView (bool = true);
    bool                  unsetTreeView();

    // Methods
    virtual int           addColumn (const FString&, int = USE_MAX_SIZE);
    void                  hide() override;
    iterator              insert (FListViewItem*);
    iterator              insert (FListViewItem*, iterator);
    template <typename DT = std::nullptr_t>
    iterator              insert ( const FStringList&
                                 , DT&& = DT() );
    iterator              insert ( const FStringList&
                                 , iterator );
    template <typename DT>
    iterator              insert ( const FStringList&
                                 , DT&&
                                 , iterator );
    template <typename T
            , typename DT = std::nullptr_t>
    iterator              insert ( const std::initializer_list<T>&
                                 , DT&& = DT() );
    template <typename T>
    iterator              insert ( const std::initializer_list<T>&
                                 , iterator );
    template <typename T
            , typename DT>
    iterator              insert ( const std::initializer_list<T>&
                                 , DT&&
                                 , iterator );
    template <typename ColT
            , typename DT = std::nullptr_t>
    iterator              insert ( const std::vector<ColT>&
                                 , DT&& = DT() );
    template <typename ColT>
    iterator              insert ( const std::vector<ColT>&
                                 , iterator );
    template <typename ColT
            , typename DT>
    iterator              insert ( const std::vector<ColT>&
                                 , DT&&
                                 , iterator );
    void                  remove (FListViewItem*);
    void                  clear();
    FListViewItems&       getData() &;
    const FListViewItems& getData() const &;

    virtual void          sort();

    // Event handlers
    void                  onKeyPress (FKeyEvent*) override;
    void                  onMouseDown (FMouseEvent*) override;
    void                  onMouseUp (FMouseEvent*) override;
    void                  onMouseMove (FMouseEvent*) override;
    void                  onMouseDoubleClick (FMouseEvent*) override;
    void                  onWheel (FWheelEvent*) override;
    void                  onTimer (FTimerEvent*) override;
    void                  onFocusIn (FFocusEvent*) override;
    void                  onFocusOut (FFocusEvent*) override;

  protected:
    // Methods
    void                  initLayout() override;
    void                  adjustViewport (const int);
    void                  adjustScrollbars (const std::size_t) const;
    void                  adjustSize() override;

  private:
    struct Header;  // forward declaration

    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;
    using KeyMapResult = std::unordered_map<FKey, std::function<bool()>, EnumHash<FKey>>;
    using HeaderItems = std::vector<Header>;
    using SortTypes = std::vector<SortType>;

    // Constants
    static constexpr std::size_t checkbox_space = 4;

    // Constants
    static constexpr int USE_MAX_SIZE = -1;

    // Accessors
    static iterator&      getNullIterator();

    // Mutators
    static void           setNullIterator (const iterator&);

    // Inquiry
    bool                  isHorizontallyScrollable() const;
    bool                  isVerticallyScrollable() const;

    // Methods
    void                  init();
    void                  mapKeyFunctions();
    void                  processKeyAction (FKeyEvent*);
    template <typename Compare>
    void                  sort (Compare);
    std::size_t           getAlignOffset ( const Align
                                         , const std::size_t
                                         , const std::size_t ) const;
    iterator              getListEnd (const FListViewItem*);
    void                  draw() override;
    void                  drawBorder() override;
    void                  drawScrollbars() const;
    void                  drawHeadlines();
    void                  drawList();
    void                  drawListLine (const FListViewItem*, bool, bool);
    void                  clearList();
    void                  setLineAttributes (bool, bool) const;
    FString               getCheckBox (const FListViewItem* item) const;
    FString               getLinePrefix (const FListViewItem*, std::size_t) const;
    void                  drawSortIndicator (std::size_t&, std::size_t);
    void                  drawHeadlineLabel (const HeaderItems::const_iterator&);
    void                  drawHeaderBorder (std::size_t);
    void                  drawBufferedHeadline();
    void                  drawColumnEllipsis ( const HeaderItems::const_iterator&
                                             , const FString& );
    void                  updateDrawing (bool, bool);
    std::size_t           determineLineWidth (FListViewItem*);
    void                  beforeInsertion (FListViewItem*);
    void                  afterInsertion();
    void                  recalculateHorizontalBar (std::size_t);
    void                  recalculateVerticalBar (std::size_t) const;
    void                  mouseHeaderClicked();
    void                  wheelUp (int);
    void                  wheelDown (int);
    bool                  dragScrollUp (int);
    bool                  dragScrollDown (int);
    void                  dragUp (MouseButton);
    void                  dragDown (MouseButton);
    void                  stopDragScroll();
    iterator              appendItem (FListViewItem*);
    void                  processClick() const;
    void                  processRowChanged() const;
    void                  processChanged() const;
    void                  changeOnResize() const;
    void                  toggleCheckbox();
    void                  collapseAndScrollLeft();
    void                  expandAndScrollRight();
    void                  firstPos();
    void                  lastPos();
    bool                  expandSubtree();
    bool                  collapseSubtree();
    void                  setRelativePosition (int);
    void                  stepForward();
    void                  stepBackward();
    void                  stepForward (int);
    void                  stepBackward (int);
    void                  scrollToX (int);
    void                  scrollToY (int);
    void                  scrollTo (const FPoint &);
    void                  scrollTo (int, int);
    void                  scrollBy (int, int);
    bool                  hasCheckableItems() const;

    // Callback methods
    void                  cb_vbarChange (const FWidget*);
    void                  cb_hbarChange (const FWidget*);

    // Data members
    iterator              root{};
    FObjectList           selflist{};
    FObjectList           itemlist{};
    FListViewIterator     current_iter{};
    FListViewIterator     first_visible_line{};
    FListViewIterator     last_visible_line{};
    HeaderItems           header{};
    FVTermBuffer          headerline{};
    FScrollbarPtr         vbar{nullptr};
    FScrollbarPtr         hbar{nullptr};
    SortTypes             sort_type{};
    FPoint                clicked_expander_pos{-1, -1};
    FPoint                clicked_header_pos{-1, -1};
    KeyMap                key_map{};
    KeyMapResult          key_map_result{};
    const FListViewItem*  clicked_checkbox_item{nullptr};
    std::size_t           nf_offset{0};
    std::size_t           max_line_width{1};
    DragScrollMode        drag_scroll{DragScrollMode::None};
    int                   first_line_position_before{-1};
    int                   scroll_repeat{100};
    int                   scroll_distance{1};
    int                   xoffset{0};
    int                   sort_column{-1};
    SortOrder             sort_order{SortOrder::Unsorted};
    bool                  scroll_timer{false};
    bool                  tree_view{false};
    bool                  hide_sort_indicator{false};
    bool                  has_checkable_items{false};

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
    Header () = default;

    FString name{};
    Align alignment{Align::Left};
    int width{0};
    bool fixed_width{false};
};


// FListView inline functions
//----------------------------------------------------------------------
inline FString FListView::getClassName() const
{ return "FListView"; }

//----------------------------------------------------------------------
inline SortOrder FListView::getSortOrder() const
{ return sort_order; }

//----------------------------------------------------------------------
inline int FListView::getSortColumn() const
{ return sort_column; }

//----------------------------------------------------------------------
inline FListViewItem* FListView::getCurrentItem()
{ return static_cast<FListViewItem*>(*current_iter); }

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
{ hide_sort_indicator = hide; }

//----------------------------------------------------------------------
inline bool FListView::setTreeView (bool enable)
{ return (tree_view = enable); }

//----------------------------------------------------------------------
inline bool FListView::unsetTreeView()
{ return setTreeView(false); }

//----------------------------------------------------------------------
inline FObject::iterator FListView::insert (FListViewItem* item)
{ return insert (item, root); }

//----------------------------------------------------------------------
template <typename DT>
inline FObject::iterator
    FListView::insert (const FStringList& cols, DT&& d)
{ return insert (cols, std::forward<DT>(d), root); }

//----------------------------------------------------------------------
inline FObject::iterator
    FListView::insert ( const FStringList& cols
                      , iterator parent_iter )
{ return insert (cols, nullptr, parent_iter); }

//----------------------------------------------------------------------
template <typename DT>
inline FObject::iterator FListView::insert ( const FStringList& cols
                                           , DT&& d
                                           , iterator parent_iter )
{
  FListViewItem* item;

  if ( cols.empty() || parent_iter == getNullIterator() )
    return getNullIterator();

  if ( ! *parent_iter )
    parent_iter = root;

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
inline FObject::iterator
    FListView::insert (const std::initializer_list<T>& list, DT&& d)
{ return insert (list, std::forward<DT>(d), root); }

//----------------------------------------------------------------------
template <typename T>
inline FObject::iterator
    FListView::insert ( const std::initializer_list<T>& list
                      , iterator parent_iter )
{ return insert (list, 0, parent_iter); }

//----------------------------------------------------------------------
template <typename T
        , typename DT>
FObject::iterator
    FListView::insert ( const std::initializer_list<T>& list
                      , DT&& d
                      , iterator parent_iter )
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
inline FObject::iterator
    FListView::insert (const std::vector<ColT>& cols, DT&& d)
{ return insert (cols, std::forward<DT>(d), root); }

//----------------------------------------------------------------------
template <typename ColT>
inline FObject::iterator
    FListView::insert ( const std::vector<ColT>& cols
                      , iterator parent_iter )
{ return insert (cols, 0, parent_iter); }

//----------------------------------------------------------------------
template <typename ColT
        , typename DT>
FObject::iterator
    FListView::insert ( const std::vector<ColT>& cols
                      , DT&& d
                      , iterator parent_iter )
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
inline FListView::FListViewItems& FListView::getData() &
{ return reinterpret_cast<FListViewItems&>(itemlist); }

//----------------------------------------------------------------------
inline const FListView::FListViewItems& FListView::getData() const &
{ return reinterpret_cast<const FListViewItems&>(itemlist); }

//----------------------------------------------------------------------
inline bool FListView::isHorizontallyScrollable() const
{ return max_line_width > getClientWidth(); }

//----------------------------------------------------------------------
inline bool FListView::isVerticallyScrollable() const
{ return getCount() > getClientHeight(); }

//----------------------------------------------------------------------
inline void FListView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline bool FListView::hasCheckableItems() const
{ return has_checkable_items; }

}  // namespace finalcut

#endif  // FLISTVIEW_H
