// File: flistview.h
// Provides: class FListViewItem
//           class FListView
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FVTerm ▏
//       ▕▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏           ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏           ▕ FObject ▏
//       ▕▁▁▁▁▁▁▁▁▁▏           ▕▁▁▁▁▁▁▁▁▁▏
//            ▲                     ▲
//            │                     │
//      ▕▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//      ▕ FListView ▏- - - -▕ FListViewItem ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
//

#ifndef FLISTVIEW_H
#define FLISTVIEW_H

#include <vector>

#include "fscrollbar.h"
#include "fstring.h"
#include "ftermbuffer.h"
#include "fwidget.h"

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
    explicit FListViewItem (FListViewItem*);
    explicit FListViewItem (FListView*);
    explicit FListViewItem (FObjectIterator);
    FListViewItem ( const std::vector<FString>&
                  , FWidget::data_ptr
                  , FObjectIterator );

    // Destructor
    ~FListViewItem();

    // Assignment operator (=)
    FListViewItem& operator = (const FListViewItem&);

    // Accessors
    const char*      getClassName() const;
    uInt             getColumnCount() const;
    FString          getText (int) const;

    // Mutator
    void             setText (int, const FString&);

    // Inquiry
    bool             isExpand();

    // Methods
    FObjectIterator  insert (FListViewItem*);
    FObjectIterator  insert (FListViewItem*, FObjectIterator);
    void             expand();
    void             collapse();

  private:
    // Inquiry
    bool             isExpandable();

    // Methods
    FObjectIterator  appendItem (FListViewItem*);
    void             replaceControlCodes();
    int              getVisibleLines();

    // Data Member
    std::vector<FString> column_list;
    FWidget::data_ptr    data_pointer;
    int                  visible_lines;
    bool                 expandable;
    bool                 is_expand;

    // Friend class
    friend class FListView;
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
inline bool FListViewItem::isExpand()
{ return is_expand; }

//----------------------------------------------------------------------
inline bool FListViewItem::isExpandable()
{ return expandable; }


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
    const char*        getClassName() const;
    uInt               getCount() const;
    fc::text_alignment getColumnAlignment (int) const;
    FString            getColumnText (int) const;
    FListViewItem*     getCurrentItem();

    // Mutators
    void               setGeometry (int, int, int, int, bool = true);
    void               setColumnAlignment (int, fc::text_alignment);
    void               setColumnText (int, const FString&);
    bool               setTreeView (bool);
    bool               setTreeView();
    bool               unsetTreeView();

    // Methods
    virtual int        addColumn (const FString&, int = USE_MAX_SIZE);
    FObjectIterator    insert (FListViewItem*);
    FObjectIterator    insert (FListViewItem*, FObjectIterator);
    FObjectIterator    insert ( const std::vector<FString>&
                              , data_ptr = 0 );
    FObjectIterator    insert ( const std::vector<FString>&
                              , FObjectIterator );
    FObjectIterator    insert ( const std::vector<FString>&
                              , data_ptr
                              , FObjectIterator );
    FObjectIterator    insert ( const std::vector<long>&
                              , data_ptr = 0 );
    FObjectIterator    insert ( const std::vector<long>&
                              , FObjectIterator );
    FObjectIterator    insert ( const std::vector<long>&
                              , data_ptr
                              , FObjectIterator );
    FObjectIterator    beginOfList();
    FObjectIterator    endOfList();

    // Event handlers
    void               onKeyPress (FKeyEvent*);
    void               onMouseDown (FMouseEvent*);
    void               onMouseUp (FMouseEvent*);
    void               onMouseMove (FMouseEvent*);
    void               onMouseDoubleClick (FMouseEvent*);
    void               onWheel (FWheelEvent*);
    void               onTimer (FTimerEvent*);
    void               onFocusIn (FFocusEvent*);
    void               onFocusOut (FFocusEvent*);

  protected:
    // Methods
    void               adjustYOffset();
    void               adjustSize();

  private:
    // Typedef
    struct Header
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

    typedef std::vector<Header> headerItems;

    // Constants
    static const int USE_MAX_SIZE = -1;

    // Disable copy constructor
    FListView (const FListView&);

    // Disable assignment operator (=)
    FListView& operator = (const FListView&);

    // Methods
    void               init();
    uInt               getAlignOffset (fc::text_alignment, uInt, uInt);
    void               draw();
    void               drawColumnLabels();
    void               drawList();
    void               drawListLine (const FListViewItem*, bool, bool);
    void               recalculateHorizontalBar (int);
    void               recalculateVerticalBar (int);
    FObjectIterator    appendItem (FListViewItem*);
    void               processClick();
    void               processChanged();
    FObjectIterator    index2iterator (int);
    void               nextElement (FObjectIterator&);

    // Callback methods
    void               cb_VBarChange (FWidget*, data_ptr);
    void               cb_HBarChange (FWidget*, data_ptr);

    // Data Members
    FObjectIterator    root;
    FObjectList        selflist;
    FObjectList        itemlist;
    headerItems        header;
    FTermBuffer        headerline;
    FScrollbar*        vbar;
    FScrollbar*        hbar;
    fc::dragScroll     drag_scroll;
    int                scroll_repeat;
    int                scroll_distance;
    bool               scroll_timer;
    bool               tree_view;
    int                current;
    int                xoffset;
    int                yoffset;
    int                nf_offset;
    int                max_line_width;

    // Friend class
    friend class FListViewItem;
};
#pragma pack(pop)


// FListView inline functions
//----------------------------------------------------------------------
inline const char* FListView::getClassName() const
{ return "FListView"; }

//----------------------------------------------------------------------
inline uInt FListView::getCount() const
{ return uInt(itemlist.size()); }

//----------------------------------------------------------------------
inline FListViewItem* FListView::getCurrentItem()
{
  FObjectIterator iter = itemlist.begin();
  std::advance (iter, current - 1);
  return static_cast<FListViewItem*>(*iter);
}

//----------------------------------------------------------------------
inline bool FListView::setTreeView (bool on)
{ return tree_view = (on) ? true : false; }

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
  FListView::insert ( const std::vector<FString>& cols, data_ptr d )
{ return insert (cols, d, root); }

//----------------------------------------------------------------------
inline FObject::FObjectIterator
  FListView::insert ( const std::vector<FString>& cols
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
inline FObject::FObjectIterator FListView::index2iterator (int index)
{
  FObjectIterator iter = itemlist.begin();
  std::advance (iter, index);
  return iter;
}

//----------------------------------------------------------------------
inline void FListView::nextElement (FObjectIterator& iter)
{
  FListViewItem* item = static_cast<FListViewItem*>(*iter);

  if ( item->isExpandable() )
  {
    //iter = item->begin();
    ++iter;
  }
  else
    ++iter;
}

#endif  // FLISTVIEW_H
