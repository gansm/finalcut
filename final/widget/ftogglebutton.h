/***********************************************************************
* ftogglebutton.h - Intermediate base class for a toggle button        *
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
 *    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *    ▕ FToggleButton ▏
 *    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTOGGLEBUTTON_H
#define FTOGGLEBUTTON_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FButtonGroup;

//----------------------------------------------------------------------
// class FToggleButton - abstract class for FRadioButton, FCheckBox, ...
//----------------------------------------------------------------------

class FToggleButton : public FWidget
{
  public:
    // Using-declaration
    using FWidget::delAccelerator;
    using FWidget::setGeometry;

    // Constructors
    explicit FToggleButton (FWidget* = nullptr);
    explicit FToggleButton (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FToggleButton (const FToggleButton&) = delete;

    // Disable move constructor
    FToggleButton (FToggleButton&&) noexcept = delete;

    // Destructor
    ~FToggleButton() override;

    // Disable copy assignment operator (=)
    auto operator = (const FToggleButton&) -> FToggleButton& = delete;

    // Disable move assignment operator (=)
    auto operator = (FToggleButton&&) noexcept -> FToggleButton& = delete;
    // Accessors
    auto getClassName() const -> FString override;
    auto getText() & -> FString&;

    // Mutators
    void setSize (const FSize&, bool = true) override;
    void setGeometry (const FPoint&, const FSize&, bool = true) override;
    void resetColors() override;
    void setNoUnderline (bool = true);
    void unsetNoUnderline();
    void setEnable (bool = true) override;
    void unsetEnable() override;
    void setDisable() override;
    void setChecked (bool = true);
    void unsetChecked();
    virtual void setText (const FString&);

    // Inquiries
    auto isChecked() const noexcept -> bool;
    auto hasRadiobuttonFocus() const noexcept -> bool;

    // Method
    void hide() override;

    // Event handlers
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onAccel (FAccelEvent*) override;
    void onFocusIn (FFocusEvent*) override;
    void onFocusOut (FFocusEvent*) override;

  protected:
    // Accessor
    auto getGroup() const -> FButtonGroup*;
    friend auto getGroup (const FToggleButton& toggle_btn) -> FButtonGroup*;

    // Mutator
    void setHotkeyAccelerator();
    void setButtonWidth (std::size_t) noexcept;
    void setLabelOffsetPos (std::size_t) noexcept;

    // Inquiries
    auto isRadioButton() const -> bool;
    auto isCheckboxButton() const -> bool;
    auto hasGroup() const -> bool;

    // Methods
    void draw() override;
    void drawLabel();
    void processClick() const;
    void processToggle() const;

    // Event handler
    void onKeyPress (FKeyEvent*) override;

  private:
    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Mutator
    void setGroup (FButtonGroup*);
    friend void setGroup (FToggleButton&, FButtonGroup*);

    // Methods
    void init();
    void drawText (const FString&, std::size_t);
    void performButtonAction();
    void correctSize (FSize&) const;

    // Data members
    FButtonGroup* button_group{nullptr};
    FString       text{};
    std::size_t   label_offset_pos{0};
    std::size_t   button_width{0};  // plus margin spaces
    bool          radiobutton_focus{false};
    bool          checked{false};
};

// FRadioButton inline functions
//----------------------------------------------------------------------
inline auto FToggleButton::getClassName() const -> FString
{ return "FToggleButton"; }

//----------------------------------------------------------------------
inline auto FToggleButton::getText() & -> FString&
{ return text; }

//----------------------------------------------------------------------
inline void FToggleButton::unsetNoUnderline()
{ setNoUnderline(false); }

//----------------------------------------------------------------------
inline void FToggleButton::unsetEnable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FToggleButton::setDisable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FToggleButton::unsetChecked()
{ setChecked(false); }

//----------------------------------------------------------------------
inline auto FToggleButton::isChecked() const noexcept -> bool
{ return checked; }

//----------------------------------------------------------------------
inline auto FToggleButton::hasRadiobuttonFocus() const noexcept -> bool
{ return radiobutton_focus; }

//----------------------------------------------------------------------
inline auto FToggleButton::getGroup() const -> FButtonGroup*
{ return button_group; }

//----------------------------------------------------------------------
inline void FToggleButton::setButtonWidth (std::size_t width) noexcept
{ button_width = width; }

//----------------------------------------------------------------------
inline void FToggleButton::setLabelOffsetPos (std::size_t offset) noexcept
{ label_offset_pos = offset; }

//----------------------------------------------------------------------
inline auto FToggleButton::hasGroup() const -> bool
{ return button_group; }

}  // namespace finalcut

#endif  // FTOGGLEBUTTON_H
