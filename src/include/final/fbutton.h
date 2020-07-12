/***********************************************************************
* fbutton.h - Widget FButton                                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2020 Markus Gans                                      *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FButton ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FBUTTON_H
#define FBUTTON_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidget.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FButton
//----------------------------------------------------------------------

class FButton : public FWidget
{
  public:
    // Constructors
    explicit FButton (FWidget* = nullptr);
    explicit FButton (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FButton (const FButton&) = delete;

    // Destructor
    ~FButton() override;

    // Disable copy assignment operator (=)
    FButton& operator = (const FButton&) = delete;

    // Overloaded operator
    FButton& operator = (const FString&);

    // Accessors
    const FString       getClassName() const override;
    FString&            getText();

    // Mutators
    void                setForegroundColor (FColor) override;
    void                setBackgroundColor (FColor) override;
    void                setHotkeyForegroundColor (FColor);
    void                setFocusForegroundColor (FColor);
    void                setFocusBackgroundColor (FColor);
    void                setInactiveForegroundColor (FColor);
    void                setInactiveBackgroundColor (FColor);
    void                resetColors() override;
    bool                setNoUnderline(bool);
    bool                setNoUnderline();
    bool                unsetNoUnderline();
    bool                setEnable(bool) override;
    bool                setEnable() override;
    bool                unsetEnable() override;
    bool                setDisable() override;
    bool                setFocus(bool) override;
    bool                setFocus() override;
    bool                unsetFocus() override;
    bool                setFlat(bool);
    bool                setFlat();
    bool                unsetFlat();
    bool                setShadow(bool);
    bool                setShadow();
    bool                unsetShadow();
    bool                setDown(bool);
    bool                setDown();
    bool                setUp();
    bool                setClickAnimation(bool);
    bool                setClickAnimation();
    bool                unsetClickAnimation();
    void                setText (const FString&);

    // Inquiries
    bool                isFlat() const;
    bool                isDown() const;
    bool                hasShadow() const;
    bool                hasClickAnimation() const;

    // Methods
    void                hide() override;

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;
    void                onTimer (FTimerEvent*) override;
    void                onAccel (FAccelEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onFocusOut (FFocusEvent*) override;

  private:
    // Constants
    static constexpr std::size_t NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void                init();
    void                setHotkeyAccelerator();
    void                detectHotkey();
    std::size_t         clickAnimationIndent (const FWidget*);
    void                clearRightMargin (const FWidget*);
    void                drawMarginLeft();
    void                drawMarginRight();
    void                drawTopBottomBackground();
    void                drawButtonTextLine (const FString&);
    void                draw() override;
    void                updateStatusBar() const;
    void                updateButtonColor();
    void                processClick();

    // Data members
    FString      text{};
    bool         button_down{false};
    bool         active_focus{false};
    bool         click_animation{true};
    int          click_time{150};
    int          space_char{int(' ')};
    FColor       button_fg{fc::Default};
    FColor       button_bg{fc::Default};
    FColor       button_hotkey_fg{fc::Default};
    FColor       button_focus_fg{fc::Default};
    FColor       button_focus_bg{fc::Default};
    FColor       button_inactive_fg{fc::Default};
    FColor       button_inactive_bg{fc::Default};
    std::size_t  hotkeypos{NOT_SET};
    std::size_t  indent{0};
    std::size_t  center_offset{0};
    std::size_t  vcenter_offset{0};
    std::size_t  column_width{0};
};

// FButton inline functions
//----------------------------------------------------------------------
inline const FString FButton::getClassName() const
{ return "FButton"; }

//----------------------------------------------------------------------
inline FString& FButton::getText()
{ return text; }

//----------------------------------------------------------------------
inline bool FButton::setNoUnderline()
{ return setNoUnderline(true); }

//----------------------------------------------------------------------
inline bool FButton::unsetNoUnderline()
{ return setNoUnderline(false); }

//----------------------------------------------------------------------
inline bool FButton::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FButton::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FButton::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FButton::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FButton::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FButton::setFlat()
{ return setFlat(true); }

//----------------------------------------------------------------------
inline bool FButton::unsetFlat()
{ return setFlat(false); }

//----------------------------------------------------------------------
inline bool FButton::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FButton::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FButton::setDown()
{ return setDown(true); }

//----------------------------------------------------------------------
inline bool FButton::setUp()
{ return setDown(false); }

//----------------------------------------------------------------------
inline bool FButton::setClickAnimation(bool enable)
{ return (click_animation = enable); }

//----------------------------------------------------------------------
inline bool FButton::setClickAnimation()
{ return setClickAnimation(true); }

//----------------------------------------------------------------------
inline bool FButton::unsetClickAnimation()
{ return setClickAnimation(false); }

//----------------------------------------------------------------------
inline bool FButton::isFlat() const
{ return getFlags().flat; }

//----------------------------------------------------------------------
inline bool FButton::isDown() const
{ return button_down; }

//----------------------------------------------------------------------
inline bool FButton::hasShadow() const
{ return getFlags().shadow; }

//----------------------------------------------------------------------
inline bool FButton::hasClickAnimation() const
{ return click_animation; }

}  // namespace finalcut

#endif  // FBUTTON_H
