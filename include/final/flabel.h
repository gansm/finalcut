// File: flabel.h
// Provides: class FLabel
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
//       ▕▔▔▔▔▔▔▔▔▏
//       ▕ FLabel ▏
//       ▕▁▁▁▁▁▁▁▁▏

#ifndef FLABEL_H
#define FLABEL_H

#include <vector>
#include "fwidget.h"


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
    void               setNumber(long);
    void               setText (const FString&);

    // Inquiries
    bool               hasEmphasis();
    bool               hasReverseMode();

    // Methods
    void               hide();

    // Event handlers
    void               onMouseDown (FMouseEvent*);
    void               onAccel (FAccelEvent*);

    // Callback method
    void               cb_accel_widget_destroyed (FWidget*, data_ptr);

  private:
    // Typedef
    typedef std::vector<FString> multiLineText;

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
    multiLineText      multiline_text;
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
inline bool FLabel::hasEmphasis()
{ return emphasis; }

//----------------------------------------------------------------------
inline bool FLabel::hasReverseMode()
{ return reverse_mode; }

#endif  // FLABEL_H
