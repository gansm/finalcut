/***********************************************************************
* ftogglebutton.h - Intermediate base class for a toggle button        *
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
    FToggleButton& operator = (const FToggleButton&) = delete;

    // Disable move assignment operator (=)
    FToggleButton& operator = (FToggleButton&&) noexcept = delete;
    // Accessors
    FString             getClassName() const override;
    FString&            getText() &;

    // Mutators
    void                setSize (const FSize&, bool = true) override;
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    void                resetColors() override;
    bool                setNoUnderline (bool = true);
    bool                unsetNoUnderline();
    bool                setEnable (bool = true) override;
    bool                unsetEnable() override;
    bool                setDisable() override;
    bool                setFocus (bool = true) override;
    bool                unsetFocus() override;
    bool                setChecked (bool = true);
    bool                unsetChecked();
    virtual void        setText (const FString&);

    // Inquiries
    bool                isChecked() const noexcept;

    // Method
    void                hide() override;

    // Event handlers
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onWheel (FWheelEvent*) override;
    void                onAccel (FAccelEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onFocusOut (FFocusEvent*) override;

  protected:
    // Accessor
    FButtonGroup*       getGroup() const;

    // Mutator
    void                setHotkeyAccelerator();
    void                setButtonWidth (std::size_t) noexcept;
    void                setLabelOffsetPos (std::size_t) noexcept;

    // Inquiries
    bool                isRadioButton() const;
    bool                isCheckboxButton() const;
    bool                hasGroup() const;

    // Methods
    void                draw() override;
    void                drawLabel();
    void                processClick() const;
    void                processToggle() const;

    // Event handler
    void                onKeyPress (FKeyEvent*) override;

  private:
    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Mutator
    void                setGroup (FButtonGroup*);

    // Methods
    void                init();
    void                drawText (const FString&, std::size_t);
    void                correctSize (FSize&) const;

    // Data members
    FButtonGroup* button_group{nullptr};
    FString       text{};
    std::size_t   label_offset_pos{0};
    std::size_t   button_width{0};  // plus margin spaces
    bool          focus_inside_group{true};
    bool          checked{false};

    // Friend classes
    friend class FButtonGroup;
};

// FRadioButton inline functions
//----------------------------------------------------------------------
inline FString FToggleButton::getClassName() const
{ return "FToggleButton"; }

//----------------------------------------------------------------------
inline FString& FToggleButton::getText() &
{ return text; }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetNoUnderline()
{ return setNoUnderline(false); }
//----------------------------------------------------------------------
inline bool FToggleButton::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetChecked()
{ return setChecked(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::isChecked() const noexcept
{ return checked; }

//----------------------------------------------------------------------
inline FButtonGroup* FToggleButton::getGroup() const
{ return button_group; }

//----------------------------------------------------------------------
inline void FToggleButton::setButtonWidth (std::size_t width) noexcept
{ button_width = width; }

//----------------------------------------------------------------------
inline void FToggleButton::setLabelOffsetPos (std::size_t offset) noexcept
{ label_offset_pos = offset; }

//----------------------------------------------------------------------
inline bool FToggleButton::hasGroup() const
{ return button_group; }

}  // namespace finalcut

#endif  // FTOGGLEBUTTON_H
