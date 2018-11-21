/***********************************************************************
* ftogglebutton.h - Intermediate base class for a toggle button        *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2018 Markus Gans                                      *
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

#pragma pack(push)
#pragma pack(1)

class FToggleButton : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Constructors
    explicit FToggleButton (FWidget* = 0);
    explicit FToggleButton (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FToggleButton();

    // Accessors
    virtual const char* getClassName() const;
    FString&      getText();

    // Mutators
    virtual void  setGeometry (int, int, std::size_t, std::size_t, bool = true);
    bool          setNoUnderline (bool);
    bool          setNoUnderline();
    bool          unsetNoUnderline();
    virtual bool  setEnable (bool);
    virtual bool  setEnable();
    virtual bool  unsetEnable();
    virtual bool  setDisable();
    virtual bool  setFocus (bool);
    virtual bool  setFocus();
    virtual bool  unsetFocus();
    bool          setChecked (bool);
    bool          setChecked();
    bool          unsetChecked();
    virtual void  setText (const FString&);

    // Inquiries
    bool          isChecked();

    // Methods
    virtual void  hide();

    // Event handlers
    virtual void  onMouseDown (FMouseEvent*);
    virtual void  onMouseUp (FMouseEvent*);
    virtual void  onWheel (FWheelEvent*);
    virtual void  onAccel (FAccelEvent*);
    virtual void  onFocusIn (FFocusEvent*);
    virtual void  onFocusOut (FFocusEvent*);

  protected:
    // Accessor
    uChar         getHotkey();
    FButtonGroup* getGroup() const;

    // Mutator
    void          setHotkeyAccelerator();

    // Inquiries
    bool          isRadioButton() const;
    bool          isCheckboxButton() const;
    bool          hasGroup() const;

    // Methods
    virtual void  draw();
    void          drawLabel();
    void          processClick();
    void          processToggle();

    // Event handler
    virtual void onKeyPress (FKeyEvent*);

    // Data Members
    bool          checked;
    std::size_t   label_offset_pos;
    std::size_t   button_width;  // plus margin spaces

  private:
    // Constants
    static const std::size_t NOT_SET = static_cast<std::size_t>(-1);

    // Disable copy constructor
    FToggleButton (const FToggleButton&);

    // Disable assignment operator (=)
    FToggleButton& operator = (const FToggleButton&);

    // Mutator
    void          setGroup (FButtonGroup*);

    // Methods
    void          init();
    std::size_t   getHotkeyPos (wchar_t[], wchar_t[], std::size_t);
    void          drawText (wchar_t[], std::size_t , std::size_t);

    // Friend classes
    friend class FButtonGroup;

    // Data Members
    FButtonGroup* button_group;
    bool          focus_inside_group;
    FString       text;
};
#pragma pack(pop)


// FRadioButton inline functions
//----------------------------------------------------------------------
inline const char* FToggleButton::getClassName() const
{ return "FToggleButton"; }

//----------------------------------------------------------------------
inline FString& FToggleButton::getText()
{ return text; }

//----------------------------------------------------------------------
inline bool FToggleButton::setNoUnderline()
{ return setNoUnderline(true); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetNoUnderline()
{ return setNoUnderline(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::setChecked()
{ return setChecked(true); }

//----------------------------------------------------------------------
inline bool FToggleButton::unsetChecked()
{ return setChecked(false); }

//----------------------------------------------------------------------
inline bool FToggleButton::isChecked()
{ return checked; }

//----------------------------------------------------------------------
inline FButtonGroup* FToggleButton::getGroup() const
{ return button_group; }

//----------------------------------------------------------------------
inline bool FToggleButton::hasGroup() const
{ return button_group; }

}  // namespace finalcut

#endif  // FTOGGLEBUTTON_H
