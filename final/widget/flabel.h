/***********************************************************************
* flabel.cpp - Widget FLabel                                           *
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
 *       ▕▔▔▔▔▔▔▔▔▏
 *       ▕ FLabel ▏
 *       ▕▁▁▁▁▁▁▁▁▏
 */

#ifndef FLABEL_H
#define FLABEL_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <vector>

#include "final/fwidget.h"
#include "final/fwidgetcolors.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FLabel
//----------------------------------------------------------------------

class FLabel : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setEnable;
    using FWidget::delAccelerator;

    // Constructor
    explicit FLabel (FWidget* = nullptr);
    explicit FLabel (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FLabel (const FLabel&) = delete;

    // Disable move constructor
    FLabel (FLabel&&) noexcept = delete;

    // Destructor
    ~FLabel() noexcept override;

    // Disable copy assignment operator (=)
    auto operator = (const FLabel&) -> FLabel& = delete;

    // Disable move assignment operator (=)
    auto operator = (FLabel&&) noexcept -> FLabel& = delete;

    // Overloaded operators
    auto operator = (const FString&) -> FLabel&;
    template <typename typeT>
    auto operator << (const typeT&) -> FLabel&;
    auto operator << (UniChar) -> FLabel&;
    auto operator << (const wchar_t) -> FLabel&;

    friend inline auto operator >> (const FLabel& lhs, FString& rhs) -> const FLabel&
    {
      rhs += lhs.text;
      return lhs;
    }

    // Accessors
    auto getClassName() const -> FString override;
    auto getAccelWidget() -> FWidget*;
    auto getAlignment() const noexcept -> Align;
    auto getText() & -> FString&;

    // Mutators
    void setAccelWidget (FWidget* = nullptr);
    void setAlignment (Align) noexcept;
    void setEmphasis (bool = true) noexcept;
    void unsetEmphasis() noexcept;
    void resetColors() override;
    void setReverseMode (bool = true) noexcept;
    void unsetReverseMode() noexcept;
    void setEnable (bool = true) override;
    void setNumber (uLong);
    void setNumber (long);
    void setNumber (float, int = FLT_DIG);
    void setNumber (double, int = DBL_DIG);
    void setNumber (lDouble, int = LDBL_DIG);
    void setText (const FString&);

    // Inquiries
    auto hasEmphasis() const noexcept -> bool;
    auto hasReverseMode() const noexcept -> bool;

    // Methods
    void hide() override;
    void clear();

    // Event handlers
    void onMouseDown (FMouseEvent*) override;
    void onAccel (FAccelEvent*) override;

    // Callback method
    void cb_accelWidgetDestroyed();

  private:
    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void init();
    void setHotkeyAccelerator();
    auto getAlignOffset (const std::size_t) const -> std::size_t;
    void draw() override;
    void drawMultiLine();
    void drawSingleLine();
    void printHotkeyChar (wchar_t);
    void printLineContent (FString&, std::size_t);
    void printLine (FString&);

    // Data members
    FStringList  multiline_text{};
    FString      text{};
    FWidget*     accel_widget{nullptr};
    Align        alignment{Align::Left};
    std::size_t  align_offset{0};
    std::size_t  hotkeypos{NOT_SET};
    std::size_t  column_width{0};
    FColor       emphasis_color{FColor::Default};
    FColor       ellipsis_color{FColor::Default};
    bool         multiline{false};
    bool         emphasis{false};
    bool         reverse_mode{false};
};

// FLabel inline functions
//----------------------------------------------------------------------
template <typename typeT>
inline auto FLabel::operator << (const typeT& s) -> FLabel&
{
  FString str{};
  str << s;
  setText(text + str);
  return *this;
}

//----------------------------------------------------------------------
inline auto FLabel::getClassName() const -> FString
{ return "FLabel"; }

//----------------------------------------------------------------------
inline auto FLabel::getAccelWidget () -> FWidget*
{ return accel_widget; }

//----------------------------------------------------------------------
inline auto FLabel::getAlignment() const noexcept -> Align
{ return alignment; }

//----------------------------------------------------------------------
inline auto FLabel::getText() & -> FString&
{ return text; }

//----------------------------------------------------------------------
inline void FLabel::setEmphasis (bool enable) noexcept
{ emphasis = enable; }

//----------------------------------------------------------------------
inline void FLabel::unsetEmphasis() noexcept
{ setEmphasis(false); }

//----------------------------------------------------------------------
inline void FLabel::setReverseMode (bool enable) noexcept
{ reverse_mode = enable; }

//----------------------------------------------------------------------
inline void FLabel::unsetReverseMode() noexcept
{ setReverseMode(false); }

//----------------------------------------------------------------------
inline void FLabel::setNumber (uLong num)
{ setText(FString().setNumber(num)); }

//----------------------------------------------------------------------
inline void FLabel::setNumber (long num)
{ setText(FString().setNumber(num)); }

//----------------------------------------------------------------------
inline void FLabel::setNumber (float num, int precision)
{ setText(FString().setNumber(num, precision)); }

//----------------------------------------------------------------------
inline void FLabel::setNumber (double num, int precision)
{ setText(FString().setNumber(num, precision)); }

//----------------------------------------------------------------------
inline void FLabel::setNumber (lDouble num, int precision)
{ setText(FString().setNumber(num, precision)); }

//----------------------------------------------------------------------
inline auto FLabel::hasEmphasis() const noexcept -> bool
{ return emphasis; }

//----------------------------------------------------------------------
inline auto FLabel::hasReverseMode() const noexcept -> bool
{ return reverse_mode; }

//----------------------------------------------------------------------
inline void FLabel::clear()
{ text.clear(); }

}  // namespace finalcut

#endif  // FLABEL_H
