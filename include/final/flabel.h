/************************************************************************
* flabel.cpp - Widget FLabel                                            *
*                                                                       *
* This file is part of the Final Cut widget toolkit                     *
*                                                                       *
* Copyright 2014-2017 Markus Gans                                       *
*                                                                       *
* The Final Cut is free software; you can redistribute it and/or modify *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation; either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* The Final Cut is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

/*  Inheritance diagram
 *  ═══════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
 * ▕ FObject ▏ ▕  FTerm  ▏
 * ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
 *      ▲           ▲
 *      │           │
 *      └─────┬─────┘
 *            │
 *       ▕▔▔▔▔▔▔▔▔▏
 *       ▕ FVTerm ▏
 *       ▕▁▁▁▁▁▁▁▁▏
 *            ▲
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

#include <vector>

#include "final/fwidget.h"


//----------------------------------------------------------------------
// class FLabel
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FLabel : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setEnable;

    // Constructor
    explicit FLabel (FWidget* = 0);
    FLabel (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FLabel();

    // Overloaded operators
    FLabel& operator = (const FString&);
    FLabel& operator << (const FString&);
    FLabel& operator << (const wchar_t);
    FLabel& operator << (const uInt);
    FLabel& operator << (const int);
    FLabel& operator << (const uLong);
    FLabel& operator << (const long);
    FLabel& operator << (const float);
    FLabel& operator << (const double);
    FLabel& operator << (const lDouble);
    const FLabel& operator >> (FString&);

    // Accessors
    const char*        getClassName() const;
    FTerm*             getAccelWidget();
    fc::text_alignment getAlignment();
    FString&           getText();

    // Mutators
    void               setAccelWidget (FWidget* = 0);
    void               setAlignment(fc::text_alignment);
    bool               setEmphasis(bool);
    bool               setEmphasis();
    bool               unsetEmphasis();
    bool               setReverseMode(bool);
    bool               setReverseMode();
    bool               unsetReverseMode();
    bool               setEnable (bool);
    void               setNumber (uLong);
    void               setNumber (long);
    void               setNumber (float, int = FLT_DIG);
    void               setNumber (double, int = DBL_DIG);
    void               setNumber (lDouble, int = LDBL_DIG);
    void               setText (const FString&);

    // Inquiries
    bool               hasEmphasis();
    bool               hasReverseMode();

    // Methods
    void               hide();
    void               clear();

    // Event handlers
    void               onMouseDown (FMouseEvent*);
    void               onAccel (FAccelEvent*);

    // Callback method
    void               cb_accel_widget_destroyed (FWidget*, data_ptr);

  private:
    // Disable copy constructor
    FLabel (const FLabel&);

    // Disable assignment operator (=)
    FLabel& operator = (const FLabel&);

    // Methods
    void               init();
    uChar              getHotkey();
    int                getHotkeyPos (wchar_t*&, wchar_t*&, uInt);
    void               setHotkeyAccelerator();
    int                getAlignOffset (int);
    void               printLine (wchar_t*&, uInt, int, int = 0);
    void               draw();

    // Data Members
    FStringList        multiline_text;
    bool               multiline;
    FString            text;
    fc::text_alignment alignment;
    short              emphasis_color;
    short              ellipsis_color;
    bool               emphasis;
    bool               reverse_mode;
    FWidget*           accel_widget;
};
#pragma pack(pop)


// FLabel inline functions
//----------------------------------------------------------------------
inline const char* FLabel::getClassName() const
{ return "FLabel"; }

//----------------------------------------------------------------------
inline FTerm* FLabel::getAccelWidget ()
{ return accel_widget; }

//----------------------------------------------------------------------
inline fc::text_alignment FLabel::getAlignment()
{ return alignment; }

//----------------------------------------------------------------------
inline FString& FLabel::getText()
{ return text; }

//----------------------------------------------------------------------
inline bool FLabel::setEmphasis()
{ return setEmphasis(true); }

//----------------------------------------------------------------------
inline bool FLabel::unsetEmphasis()
{ return setEmphasis(false); }

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

#endif  // FLABEL_H
