/***********************************************************************
* flistbox.h - Widget FListBox and FListBoxItem                        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2021 Markus Gans                                      *
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

#include "final/fdata.h"
#include "final/fscrollbar.h"
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
    FListBoxItem() = default;
    FListBoxItem (const FListBoxItem&);  // copy constructor
    template <typename DT = std::nullptr_t>
    explicit FListBoxItem (const FString&, DT&& = DT() );

    // Destructor
    virtual ~FListBoxItem() noexcept;

    // copy copy assignment operator (=)
    FListBoxItem& operator = (const FListBoxItem&);

    // Accessors
    virtual FString       getClassName() const;
    virtual FString       getText() const;
    template <typename DT>
    clean_fdata_t<DT>&    getData() const;

    // Mutators
    void                  setText (const FString&);
    template <typename DT>
    void                  setData (DT&&);

    // Inquiries
    bool                  isSelected() const;

    // Methods
    void                  clear();

  private:
    // Using-declaration
    using FDataAccessPtr = std::shared_ptr<FDataAccess>;

    // Data members
    FString               text{};
    FDataAccessPtr        data_pointer{};
    BracketType           brackets{BracketType::None};
    bool                  selected{false};

    // Friend classes
    friend class FListBox;
};


// FListBoxItem inline functions
//----------------------------------------------------------------------
template <typename DT>
inline FListBoxItem::FListBoxItem (const FString& txt, DT&& data)
  : text{txt}
  , data_pointer{makeFData(std::forward<DT>(data))}
{ }

//----------------------------------------------------------------------
inline FString FListBoxItem::getClassName() const
{ return "FListBoxItem"; }

//----------------------------------------------------------------------
inline FString FListBoxItem::getText() const
{ return text; }

//----------------------------------------------------------------------
template <typename DT>
inline clean_fdata_t<DT>& FListBoxItem::getData() const
{
  return static_cast<FData<clean_fdata_t<DT>>&>(*data_pointer).get();
}

//----------------------------------------------------------------------
inline void FListBoxItem::setText (const FString& txt)
{ text.setString(txt); }

//----------------------------------------------------------------------
template <typename DT>
inline void FListBoxItem::setData (DT&& data)
{
  const auto data_obj = makeFData(std::forward<DT>(data));
  data_pointer.reset(data_obj);
}

//----------------------------------------------------------------------
inline bool FListBoxItem::isSelected() const
{ return selected; }

//----------------------------------------------------------------------
inline void FListBoxItem::clear()
{ text.clear(); }


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

    // Destructor
    ~FListBox() override;

    // Disable copy assignment operator (=)
    FListBox& operator = (const FListBox&) = delete;

    // Accessors
    FString              getClassName() const override;
    std::size_t          getCount() const;
    FListBoxItem&        getItem (std::size_t);
    const FListBoxItem&  getItem (std::size_t) const;
    FListBoxItem&        getItem (FListBoxItems::iterator);
    const FListBoxItem&  getItem (FListBoxItems::const_iterator) const;
    std::size_t          currentItem() const;
    FListBoxItems&       getData();
    const FListBoxItems& getData() const;
    FString&             getText();

    // Mutators
    void                 setCurrentItem (std::size_t);
    void                 setCurrentItem (FListBoxItems::iterator);
    void                 selectItem (std::size_t);
    void                 selectItem (FListBoxItems::iterator) const;
    void                 unselectItem (std::size_t);
    void                 unselectItem (FListBoxItems::iterator) const;
    void                 showInsideBrackets (const std::size_t, BracketType);
    void                 showNoBrackets (std::size_t);
    void                 showNoBrackets (FListBoxItems::iterator) const;
    void                 setSize (const FSize&, bool = true) override;
    void                 setGeometry ( const FPoint&, const FSize&
                                     , bool = true ) override;
    void                 setMultiSelection (bool = true);
    void                 unsetMultiSelection ();
    bool                 setDisable() override;
    void                 setText (const FString&);

    // Inquiries
    bool                 isSelected (std::size_t) const;
    bool                 isSelected (FListBoxItems::iterator) const;
    bool                 isMultiSelection() const;
    bool                 hasBrackets (std::size_t) const;
    bool                 hasBrackets (FListBoxItems::iterator) const;

    // Methods
    void                 hide() override;
    template <typename Iterator
            , typename InsertConverter>
    void                 insert ( Iterator, Iterator
                                , const InsertConverter& );
    template <typename Container
            , typename LazyConverter>
    void                 insert ( const Container&
                                , LazyConverter&& );
    template <typename Container
            , typename LazyConverter>
    void                 insert (Container*, LazyConverter&&);
    void                 insert (const FListBoxItem&);
    template <typename T
            , typename DT = std::nullptr_t>
    void                 insert ( const std::initializer_list<T>& list
                                , BracketType = BracketType::None
                                , bool = false
                                , DT&& = DT() );
    template <typename ItemT
            , typename DT = std::nullptr_t>
    void                 insert ( const ItemT&
                                , BracketType = BracketType::None
                                , bool = false
                                , DT&& = DT() );
    void                 remove (std::size_t);
    void                 reserve (std::size_t);
    void                 clear();

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
    void                 adjustYOffset (std::size_t);
    void                 adjustSize() override;

  private:
    // Using-declaration
    using KeyMap = std::unordered_map<FKey, std::function<void()>, FKeyHash>;
    using KeyMapResult = std::unordered_map<FKey, std::function<bool()>, FKeyHash>;
    using LazyInsert = std::function<void(FListBoxItem&, FDataAccess*, std::size_t)>;

    // Enumeration
    enum class ConvertType
    {
      None   = 0,
      Direct = 1,
      Lazy   = 2
    };

    // Accessors
    static FString       getString (FListBoxItems::iterator);

    // Inquiry
    bool                 isHorizontallyScrollable() const;
    bool                 isVerticallyScrollable() const;

    // Methods
    void                 init();
    void                 mapKeyFunctions();
    void                 processKeyAction (FKeyEvent*);
    void                 draw() override;
    void                 drawBorder() override;
    void                 drawScrollbars() const;
    void                 drawHeadline();
    void                 drawList();
    void                 drawListLine (int, FListBoxItems::iterator, bool);
    void                 printLeftBracket (BracketType);
    void                 printRightBracket (BracketType);
    void                 drawListBracketsLine (int, FListBoxItems::iterator, bool);
    void                 setLineAttributes (int, bool, bool, bool&);
    void                 unsetAttributes() const;
    void                 updateDrawing (bool, bool);
    void                 recalculateHorizontalBar (std::size_t, bool);
    void                 recalculateVerticalBar (std::size_t) const;
    void                 multiSelection (std::size_t);
    void                 multiSelectionUpTo (std::size_t);
    void                 wheelUp (int);
    void                 wheelDown (int);
    bool                 dragScrollUp();
    bool                 dragScrollDown();
    void                 dragUp (MouseButton);
    void                 dragDown (MouseButton);
    void                 stopDragScroll();
    void                 prevListItem (int);
    void                 nextListItem (int);
    void                 scrollToX (int);
    void                 scrollToY (int);
    void                 scrollLeft (int);
    void                 scrollRight (int);
    void                 scrollLeft();
    void                 scrollRight();
    void                 onePosUp();
    void                 onePosDown();
    void                 onePageUp();
    void                 onePageDown();
    void                 firstPos();
    void                 lastPos();
    bool                 skipIncrementalSearch();
    void                 acceptSelection();
    bool                 spacebarProcessing();
    bool                 changeSelectionAndPosition();
    bool                 deletePreviousCharacter();
    bool                 keyIncSearchInput (FKey);
    void                 processClick() const;
    void                 processSelect() const;
    void                 processChanged() const;
    void                 changeOnResize() const;
    void                 lazyConvert (FListBoxItems::iterator, std::size_t);
    FListBoxItems::iterator index2iterator (std::size_t);
    FListBoxItems::const_iterator index2iterator (std::size_t index) const;
    // Callback methods
    void                 cb_vbarChange (const FWidget*) noexcept;
    void                 cb_hbarChange (const FWidget*) noexcept;

    // Function Pointer
    LazyInsert      lazy_inserter{};

    // Data members
    FListBoxItems   itemlist{};
    FDataAccess*    source_container{nullptr};
    FScrollbarPtr   vbar{nullptr};
    FScrollbarPtr   hbar{nullptr};
    FString         text{};
    FString         inc_search{};
    KeyMap          key_map{};
    KeyMapResult    key_map_result{};
    ConvertType     conv_type{ConvertType::None};
    DragScrollMode  drag_scroll{DragScrollMode::None};
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
    bool            click_on_list{false};
};

// non-member function
//----------------------------------------------------------------------
namespace flistboxhelper
{

template <typename Container>
constexpr clean_fdata_t<Container>& getContainer(FDataAccess* container)
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
inline FString FListBox::getClassName() const
{ return "FListBox"; }

//----------------------------------------------------------------------
inline std::size_t FListBox::getCount() const
{ return itemlist.size(); }

//----------------------------------------------------------------------
inline FListBoxItem& FListBox::getItem (std::size_t index)
{
  FListBoxItems::iterator iter = index2iterator(index - 1);
  return *iter;
}

//----------------------------------------------------------------------
inline const FListBoxItem& FListBox::getItem (std::size_t index) const
{
  FListBoxItems::const_iterator iter = index2iterator(index - 1);
  return *iter;
}

//----------------------------------------------------------------------
inline FListBoxItem& FListBox::getItem (FListBoxItems::iterator iter)
{ return *iter; }

//----------------------------------------------------------------------
inline const FListBoxItem& FListBox::getItem (FListBoxItems::const_iterator iter) const
{ return *iter; }

//----------------------------------------------------------------------
inline std::size_t FListBox::currentItem() const
{ return current; }

//----------------------------------------------------------------------
inline FListBox::FListBoxItems& FListBox::getData()
{ return itemlist; }

//----------------------------------------------------------------------
inline const FListBox::FListBoxItems& FListBox::getData() const
{ return itemlist; }

//----------------------------------------------------------------------
inline FString& FListBox::getText()
{ return text; }

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
{ multi_select = enable; }

//----------------------------------------------------------------------
inline void FListBox::unsetMultiSelection()
{ setMultiSelection(false); }

//----------------------------------------------------------------------
inline bool FListBox::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FListBox::isSelected (std::size_t index) const
{ return index2iterator(index - 1)->selected; }

//----------------------------------------------------------------------
inline bool FListBox::isSelected (FListBoxItems::iterator iter) const
{ return iter->selected; }

//----------------------------------------------------------------------
inline bool FListBox::isMultiSelection() const
{ return multi_select; }

//----------------------------------------------------------------------
inline bool FListBox::hasBrackets(std::size_t index) const
{ return index2iterator(index - 1)->brackets != BracketType::None; }

//----------------------------------------------------------------------
inline bool FListBox::hasBrackets(FListBoxItems::iterator iter) const
{ return iter->brackets != BracketType::None; }

//----------------------------------------------------------------------
inline void FListBox::reserve (std::size_t new_cap)
{ itemlist.reserve(new_cap); }

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
  source_container = makeFData(container);
  lazy_inserter = std::forward<LazyConverter>(converter);
  const std::size_t size = container.size();

  if ( size > 0 )
    itemlist.resize(size);

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
  for (auto& item : list)
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
inline bool FListBox::isHorizontallyScrollable() const
{ return max_line_width + 1 >= getClientWidth(); }

//----------------------------------------------------------------------
inline bool FListBox::isVerticallyScrollable() const
{ return getCount() > getClientHeight(); }

//----------------------------------------------------------------------
inline FListBox::FListBoxItems::iterator \
    FListBox::index2iterator (std::size_t index)
{
  FListBoxItems::iterator iter = itemlist.begin();
  std::advance (iter, index);
  return iter;
}

//----------------------------------------------------------------------
inline FListBox::FListBoxItems::const_iterator \
    FListBox::index2iterator (std::size_t index) const
{
  FListBoxItems::const_iterator iter = itemlist.begin();
  std::advance (iter, index);
  return iter;
}

}  // namespace finalcut

#endif  // FLISTBOX_H
