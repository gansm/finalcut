// File: fmenuitem.h
// Provides: class FMenuItem
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
//      ▕▔▔▔▔▔▔▔▔▔▔▔▏*       1▕▔▔▔▔▔▔▔▏
//      ▕ FMenuItem ▏-┬- - - -▕ FMenu ▏
//      ▕▁▁▁▁▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▏
//                    :
//                    :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
//                    └- - - -▕ FMenuList ▏
//                            ▕▁▁▁▁▁▁▁▁▁▁▁▏

#ifndef FMENUITEM_H
#define FMENUITEM_H

#include "fwidget.h"

// class forward declaration
class FDialog;
class FMenu;
class FMenuList;

//----------------------------------------------------------------------
// class FMenuItem
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMenuItem : public FWidget
{
  public:
    // Using-declarations
    using FWidget::addAccelerator;
    using FWidget::delAccelerator;
    using FWidget::setEnable;

    // Constructor
    explicit FMenuItem (FWidget* = 0);
    FMenuItem (const FString&, FWidget* = 0);
    FMenuItem (int, const FString&, FWidget* = 0);

    // Destructor
    virtual ~FMenuItem();

    // Accessors
    const char* getClassName() const;
    int         getHotkey() const;
    FMenu*      getMenu() const;
    uInt        getTextLength() const;
    FString     getText() const;

    // Mutators
    bool        setEnable (bool);
    bool        setFocus (bool);
    bool        setFocus();
    bool        unsetFocus();
    void        setSelected();
    void        unsetSelected();
    void        setSeparator();
    void        unsetSeparator();
    void        setChecked();
    void        unsetChecked();
    void        setMenu (FMenu*);
    void        setText (const FString&);

    // Inquiries
    bool        isSelected() const;
    bool        isSeparator() const;
    bool        isChecked() const;
    bool        hasHotkey() const;
    bool        hasMenu() const;

    // Methods
    void        addAccelerator (int, FWidget*);
    void        delAccelerator (FWidget*);
    void        openMenu();

    // Event handlers
    void        onKeyPress (FKeyEvent*);
    void        onMouseDoubleClick (FMouseEvent*);
    void        onMouseDown (FMouseEvent*);
    void        onMouseUp (FMouseEvent*);
    void        onMouseMove (FMouseEvent*);
    void        onAccel (FAccelEvent*);
    void        onFocusIn (FFocusEvent*);
    void        onFocusOut (FFocusEvent*);

  protected:
    // Accessor
    FWidget*    getSuperMenu() const;

    // Mutator
    void        setSuperMenu (FWidget*);

    // Inquiries
    bool        isWindowsMenu (FWidget*) const;
    bool        isMenuBar (FWidget*) const;
    bool        isMenu (FWidget*) const;

    // Data Members
    FString     text;
    bool        selected;
    bool        separator;
    bool        checkable;
    bool        checked;
    bool        radio_button;
    bool        dialog_index;
    uInt        text_length;
    int         hotkey;
    int         accel_key;
    FMenu*      menu;
    FWidget*    super_menu;
    FDialog*    associated_window;

  private:
    // Disable copy constructor
    FMenuItem (const FMenuItem&);

    // Disable assignment operator (=)
    FMenuItem& operator = (const FMenuItem&);

    // Methods
    void        init (FWidget*);
    uChar       hotKey();
    void        processActivate();
    void        processDeactivate();
    void        createDialogList (FMenu*);

    // Callback methods
    void        cb_switchToDialog (FWidget*, data_ptr);
    void        cb_destroyDialog (FWidget*, data_ptr);

    virtual void processClicked();

    // Friend classes
    friend class FDialogListMenu;
    friend class FMenuList;
    friend class FMenuBar;
    friend class FMenu;
};
#pragma pack(pop)


// FMenuItem inline functions
//----------------------------------------------------------------------
inline const char* FMenuItem::getClassName() const
{ return "FMenuItem"; }

//----------------------------------------------------------------------
inline int FMenuItem::getHotkey() const
{ return hotkey; }

//----------------------------------------------------------------------
inline FMenu* FMenuItem::getMenu() const
{ return menu; }

//----------------------------------------------------------------------
inline uInt FMenuItem::getTextLength() const
{ return text_length; }

//----------------------------------------------------------------------
inline FString FMenuItem::getText() const
{ return text; }

//----------------------------------------------------------------------
inline bool FMenuItem::setFocus()
{ return setFocus(true); }

//----------------------------------------------------------------------
inline bool FMenuItem::unsetFocus()
{ return setFocus(false); }

//----------------------------------------------------------------------
inline void FMenuItem::setSeparator()
{
  separator = true;
  unsetFocusable();
}

//----------------------------------------------------------------------
inline void FMenuItem::unsetSeparator()
{
  separator = false;
  setFocusable();
}

//----------------------------------------------------------------------
inline void FMenuItem::setChecked()
{ checked = true; }

//----------------------------------------------------------------------
inline void FMenuItem::unsetChecked()
{ checked = false; }

//----------------------------------------------------------------------
inline void FMenuItem::setMenu(FMenu* m)
{ menu = m; }

//----------------------------------------------------------------------
inline bool FMenuItem::isSelected() const
{ return selected; }

//----------------------------------------------------------------------
inline bool FMenuItem::isSeparator() const
{ return separator; }

//----------------------------------------------------------------------
inline bool FMenuItem::isChecked() const
{ return checked; }

//----------------------------------------------------------------------
inline bool FMenuItem::hasHotkey() const
{ return bool(hotkey != 0); }

//----------------------------------------------------------------------
inline bool FMenuItem::hasMenu() const
{ return bool(menu != 0); }

//----------------------------------------------------------------------
inline FWidget* FMenuItem::getSuperMenu() const
{ return super_menu; }

//----------------------------------------------------------------------
inline void FMenuItem::setSuperMenu (FWidget* smenu)
{ super_menu = smenu; }

#endif  // FMENUITEM_H
