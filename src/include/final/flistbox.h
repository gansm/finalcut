/***********************************************************************
* flistbox.h - Widget FListBox and FListBoxItem                        *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
e Copyright 2014-2019 Markus Gans                                      *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidget ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FListBox ▏- - - -▕ FListBoxItem ▏
 *       ▕▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *
 */

#ifndef FLISTBOX_H
#define FLISTBOX_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <memory>
#include <vector>

#include "final/fwidget.h"

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
    FListBoxItem ();
    FListBoxItem (const FListBoxItem&);  // copy constructor
    explicit FListBoxItem (const FString&, FDataPtr = nullptr);

    // Destructor
    virtual ~FListBoxItem();

    // Assignment operator (=)
    FListBoxItem& operator = (const FListBoxItem&);

    // Accessors
    virtual const char* getClassName() const;
    virtual FString&    getText();
    virtual FDataPtr    getData() const;

    // Mutators
    void                setText (const FString&);
    void                setData (FDataPtr);

    // Methods
    void                clear();

  private:
    // Friend classes
    friend class FListBox;

    // Data members
    FString           text{};
    FDataPtr          data_pointer{nullptr};
    fc::brackets_type brackets{fc::NoBrackets};
    bool              selected{false};
};


// FListBoxItem inline functions
//----------------------------------------------------------------------
inline const char* FListBoxItem::getClassName() const
{ return "FListBoxItem"; }

//----------------------------------------------------------------------
inline FString& FListBoxItem::getText()
{ return text; }

//----------------------------------------------------------------------
inline FDataPtr FListBoxItem::getData() const
{ return data_pointer; }

//----------------------------------------------------------------------
inline void FListBoxItem::setText (const FString& txt)
{ text.setString(txt); }

//----------------------------------------------------------------------
inline void FListBoxItem::setData (FDataPtr data)
{ data_pointer = data; }

//----------------------------------------------------------------------
inline void FListBoxItem::clear()
{ text.clear(); }


//----------------------------------------------------------------------
// class FListBox
//----------------------------------------------------------------------

class FListBox : public FWidget
{
  public:
    // Typedef
    typedef std::vector<FListBoxItem> listBoxItems;

    // Using-declaration
    using FWidget::setGeometry;

    // Constructor
    explicit FListBox (FWidget* = nullptr);
    template <typename Iterator, typename InsertConverter>
    FListBox (Iterator, Iterator, InsertConverter, FWidget* = nullptr);
    template <typename Container, typename LazyConverter>
    FListBox (Container, LazyConverter, FWidget* = nullptr);

    // Disable copy constructor
    FListBox (const FListBox&) = delete;

    // Destructor
    virtual  ~FListBox();

    // Disable assignment operator (=)
    FListBox& operator = (const FListBox&) = delete;

    // Accessors
    const char*         getClassName() const override;
    std::size_t         getCount() const;
    FListBoxItem        getItem (std::size_t);
    FListBoxItem        getItem (listBoxItems::iterator) const;
    std::size_t         currentItem() const;
    FString&            getText();

    // Mutators
    void                setCurrentItem (std::size_t);
    void                setCurrentItem (listBoxItems::iterator);
    void                selectItem (std::size_t);
    void                selectItem (listBoxItems::iterator);
    void                unselectItem (std::size_t);
    void                unselectItem (listBoxItems::iterator);
    void                showInsideBrackets (std::size_t, fc::brackets_type);
    void                showNoBrackets (std::size_t);
    void                showNoBrackets (listBoxItems::iterator);
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    void                setMultiSelection (bool);
    void                setMultiSelection ();
    void                unsetMultiSelection ();
    bool                setDisable() override;
    bool                setFocus (bool) override;
    bool                setFocus() override;
    bool                unsetFocus() override;
    void                setText (const FString&);

    // Inquiries
    bool                isSelected (std::size_t);
    bool                isSelected (listBoxItems::iterator) const;
    bool                isMultiSelection() const;
    bool                hasBrackets (std::size_t);
    bool                hasBrackets (listBoxItems::iterator) const;

