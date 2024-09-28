/***********************************************************************
* fbuttongroup.h - The FButtonGroup widget organizes FToggleButton     *
*                  widgets in a group.                                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FButtonGroup ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FBUTTONGROUP_H
#define FBUTTONGROUP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidgetcolors.h"
#include "final/widget/fscrollview.h"

namespace finalcut
{

// class forward declaration
class FToggleButton;

//----------------------------------------------------------------------
// class FButtonGroup
//----------------------------------------------------------------------

class FButtonGroup : public FScrollView
{
  public:
    // Constructors
    explicit FButtonGroup (FWidget* = nullptr);
    explicit FButtonGroup (const FString&, FWidget* = nullptr);

    // Destructor
    ~FButtonGroup() override;

    // Accessor
    auto getClassName() const -> FString override;
    auto getButton (int) const -> FToggleButton*;
    auto getFocusedButton() const -> FToggleButton*;
    auto getCheckedButton() const -> FToggleButton*;
    auto getCount() const -> std::size_t;

    // Mutator
    void setEnable (bool = true) override;
    void unsetEnable() override;
    void setDisable() override;

    // Inquiries
    auto isChecked (int) const -> bool;
    auto hasFocusedButton() const -> bool;
    auto hasCheckedButton() const -> bool;

    // Methods
    void hide() override;
    void insert (FToggleButton*);
    void remove (FToggleButton*);
    void checkScrollSize (const FToggleButton*);
    void checkScrollSize (const FRect&);

    // Event handlers
    void onMouseDown (FMouseEvent*) override;
    void onAccel (FAccelEvent*) override;
    void onFocusIn (FFocusEvent*) override;
    void onChildFocusOut (FFocusEvent*) override;

  protected:
    // Methods
    void draw() override;

  private:
    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Inquiries
    auto isRadioButton (const FToggleButton*) const -> bool;

    // Methods
    void init();
    auto directFocusCheckedRadioButton (FToggleButton*) const -> bool;
    auto directFocusRadioButton() const -> bool;
    void directFocus();
    auto getCheckedRadioButton() -> FToggleButton*;
    template <typename UnaryPredicate>
    auto findButtonIf (UnaryPredicate) const -> FToggleButton*;
    auto needToUncheck (const FToggleButton*, const FToggleButton*) const -> bool;

    // Callback method
    void cb_buttonToggled (const FToggleButton*) const;

    // Data members
    FString      text{};
    FObjectList  buttonlist{};
};

// FButtonGroup inline functions
//----------------------------------------------------------------------
inline auto FButtonGroup::getClassName() const -> FString
{ return "FButtonGroup"; }

//----------------------------------------------------------------------
inline void FButtonGroup::unsetEnable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FButtonGroup::setDisable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline auto FButtonGroup::getCount() const -> std::size_t
{ return buttonlist.size(); }


}  // namespace finalcut

#endif  // FBUTTONGROUP_H
