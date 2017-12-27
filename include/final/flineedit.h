/***********************************************************************
* flineedit.h - Widget FLineEdit                                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2017 Markus Gans                                      *
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
 *      ▕▔▔▔▔▔▔▔▔▔▔▔▏
 *      ▕ FLineEdit ▏
 *      ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FLINEEDIT_H
#define FLINEEDIT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fwidget.h"
#include "final/flabel.h"


//----------------------------------------------------------------------
// class FLineEdit
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FLineEdit : public FWidget
{
  public:
    // Enumeration
    enum label_o
    {
      label_above = 0,
      label_left = 1
    };

    // Constructor
    explicit FLineEdit (FWidget* = 0);
    FLineEdit (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FLineEdit();

    // Overloaded operators
    FLineEdit& operator = (const FString&);
    FLineEdit& operator << (const FString&);
    FLineEdit& operator << (const wchar_t);
    FLineEdit& operator << (const uInt);
    FLineEdit& operator << (const int);
    FLineEdit& operator << (const uLong);
    FLineEdit& operator << (const long);
    FLineEdit& operator << (const float);
    FLineEdit& operator << (const double);
    FLineEdit& operator << (const lDouble);
    const FLineEdit& operator >> (FString&);

    // Accessors
    const char* getClassName() const;
    FString     getText() const;
    int         getLabelOrientation();

    // Mutators
    void        setText (const FString&);
    void        setLabelText (const FString&);
    void        setLabelOrientation(const label_o);
    bool        setEnable(bool);
    bool        setEnable();
    bool        unsetEnable();
    bool        setDisable();
    bool        setFocus(bool);
    bool        setFocus();
    bool        unsetFocus();
    bool        setShadow(bool);
    bool        setShadow();
    bool        unsetShadow();

    // Inquiry
    bool        hasShadow();

    // Methods
    void        hide();
    void        clear();

    // Event handlers
    void        onKeyPress (FKeyEvent*);
    void        onMouseDown (FMouseEvent*);
    void        onMouseUp (FMouseEvent*);
    void        onMouseMove (FMouseEvent*);
    void        onTimer (FTimerEvent*);
    void        onAccel (FAccelEvent*);
    void        onHide (FHideEvent*);
    void        onFocusIn (FFocusEvent*);
    void        onFocusOut (FFocusEvent*);

  protected:
    void        adjustLabel();
    void        adjustSize();

  private:
    // Enumeration
    enum dragScroll
    {
      noScroll    = 0,
      scrollLeft  = 1,
      scrollRight = 2
    };

    // Disable copy constructor
    FLineEdit (const FLineEdit&);

    // Disable assignment operator (=)
    FLineEdit& operator = (const FLineEdit&);

    // Methods
    void        init();
    bool        hasHotkey();
    void        draw();
    void        drawInputField();
    void        keyLeft();
    void        keyRight();
    void        keyHome();
    void        keyEnd();
    void        keyDel();
    void        keyBackspace();
    void        keyInsert();
    void        keyEnter();
    bool        keyInput (int);
    void        processActivate();
    void        processChanged();

    // Data Members
    FString     text;
    FString     label_text;
    FLabel*     label;
    label_o     label_orientation;
    dragScroll  drag_scroll;
    bool        scroll_timer;
    int         scroll_repeat;
    bool        insert_mode;
    int         cursor_pos;
    int         text_offset;
};
#pragma pack(pop)


// FLineEdit inline functions
//----------------------------------------------------------------------
inline const char* FLineEdit::getClassName() const
{ return "FLineEdit"; }

//----------------------------------------------------------------------
inline FString FLineEdit::getText() const
{ return text; }

//----------------------------------------------------------------------
inline int FLineEdit::getLabelOrientation()
{ return int(label_orientation); }

//----------------------------------------------------------------------
inline bool FLineEdit::setEnable()
{ return setEnable(true); }

//----------------------------------------------------------------------
inline bool FLineEdit::unsetEnable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FLineEdit::setDisable()
{ return setEnable(false); }

//----------------------------------------------------------------------
inline bool FLineEdit::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FLineEdit::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline bool FLineEdit::setShadow()
{ return setShadow(true); }

//----------------------------------------------------------------------
inline bool FLineEdit::unsetShadow()
{ return setShadow(false); }

//----------------------------------------------------------------------
inline bool FLineEdit::hasShadow()
{ return ((flags & fc::shadow) != 0); }

#endif  // FLINEEDIT_H
