/***********************************************************************
* flabel.cpp - Widget FLabel                                           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2019 Markus Gans                                      *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
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
 * ▕▔▔▔▔▔▔▔▔▔▏
 * ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲
 *      │
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

    // Constructor
    explicit FLabel (FWidget* = nullptr);
    explicit FLabel (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FLabel (const FLabel&) = delete;

    // Destructor
    virtual ~FLabel();

    // Disable assignment operator (=)
    FLabel& operator = (const FLabel&) = delete;

    // Overloaded operators
    FLabel& operator = (const FString&);
    FLabel& operator << (const FString&);
    FLabel& operator << (fc::SpecialCharacter);
    FLabel& operator << (const wchar_t);
    FLabel& operator << (const uInt);
    FLabel& operator << (const int);
    FLabel& operator << (const uInt64);
    FLabel& operator << (const sInt64);
    FLabel& operator << (const float);
    FLabel& operator << (const double);
    FLabel& operator << (const lDouble);
    const FLabel& operator >> (FString&);

    // Accessors
    const FString       getClassName() const override;
    FWidget*            getAccelWidget();
    fc::text_alignment  getAlignment();
    FString&            getText();

    // Mutators
    void                setAccelWidget (FWidget* = nullptr);
    void                setAlignment (fc::text_alignment);
    bool                setEmphasis (bool);
    bool                setEmphasis();
    bool                unsetEmphasis();
    bool                setReverseMode (bool);
    bool                setReverseMode();
    bool                unsetReverseMode();
    bool                setEnable (bool) override;
    void                setNumber (uLong);
    void                setNumber (long);
    void                setNumber (float, int = FLT_DIG);
    void                setNumber (double, int = DBL_DIG);
    void                setNumber (lDouble, int = LDBL_DIG);
    void                setText (const FString&);

    // Inquiries
    bool                hasEmphasis();
    bool                hasReverseMode();

    // Methods
    void                hide() override;
    void                clear();

    // Event handlers
    void                onMouseDown (FMouseEvent*) override;
    void                onAccel (FAccelEvent*) override;

    // Callback method
    void                cb_accel_widget_destroyed (FWidget*, FDataPtr);

  private:
    // Constants
    static constexpr std::size_t NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void                init();
    void                setHotkeyAccelerator();
    std::size_t         getAlignOffset (std::size_t);
    void                draw() override;
    void                drawMultiLine();
    void                drawSingleLine();
    void                printLine (FString&&);

    // Data members
    FStringList         multiline_text{};
    FString             text{};
    FWidget*            accel_widget{nullptr};
    fc::text_alignment  alignment{fc::alignLeft};
    std::size_t         align_offset{0};
    std::size_t         hotkeypos{NOT_SET};
    std::size_t         column_width{0};
    FColor              emphasis_color{getFWidgetColors().label_emphasis_fg};
    FColor              ellipsis_color{getFWidgetColors().label_ellipsis_fg};
    bool                multiline{false};
    bool                emphasis{false};
    bool                reverse_mode{false};
};

// FLabel inline functions
//----------------------------------------------------------------------
inline const FString FLabel::getClassName() const
{ return "FLabel"; }

//----------------------------------------------------------------------
inline FWidget* FLabel::getAccelWidget ()
{ return accel_widget; }

//----------------------------------------------------------------------
inline fc::text_alignment FLabel::getAlignment()
{ return alignment; }

//----------------------------------------------------------------------
inline FString& FLabel::getText()
{ return text; }

//----------------------------------------------------------------------
inline bool FLabel::setEmphasis (bool enable)
{ return (emphasis = enable); }

//----------------------------------------------------------------------
inline bool FLabel::setEmphasis()
{ return setEmphasis(true); }

//----------------------------------------------------------------------
inline bool FLabel::unsetEmphasis()
{ return setEmphasis(false); }

//----------------------------------------------------------------------
inline bool FLabel::setReverseMode (bool enable)
{ return (reverse_mode = enable); }

//----------------------------------------------------------------------
inline bool FLabel::setReverseMode()
{ return setReverseMode(true); }

//----------------------------------------------------------------------
inline bool FLabel::unsetReverseMode()
{ return setReverseMode(false); }

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
inline bool FLabel::hasEmphasis()
{ return emphasis; }

//----------------------------------------------------------------------
inline bool FLabel::hasReverseMode()
{ return reverse_mode; }

//----------------------------------------------------------------------
inline void FLabel::clear()
{ text.clear(); }

}  // namespace finalcut

#endif  // FLABEL_H
