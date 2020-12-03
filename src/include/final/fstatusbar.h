/***********************************************************************
* fstatusbar.h - Widget FStatusBar and FStatusKey                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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

#include <memory>
#include <vector>

#include "final/fwidget.h"
#include "final/fwindow.h"

namespace finalcut
{

// class forward declaration
class FStatusBar;

//----------------------------------------------------------------------
// class FStatusKey
//----------------------------------------------------------------------

class FStatusKey : public FWidget
{
  public:
    // Constructors
    explicit FStatusKey (FWidget* = nullptr);
    FStatusKey (FKey, const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FStatusKey (const FStatusKey&) = delete;

    // Destructor
    ~FStatusKey() override;

    // Disable copy assignment operator (=)
    FStatusKey& operator = (const FStatusKey&) = delete;

    // Accessors
    FString             getClassName() const override;
    virtual FKey        getKey() const;
    virtual FString     getText() const;

    // Mutators
    void                setKey (FKey);
    void                setText (const FString&);
    void                setActive();
    void                unsetActive();
    bool                setMouseFocus(bool);
    bool                setMouseFocus();
    bool                unsetMouseFocus();

    // Inquiry
    bool                isActivated() const;
    bool                hasMouseFocus() const;

    // Event handler
    void                onAccel (FAccelEvent*) override;

  private:
    // Methods
    void                init();
    void                processActivate() const;
    FStatusBar*         getConnectedStatusbar() const;
    void                setConnectedStatusbar (FStatusBar*);

    // Data members
    FString     text{};
    FStatusBar* bar{nullptr};
    FKey        key{0};
    bool        active{false};
    bool        mouse_focus{false};

    // Friend class
    friend class FStatusBar;
};


// FStatusKey inline functions
//----------------------------------------------------------------------
inline FString FStatusKey::getClassName() const
{ return "FStatusKey"; }

//----------------------------------------------------------------------
inline FKey FStatusKey::getKey() const
{ return key; }

//----------------------------------------------------------------------
inline FString FStatusKey::getText() const
{ return text; }

//----------------------------------------------------------------------
inline void FStatusKey::setKey (FKey k)
{ key = k; }

//----------------------------------------------------------------------
inline void FStatusKey::setText (const FString& txt)
{ text.setString(txt); }

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
inline FStatusBar* FStatusKey::getConnectedStatusbar() const
{ return bar; }

//----------------------------------------------------------------------
inline void FStatusKey::setConnectedStatusbar (FStatusBar* sb)
{ bar = sb; }


//----------------------------------------------------------------------
// class FStatusBar
//----------------------------------------------------------------------

class FStatusBar : public FWindow
{
  public:
    // Constructor
    explicit FStatusBar (FWidget* = nullptr);

    // Disable copy constructor
    FStatusBar (const FStatusBar&) = delete;

    // Destructor
    ~FStatusBar() override;

    // Disable copy assignment operator (=)
    FStatusBar& operator = (const FStatusBar&) = delete;

    // Accessors
    FString             getClassName() const override;
    FStatusKey*         getStatusKey (int) const;
    FString             getMessage() const;
    std::size_t         getCount() const;

    // Mutators
    void                activateKey (int);
    void                deactivateKey (int);
    void                setMessage (const FString&);
    void                resetColors() override;

    // Inquiries
    bool                isActivated (int) const;
    bool                hasActivatedKey() const;

    // Methods
    void                hide() override;
    void                drawMessage();
    void                clearMessage();
    void                insert (FStatusKey*);
    void                remove (FStatusKey*);
    void                remove (int);
    void                clear();
    void                adjustSize() override;

    // Event handlers
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;

    // Callback method
    void                cb_statuskey_activated (const FStatusKey*);

  private:
    // Using-declaration
    using FKeyList = std::vector<FStatusKey*>;

    // Methods
    void                init();
    int                 getKeyNameWidth (const FStatusKey*) const;
    int                 getKeyTextWidth (const FStatusKey*) const;
    void                draw() override;
    void                drawKeys();
    void                drawKey (FKeyList::const_iterator);
    void                drawActiveKey (FKeyList::const_iterator);

    // Data members
    FKeyList            key_list{};
    FString             text{""};
    std::size_t         screenWidth{80};
    int                 keyname_len{0};
    int                 x{-1};
    int                 x_msg{-1};
    bool                mouse_down{};
};


// FStatusBar inline functions
//----------------------------------------------------------------------
inline FString FStatusBar::getClassName() const
{ return "FStatusBar"; }

//----------------------------------------------------------------------
inline FStatusKey* FStatusBar::getStatusKey (int index) const
{ return key_list[uInt(index - 1)]; }

//----------------------------------------------------------------------
inline std::size_t FStatusBar::getCount() const
{ return key_list.size(); }

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

}  // namespace finalcut

#endif  // FSTATUSBAR_H
