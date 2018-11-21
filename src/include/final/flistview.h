/***********************************************************************
* flistview.h - Widget FListView and FListViewItem                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
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
                          #include "final/fmessagebox.h"
#include "final/fscrollbar.h"
#include "final/fstring.h"
#include "final/ftermbuffer.h"
#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FListView;

//----------------------------------------------------------------------
// class FListViewItem
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FListViewItem : public FObject
{
  public:
    // Constructor
    FListViewItem (const FListViewItem&);  // copy constructor
    explicit FListViewItem (FObjectIterator);
    FListViewItem ( const FStringList&
                  , FWidget::data_ptr
                  , FObjectIterator );

    // Destructor
    virtual ~FListViewItem();

    // Assignment operator (=)
    FListViewItem& operator = (const FListViewItem&);

    // Accessors
    const char*       getClassName() const;
    uInt              getColumnCount() const;
    int               getSortColumn() const;
    FString           getText (int) const;
    FWidget::data_ptr getData() const;
    uInt              getDepth() const;

    // Mutators
    void              setText (int, const FString&);
    void              setData (FWidget::data_ptr);
    void              setCheckable (bool);
    void              setChecked (bool);

    // Inquiry
    bool              isChecked() const;
    bool              isExpand() const;

    // Methods
    FObjectIterator   insert (FListViewItem*);
    FObjectIterator   insert (FListViewItem*, FObjectIterator);
    void              expand();
    void              collapse();

  private:
    // Inquiry
    bool              isExpandable() const;
    bool              isCheckable() const;

    // Methods
    template <typename Compare>
    void              sort (Compare);
    FObjectIterator   appendItem (FListViewItem*);
    void              replaceControlCodes();
    std::size_t       getVisibleLines();
    void              resetVisibleLineCounter();

    // Data Members
    FStringList       column_list;
    FWidget::data_ptr data_pointer;
    FObjectIterator   root;
    std::size_t       visible_lines;
    bool              expandable;
    bool              is_expand;
    bool              checkable;
    bool              is_checked;

    // Friend class
    friend class FListView;
    friend class FListViewIterator;
};
#pragma pack(pop)


// FListViewItem inline functions
//----------------------------------------------------------------------
inline const char* FListViewItem::getClassName() const
{ return "FListViewItem"; }

//----------------------------------------------------------------------
inline uInt FListViewItem::getColumnCount() const
{ return uInt(column_list.size()); }

//----------------------------------------------------------------------
inline FWidget::data_ptr FListViewItem::getData() const
{ return data_pointer; }

//----------------------------------------------------------------------
inline void FListViewItem::setData (FWidget::data_ptr data)
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

#pragma pack(push)
#pragma pack(1)

class FListViewIterator
{
  public:
    // Typedefs
    typedef std::list<FObject*>         FObjectList;
    typedef FObjectList::iterator       FObjectIterator;
    typedef std::stack<FObjectIterator> FObjectIteratorStack;

    // Constructor
    FListViewIterator ();
    FListViewIterator (FObjectIterator);

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
    const char*        getClassName() const;
    int                getPosition() const;

    // Methods
    void parentElement();

  private:
    // Methods
    void nextElement (FObjectIterator&);
    void prevElement (FObjectIterator&);

    // Data Members
    FObjectIteratorStack iter_path;
    FObjectIterator      node;
    int                  position;
};
#pragma pack(pop)


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
inline const char* FListViewIterator::getClassName() const
{ return "FListViewIterator"; }

//----------------------------------------------------------------------
inline int FListViewIterator::getPosition() const
{ return position; }

//----------------------------------------------------------------------
// class FListView
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FListView : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Constructor
    explicit FListView (FWidget* = 0);

    // Destructor
    virtual ~FListView();

    // Accessors
    const char*          getClassName() const;
    std::size_t          getCount();
    fc::text_alignment   getColumnAlignment (int) const;
    FString              getColumnText (int) const;
    fc::sorting_type     getColumnSortType (int) const;
    fc::sorting_order    getSortOrder() const;
    int                  getSortColumn() const;
    FListViewItem*       getCurrentItem();

    // Mutators
    virtual void         setGeometry ( int, int
                                     , std::size_t, std::size_t
                                     , bool = true );
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
    FObjectIterator      insert (FListViewItem*);
    FObjectIterator      insert (FListViewItem*, FObjectIterator);
    FObjectIterator      insert ( const FStringList&
                                , data_ptr = 0 );
    FObjectIterator      insert ( const FStringList&
                                , FObjectIterator );
    FObjectIterator      insert ( const FStringList&
                                , data_ptr
                                , FObjectIterator );
    FObjectIterator      insert ( const std::vector<long>&
                                , data_ptr = 0 );
    FObjectIterator      insert ( const std::vector<long>&
                                , FObjectIterator );
    FObjectIterator      insert ( const std::vector<long>&
                                , data_ptr
                                , FObjectIterator );
    FObjectIterator      beginOfList();
    FObjectIterator      endOfList();
    virtual void         sort();

    // Event handlers
    virtual void         onKeyPress (FKeyEvent*);
    virtual void         onMouseDown (FMouseEvent*);
    virtual void         onMouseUp (FMouseEvent*);
    virtual void         onMouseMove (FMouseEvent*);
    virtual void         onMouseDoubleClick (FMouseEvent*);
    virtual void         onWheel (FWheelEvent*);
    virtual void         onTimer (FTimerEvent*);
    virtual void         onFocusIn (FFocusEvent*);
    virtual void         onFocusOut (FFocusEvent*);

    // Data Members
    static FObjectIterator null_iter;

  protected:
    // Methods
    void                 adjustViewport();
    virtual void         adjustSize();

  private:
    // Typedef
    struct Header;  // forward declaration
    typedef std::vector<Header> headerItems;
    typedef std::vector<fc::sorting_type> sortTypes;

    // Constants
    static const int USE_MAX_SIZE = -1;

    // Disable copy constructor
    FListView (const FListView&);

    // Disable assignment operator (=)
    FListView& operator = (const FListView&);

    // Methods
    void                 init();
    template <typename Compare>
    void                 sort (Compare);
    std::size_t          getAlignOffset ( fc::text_alignment
                                        , std::size_t
                                        , std::size_t );
    virtual void         draw();
    void                 drawHeadlines();
    void                 drawList();
    void                 drawListLine (const FListViewItem*, bool, bool);
    void                 setLineAttributes (bool, bool);
    FString              getCheckBox (const FListViewItem* item);
    FString              getLinePrefix (const FListViewItem*, std::size_t);
    void                 drawSortIndicator (std::size_t&, std::size_t);
    void                 drawHeadlineLabel (headerItems::const_iterator&);
    void                 drawHeaderBorder (std::size_t);
    void                 drawColumnEllipsis ( headerItems::const_iterator&
                                            , const FString& );
    void                 updateDrawing (bool, bool);
    int                  determineLineWidth (FListViewItem*);
    void                 beforeInsertion (FListViewItem*);
    void                 afterInsertion();
    void                 recalculateHorizontalBar (int);
    void                 recalculateVerticalBar (int);
    void                 mouseHeaderClicked();
    void                 wheelUp (int);
    void                 wheelDown (int);
    bool                 dragScrollUp (int);
    bool                 dragScrollDown (int);
    void                 dragUp (int);
    void                 dragDown (int);
    void                 stopDragScroll();
    FObjectIterator      appendItem (FListViewItem*);
    void                 processClick();
    void                 processChanged();
    void                 keySpace();
    void                 keyLeft (int&);
    void                 keyRight (int&);
    void                 keyHome();
    void                 keyEnd();
    bool                 keyPlus();
    bool                 keyMinus();
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
    void                 cb_VBarChange (FWidget*, data_ptr);
    void                 cb_HBarChange (FWidget*, data_ptr);

    // Data Members
    FObjectIterator      root;
    FObjectList          selflist;
    FObjectList          itemlist;
    FListViewIterator    current_iter;
    FListViewIterator    first_visible_line;
    FListViewIterator    last_visible_line;
    headerItems          header;
    FTermBuffer          headerline;
    FScrollbar*          vbar;
    FScrollbar*          hbar;
    fc::dragScroll       drag_scroll;
    int                  scroll_repeat;
    int                  scroll_distance;
    bool                 scroll_timer;
    bool                 tree_view;
    bool                 hide_sort_indicator;
    bool                 has_checkable_items;
    FPoint               clicked_expander_pos;
    FPoint               clicked_header_pos;
    const FListViewItem* clicked_checkbox_item;
    int                  xoffset;
    int                  nf_offset;
    int                  max_line_width;
    int                  sort_column;
    sortTypes            sort_type;
    fc::sorting_order    sort_order;
    bool (*user_defined_ascending) (const FObject*, const FObject*);
    bool (*user_defined_descending) (const FObject*, const FObject*);

    // Friend class
    friend class FListViewItem;
};
#pragma pack(pop)


//----------------------------------------------------------------------
// struct FListView::Header
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)
struct FListView::Header
{
  public:
    Header()
      : name()
      , width (0)
      , fixed_width (false)
      , alignment (fc::alignLeft)
    { }

    FString name;
    int width;
    bool fixed_width;
    fc::text_alignment alignment;
};
#pragma pack(pop)


// FListView inline functions
//----------------------------------------------------------------------
inline const char* FListView::getClassName() const
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
inline bool FListView::setTreeView (bool on)
{ return (tree_view = on); }

//----------------------------------------------------------------------
inline bool FListView::setTreeView()
{ return setTreeView(true); }

//----------------------------------------------------------------------
inline bool FListView::unsetTreeView()
{ return setTreeView(false); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator FListView::insert (FListViewItem* item)
{ return insert (item, root); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator
    FListView::insert (const FStringList& cols, data_ptr d)
{ return insert (cols, d, root); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator
    FListView::insert ( const FStringList& cols
                      , FObjectIterator parent_iter )
{ return insert (cols, 0, parent_iter); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator
    FListView::insert (const std::vector<long>& cols, data_ptr d)
{ return insert (cols, d, root); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator
    FListView::insert ( const std::vector<long>& cols
                      , FObjectIterator parent_iter )
{ return insert (cols, 0, parent_iter); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator FListView::beginOfList()
{ return itemlist.begin(); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator FListView::endOfList()
{ return itemlist.end(); }

//----------------------------------------------------------------------
inline void FListView::scrollTo (const FPoint& pos)
{ scrollTo(pos.getX(), pos.getY()); }

//----------------------------------------------------------------------
inline bool FListView::hasCheckableItems() const
{ return has_checkable_items; }

}  // namespace finalcut

#endif  // FLISTVIEW_H
