/***********************************************************************
* fstatusbar.h - Widget FStatusBar and FStatusKey                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2014-2024 Markus Gans                                      *
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
#include "final/widget/fwindow.h"

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
    // Using-declaration
    using FWidget::delAccelerator;

    // Constructors
    explicit FStatusKey (FWidget* = nullptr);
    FStatusKey (FKey, FString&&, FWidget* = nullptr);

    // Disable copy constructor
    FStatusKey (const FStatusKey&) = delete;

    // Disable move constructor
    FStatusKey (FStatusKey&&) noexcept = delete;

    // Destructor
    ~FStatusKey() override;

    // Disable copy assignment operator (=)
    auto operator = (const FStatusKey&) -> FStatusKey& = delete;

    // Disable move assignment operator (=)
    auto operator = (FStatusKey&&) noexcept -> FStatusKey& = delete;

    // Accessors
    auto getClassName() const -> FString override;
    virtual auto getKey() const noexcept -> FKey;
    virtual auto getText() const -> FString;

    // Mutators
    void setKey (FKey) noexcept;
    void setText (const FString&);
    void setActive();
    void unsetActive() noexcept;
    void setMouseFocus (bool = true);
    void unsetMouseFocus();

    // Inquiry
    auto isActivated() const noexcept -> bool;
    auto hasMouseFocus() const noexcept -> bool;

    // Event handler
    void onAccel (FAccelEvent*) override;

  private:
    // Methods
    void init();
    void processActivate() const;
    auto getConnectedStatusbar() const -> FStatusBar*;
    void setConnectedStatusbar (FStatusBar*);

    // Data members
    FString      text{};
    FStatusBar*  bar{nullptr};
    FKey         key{};
    bool         active{false};
    bool         mouse_focus{false};

    // Friend class
    friend class FStatusBar;
};


// FStatusKey inline functions
//----------------------------------------------------------------------
inline auto FStatusKey::getClassName() const -> FString
{ return "FStatusKey"; }

//----------------------------------------------------------------------
inline auto FStatusKey::getKey() const noexcept -> FKey
{ return key; }

//----------------------------------------------------------------------
inline auto FStatusKey::getText() const -> FString
{ return text; }

//----------------------------------------------------------------------
inline void FStatusKey::setKey (FKey k) noexcept
{ key = k; }

//----------------------------------------------------------------------
inline void FStatusKey::setText (const FString& txt)
{ text.setString(txt); }

//----------------------------------------------------------------------
inline void FStatusKey::unsetActive() noexcept
{ active = false; }

//----------------------------------------------------------------------
inline void FStatusKey::unsetMouseFocus()
{ setMouseFocus(false); }

//----------------------------------------------------------------------
inline auto FStatusKey::isActivated() const noexcept -> bool
{ return active; }

//----------------------------------------------------------------------
inline auto FStatusKey::hasMouseFocus() const noexcept -> bool
{ return mouse_focus; }

//----------------------------------------------------------------------
inline auto FStatusKey::getConnectedStatusbar() const -> FStatusBar*
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
    // Using-declaration
    using FWidget::delAccelerator;

    // Constructor
    explicit FStatusBar (FWidget* = nullptr);

    // Destructor
    ~FStatusBar() override;

    // Accessors
    auto getClassName() const -> FString override;
    auto getStatusKey (int) const -> FStatusKey*;
    auto getMessage() const -> FString;
    auto getCount() const -> std::size_t;

    // Mutators
    void activateKey (int);
    void deactivateKey (int);
    void setMessage (const FString&);
    void resetColors() override;

    // Inquiries
    auto isActivated (int) const -> bool;
    auto hasActivatedKey() const -> bool;

    // Methods
    void hide() override;
    void drawMessage();
    void clearMessage();
    void insert (FStatusKey*);
    void remove (FStatusKey*);
    void remove (int);
    void clear();
    void adjustSize() override;

    // Event handlers
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;

    // Callback method
    void cb_statuskey_activated (const FStatusKey*);

  private:
    // Using-declaration
    using FKeyList = std::vector<FStatusKey*>;

    // Methods
    void init();
    auto getKeyNameWidth (const FStatusKey*) const -> int;
    auto getKeyTextWidth (const FStatusKey*) const -> int;
    void draw() override;
    void drawKeys();
    void drawKey (FKeyList::const_iterator);
    void drawKeySeparator (FKeyList::const_iterator);
    void drawActiveKey (FKeyList::const_iterator);
    void drawHotKeyName (FKeyList::const_iterator, const FColorPair);
    auto canPrintLeftActiveKeySeparator (FKeyList::const_iterator) const -> bool;
    auto canPrintKeySeparator (FKeyList::const_iterator) const -> bool;
    auto canDrawMessage() const -> bool;
    auto isLastActiveFocus() const -> bool;
    auto isClickInsideRange (const FMouseEvent*, const int, const int) const -> bool;
    void setStatusBarColor() const;
    void nonLeftButtonClick();
    void leftButtonClick (const FMouseEvent*);
    void printMessageWithEllipsis (std::size_t);
    void printPaddingSpaces (int);

    // Data members
    FKeyList     key_list{};
    FString      text{""};
    std::size_t  screenWidth{80};
    int          keyname_len{0};
    int          x{-1};
    int          x_msg{-1};
    bool         mouse_down{};
};


// FStatusBar inline functions
//----------------------------------------------------------------------
inline auto FStatusBar::getClassName() const -> FString
{ return "FStatusBar"; }

//----------------------------------------------------------------------
inline auto FStatusBar::getStatusKey (int index) const -> FStatusKey*
{ return key_list[uInt(index - 1)]; }

//----------------------------------------------------------------------
inline auto FStatusBar::getCount() const -> std::size_t
{ return key_list.size(); }

//----------------------------------------------------------------------
inline void FStatusBar::activateKey (int index)
{ key_list[uInt(index - 1)]->setActive(); }

//----------------------------------------------------------------------
inline void FStatusBar::deactivateKey (int index)
{ key_list[uInt(index - 1)]->unsetActive(); }

//----------------------------------------------------------------------
inline auto FStatusBar::isActivated(int index) const -> bool
{ return key_list[uInt(index - 1)]->isActivated(); }

//----------------------------------------------------------------------
inline auto FStatusBar::getMessage() const -> FString
{ return text; }

//----------------------------------------------------------------------
inline void FStatusBar::clearMessage()
{ text.clear(); }

}  // namespace finalcut

#endif  // FSTATUSBAR_H
