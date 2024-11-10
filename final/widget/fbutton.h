/***********************************************************************
* fbutton.h - Widget FButton                                           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2024 Markus Gans                                      *
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
    // Using-declaration
    using FWidget::delAccelerator;

    // Constructors
    explicit FButton (FWidget* = nullptr);
    explicit FButton (FString&&, FWidget* = nullptr);

    // Destructor
    ~FButton() override;

    // Overloaded operator
    auto operator = (const FString&) -> FButton&;

    // Accessors
    auto getClassName() const -> FString override;
    auto getText() const -> FString;

    // Mutators
    void setForegroundColor (FColor) override;
    void setBackgroundColor (FColor) override;
    void setHotkeyForegroundColor (FColor);
    void setFocusForegroundColor (FColor);
    void setFocusBackgroundColor (FColor);
    void setInactiveForegroundColor (FColor);
    void setInactiveBackgroundColor (FColor);
    void resetColors() override;
    void setNoUnderline (bool = true);
    void unsetNoUnderline();
    void setEnable (bool = true) override;
    void unsetEnable() override;
    void setDisable() override;
    void setFlat (bool = true);
    void unsetFlat();
    void setShadow (bool = true);
    void unsetShadow();
    void setDown (bool = true);
    void setUp();
    void setClickAnimation (bool = true);
    void unsetClickAnimation();
    void setText (const FString&);

    // Inquiries
    auto isFlat() const -> bool;
    auto isDown() const noexcept -> bool;
    auto hasShadow() const -> bool;
    auto hasClickAnimation() const noexcept -> bool;

    // Methods
    void hide() override;

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onTimer (FTimerEvent*) override;
    void onAccel (FAccelEvent*) override;
    void onFocusIn (FFocusEvent*) override;
    void onFocusOut (FFocusEvent*) override;

  private:
    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void init();
    void setHotkeyAccelerator();
    void detectHotkey();
    auto clickAnimationIndent (const FWidget*) -> std::size_t;
    void clearRightMargin (const FWidget*);
    void drawMarginLeft();
    void drawMarginRight();
    void drawTopBottomBackground();
    void printLeadingSpaces (std::size_t&);
    void setCursorPositionOnButton();
    void modifyStyle() const;
    void printButtonText (const FString&, std::size_t&);
    auto hasMoreCharacter (std::size_t, std::size_t) const -> bool;
    void setHotkeyStyle() const;
    void resetHotkeyStyle() const;
    void printEllipsis();
    void resetStyle() const;
    void printTrailingSpaces (std::size_t);
    void drawButtonTextLine (const FString&);
    void draw() override;
    void initializeDrawing();
    void finalizingDrawing() const;
    auto getSpaceChar() const -> wchar_t;
    void handleMonochronBackground() const;
    void drawFlatBorder();
    void drawShadow();
    void updateButtonColor();
    void processClick() const;

    // Data members
    FString      text{};
    bool         button_down{false};
    bool         active_focus{false};
    bool         click_animation{true};
    int          click_time{150};
    wchar_t      space_char{L' '};
    FColor       button_fg{FColor::Default};
    FColor       button_bg{FColor::Default};
    FColor       button_hotkey_fg{FColor::Default};
    FColor       button_focus_fg{FColor::Default};
    FColor       button_focus_bg{FColor::Default};
    FColor       button_inactive_fg{FColor::Default};
    FColor       button_inactive_bg{FColor::Default};
    std::size_t  hotkeypos{NOT_SET};
    std::size_t  indent{0};
    std::size_t  center_offset{0};
    std::size_t  vcenter_offset{0};
    std::size_t  column_width{0};
};

// FButton inline functions
//----------------------------------------------------------------------
inline auto FButton::getClassName() const -> FString
{ return "FButton"; }

//----------------------------------------------------------------------
inline auto FButton::getText() const -> FString
{ return text; }

//----------------------------------------------------------------------
inline void FButton::unsetNoUnderline()
{ setNoUnderline(false); }

//----------------------------------------------------------------------
inline void FButton::unsetEnable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FButton::setDisable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FButton::unsetFlat()
{ setFlat(false); }

//----------------------------------------------------------------------
inline void FButton::unsetShadow()
{ setShadow(false); }

//----------------------------------------------------------------------
inline void FButton::setUp()
{ setDown(false); }

//----------------------------------------------------------------------
inline void FButton::setClickAnimation(bool enable)
{ click_animation = enable; }

//----------------------------------------------------------------------
inline void FButton::unsetClickAnimation()
{ setClickAnimation(false); }

//----------------------------------------------------------------------
inline auto FButton::isFlat() const -> bool
{ return getFlags().feature.flat; }

//----------------------------------------------------------------------
inline auto FButton::isDown() const noexcept -> bool
{ return button_down; }

//----------------------------------------------------------------------
inline auto FButton::hasShadow() const -> bool
{ return getFlags().shadow.shadow; }

//----------------------------------------------------------------------
inline auto FButton::hasClickAnimation() const noexcept -> bool
{ return click_animation; }

}  // namespace finalcut

#endif  // FBUTTON_H
