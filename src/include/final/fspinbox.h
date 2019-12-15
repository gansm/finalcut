/***********************************************************************
* fspinbox.h - Widget FSpinBox                                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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
 *       ▕▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FSpinBox ▏
 *       ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSPINBOX_H
#define FSPINBOX_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FLineEdit;

//----------------------------------------------------------------------
// class FSpinBox
//----------------------------------------------------------------------

class FSpinBox : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Constructors
    explicit FSpinBox (FWidget* = nullptr);

    // Disable copy constructor
    FSpinBox (const FSpinBox&) = delete;

    // Destructor
    ~FSpinBox();

    // Disable assignment operator (=)
    FSpinBox& operator = (const FSpinBox&) = delete;

    // Accessors
    sInt64              getValue();
    FString             getPrefix() const;
    FString             getSuffix() const;
    FLineEdit::label_o  getLabelOrientation();

    // Mutators
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    bool                setEnable (bool) override;
    bool                setEnable() override;
    bool                unsetEnable() override;
    bool                setDisable() override;
    bool                setFocus (bool) override;
    bool                setFocus() override;
    bool                unsetFocus() override;
    bool                setShadow (bool);
    bool                setShadow();
    bool                unsetShadow();
    void                setValue (sInt64);
    void                setMinValue (sInt64);
    void                setMaxValue (sInt64);
    void                setRange (sInt64, sInt64);
    void                setPrefix (const FString&);
    void                setSuffix (const FString&);
    void                setLabelText (const FString&);
    void                setLabelOrientation (const FLineEdit::label_o);

    // Inquiries
    bool                hasShadow();

    // Methods
    void                hide() override;

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onWheel (FWheelEvent*) override;
    void                onTimer (FTimerEvent*) override;

  private:
    // Enumeration
    enum spiningState
    {
      noSpin   = 0,
      spinUp   = 1,
      spinDown = 2
    };

    // Methods
    void                init();
    void                draw() override;
    void                updateInputField();
    void                increaseValue();
    void                decreaseValue();
    void                processChanged();
    void                forceFocus();

    // Callback methods
    void                cb_inputFieldChange (finalcut::FWidget*, FDataPtr);

    // Data members
    FLineEdit           input_field{this};
    sInt64              value{0};
    sInt64              min{std::numeric_limits<sInt64>::min()};
    sInt64              max{std::numeric_limits<sInt64>::max()};
    FString             pfix{};
    FString             sfix{};
    spiningState        spining_state{FSpinBox::noSpin};
    bool                threshold_reached{false};
    int                 threshold_time{500};
    int                 repeat_time{10};
};


// FSpinBox inline functions
//----------------------------------------------------------------------
inline sInt64 FSpinBox::getValue()
{ return value; }

//----------------------------------------------------------------------
inline FString FSpinBox::getPrefix() const
{ return pfix; }

//----------------------------------------------------------------------
inline FString FSpinBox::getSuffix() const
{ return sfix; }

//----------------------------------------------------------------------
inline FLineEdit::label_o FSpinBox::getLabelOrientation()
{ return input_field.getLabelOrientation(); }

//----------------------------------------------------------------------
inline bool FSpinBox::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FSpinBox::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FSpinBox::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FSpinBox::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FSpinBox::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FSpinBox::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FSpinBox::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FSpinBox::hasShadow()
{ return getFlags().shadow; }

//----------------------------------------------------------------------
inline void FSpinBox::setLabelText (const FString& s)
{ input_field.setLabelText(s); }

//----------------------------------------------------------------------
inline void FSpinBox::setLabelOrientation (const FLineEdit::label_o o)
{ input_field.setLabelOrientation(o); }

}  // namespace finalcut

#endif  // FSPINBOX_H
