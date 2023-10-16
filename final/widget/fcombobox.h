/***********************************************************************
* fcombobox.h - Widget FComboBox                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2023 Markus Gans                                      *
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

#include "final/fwidget.h"
#include "final/util/fdata.h"
#include "final/widget/flineedit.h"
#include "final/widget/flistbox.h"
#include "final/widget/fwindow.h"

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

    // Destructor
    ~FDropDownListBox () override;

    // Accessors
    auto getClassName() const -> FString override;

    // Mutators
    void setGeometry (const FPoint&, const FSize&, bool = true ) override;
    // Inquiries
    auto isEmpty() const -> bool;

    // Methods
    void show() override;
    void hide() override;

  private:
    // Methods
    void init();
    void draw() override;
    void drawShadow();
    auto containsWidget (const FPoint&) -> bool;

    // Data members
    FListBox list{this};

    // Friend functions
    friend auto closeComboBox (FDropDownListBox*, const FPoint&) -> bool;

    // Friend classes
    friend class FComboBox;
};

// FDropDownListBox inline functions
//----------------------------------------------------------------------
inline auto FDropDownListBox::getClassName() const -> FString
{ return "FDropDownListBox"; }

//----------------------------------------------------------------------
inline auto FDropDownListBox::isEmpty() const -> bool
{ return list.getCount() == 0; }


//----------------------------------------------------------------------
// class FComboBox
//----------------------------------------------------------------------

class FComboBox : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;
    using LabelOrientation = FLineEdit::LabelOrientation;

    // Constructors
    explicit FComboBox (FWidget* = nullptr);

    // Destructor
    ~FComboBox() noexcept override;

    // Accessors
    auto getClassName() const -> FString override;
    auto getCount() const -> std::size_t;
    auto getText() const -> FString;
    template <typename DT>
    auto getItemData() -> clean_fdata_t<DT>&;
    auto getLabelOrientation() const -> LabelOrientation;

    // Mutators
    void setSize (const FSize&, bool = true) override;
    void setGeometry (const FPoint&, const FSize&, bool = true) override;
    void setEnable (bool = true) override;
    void unsetEnable() override;
    void setDisable() override;
    void setShadow (bool = true);
    void unsetShadow();
    void setEditable (bool = true);
    void unsetEditable();
    void setCurrentItem (std::size_t);
    void setMaxVisibleItems (std::size_t);
    void setText (const FString&);
    void clearText();
    void setLabelText (const FString&);
    void setLabelOrientation (const LabelOrientation);

    // Inquiries
    auto hasShadow() const -> bool;

    // Methods
    void insert (const FListBoxItem&);
    template <typename T
            , typename DT = std::nullptr_t>
    void insert (const std::initializer_list<T>& list, DT&& = DT());
    template <typename ItemT
            , typename DT = std::nullptr_t>
    void insert (const ItemT&, DT&& = DT());
    void remove (std::size_t);
    void reserve (std::size_t);
    void clear();
    virtual void showDropDown();
    virtual void hideDropDown();

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onFocusIn (FFocusEvent*) override;
    void onFocusOut (FFocusEvent*) override;
    void onFailAtChildFocus (FFocusEvent*) override;

  private:
    // Inquiries
    auto isMouseOverListWindow (const FPoint&) -> bool;

    // Methods
    void init();
    void initCallbacks();
    void draw() override;
    void onePosUp();
    void onePosDown();
    void passEventToListWindow (const FMouseEvent&);
    void processClick() const;
    void processRowChanged() const;

    // Callback methods
    void cb_setInputField();
    void cb_closeComboBox();
    void cb_inputFieldSwitch();
    void cb_inputFieldHandOver();

    // Data members
    FLineEdit         input_field{this};
    FDropDownListBox  list_window{this};
    std::size_t       max_items{8};
    int               nf{0};
    bool              is_editable{true};
};

// non-member function forward declarations
//----------------------------------------------------------------------
void closeOpenComboBox();
auto closeComboBox (FDropDownListBox*, const FPoint&) -> bool;

// FComboBox inline functions
//----------------------------------------------------------------------
inline auto FComboBox::getClassName() const -> FString
{ return "FComboBox"; }

//----------------------------------------------------------------------
inline auto FComboBox::getCount() const -> std::size_t
{ return list_window.list.getCount(); }

//----------------------------------------------------------------------
inline auto FComboBox::getText() const -> FString
{ return input_field.getText(); }

//----------------------------------------------------------------------
template <typename DT>
inline auto FComboBox::getItemData() -> clean_fdata_t<DT>&
{
  const std::size_t index = list_window.list.currentItem();
  return list_window.list.getItem(index).getData<DT>();
}

//----------------------------------------------------------------------
inline auto FComboBox::getLabelOrientation() const -> FLineEdit::LabelOrientation
{ return input_field.getLabelOrientation(); }

//----------------------------------------------------------------------
inline void FComboBox::unsetEnable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FComboBox::setDisable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FComboBox::unsetShadow()
{ setShadow(false); }

//----------------------------------------------------------------------
inline void FComboBox::unsetEditable()
{ setEditable(false); }

//----------------------------------------------------------------------
inline auto FComboBox::hasShadow() const -> bool
{ return getFlags().shadow.shadow; }

//----------------------------------------------------------------------
template <typename T
        , typename DT>
void FComboBox::insert (const std::initializer_list<T>& list, DT&& d)
{
  for (const auto& item : list)
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
inline void FComboBox::setLabelOrientation (const LabelOrientation o)
{ input_field.setLabelOrientation(o); }

}  // namespace finalcut

#endif  // FCOMBOBOX_H


