/***********************************************************************
* fstatusbar.h - Widget FStatusBar and FStatusKey                      *
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
 *            ├────────────────────┐
 *            │                    │
 *       ▕▔▔▔▔▔▔▔▔▔▏               │
 *       ▕ FWindow ▏               │
 *       ▕▁▁▁▁▁▁▁▁▁▏               │
 *            ▲                    │
 *            │                    │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▏1     *▕▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FStatusBar ▏- - - -▕ FStatusKey ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▏       ▕▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSTATUSBAR_H
#define FSTATUSBAR_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <vector>

#include "final/fwindow.h"


// class forward declaration
class FStatusBar;

//----------------------------------------------------------------------
// class FStatusKey
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FStatusKey : public FWidget
{
  public:
    // Constructors
    explicit FStatusKey (FWidget* = 0);
    FStatusKey (int, const FString&, FWidget* = 0);

    // Destructor
    virtual ~FStatusKey();

    // Accessors
    virtual const char* getClassName() const;
    virtual int         getKey() const;
    virtual FString     getText() const;

    // Mutators
    void                setActive();
    void                unsetActive();
    bool                setMouseFocus(bool);
    bool                setMouseFocus();
    bool                unsetMouseFocus();

    // Inquiry
    bool                isActivated() const;
    bool                hasMouseFocus() const;

    // Event handler
    void                onAccel (FAccelEvent*);

  protected:
    // Mutators
    void                setKey (int);
    void                setText (const FString&);

  private:
    // Disable copy constructor
    FStatusKey (const FStatusKey&);

    // Disable assignment operator (=)
    FStatusKey& operator = (const FStatusKey&);

    // Methods
    void                init (FWidget*);
    void                processActivate();
    FStatusBar*         getConnectedStatusbar() const;
    void                setConnectedStatusbar (FStatusBar*);

    // Friend class
    friend class FStatusBar;

    // Data Members
    int         key;
    FString     text;
    bool        active;
    bool        mouse_focus;
    FStatusBar* bar;
};
#pragma pack(pop)


// FStatusKey inline functions
//----------------------------------------------------------------------
inline const char* FStatusKey::getClassName() const
{ return "FStatusKey"; }

//----------------------------------------------------------------------
inline int FStatusKey::getKey() const
{ return key; }

//----------------------------------------------------------------------
inline FString FStatusKey::getText() const
{ return text; }

//----------------------------------------------------------------------
inline void FStatusKey::unsetActive()
{ active = false; }

//----------------------------------------------------------------------
inline bool FStatusKey::setMouseFocus()
{ return setMouseFocus(true); }

//----------------------------------------------------------------------
inline bool FStatusKey::unsetMouseFocus()
{ return setMouseFocus(false); }

//----------------------------------------------------------------------
inline bool FStatusKey::isActivated() const
{ return active; }

//----------------------------------------------------------------------
inline bool FStatusKey::hasMouseFocus() const
{ return mouse_focus; }

//----------------------------------------------------------------------
inline void FStatusKey::setKey (int k)
{ key = k; }

//----------------------------------------------------------------------
inline void FStatusKey::setText (const FString& txt)
{ text = txt; }

//----------------------------------------------------------------------
inline FStatusBar* FStatusKey::getConnectedStatusbar() const
{ return bar; }

//----------------------------------------------------------------------
inline void FStatusKey::setConnectedStatusbar (FStatusBar* sb)
{ bar = sb; }


//----------------------------------------------------------------------
// class FStatusBar
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FStatusBar : public FWindow
{
  public:
    // Constructor
    explicit FStatusBar (FWidget* = 0);

    // Destructor
    virtual ~FStatusBar();

    // Accessors
    virtual const char* getClassName() const;
    FStatusKey*         getStatusKey (int) const;
    FString             getMessage() const;
    uInt                getCount() const;

    // Mutators
    void                activateKey (int);
    void                deactivateKey (int);
    void                setMessage (const FString&);

    // Inquiries
    bool                isActivated (int) const;
    bool                hasActivatedKey();

    // Methods
    void                hide();
    void                drawMessage();
    void                clearMessage();
    void                insert (FStatusKey*);
    void                remove (FStatusKey*);
    void                remove (int);
    void                clear();
    void                adjustSize();

    // Event handlers
    void                onMouseDown (FMouseEvent*);
    void                onMouseUp (FMouseEvent*);
    void                onMouseMove (FMouseEvent*);

    // Callback method
    void                cb_statuskey_activated (FWidget*, data_ptr);

  private:
    // Typedef
    typedef std::vector<FStatusKey*> keyList;

    // Disable copy constructor
    FStatusBar (const FStatusBar&);

    // Disable assignment operator (=)
    FStatusBar& operator = (const FStatusBar&);

    // Methods
    void                init();
    void                draw();
    void                drawKeys();
    void                drawKey (keyList::const_iterator);
    void                drawActiveKey (keyList::const_iterator);

    // Data Members
    keyList             key_list;
    FString             text;
    bool                mouse_down;
    int                 screenWidth;
    int                 keyname_len;
    int                 x;
    int                 x_msg;
};
#pragma pack(pop)


// FStatusBar inline functions
//----------------------------------------------------------------------
inline const char* FStatusBar::getClassName() const
{ return "FStatusBar"; }

//----------------------------------------------------------------------
inline FStatusKey* FStatusBar::getStatusKey (int index) const
{ return key_list[uInt(index - 1)]; }

//----------------------------------------------------------------------
inline uInt FStatusBar::getCount() const
{ return uInt(key_list.size()); }

//----------------------------------------------------------------------
inline void FStatusBar::activateKey (int index)
{ key_list[uInt(index - 1)]->setActive(); }

//----------------------------------------------------------------------
inline void FStatusBar::deactivateKey (int index)
{ key_list[uInt(index - 1)]->unsetActive(); }

//----------------------------------------------------------------------
inline bool FStatusBar::isActivated(int index) const
{ return key_list[uInt(index - 1)]->isActivated(); }

//----------------------------------------------------------------------
inline FString FStatusBar::getMessage() const
{ return text; }

//----------------------------------------------------------------------
inline void FStatusBar::clearMessage()
{ text.clear(); }

#endif  // FSTATUSBAR_H
