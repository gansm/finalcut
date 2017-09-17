// File: ftogglebutton.h
// Provides: class FToggleButton
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
//    ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
//    ▕ FToggleButton ▏
//    ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef FTOGGLEBUTTON_H
#define FTOGGLEBUTTON_H

#include "fwidget.h"


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
    FToggleButton (const FString&, FWidget* = 0);

    // Destructor
    virtual ~FToggleButton();

    // Accessors
    virtual const char* getClassName() const;
    FString&      getText();

    // Mutators
    void          setGeometry (int, int, int, int, bool = true);
    bool          setNoUnderline (bool);
    bool          setNoUnderline();
    bool          unsetNoUnderline();
    bool          setEnable (bool);
    bool          setEnable();
    bool          unsetEnable();
    bool          setDisable();
    bool          setFocus (bool);
    bool          setFocus();
    bool          unsetFocus();
    bool          setChecked (bool);
    bool          setChecked();
    bool          unsetChecked();
    virtual void  setText (const FString&);

    // Inquiries
    bool          isChecked();

    // Methods
    void          hide();

    // Event handlers
    void          onMouseDown (FMouseEvent*);
    void          onMouseUp (FMouseEvent*);
    void          onWheel (FWheelEvent*);
    void          onAccel (FAccelEvent*);
    void          onFocusIn (FFocusEvent*);
    void          onFocusOut (FFocusEvent*);

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
    int           label_offset_pos;
    int           button_width;  // plus margin spaces

  private:
    // Disable copy constructor
    FToggleButton (const FToggleButton&);

    // Disable assignment operator (=)
    FToggleButton& operator = (const FToggleButton&);

    // Mutator
    void          setGroup (FButtonGroup*);

    // Methods
    void          init();

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

#endif  // FTOGGLEBUTTON_H
