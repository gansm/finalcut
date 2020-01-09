/***********************************************************************
* fcombobox.h - Widget FComboBox                                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏1     1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FComboBox ▏- - - -▕ FDropDownListBox ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCOMBOBOX_H
#define FCOMBOBOX_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/flineedit.h"
#include "final/flistbox.h"
#include "final/fwidget.h"
#include "final/fwindow.h"


namespace finalcut
{

// class forward declaration
class FLineEdit;
class FListBox;

//----------------------------------------------------------------------
// class FDropDownListBox
//----------------------------------------------------------------------

class FDropDownListBox : public FWindow
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Constructor
    explicit FDropDownListBox (FWidget* = nullptr);

    // Disable copy constructor
    FDropDownListBox (const FDropDownListBox&) = delete;

    // Destructor
    virtual ~FDropDownListBox ();

    // Disable assignment operator (=)
    FDropDownListBox& operator = (const FDropDownListBox&) = delete;

    // Accessors
    const FString       getClassName() const override;

    // Mutators
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    // Inquiries
    bool                isEmpty();

    // Methods
    void                show() override;
    void                hide() override;

  private:
    // Methods
    void                init();
    void                draw() override;
    void                drawShadow();
    bool                containsWidget (const FPoint&);

    // Data members
    FListBox            list{this};

    // Friend functions
    friend bool closeComboBox (FDropDownListBox*, const FPoint&);

    // Friend classes
    friend class FComboBox;
};

// FDropDownListBox inline functions
//----------------------------------------------------------------------
inline const FString FDropDownListBox::getClassName() const
{ return "FDropDownListBox"; }

//----------------------------------------------------------------------
inline bool FDropDownListBox::isEmpty()
{ return bool( list.getCount() == 0 ); }


//----------------------------------------------------------------------
// class FComboBox
//----------------------------------------------------------------------

class FComboBox : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Constructors
    explicit FComboBox (FWidget* = nullptr);

    // Disable copy constructor
    FComboBox (const FComboBox&) = delete;

    // Destructor
    ~FComboBox();

    // Disable assignment operator (=)
    FComboBox& operator = (const FComboBox&) = delete;

    // Overloaded operators

    // Accessors
    const FString       getClassName() const override;
    std::size_t         getCount() const;
    FString             getText() const;
    FDataPtr            getItemData();
    FLineEdit::label_o  getLabelOrientation();

    // Mutators
    void                setSize (const FSize&, bool = true) override;
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    bool                setEnable (bool) override;
    bool                setEnable() override;
    bool                unsetEnable() override;
    bool                setDisable() override;
    bool                setFocus (bool) override;
    bool                setFocus() override;
    bool                unsetFocus() override;
    bool                setShadow (bool);
    bool                setShadow();
    bool                unsetShadow();
    bool                setEditable (bool);
    bool                setEditable();
    bool                unsetEditable();
    void                setCurrentItem (std::size_t);
    void                setMaxVisibleItems (std::size_t);
    void                setLabelText (const FString&);
    void                setLabelOrientation (const FLineEdit::label_o);

    // Inquiries
    bool                hasShadow();

    // Methods
    void                insert (FListBoxItem);
    template <typename T>
    void                insert ( const std::initializer_list<T>& list
                               , FDataPtr = nullptr );
    template <typename ItemT>
    void                insert (const ItemT&, FDataPtr = nullptr);
    void                remove (std::size_t);
    void                reserve (std::size_t);
    void                clear();
    virtual void        showDropDown();
    virtual void        hideDropDown();

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;
    void                onWheel (FWheelEvent*) override;
    void                onFocusOut (FFocusEvent*) override;

  private:
    // Inquiries
    bool                isMouseOverListWindow (const FPoint&);

    // Methods
    void                init();
    void                initCallbacks();
    void                draw() override;
    void                onePosUp();
    void                onePosDown();
    void                passEventToListWindow (FMouseEvent*&);
    void                processClick();
    void                processChanged();

    // Callback methods
    void                cb_setInputField (FWidget*, FDataPtr);
    void                cb_closeComboBox (FWidget*, FDataPtr);
    void                cb_inputFieldSwitch (FWidget*, FDataPtr);
    void                cb_inputFieldHandOver (FWidget*, FDataPtr);

    // Data members
    FLineEdit           input_field{this};
    FDropDownListBox    list_window{this};
    std::size_t         max_items{8};
    int                 nf{0};
    bool                is_editable{true};
};

// non-member function forward declarations
//----------------------------------------------------------------------
void closeOpenComboBox();
bool closeComboBox (FDropDownListBox*, const FPoint&);

// FComboBox inline functions
//----------------------------------------------------------------------
inline const FString FComboBox::getClassName() const
{ return "FComboBox"; }

//----------------------------------------------------------------------
inline std::size_t FComboBox::getCount() const
{ return list_window.list.getCount(); }

//----------------------------------------------------------------------
inline FString FComboBox::getText() const
{ return input_field.getText(); }

//----------------------------------------------------------------------
inline FDataPtr FComboBox::getItemData()
{
  std::size_t index = list_window.list.currentItem();
  return list_window.list.getItem(index).getData();
}

//----------------------------------------------------------------------
inline FLineEdit::label_o FComboBox::getLabelOrientation()
{ return input_field.getLabelOrientation(); }

//----------------------------------------------------------------------
inline bool FComboBox::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FComboBox::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FComboBox::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FComboBox::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FComboBox::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FComboBox::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FComboBox::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FComboBox::setEditable()
{ return setEditable(true); }

//----------------------------------------------------------------------
inline bool FComboBox::unsetEditable()
{ return setEditable(false); }

//----------------------------------------------------------------------
inline bool FComboBox::hasShadow()
{ return getFlags().shadow; }

//----------------------------------------------------------------------
template <typename T>
void FComboBox::insert ( const std::initializer_list<T>& list
                       , FDataPtr d )
{
  for (auto& item : list)
  {
    FListBoxItem listItem (FString() << item, d);
    insert (listItem);
  }
}

//----------------------------------------------------------------------
template <typename ItemT>
void FComboBox::insert ( const ItemT& item
                       , FDataPtr d )
{
  FListBoxItem listItem (FString() << item, d);
  insert (listItem);
}

//----------------------------------------------------------------------
inline void FComboBox::reserve (std::size_t new_cap)
{ list_window.list.reserve(new_cap); }

//----------------------------------------------------------------------
inline void FComboBox::setLabelText (const FString& s)
{ input_field.setLabelText(s); }

//----------------------------------------------------------------------
inline void FComboBox::setLabelOrientation (const FLineEdit::label_o o)
{ input_field.setLabelOrientation(o); }

}  // namespace finalcut

#endif  // FCOMBOBOX_H