    // Methods
    void                hide() override;
    template <typename Iterator, typename InsertConverter>
    void                insert (Iterator, Iterator, InsertConverter);
    template <typename Container, typename LazyConverter>
    void                insert (Container, LazyConverter);
    void                insert (FListBoxItem);
    template <typename T>
    void                insert ( const std::initializer_list<T>& list
                               , fc::brackets_type = fc::NoBrackets
                               , bool = false
                               , FDataPtr = nullptr );
    template <typename ItemT>
    void                insert ( const ItemT&
                               , fc::brackets_type = fc::NoBrackets
                               , bool = false
                               , FDataPtr = nullptr );
    void                remove (std::size_t);
    void                reserve (std::size_t);
    void                clear();

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;
    void                onMouseDoubleClick (FMouseEvent*) override;
    void                onWheel (FWheelEvent*) override;
    void                onTimer (FTimerEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onFocusOut (FFocusEvent*) override;

  protected:
    // Methods
    void                adjustYOffset (std::size_t);
    void                adjustSize() override;

  private:
    // Typedef
    typedef std::shared_ptr<FScrollbar> FScrollbarPtr;
    typedef void (FListBox::*FListBoxCallback)(FWidget*, FDataPtr);

    // Enumeration
    enum convert_type
    {
      no_convert     = 0,
      direct_convert = 1,
      lazy_convert   = 2
    };

    // Accessors
    static FString&     getString (listBoxItems::iterator);

    // Inquiry
    bool                isHorizontallyScrollable();
    bool                isVerticallyScrollable();

    // Methods
    void                init();
    void                initScrollbar ( FScrollbarPtr&
                                      , fc::orientation
                                      , FListBoxCallback );
    void                draw() override;
    void                drawBorder() override;
    void                drawScrollbars();
    void                drawHeadline();
    void                drawList();
    void                drawListLine (int, listBoxItems::iterator, bool);
    void                printLeftBracket (fc::brackets_type);
    void                printRightBracket (fc::brackets_type);
    void                drawListBracketsLine (int, listBoxItems::iterator, bool);
    void                setLineAttributes (int, bool, bool, bool&);
    void                unsetAttributes();
    void                updateDrawing (bool, bool);
    void                recalculateHorizontalBar (std::size_t, bool);
    void                recalculateVerticalBar (std::size_t);
    void                getWidgetFocus();
    void                multiSelection (std::size_t);
    void                multiSelectionUpTo (std::size_t);
    void                wheelUp (int);
    void                wheelDown (int);
    bool                dragScrollUp();
    bool                dragScrollDown();
    void                dragUp (int);
    void                dragDown (int);
    void                stopDragScroll();
    void                prevListItem (int);
    void                nextListItem (int);
    void                scrollToX (int);
    void                scrollToY (int);
    void                scrollLeft (int);
    void                scrollRight (int);
    void                scrollLeft();
    void                scrollRight();
    void                onePosUp();
    void                onePosDown();
    void                onePageUp();
    void                onePageDown();
    void                firstPos();
    void                lastPos();
    bool                skipIncrementalSearch();
    void                acceptSelection();
    bool                spacebarProcessing();
    bool                changeSelectionAndPosition();
    bool                deletePreviousCharacter();
    bool                keyIncSearchInput (FKey);
    void                processClick();
    void                processSelect();
    void                processChanged();
    void                lazyConvert (listBoxItems::iterator, int);
    listBoxItems::iterator index2iterator (std::size_t);

    // Callback methods
    void                cb_VBarChange (FWidget*, FDataPtr);
    void                cb_HBarChange (FWidget*, FDataPtr);

    // Function Pointer
    void                (*convertToItem) ( FListBoxItem&
                                         , FDataPtr
                                         , int index ){nullptr};

    // Data members
    listBoxItems    itemlist{};
    FDataPtr        source_container{nullptr};
    FScrollbarPtr   vbar{nullptr};
    FScrollbarPtr   hbar{nullptr};
    FString         text{};
    FString         inc_search{};
    convert_type    conv_type{FListBox::no_convert};
    fc::dragScroll  drag_scroll{fc::noScroll};
    int             scroll_repeat{100};
    int             scroll_distance{1};
    int             last_current{-1};
    int             secect_from_item{-1};
    int             xoffset{0};
    int             yoffset{0};
    int             last_yoffset{-1};
    std::size_t     current{0};
    std::size_t     nf_offset{0};
    std::size_t     max_line_width{0};
    bool            multi_select{false};
    bool            mouse_select{false};
    bool            scroll_timer{false};
};


// FListBox inline functions
//----------------------------------------------------------------------
template <typename Iterator, typename InsertConverter>
inline FListBox::FListBox ( Iterator first
                          , Iterator last
                          , InsertConverter convert
                          , FWidget* parent )
  : FWidget(parent)
{
  init();

  while ( first != last )
  {
    insert(convert(first), fc::NoBrackets, false, &(*first));
    ++first;
  }
}

//----------------------------------------------------------------------
template <typename Container, typename LazyConverter>
inline FListBox::FListBox ( Container container
                          , LazyConverter convert
                          , FWidget* parent )
  : FWidget(parent)
{
  init();
  insert (container, convert);
}

//----------------------------------------------------------------------
inline const char* FListBox::getClassName() const
{ return "FListBox"; }

//----------------------------------------------------------------------
inline std::size_t FListBox::getCount() const
{ return itemlist.size(); }

//----------------------------------------------------------------------
inline FListBoxItem FListBox::getItem (std::size_t index)
{
  listBoxItems::iterator iter = index2iterator(index - 1);
  return *iter;
}

//----------------------------------------------------------------------
inline FListBoxItem FListBox::getItem (listBoxItems::iterator iter) const
{ return *iter; }

//----------------------------------------------------------------------
inline std::size_t FListBox::currentItem() const
{ return current; }

//----------------------------------------------------------------------
inline FString& FListBox::getText()
{ return text; }

//----------------------------------------------------------------------
inline void FListBox::selectItem (std::size_t index)
{ index2iterator(index - 1)->selected = true; }

//----------------------------------------------------------------------
inline void FListBox::selectItem (listBoxItems::iterator iter)
{ iter->selected = true; }

//----------------------------------------------------------------------
inline void FListBox::unselectItem (std::size_t index)
{ index2iterator(index - 1)->selected = false; }

//----------------------------------------------------------------------
inline void FListBox::unselectItem (listBoxItems::iterator iter)
{ iter->selected = false; }

//----------------------------------------------------------------------
inline void FListBox::showNoBrackets (std::size_t index)
{ index2iterator(index - 1)->brackets = fc::NoBrackets; }

//----------------------------------------------------------------------
inline void FListBox::showNoBrackets (listBoxItems::iterator iter)
{ iter->brackets = fc::NoBrackets; }

//----------------------------------------------------------------------
inline void FListBox::setMultiSelection (bool enable)
{ multi_select = enable; }

//----------------------------------------------------------------------
inline void FListBox::setMultiSelection()
{ setMultiSelection(true); }

//----------------------------------------------------------------------
inline void FListBox::unsetMultiSelection()
{ setMultiSelection(false); }

//----------------------------------------------------------------------
inline bool FListBox::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FListBox::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FListBox::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FListBox::isSelected (std::size_t index)
{ return index2iterator(index - 1)->selected; }

//----------------------------------------------------------------------
inline bool FListBox::isSelected (listBoxItems::iterator iter) const
{ return iter->selected; }

//----------------------------------------------------------------------
inline bool FListBox::isMultiSelection() const
{ return multi_select; }

//----------------------------------------------------------------------
inline bool FListBox::hasBrackets(std::size_t index)
{ return bool(index2iterator(index - 1)->brackets > 0); }

//----------------------------------------------------------------------
inline bool FListBox::hasBrackets(listBoxItems::iterator iter) const
{ return bool(iter->brackets > 0); }

//----------------------------------------------------------------------
inline void FListBox::reserve (std::size_t new_cap)
{ itemlist.reserve(new_cap); }

//----------------------------------------------------------------------
template <typename Iterator, typename InsertConverter>
inline void FListBox::insert ( Iterator first
                             , Iterator last
                             , InsertConverter convert )
{
  conv_type = direct_convert;

  while ( first != last )
  {
    insert (convert(first), fc::NoBrackets, false, &(*first));
    ++first;
  }
}

//----------------------------------------------------------------------
template <typename Container, typename LazyConverter>
void FListBox::insert (Container container, LazyConverter convert)
{
  conv_type = lazy_convert;
  source_container = container;
  convertToItem = convert;
  std::size_t size = container->size();

  if ( size > 0 )
    itemlist.resize(size);

  recalculateVerticalBar(size);
}

//----------------------------------------------------------------------
template <typename T>
void FListBox::insert ( const std::initializer_list<T>& list
                      , fc::brackets_type b
                      , bool s
                      , FDataPtr d )
{
  for (auto& item : list)
  {
    FListBoxItem listItem (FString() << item, d);
    listItem.brackets = b;
    listItem.selected = s;
    insert (listItem);
  }
}

//----------------------------------------------------------------------
template <typename ItemT>
void FListBox::insert ( const ItemT& item
                      , fc::brackets_type b
                      , bool s
                      , FDataPtr d )
{
  FListBoxItem listItem (FString() << item, d);
  listItem.brackets = b;
  listItem.selected = s;
  insert (listItem);
}

//----------------------------------------------------------------------
inline bool FListBox::isHorizontallyScrollable()
{ return bool( max_line_width + 1 >= getClientWidth() ); }

//----------------------------------------------------------------------
inline bool FListBox::isVerticallyScrollable()
{ return bool( getCount() > getClientHeight() ); }

//----------------------------------------------------------------------
inline FListBox::listBoxItems::iterator \
    FListBox::index2iterator (std::size_t index)
{
  listBoxItems::iterator iter = itemlist.begin();
  std::advance (iter, index);
  return iter;
}

}  // namespace finalcut

#endif  // FLISTBOX_H
