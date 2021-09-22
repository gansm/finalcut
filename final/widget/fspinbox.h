/***********************************************************************
* fspinbox.h - Widget FSpinBox                                         *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2021 Markus Gans                                      *
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
 *       ▕▔▔▔▔▔▔▔▔▔▔▏
 *       ▕ FSpinBox ▏
 *       ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSPINBOX_H
#define FSPINBOX_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <limits>

#include "final/fwidget.h"
#include "final/widget/flineedit.h"

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
    using LabelOrientation = FLineEdit::LabelOrientation;

    // Constructors
    explicit FSpinBox (FWidget* = nullptr);

    // Disable copy constructor
    FSpinBox (const FSpinBox&) = delete;

    // Destructor
    ~FSpinBox() noexcept override;

    // Disable copy assignment operator (=)
    FSpinBox& operator = (const FSpinBox&) = delete;

    // Accessors
    FString             getClassName() const override;
    sInt64              getValue() const;
    FString             getPrefix() const;
    FString             getSuffix() const;
    LabelOrientation    getLabelOrientation() const;

    // Mutators
    void                setSize (const FSize&, bool = true) override;
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    bool                setEnable (bool = true) override;
    bool                unsetEnable() override;
    bool                setDisable() override;
    bool                setFocus (bool = true) override;
    bool                unsetFocus() override;
    bool                setShadow (bool = true);
    bool                unsetShadow();
    void                setValue (sInt64);
    void                setMinValue (sInt64);
    void                setMaxValue (sInt64);
    void                setRange (sInt64, sInt64);
    void                setPrefix (const FString&);
    void                setSuffix (const FString&);
    void                setLabelText (const FString&);
    void                setLabelOrientation (const LabelOrientation);

    // Inquiries
    bool                hasShadow() const;

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
    enum class SpiningState
    {
      None = 0,
      Up   = 1,
      Down = 2
    };

    // Methods
    void                init();
    void                draw() override;
    void                updateInputField();
    void                increaseValue (sInt64 = 1);
    void                decreaseValue (sInt64 = 1);
    void                processActivate() const;
    void                processChanged() const;
    void                forceFocus();

    // Callback methods
    void                cb_inputFieldActivate() const;
    void                cb_inputFieldChange (const FLineEdit&);

    // Data members
    FLineEdit           input_field{this};
    sInt64              value{0};
    sInt64              min{std::numeric_limits<sInt64>::min()};
    sInt64              max{std::numeric_limits<sInt64>::max()};
    FString             pfix{};
    FString             sfix{};
    SpiningState        spining_state{SpiningState::None};
    bool                threshold_reached{false};
    int                 threshold_time{500};
    int                 repeat_time{80};
};


// FSpinBox inline functions
//----------------------------------------------------------------------
inline FString FSpinBox::getClassName() const
{ return "FSpinBox"; }

//----------------------------------------------------------------------
inline sInt64 FSpinBox::getValue() const
{ return value; }

//----------------------------------------------------------------------
inline FString FSpinBox::getPrefix() const
{ return pfix; }

//----------------------------------------------------------------------
inline FString FSpinBox::getSuffix() const
{ return sfix; }

//----------------------------------------------------------------------
inline FLineEdit::LabelOrientation FSpinBox::getLabelOrientation() const
{ return input_field.getLabelOrientation(); }

//----------------------------------------------------------------------
inline bool FSpinBox::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FSpinBox::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FSpinBox::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FSpinBox::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FSpinBox::hasShadow() const
{ return getFlags().shadow; }

//----------------------------------------------------------------------
inline void FSpinBox::setLabelText (const FString& s)
{ input_field.setLabelText(s); }

//----------------------------------------------------------------------
inline void FSpinBox::setLabelOrientation (const LabelOrientation o)
{ input_field.setLabelOrientation(o); }

}  // namespace finalcut

#endif  // FSPINBOX_H
