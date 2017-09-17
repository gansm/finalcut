// File: fbutton.h
// Provides: class FButton
//
//  Inheritance diagram
//  ═══════════════════
//
// ▕▔▔▔▔▔▔▔▔▔▏ ▕▔▔▔▔▔▔▔▔▔▏
// ▕ FObject ▏ ▕  FTerm  ▏
// ▕▁▁▁▁▁▁▁▁▁▏ ▕▁▁▁▁▁▁▁▁▁▏
//      ▲           ▲
//      │           │
//      └─────┬─────┘
//            │
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FVTerm ▏
//       ▕▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FWidget ▏
//       ▕▁▁▁▁▁▁▁▁▁▏
//            ▲
//            │
//       ▕▔▔▔▔▔▔▔▔▔▏
//       ▕ FButton ▏
//       ▕▁▁▁▁▁▁▁▁▁▏

#ifndef FBUTTON_H
#define FBUTTON_H

#include "fwidget.h"


//----------------------------------------------------------------------
// class FButton
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FButton : public FWidget
{
  public:
    // Constructors
    explicit FButton (FWidget* = 0);
    FButton (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FButton();

    // Accessors
    const char*  getClassName() const;
    FString&     getText();

    // Mutators
    void         setForegroundColor (short);
    void         setBackgroundColor (short);
    void         setHotkeyForegroundColor (short);
    void         setFocusForegroundColor (short);
    void         setFocusBackgroundColor (short);
    void         setInactiveForegroundColor (short);
    void         setInactiveBackgroundColor (short);
    bool         setNoUnderline(bool);
    bool         setNoUnderline();
    bool         unsetNoUnderline();
    bool         setEnable(bool);
    bool         setEnable();
    bool         unsetEnable();
    bool         setDisable();
    bool         setFocus(bool);
    bool         setFocus();
    bool         unsetFocus();
    bool         setFlat(bool);
    bool         setFlat();
    bool         unsetFlat();
    bool         setShadow(bool);
    bool         setShadow();
    bool         unsetShadow();
    bool         setDown(bool);
    bool         setDown();
    bool         setUp();
    bool         setClickAnimation(bool);
    bool         setClickAnimation();
    bool         unsetClickAnimation();
    void         setText (const FString&);

    // Inquiries
    bool         isFlat() const;
    bool         isDown() const;
    bool         hasShadow() const;
    bool         hasClickAnimation();

    // Methods
    void         hide();

    // Event handlers
    void         onKeyPress (FKeyEvent*);
    void         onMouseDown (FMouseEvent*);
    void         onMouseUp (FMouseEvent*);
    void         onMouseMove (FMouseEvent*);
    void         onTimer (FTimerEvent*);
    void         onAccel (FAccelEvent*);
    void         onFocusIn (FFocusEvent*);
    void         onFocusOut (FFocusEvent*);

  private:
    // Disable copy constructor
    FButton (const FButton&);

    // Disable assignment operator (=)
    FButton& operator = (const FButton&);

    // Methods
    void         init();
    uChar        getHotkey();
    void         setHotkeyAccelerator();
    void         detectHotkey();
    void         draw();
    void         updateButtonColor();
    void         processClick();

    // Data Members
    FString      text;
    bool         button_down;
    bool         click_animation;
    int          click_time;
    short        button_fg;
    short        button_bg;
    short        button_hotkey_fg;
    short        button_focus_fg;
    short        button_focus_bg;
    short        button_inactive_fg;
    short        button_inactive_bg;
};
#pragma pack(pop)


// FButton inline functions
//----------------------------------------------------------------------
inline const char* FButton::getClassName() const
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
inline bool FButton::setClickAnimation(bool on)
{ return click_animation = on; }

//----------------------------------------------------------------------
inline bool FButton::setClickAnimation()
{ return setClickAnimation(true); }

//----------------------------------------------------------------------
inline bool FButton::unsetClickAnimation()
{ return setClickAnimation(false); }

//----------------------------------------------------------------------
inline bool FButton::isFlat() const
{ return ((flags & fc::flat) != 0); }

//----------------------------------------------------------------------
inline bool FButton::isDown() const
{ return button_down; }

//----------------------------------------------------------------------
inline bool FButton::hasShadow() const
{ return ((flags & fc::shadow) != 0); }

//----------------------------------------------------------------------
inline bool FButton::hasClickAnimation()
{ return click_animation; }

#endif  // FBUTTON_H
