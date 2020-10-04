/***********************************************************************
* fcombobox.h - Widget FComboBox                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2020 Markus Gans                                      *
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
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏1     1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FComboBox ▏- - - -▕ FDropDownListBox ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FCOMBOBOX_H
#define FCOMBOBOX_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <utility>

#include "final/fdata.h"
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
    ~FDropDownListBox () override;

    // Disable copy assignment operator (=)
    FDropDownListBox& operator = (const FDropDownListBox&) = delete;

    // Accessors
    FString             getClassName() const override;

    // Mutators
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    // Inquiries
    bool                isEmpty() const;

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
inline FString FDropDownListBox::getClassName() const
{ return "FDropDownListBox"; }

//----------------------------------------------------------------------
inline bool FDropDownListBox::isEmpty() const
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
    ~FComboBox() override;

    // Disable copy assignment operator (=)
    FComboBox& operator = (const FComboBox&) = delete;

    // Overloaded operators

    // Accessors
    FString             getClassName() const override;
    std::size_t         getCount() const;
    FString             getText() const;
    template <typename DT>
    clean_fdata_t<DT>&  getItemData();
    FLineEdit::label_o  getLabelOrientation() const;

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
    void                setText (const FString&);
    void                clearText();
    void                setLabelText (const FString&);
    void                setLabelOrientation (const FLineEdit::label_o);

    // Inquiries
    bool                hasShadow() const;

    // Methods
    void                insert (const FListBoxItem&);
    template <typename T
            , typename DT = std::nullptr_t>
    void                insert ( const std::initializer_list<T>& list
                               , DT&& = DT() );
    template <typename ItemT
            , typename DT = std::nullptr_t>
    void                insert (const ItemT&, DT&& = DT());
    void                remove (std::size_t);
    void                reserve (std::size_t);
    void                clear();
    virtual void        showDropDown();
    virtual void        hideDropDown();

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
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
    void                passEventToListWindow (FMouseEvent* const&);
    void                processClick() const;
    void                processChanged() const;

    // Callback methods
    void                cb_setInputField();
    void                cb_closeComboBox();
    void                cb_inputFieldSwitch();
    void                cb_inputFieldHandOver();

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
inline FString FComboBox::getClassName() const
{ return "FComboBox"; }

//----------------------------------------------------------------------
inline std::size_t FComboBox::getCount() const
{ return list_window.list.getCount(); }

//----------------------------------------------------------------------
inline FString FComboBox::getText() const
{ return input_field.getText(); }

//----------------------------------------------------------------------
template <typename DT>
inline clean_fdata_t<DT>& FComboBox::getItemData()
{
  const std::size_t index = list_window.list.currentItem();
  return list_window.list.getItem(index).getData<DT>();
}

//----------------------------------------------------------------------
inline FLineEdit::label_o FComboBox::getLabelOrientation() const
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
inline bool FComboBox::hasShadow() const
{ return getFlags().shadow; }

//----------------------------------------------------------------------
template <typename T
        , typename DT>
void FComboBox::insert (const std::initializer_list<T>& list, DT&& d)
{
  for (auto& item : list)
  {
    FListBoxItem listItem (FString() << item, std::forward<DT>(d));
    insert (listItem);
  }
}

//----------------------------------------------------------------------
template <typename ItemT
        , typename DT>
void FComboBox::insert (const ItemT& item, DT&& d)
{
  FListBoxItem listItem (FString() << item, std::forward<DT>(d));
  insert (listItem);
}

//----------------------------------------------------------------------
inline void FComboBox::reserve (std::size_t new_cap)
{ list_window.list.reserve(new_cap); }

//----------------------------------------------------------------------
inline void FComboBox::setText (const FString& s)
{ input_field.setText(s); }

//----------------------------------------------------------------------
inline void FComboBox::clearText()
{ input_field.clear(); }

//----------------------------------------------------------------------
inline void FComboBox::setLabelText (const FString& s)
{ input_field.setLabelText(s); }

//----------------------------------------------------------------------
inline void FComboBox::setLabelOrientation (const FLineEdit::label_o o)
{ input_field.setLabelOrientation(o); }

}  // namespace finalcut

#endif  // FCOMBOBOX_H


