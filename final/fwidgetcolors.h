/***********************************************************************
* fwidgetcolors.h - Set widget color theme                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2024 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FWidgetColors ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FWIDGETCOLORS_H
#define FWIDGETCOLORS_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FWidgetColors
//----------------------------------------------------------------------

class FWidgetColors
{
  public:
    // Constructor
    FWidgetColors() = default;

    // Destructor
    virtual ~FWidgetColors() noexcept;

    // Method
    virtual auto getClassName() const -> FString;
    virtual void setColorTheme() = 0;

    struct NonContextualColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
    };

    struct DialogColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor resize_fg{FColor::Default};
      FColor emphasis_fg{FColor::Default};
    };

    struct TextColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor selected_fg{FColor::Default};
      FColor selected_bg{FColor::Default};
      FColor selected_focus_fg{FColor::Default};
      FColor selected_focus_bg{FColor::Default};
    };

    struct ErrorBoxColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor emphasis_fg{FColor::Default};
    };

    struct CurrentElementColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor focus_fg{FColor::Default};
      FColor focus_bg{FColor::Default};
      FColor inc_search_fg{FColor::Default};
      FColor selected_fg{FColor::Default};
      FColor selected_bg{FColor::Default};
      FColor selected_focus_fg{FColor::Default};
      FColor selected_focus_bg{FColor::Default};
    };

    struct ListColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor selected_fg{FColor::Default};
      FColor selected_bg{FColor::Default};
    };

    struct LabelColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor inactive_fg{FColor::Default};
      FColor inactive_bg{FColor::Default};
      FColor hotkey_fg{FColor::Default};
      FColor hotkey_bg{FColor::Default};
      FColor emphasis_fg{FColor::Default};
      FColor ellipsis_fg{FColor::Default};
    };

    struct InputFieldColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor focus_fg{FColor::Default};
      FColor focus_bg{FColor::Default};
      FColor inactive_fg{FColor::Default};
      FColor inactive_bg{FColor::Default};
    };

    struct ToggleButtonColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor focus_fg{FColor::Default};
      FColor focus_bg{FColor::Default};
      FColor inactive_fg{FColor::Default};
      FColor inactive_bg{FColor::Default};
    };

    struct ButtonColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor focus_fg{FColor::Default};
      FColor focus_bg{FColor::Default};
      FColor inactive_fg{FColor::Default};
      FColor inactive_bg{FColor::Default};
      FColor hotkey_fg{FColor::Default};
    };

    struct TitlebarColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor inactive_fg{FColor::Default};
      FColor inactive_bg{FColor::Default};
      FColor button_fg{FColor::Default};
      FColor button_bg{FColor::Default};
      FColor button_focus_fg{FColor::Default};
      FColor button_focus_bg{FColor::Default};
    };

    struct MenuColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor focus_fg{FColor::Default};
      FColor focus_bg{FColor::Default};
      FColor inactive_fg{FColor::Default};
      FColor inactive_bg{FColor::Default};
      FColor hotkey_fg{FColor::Default};
      FColor hotkey_bg{FColor::Default};
    };

    struct StatusbarColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor focus_fg{FColor::Default};
      FColor focus_bg{FColor::Default};
      FColor separator_fg{FColor::Default};
      FColor hotkey_fg{FColor::Default};
      FColor hotkey_bg{FColor::Default};
      FColor focus_hotkey_fg{FColor::Default};
      FColor focus_hotkey_bg{FColor::Default};
    };

    struct ScrollbarColors
    {
      FColor fg{FColor::Default};
      FColor bg{FColor::Default};
      FColor button_fg{FColor::Default};
      FColor button_bg{FColor::Default};
      FColor button_inactive_fg{FColor::Default};
      FColor button_inactive_bg{FColor::Default};
    };

    // Data members
    NonContextualColors  term{};
    DialogColors         dialog{};
    TextColors           text{};
    ErrorBoxColors       error_box{};
    NonContextualColors  tooltip{};
    NonContextualColors  shadow{};
    CurrentElementColors current_element{};
    ListColors           list{};
    LabelColors          label{};
    InputFieldColors     input_field{};
    ToggleButtonColors   toggle_button{};
    ButtonColors         button{};
    TitlebarColors       titlebar{};
    MenuColors           menu{};
    StatusbarColors      statusbar{};
    ScrollbarColors      scrollbar{};
    NonContextualColors  progressbar{};
};

// FWidgetColors inline functions
//----------------------------------------------------------------------
inline auto FWidgetColors::getClassName() const -> FString
{ return "FWidgetColors"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *    ▕ FWidgetColors ▏
 *    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default8ColorTheme ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default8ColorTheme
//----------------------------------------------------------------------

class default8ColorTheme final : public FWidgetColors
{
  public:
    // Constructor
    default8ColorTheme();

    // Destructor
    ~default8ColorTheme() noexcept override;

    // Method
    auto getClassName() const -> FString override;
    void setColorTheme() override;

  private:
    void setTermColors();
    void setDialogColors();
    void setTextColors();
    void setErrorBoxColors();
    void setTooltipColors();
    void setShadowColors();
    void setCurrentElementColors();
    void setListColors();
    void setLabelColors();
    void setInputFieldColors();
    void setToggleButtonColors();
    void setButtonColors();
    void setTitlebarColors();
    void setMenuColors();
    void setStatusbarColors();
    void setScrollbarColors();
    void setProgressbarColors();
};

// default8ColorTheme inline functions
//----------------------------------------------------------------------
inline auto default8ColorTheme::getClassName() const -> FString
{ return "default8ColorTheme"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FWidgetColors ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *             ▲
 *             │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default16ColorTheme ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default16ColorTheme
//----------------------------------------------------------------------

class default16ColorTheme final : public FWidgetColors
{
  public:
    // Constructor
    default16ColorTheme();

    // Destructor
    ~default16ColorTheme() noexcept override;

    // Method
    auto getClassName() const -> FString override;
    void setColorTheme() override;

  private:
    void setTermColors();
    void setDialogColors();
    void setTextColors();
    void setErrorBoxColors();
    void setTooltipColors();
    void setShadowColors();
    void setCurrentElementColors();
    void setListColors();
    void setLabelColors();
    void setInputFieldColors();
    void setToggleButtonColors();
    void setButtonColors();
    void setTitlebarColors();
    void setMenuColors();
    void setStatusbarColors();
    void setScrollbarColors();
    void setProgressbarColors();
};

// default16ColorTheme inline functions
//----------------------------------------------------------------------
inline auto default16ColorTheme::getClassName() const -> FString
{ return "default16ColorTheme"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FWidgetColors ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *              ▲
 *              │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default8ColorDarkTheme ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default8ColorDarkTheme
//----------------------------------------------------------------------

class default8ColorDarkTheme final : public FWidgetColors
{
  public:
    // Constructor
    default8ColorDarkTheme();

    // Destructor
    ~default8ColorDarkTheme() noexcept override;

    // Method
    auto getClassName() const -> FString override;
    void setColorTheme() override;

  private:
    void setTermColors();
    void setDialogColors();
    void setTextColors();
    void setErrorBoxColors();
    void setTooltipColors();
    void setShadowColors();
    void setCurrentElementColors();
    void setListColors();
    void setLabelColors();
    void setInputFieldColors();
    void setToggleButtonColors();
    void setButtonColors();
    void setTitlebarColors();
    void setMenuColors();
    void setStatusbarColors();
    void setScrollbarColors();
    void setProgressbarColors();
};

// default8ColorDarkTheme inline functions
//----------------------------------------------------------------------
inline auto default8ColorDarkTheme::getClassName() const -> FString
{ return "default8ColorDarkTheme"; }


/*  Inheritance diagram
 *  ═══════════════════
 *
 *       ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWidgetColors ▏
 *       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *               ▲
 *               │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ default16ColorDarkTheme ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

//----------------------------------------------------------------------
// class default16ColorDarkTheme
//----------------------------------------------------------------------

class default16ColorDarkTheme final : public FWidgetColors
{
  public:
    // Constructor
    default16ColorDarkTheme();

    // Destructor
    ~default16ColorDarkTheme() noexcept override;

    // Method
    auto getClassName() const -> FString override;
    void setColorTheme() override;

  private:
    void setTermColors();
    void setDialogColors();
    void setTextColors();
    void setErrorBoxColors();
    void setTooltipColors();
    void setShadowColors();
    void setCurrentElementColors();
    void setListColors();
    void setLabelColors();
    void setInputFieldColors();
    void setToggleButtonColors();
    void setButtonColors();
    void setTitlebarColors();
    void setMenuColors();
    void setStatusbarColors();
    void setScrollbarColors();
    void setProgressbarColors();
};

// default16ColorDarkTheme inline functions
//----------------------------------------------------------------------
inline auto default16ColorDarkTheme::getClassName() const -> FString
{ return "default16ColorDarkTheme"; }

}  // namespace finalcut

#endif  // FWIDGETCOLORS_H
