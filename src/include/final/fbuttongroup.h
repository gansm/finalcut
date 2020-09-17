/***********************************************************************
* fbuttongroup.h - The FButtonGroup widget organizes FToggleButton     *
*                  widgets in a group.                                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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

#include "final/fscrollview.h"
#include "final/fwidgetcolors.h"

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

    // Disable copy constructor
    FButtonGroup (const FButtonGroup&) = delete;

    // Destructor
    ~FButtonGroup() override;

    // Disable copy assignment operator (=)
    FButtonGroup& operator = (const FButtonGroup&) = delete;

    // Accessor
    const FString       getClassName() const override;
    FToggleButton*      getFirstButton();
    FToggleButton*      getLastButton();
    FToggleButton*      getButton (int) const;
    std::size_t         getCount() const;
    FString&            getText();

    // Mutator
    bool                setEnable(bool) override;
    bool                setEnable() override;
    bool                unsetEnable() override;
    bool                setDisable() override;
    void                setText (const FString&);

    // Inquiries
    bool                isChecked(int) const;
    bool                hasFocusedButton() const;
    bool                hasCheckedButton() const;

    // Methods
    void                hide() override;
    void                insert (FToggleButton*);
    void                remove (FToggleButton*);
    void                checkScrollSize (const FToggleButton*);
    void                checkScrollSize (const FRect&);

    // Event handlers
    void                onMouseDown (FMouseEvent*) override;
    void                onAccel (FAccelEvent*) override;
    void                onFocusIn (FFocusEvent*) override;

  protected:
    // Mutator
    void                setHotkeyAccelerator();

    // Methods
    void                draw() override;
    void                drawLabel();

  private:
    // Constants
    static constexpr std::size_t NOT_SET = static_cast<std::size_t>(-1);

    // Inquiries
    bool                isRadioButton (const FToggleButton*) const;

    // Methods
    void                init();
    void                drawText (const FString&, std::size_t);
    void                directFocus();

    // Callback method
    void                cb_buttonToggled (const FToggleButton*) const;

    // Data members
    FString        text{};
    FObjectList    buttonlist{};
};

// FButtonGroup inline functions
//----------------------------------------------------------------------
inline const FString FButtonGroup::getClassName() const
{ return "FButtonGroup"; }

//----------------------------------------------------------------------
inline bool FButtonGroup::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FButtonGroup::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FButtonGroup::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline std::size_t FButtonGroup::getCount() const
{ return buttonlist.size(); }

//----------------------------------------------------------------------
inline FString& FButtonGroup::getText()
{ return text; }

}  // namespace finalcut

#endif  // FBUTTONGROUP_H
