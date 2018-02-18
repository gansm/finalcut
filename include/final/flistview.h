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

#include <stack>
#include <vector>

#include "final/fscrollbar.h"
#include "final/fstring.h"
#include "final/ftermbuffer.h"
#include "final/fwidget.h"

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
    ~FListViewItem();

    // Assignment operator (=)
    FListViewItem& operator = (const FListViewItem&);

    // Accessors
    const char*       getClassName() const;
    uInt              getColumnCount() const;
    FString           getText (int) const;
    uInt              getDepth() const;

    // Mutator
    void              setText (int, const FString&);


    // Inquiry
    bool              isExpand() const;

    // Methods
    FObjectIterator   insert (FListViewItem*);
    FObjectIterator   insert (FListViewItem*, FObjectIterator);
    void              expand();
    void              collapse();

  private:
    // Inquiry
    bool              isExpandable() const;

    // Methods
    FObjectIterator   appendItem (FListViewItem*);
    void              replaceControlCodes();
    int               getVisibleLines();
    void              resetVisibleLineCounter();

    // Data Members
    FStringList       column_list;
    FWidget::data_ptr data_pointer;
    int               visible_lines;
    bool              expandable;
    bool              is_expand;

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
inline bool FListViewItem::isExpand() const
{ return is_expand; }

//----------------------------------------------------------------------
inline bool FListViewItem::isExpandable() const
{ return expandable; }


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

    // Destructor
    ~FListViewIterator();

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
    ~FListView();

    // Accessors
    const char*          getClassName() const;
    uInt                 getCount();
    fc::text_alignment   getColumnAlignment (int) const;
    FString              getColumnText (int) const;
    FListViewItem*       getCurrentItem();

    // Mutators
    void                 setGeometry (int, int, int, int, bool = true);
    void                 setColumnAlignment (int, fc::text_alignment);
    void                 setColumnText (int, const FString&);
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

    // Event handlers
    void                 onKeyPress (FKeyEvent*);
    void                 onMouseDown (FMouseEvent*);
    void                 onMouseUp (FMouseEvent*);
    void                 onMouseMove (FMouseEvent*);
    void                 onMouseDoubleClick (FMouseEvent*);
    void                 onWheel (FWheelEvent*);
    void                 onTimer (FTimerEvent*);
    void                 onFocusIn (FFocusEvent*);
    void                 onFocusOut (FFocusEvent*);

    // Data Members
    static FObjectIterator null_iter;

  protected:
    // Methods
    void                 adjustViewport();
    void                 adjustSize();

  private:
    // Typedef
    struct Header;  // forward declaration
    typedef std::vector<Header> headerItems;

    // Constants
    static const int USE_MAX_SIZE = -1;

    // Disable copy constructor
    FListView (const FListView&);

    // Disable assignment operator (=)
    FListView& operator = (const FListView&);

    // Methods
    void                 init();
    uInt                 getAlignOffset (fc::text_alignment, uInt, uInt);
    void                 draw();
    void                 drawColumnLabels();
    void                 drawList();
    void                 drawListLine (const FListViewItem*, bool, bool);
    void                 setLineAttributes (bool, bool);
    FString              getLinePrefix (const FListViewItem*, uInt);
    void                 drawColumnText (headerItems::const_iterator&);
    void                 drawColumnEllipsis ( headerItems::const_iterator&
                                            , const FString& );
    void                 updateDrawing (bool, bool);
    void                 recalculateHorizontalBar (int);
    void                 recalculateVerticalBar (int);
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
    FPoint               clicked_expander_pos;
    int                  xoffset;
    int                  nf_offset;
    int                  max_line_width;

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

    ~Header()
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
inline FListViewItem* FListView::getCurrentItem()
{ return static_cast<FListViewItem*>(*current_iter); }

//----------------------------------------------------------------------
inline bool FListView::setTreeView (bool on)
{ return tree_view = ( on ) ? true : false; }

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
  FListView::insert ( const FStringList& cols, data_ptr d )
{ return insert (cols, d, root); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator
  FListView::insert ( const FStringList& cols
                    , FObjectIterator parent_iter )
{ return insert (cols, 0, parent_iter); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator
  FListView::insert ( const std::vector<long>& cols, data_ptr d )
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

#endif  // FLISTVIEW_H
