/***********************************************************************
* flistview.h - Widget FListView and FListViewItem                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
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
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FListView ▏- - - -▕ FListViewItem ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FLISTVIEW_H
#define FLISTVIEW_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <list>
#include <stack>
#include <vector>

#include "final/fscrollbar.h"
#include "final/ftermbuffer.h"
#include "final/fwidget.h"

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
    FListViewItem (const FListViewItem&);  // copy constructor
    explicit FListViewItem (iterator);
    FListViewItem ( const FStringList&
                  , FDataPtr
                  , iterator );

    // Destructor
    ~FListViewItem() override;

    // Assignment operator (=)
    FListViewItem& operator = (const FListViewItem&);

    // Accessors
    const FString       getClassName() const override;
    uInt                getColumnCount() const;
    int                 getSortColumn() const;
    FString             getText (int) const;
    FDataPtr            getData() const;
    uInt                getDepth() const;

    // Mutators
    void                setText (int, const FString&);
    void                setData (FDataPtr);
    void                setCheckable (bool);
    void                setChecked (bool);

    // Inquiry
    bool                isChecked() const;
    bool                isExpand() const;

    // Methods
    iterator            insert (FListViewItem*);
    iterator            insert (FListViewItem*, iterator);
    void                remove (FListViewItem*);
    void                expand();
    void                collapse();

  private:
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
    FDataPtr            data_pointer{nullptr};
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
inline const FString FListViewItem::getClassName() const
{ return "FListViewItem"; }

//----------------------------------------------------------------------
inline uInt FListViewItem::getColumnCount() const
{ return uInt(column_list.size()); }

//----------------------------------------------------------------------
inline FDataPtr FListViewItem::getData() const
{ return data_pointer; }

//----------------------------------------------------------------------
inline void FListViewItem::setData (FDataPtr data)
{ data_pointer = data; }

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
    // Typedefs
    typedef std::list<FObject*>   FObjectList;
    typedef FObjectList::iterator iterator;
    typedef std::stack<iterator>  iterator_stack;

    // Constructor
    FListViewIterator () = default;
    FListViewIterator (iterator);

    // Overloaded operators
    FListViewIterator& operator ++ ();     // prefix
    FListViewIterator  operator ++ (int);  // postfix
    FListViewIterator& operator -- ();     // prefix
    FListViewIterator  operator -- (int);  // postfix
    FListViewIterator& operator += (volatile int);
    FListViewIterator& operator -= (volatile int);
    FObject*&          operator * () const;
    FObject*           operator -> () const;
    bool               operator == (const FListViewIterator&) const;
    bool               operator != (const FListViewIterator&) const;

    // Accessor
    const FString      getClassName() const;
    int&               getPosition();

    // Methods
    void               parentElement();

  private:
    // Methods
    void               nextElement (iterator&);
    void               prevElement (iterator&);

    // Data members
    iterator_stack     iter_path{};
    iterator           node{};
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
inline const FString FListViewIterator::getClassName() const
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

    // Constructor
    explicit FListView (FWidget* = nullptr);

    // Disable copy constructor
    FListView (const FListView&) = delete;

    // Destructor
    ~FListView() override;

    // Disable assignment operator (=)
    FListView& operator = (const FListView&) = delete;

    // Accessors
    const FString        getClassName() const override;
    std::size_t          getCount();
    fc::text_alignment   getColumnAlignment (int) const;
    FString              getColumnText (int) const;
    fc::sorting_type     getColumnSortType (int) const;
    fc::sorting_order    getSortOrder() const;
    int                  getSortColumn() const;
    FListViewItem*       getCurrentItem();

    // Mutators
    void                 setSize (const FSize&, bool = true) override;
    void                 setGeometry ( const FPoint&, const FSize&
                                     , bool = true ) override;
    void                 setColumnAlignment (int, fc::text_alignment);
    void                 setColumnText (int, const FString&);
    void                 setColumnSortType (int, fc::sorting_type \
                                                     = fc::by_name);
    void                 setColumnSort (int, fc::sorting_order \
                                                 = fc::ascending);
    template <typename Compare>
    void                 setUserAscendingCompare (Compare);
    template <typename Compare>
    void                 setUserDescendingCompare (Compare);
    void                 hideSortIndicator (bool);
    bool                 setTreeView (bool);
    bool                 setTreeView();
    bool                 unsetTreeView();

    // Methods
    virtual int          addColumn (const FString&, int = USE_MAX_SIZE);
    void                 hide() override;
    iterator             insert (FListViewItem*);
    iterator             insert (FListViewItem*, iterator);
    iterator             insert ( const FStringList&
                                , FDataPtr = nullptr );
    iterator             insert ( const FStringList&
                                , iterator );
    iterator             insert ( const FStringList&
                                , FDataPtr
                                , iterator );
    template <typename T>
    iterator             insert ( const std::initializer_list<T>&
                                , FDataPtr = nullptr );
    template <typename T>
    iterator             insert ( const std::initializer_list<T>&
                                , iterator );
    template <typename T>
    iterator             insert ( const std::initializer_list<T>&
                                , FDataPtr
                                , iterator );
    template <typename ColT>
    iterator             insert ( const std::vector<ColT>&
                                , FDataPtr = nullptr );
    template <typename ColT>
    iterator             insert ( const std::vector<ColT>&
                                , iterator );
    template <typename ColT>
    iterator             insert ( const std::vector<ColT>&
                                , FDataPtr
                                , iterator );
    void                 remove (FListViewItem*);
    void                 clear();
    iterator             beginOfList();
    iterator             endOfList();
    virtual void         sort();

    // Event handlers
    void                 onKeyPress (FKeyEvent*) override;
    void                 onMouseDown (FMouseEvent*) override;
    void                 onMouseUp (FMouseEvent*) override;
    void                 onMouseMove (FMouseEvent*) override;
    void                 onMouseDoubleClick (FMouseEvent*) override;
    void                 onWheel (FWheelEvent*) override;
    void                 onTimer (FTimerEvent*) override;
    void                 onFocusIn (FFocusEvent*) override;
    void                 onFocusOut (FFocusEvent*) override;

  protected:
    // Methods
    void                 adjustViewport (const int);
    void                 adjustScrollbars (const std::size_t);
    void                 adjustSize() override;

  private:
    // Typedefs
    typedef std::unordered_map<int, std::function<void()>> keyMap;
    typedef std::unordered_map<int, std::function<bool()>> keyMapResult;

    // Constants
    static constexpr std::size_t checkbox_space = 4;

    // Typedef
    struct Header;  // forward declaration
    typedef std::vector<Header> headerItems;
    typedef std::vector<fc::sorting_type> sortTypes;

    // Constants
    static constexpr int USE_MAX_SIZE = -1;

    // Accessors
    static iterator&     getNullIterator();

    // Mutators
    static void          setNullIterator (iterator&);

    // Inquiry
    bool                 isHorizontallyScrollable();
    bool                 isVerticallyScrollable();

    // Methods
    void                 init();
    void                 mapKeyFunctions();
    void                 processKeyAction (FKeyEvent*);
    template <typename Compare>
    void                 sort (Compare);
    std::size_t          getAlignOffset ( const fc::text_alignment
                                        , const std::size_t
                                        , const std::size_t );
    iterator             getListEnd (FListViewItem*);
    void                 draw() override;
    void                 drawBorder() override;
    void                 drawScrollbars();
    void                 drawHeadlines();
    void                 drawList();
    void                 drawListLine (const FListViewItem*, bool, bool);
    void                 clearList();
    void                 setLineAttributes (bool, bool);
    FString              getCheckBox (const FListViewItem* item);
    FString              getLinePrefix (const FListViewItem*, std::size_t);
    void                 drawSortIndicator (std::size_t&, std::size_t);
    void                 drawHeadlineLabel (const headerItems::const_iterator&);
    void                 drawHeaderBorder (std::size_t);
    void                 drawBufferedHeadline();
    void                 drawColumnEllipsis ( const headerItems::const_iterator&
                                            , const FString& );
    void                 updateDrawing (bool, bool);
    std::size_t          determineLineWidth (FListViewItem*);
    void                 beforeInsertion (FListViewItem*);
    void                 afterInsertion();
    void                 recalculateHorizontalBar (std::size_t);
    void                 recalculateVerticalBar (std::size_t);
    void                 mouseHeaderClicked();
    void                 wheelUp (int);
    void                 wheelDown (int);
    bool                 dragScrollUp (int);
    bool                 dragScrollDown (int);
    void                 dragUp (int);
    void                 dragDown (int);
    void                 stopDragScroll();
    iterator             appendItem (FListViewItem*);
    void                 processClick();
    void                 processChanged();
    void                 changeOnResize();
    void                 toggleCheckbox();
    void                 collapseAndScrollLeft();
    void                 expandAndScrollRight();
    void                 firstPos();
    void                 lastPos();
    bool                 expandSubtree();
    bool                 collapseSubtree();
    void                 setRelativePosition (int);
    void                 stepForward();
    void                 stepBackward();
    void                 stepForward (int);
    void                 stepBackward (int);
    void                 scrollToX (int);
    void                 scrollToY (int);
    void                 scrollTo (const FPoint &);
    void                 scrollTo (int, int);
    void                 scrollBy (int, int);
    bool                 hasCheckableItems() const;

    // Callback methods
    void                 cb_vbarChange (FWidget*, FDataPtr);
    void                 cb_hbarChange (FWidget*, FDataPtr);

    // Data members
    iterator             root{};
    FObjectList          selflist{};
    FObjectList          itemlist{};
    FListViewIterator    current_iter{};
    FListViewIterator    first_visible_line{};
    FListViewIterator    last_visible_line{};
    headerItems          header{};
    FTermBuffer          headerline{};
    FScrollbarPtr        vbar{nullptr};
    FScrollbarPtr        hbar{nullptr};
    sortTypes            sort_type{};
    FPoint               clicked_expander_pos{-1, -1};
    FPoint               clicked_header_pos{-1, -1};
    keyMap               key_map{};
    keyMapResult         key_map_result{};
    const FListViewItem* clicked_checkbox_item{nullptr};
    std::size_t          nf_offset{0};
    std::size_t          max_line_width{1};
    fc::dragScroll       drag_scroll{fc::noScroll};
    int                  first_line_position_before{-1};
    int                  scroll_repeat{100};
    int                  scroll_distance{1};
    int                  xoffset{0};
    int                  sort_column{-1};
    fc::sorting_order    sort_order{fc::unsorted};
    bool                 scroll_timer{false};
    bool                 tree_view{false};
    bool                 hide_sort_indicator{false};
    bool                 has_checkable_items{false};

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
    fc::text_alignment alignment{fc::alignLeft};
    int width{0};
    bool fixed_width{false};
};


// FListView inline functions
//----------------------------------------------------------------------
inline const FString FListView::getClassName() const
{ return "FListView"; }

//----------------------------------------------------------------------
inline fc::sorting_order FListView::getSortOrder() const
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
inline bool FListView::setTreeView()
{ return setTreeView(true); }

//----------------------------------------------------------------------
inline bool FListView::unsetTreeView()
{ return setTreeView(false); }

//----------------------------------------------------------------------
inline FObject::iterator FListView::insert (FListViewItem* item)
{ return insert (item, root); }

//----------------------------------------------------------------------
inline FObject::iterator
    FListView::insert (const FStringList& cols, FDataPtr d)
{ return insert (cols, d, root); }

//----------------------------------------------------------------------
inline FObject::iterator
    FListView::insert ( const FStringList& cols
                      , iterator parent_iter )
{ return insert (cols, nullptr, parent_iter); }

//----------------------------------------------------------------------
template<typename T>
inline FObject::iterator
    FListView::insert (const std::initializer_list<T>& list, FDataPtr d)
{ return insert (list, d, root); }

//----------------------------------------------------------------------
template<typename T>
inline FObject::iterator
    FListView::insert ( const std::initializer_list<T>& list
                      , iterator parent_iter )
{ return insert (list, 0, parent_iter); }

//----------------------------------------------------------------------
template<typename T>
FObject::iterator
    FListView::insert ( const std::initializer_list<T>& list
                      , FDataPtr d
                      , iterator parent_iter )
{
  FStringList str_cols;

  std::transform ( std::begin(list)
                 , std::end(list)
                 , std::back_inserter(str_cols)
                 , [] (const T& col) -> const FString
                   {
                     const FString s(FString() << col);
                     return s;
                   }
                 );

  auto item_iter = insert (str_cols, d, parent_iter);
  return item_iter;
}

//----------------------------------------------------------------------
template <typename ColT>
inline FObject::iterator
    FListView::insert (const std::vector<ColT>& cols, FDataPtr d)
{ return insert (cols, d, root); }

//----------------------------------------------------------------------
template <typename ColT>
inline FObject::iterator
    FListView::insert ( const std::vector<ColT>& cols
                      , iterator parent_iter )
{ return insert (cols, 0, parent_iter); }

//----------------------------------------------------------------------
template <typename ColT>
FObject::iterator
    FListView::insert ( const std::vector<ColT>& cols
                      , FDataPtr d
                      , iterator parent_iter )
{
  FStringList str_cols;

  std::transform ( std::begin(cols)
                 , std::end(cols)
                 , std::back_inserter(str_cols)
                 , [] (const ColT& col) -> const FString
                   {
                     const FString s(FString() << col);
                     return s;
                   }
                 );

  auto item_iter = insert (str_cols, d, parent_iter);
  return item_iter;
}

//----------------------------------------------------------------------
inline FObject::iterator FListView::beginOfList()
{ return itemlist.begin(); }

//----------------------------------------------------------------------
inline FObject::iterator FListView::endOfList()
{ return itemlist.end(); }

//----------------------------------------------------------------------
inline bool FListView::isHorizontallyScrollable()
{ return bool( max_line_width > getClientWidth() ); }

//----------------------------------------------------------------------
inline bool FListView::isVerticallyScrollable()
{ return bool( getCount() > getClientHeight() ); }

//----------------------------------------------------------------------
inline void FListView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline bool FListView::hasCheckableItems() const
{ return has_checkable_items; }

}  // namespace finalcut

#endif  // FLISTVIEW_H
