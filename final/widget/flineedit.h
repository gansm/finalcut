/***********************************************************************
* flineedit.h - Widget FLineEdit                                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2024 Markus Gans                                      *
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
#include <tuple>
#include <unordered_map>
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
    using FWidget::setVisibleCursor;
    using FWidget::unsetVisibleCursor;

    // Enumerations
    enum class LabelOrientation
    {
      Above = 0,
      Left  = 1
    };

    enum class InputType
    {
      Textfield = 0,
      Password  = 1
    };

    // Constructor
    explicit FLineEdit (FWidget* = nullptr);
    explicit FLineEdit (const FString&, FWidget* = nullptr);

    // Disable copy constructor
    FLineEdit (const FLineEdit&) = delete;

    // Disable move constructor
    FLineEdit (FLineEdit&&) noexcept = delete;

    // Destructor
    ~FLineEdit() override;

    // Disable copy assignment operator (=)
    auto operator = (const FLineEdit&) -> FLineEdit& = delete;

    // Disable move assignment operator (=)
    auto operator = (FLineEdit&&) noexcept -> FLineEdit& = delete;

    // Overloaded operators
    auto operator = (const FString&) -> FLineEdit&;
    template <typename typeT>
    auto operator << (const typeT&) -> FLineEdit&;
    auto operator << (UniChar) -> FLineEdit&;
    auto operator << (const wchar_t) -> FLineEdit&;

    friend auto operator >> (const FLineEdit& lhs, FString& rhs) -> const FLineEdit&
    {
      rhs += lhs.text;
      return lhs;
    }

    // Accessors
    auto getClassName() const -> FString override;
    auto getAlignment() const noexcept -> Align;
    auto getText() const -> FString;
    auto getMaxLength() const noexcept -> std::size_t;
    auto getCursorPosition() const noexcept -> std::size_t;
    auto getLabelObject() const -> FLabel*;
    auto getLabelOrientation() const -> LabelOrientation;

    // Mutators
    void setAlignment (Align) noexcept;
    void setText (const FString&);
    void inputText (const FString&);
    void deletesCharacter();
    void setInputFilter (const FString&);
    void clearInputFilter();
    void setMaxLength (std::size_t);
    void setCursorPosition (std::size_t);
    void moveCursorToBegin();
    void moveCursorToEnd();
    void stepCursorForward (std::size_t = 1);
    void stepCursorBackward (std::size_t = 1);
    void setLabelText (const FString&);
    void setInputType (const InputType);
    void setOverwriteMode (bool = true);
    void setLabelOrientation (const LabelOrientation);
    void setLabelAssociatedWidget (FWidget*);
    void resetColors() override;
    void setSize (const FSize&, bool = true) override;
    void setGeometry ( const FPoint&, const FSize&
                                    , bool = true ) override;
    void setEnable (bool = true) override;
    void unsetEnable() override;
    void setDisable() override;
    void setShadow (bool = true);
    void unsetShadow();
    void setReadOnly (bool = true);
    void unsetReadOnly();

    // Inquiry
    auto hasShadow() const -> bool;
    auto isReadOnly() const noexcept -> bool;

    // Methods
    void hide() override;
    void clear();

    // Event handlers
    void onKeyPress (FKeyEvent*) override;
    void onMouseDown (FMouseEvent*) override;
    void onMouseUp (FMouseEvent*) override;
    void onMouseMove (FMouseEvent*) override;
    void onWheel (FWheelEvent*) override;
    void onTimer (FTimerEvent*) override;
    void onAccel (FAccelEvent*) override;
    void onHide (FHideEvent*) override;
    void onFocusIn (FFocusEvent*) override;
    void onFocusOut (FFocusEvent*) override;

  protected:
    void adjustLabel();
    void adjustSize() override;

  private:
    // Using-declaration
    using offsetPair = std::pair<std::size_t, std::size_t>;
    using KeyMap = std::unordered_map<FKey, std::function<void()>, EnumHash<FKey>>;

    // Constants
    static constexpr auto NOT_SET = static_cast<std::size_t>(-1);
    static constexpr auto NOT_FOUND = static_cast<std::size_t>(-1);

    // Methods
    void init();
    void mapKeyFunctions();
    auto hasHotkey() const -> bool;
    auto getAlignOffset (const std::size_t) const -> std::size_t;
    void draw() override;
    void drawInputField();
    void initializeDrawing();
    void finalizingDrawing() const;
    void printTrailingSpaces (std::size_t x_pos);
    auto choosePrintMethod() -> std::tuple<std::size_t, std::size_t>;
    auto printTextField() -> std::tuple<std::size_t, std::size_t>;
    auto printPassword() -> std::tuple<std::size_t, std::size_t>;
    void drawShadow();
    auto getCursorColumnPos() const -> std::size_t;
    auto getPasswordText() const -> FString;
    auto isPasswordField() const -> bool;
    auto isFullwidthChar (std::size_t) const -> bool;
    auto getColumnWidthWithErrorHandling ( FString::const_reference
                                         , std::size_t = 0 ) const ->std::size_t;
    auto endPosToOffset (std::size_t) -> offsetPair;
    auto clickPosToCursorPos (std::size_t) -> std::size_t;
    void setCursorPositionByMouseClick (const FMouseEvent*);
    void handleAutoScroll (const FMouseEvent*);
    void handleLeftDragScroll();
    void handleRightDragScroll();
    void adjustTextOffset();
    void cursorLeft();
    void cursorRight();
    void cursorHome();
    void cursorEnd();
    void deleteCurrentCharacter();
    void deletePreviousCharacter();
    void switchInsertMode();
    void acceptInput();
    auto keyInput (FKey) -> bool;
    auto characterFilter (const wchar_t) const -> wchar_t;
    void processActivate();
    void processChanged() const;

    // Data members
    FString          text{""};
    FString          print_text{""};
    FString          label_text{""};
    FLabel*          label{};
    FWidget*         label_associated_widget{this};
    std::wstring     input_filter{};
    KeyMap           key_map{};
    DragScrollMode   drag_scroll{DragScrollMode::None};
    LabelOrientation label_orientation{LabelOrientation::Left};
    InputType        input_type{InputType::Textfield};
    int              scroll_repeat{100};
    bool             scroll_timer{false};
    bool             insert_mode{true};
    bool             read_only{false};
    Align            alignment{Align::Left};
    std::size_t      align_offset{0};
    std::size_t      cursor_pos{NOT_SET};
    std::size_t      text_offset{0};
    std::size_t      char_width_offset{0};
    std::size_t      max_length{std::numeric_limits<std::size_t>::max()};
};


// FLineEdit inline functions
//----------------------------------------------------------------------
template <typename typeT>
inline auto FLineEdit::operator << (const typeT& s) -> FLineEdit&
{
  FString str{};
  str << s;
  setText(text + str);
  return *this;
}

//----------------------------------------------------------------------
inline auto FLineEdit::getClassName() const -> FString
{ return "FLineEdit"; }

//----------------------------------------------------------------------
inline auto FLineEdit::getAlignment() const noexcept -> Align
{ return alignment; }

//----------------------------------------------------------------------
inline auto FLineEdit::getText() const -> FString
{ return text; }

//----------------------------------------------------------------------
inline auto FLineEdit::getMaxLength() const noexcept -> std::size_t
{ return max_length; }

//----------------------------------------------------------------------
inline auto FLineEdit::getCursorPosition() const noexcept -> std::size_t
{ return cursor_pos; }

//----------------------------------------------------------------------
inline auto FLineEdit::getLabelObject() const -> FLabel*
{ return label; }

//----------------------------------------------------------------------
inline auto FLineEdit::getLabelOrientation() const -> LabelOrientation
{ return label_orientation; }

//----------------------------------------------------------------------
inline void FLineEdit::setInputFilter (const FString& regex_string)
{ input_filter = regex_string.toWString(); }

//----------------------------------------------------------------------
inline void FLineEdit::clearInputFilter()
{ input_filter.clear(); }

//----------------------------------------------------------------------
inline void FLineEdit::setInputType (const InputType type)
{ input_type = type; }

//----------------------------------------------------------------------
inline void FLineEdit::setLabelAssociatedWidget (FWidget* w)
{ label_associated_widget = w; }

//----------------------------------------------------------------------
inline void FLineEdit::unsetEnable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FLineEdit::setDisable()
{ setEnable(false); }

//----------------------------------------------------------------------
inline void FLineEdit::unsetShadow()
{ setShadow(false); }

//----------------------------------------------------------------------
inline void FLineEdit::unsetReadOnly()
{ setReadOnly(true); }

//----------------------------------------------------------------------
inline auto FLineEdit::hasShadow() const -> bool
{ return getFlags().shadow.shadow; }

//----------------------------------------------------------------------
inline auto FLineEdit::isReadOnly() const noexcept -> bool
{ return read_only; }

}  // namespace finalcut

#endif  // FLINEEDIT_H
