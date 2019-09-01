/***********************************************************************
* flineedit.h - Widget FLineEdit                                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2012-2019 Markus Gans                                      *
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

#include <limits>
#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FLabel;

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

    // Using-declaration
    using FWidget::setGeometry;

    // Constructor
    explicit FLineEdit (FWidget* = nullptr);
    explicit FLineEdit (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FLineEdit (const FLineEdit&) = delete;

    // Destructor
    virtual ~FLineEdit();

    // Disable assignment operator (=)
    FLineEdit& operator = (const FLineEdit&) = delete;

    // Overloaded operators
    FLineEdit& operator = (const FString&);
    FLineEdit& operator << (const FString&);
    FLineEdit& operator << (fc::SpecialCharacter);
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
    const char*         getClassName() const override;
    FString             getText() const;
    std::size_t         getMaxLength() const;
    std::size_t         getCursorPosition() const;
    int                 getLabelOrientation();

    // Mutators
    void                setText (const FString&);
    void                setInputFilter (const FString&);
    void                clearInputFilter();
    void                setMaxLength (std::size_t);
    void                setCursorPosition (std::size_t);
    void                setLabelText (const FString&);
    void                setLabelOrientation (const label_o);
    void                setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    bool                setEnable(bool) override;
    bool                setEnable() override;
    bool                unsetEnable() override;
    bool                setDisable() override;
    bool                setFocus(bool) override;
    bool                setFocus() override;
    bool                unsetFocus() override;
    bool                setShadow(bool);
    bool                setShadow();
    bool                unsetShadow();

    // Inquiry
    bool                hasShadow();

    // Methods
    void                hide() override;
    void                clear();

    // Event handlers
    void                onKeyPress (FKeyEvent*) override;
    void                onMouseDown (FMouseEvent*) override;
    void                onMouseUp (FMouseEvent*) override;
    void                onMouseMove (FMouseEvent*) override;
    void                onTimer (FTimerEvent*) override;
    void                onAccel (FAccelEvent*) override;
    void                onHide (FHideEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onFocusOut (FFocusEvent*) override;

  protected:
    void                adjustLabel();
    void                adjustSize() override;

  private:
    // Enumeration
    enum dragScroll
    {
      noScroll    = 0,
      scrollLeft  = 1,
      scrollRight = 2
    };

    // Methods
    void                init();
    bool                hasHotkey();
    void                draw() override;
    void                drawInputField();
    void                keyLeft();
    void                keyRight();
    void                keyHome();
    void                keyEnd();
    void                keyDel();
    void                keyBackspace();
    void                keyInsert();
    void                keyEnter();
    bool                keyInput (FKey);
    wchar_t             characterFilter (const wchar_t);
    void                processActivate();
    void                processChanged();

    // Data Members
    FString      text{""};
    FString      label_text{""};
    FLabel*      label{};
    label_o      label_orientation{FLineEdit::label_left};
    std::wstring input_filter{};
    dragScroll   drag_scroll{FLineEdit::noScroll};
    bool         scroll_timer{false};
    int          scroll_repeat{100};
    bool         insert_mode{true};
    std::size_t  cursor_pos{0};
    std::size_t  text_offset{0};
    std::size_t  max_length{std::numeric_limits<std::size_t>::max()};
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
inline std::size_t FLineEdit::getMaxLength() const
{ return max_length; }

//----------------------------------------------------------------------
inline std::size_t FLineEdit::getCursorPosition() const
{ return cursor_pos; }

//----------------------------------------------------------------------
inline int FLineEdit::getLabelOrientation()
{ return int(label_orientation); }

//----------------------------------------------------------------------
inline void FLineEdit::setInputFilter (const FString& regex_string)
{ input_filter = regex_string.wc_str(); }

//----------------------------------------------------------------------
inline void FLineEdit::clearInputFilter()
{ input_filter.clear(); }

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
{ return getFlags().shadow; }

}  // namespace finalcut

#endif  // FLINEEDIT_H
