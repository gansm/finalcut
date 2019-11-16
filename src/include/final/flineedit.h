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
#include <utility>

#include "final/fwidget.h"

namespace finalcut
{

// class forward declaration
class FLabel;

//----------------------------------------------------------------------
// class FLineEdit
//----------------------------------------------------------------------

class FLineEdit : public FWidget
{
  public:
    // Using-declaration
    using FWidget::setGeometry;

    // Enumerations
    enum label_o
    {
      label_above = 0,
      label_left  = 1
    };

    enum inputType
    {
      textfield = 0,
      password  = 1
    };

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
    FLineEdit& operator << (const uInt64);
    FLineEdit& operator << (const sInt64);
    FLineEdit& operator << (const float);
    FLineEdit& operator << (const double);
    FLineEdit& operator << (const lDouble);
    const FLineEdit& operator >> (FString&);

    // Accessors
    const FString       getClassName() const override;
    FString             getText() const;
    std::size_t         getMaxLength() const;
    std::size_t         getCursorPosition() const;
    FLabel*             getLabelObject() const;
    label_o             getLabelOrientation();

    // Mutators
    void                setText (const FString&);
    void                setInputFilter (const FString&);
    void                clearInputFilter();
    void                setMaxLength (std::size_t);
    void                setCursorPosition (std::size_t);
    void                setLabelText (const FString&);
    void                setInputType (const inputType);
    void                setLabelOrientation (const label_o);
    void                setLabelAssociatedWidget (FWidget*);
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
    void                onWheel (FWheelEvent*) override;
    void                onTimer (FTimerEvent*) override;
    void                onAccel (FAccelEvent*) override;
    void                onHide (FHideEvent*) override;
    void                onFocusIn (FFocusEvent*) override;
    void                onFocusOut (FFocusEvent*) override;

  protected:
    void                adjustLabel();
    void                adjustSize() override;

  private:
    // Typedef
    typedef std::pair<std::size_t, std::size_t> offsetPair;

    // Enumeration
    enum dragScroll
    {
      noScroll    = 0,
      scrollLeft  = 1,
      scrollRight = 2
    };

    // Constants
    static constexpr std::size_t NOT_SET = static_cast<std::size_t>(-1);

    // Methods
    void                init();
    bool                hasHotkey();
    void                draw() override;
    void                drawInputField();
    std::size_t         printTextField();
    std::size_t         printPassword();
    std::size_t         getCursorColumnPos();
    const FString       getPasswordText() const;
    bool                isPasswordField() const;
    offsetPair          endPosToOffset (std::size_t);
    std::size_t         clickPosToCursorPos (std::size_t);
    void                adjustTextOffset();
    void                cursorLeft();
    void                cursorRight();
    void                cursorHome();
    void                cursorEnd();
    void                deleteCurrentCharacter();
    void                deletePreviousCharacter();
    void                switchInsertMode();
    void                acceptInput();
    bool                keyInput (FKey);
    wchar_t             characterFilter (const wchar_t);
    void                processActivate();
    void                processChanged();

    // Data members
    FString       text{""};
    FString       print_text{""};
    FString       label_text{""};
    FLabel*       label{};
    FWidget*      label_associated_widget{this};
    std::wstring  input_filter{};
    dragScroll    drag_scroll{FLineEdit::noScroll};
    label_o       label_orientation{FLineEdit::label_left};
    inputType     input_type{FLineEdit::textfield};
    int           scroll_repeat{100};
    bool          scroll_timer{false};
    bool          insert_mode{true};
    std::size_t   cursor_pos{NOT_SET};
    std::size_t   text_offset{0};
    std::size_t   char_width_offset{0};
    std::size_t   x_pos{0};
    std::size_t   max_length{std::numeric_limits<std::size_t>::max()};

    // Friend class
    friend class FSpinBox;
};


// FLineEdit inline functions
//----------------------------------------------------------------------
inline const FString FLineEdit::getClassName() const
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
inline FLabel* FLineEdit::getLabelObject() const
{ return label; }

//----------------------------------------------------------------------
inline FLineEdit::label_o FLineEdit::getLabelOrientation()
{ return label_orientation; }

//----------------------------------------------------------------------
inline void FLineEdit::setInputFilter (const FString& regex_string)
{ input_filter = regex_string.wc_str(); }

//----------------------------------------------------------------------
inline void FLineEdit::clearInputFilter()
{ input_filter.clear(); }

//----------------------------------------------------------------------
inline void FLineEdit::setInputType (const inputType type)
{ input_type = type; }

//----------------------------------------------------------------------
inline void FLineEdit::setLabelAssociatedWidget (FWidget* w)
{ label_associated_widget = w; }

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
