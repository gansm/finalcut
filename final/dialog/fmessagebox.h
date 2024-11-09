/***********************************************************************
* fmessagebox.h - Widget FMessageBox (a text message window)           *
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
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FWindow ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *       ▕▔▔▔▔▔▔▔▔▔▏
 *       ▕ FDialog ▏
 *       ▕▁▁▁▁▁▁▁▁▁▏
 *            ▲
 *            │
 *     ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *     ▕ FMessageBox ▏
 *     ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FMESSAGEBOX_H
#define FMESSAGEBOX_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <array>
#include <cstring>
#include <memory>
#include <utility>

#include "final/dialog/fdialog.h"
#include "final/fwidgetcolors.h"
#include "final/widget/fbutton.h"

namespace finalcut
{

// class forward declaration
class FButton;

//----------------------------------------------------------------------
// class FMessageBox
//----------------------------------------------------------------------

class FMessageBox : public FDialog
{
  public:
    // Enumeration
    enum class ButtonType : std::size_t
    {
      Reject = 0,
      Ok     = 1,
      Cancel = 2,
      Yes    = 3,
      No     = 4,
      Abort  = 5,
      Retry  = 6,
      Ignore = 7
    };

    // Constructors
    explicit FMessageBox (FWidget* = nullptr);

    FMessageBox ( const FString&, FString&&
                , ButtonType, ButtonType, ButtonType
                , FWidget* = nullptr );

    // Destructor
    ~FMessageBox() noexcept override;

    // Accessor
    auto getClassName() const -> FString override;
    auto getTitlebarText() const -> FString;
    auto getHeadline() const -> FString;
    auto getText() const -> FString override;

    // Mutator
    void setTitlebarText (const FString&);
    void setHeadline (const FString&);
    void setCenterText (bool = true);
    void unsetCenterText();
    void setText (const FString&) override;

    // Methods
    auto exec() -> ButtonType;
    template <typename messageType>
    static auto info ( FWidget*
                     , const FString&
                     , const messageType&
                     , ButtonType = ButtonType::Ok
                     , ButtonType = ButtonType::Reject
                     , ButtonType = ButtonType::Reject ) -> ButtonType;

    template <typename messageType>
    static auto error ( FWidget*
                      , const messageType&
                      , ButtonType = ButtonType::Ok
                      , ButtonType = ButtonType::Reject
                      , ButtonType = ButtonType::Reject ) -> ButtonType;
   protected:
    // Method
    void initLayout() override;
    void adjustSize() override;
    void done (ButtonType);

    // Callback method
    void cb_processClick (ButtonType);

  private:
    // Constants
    static constexpr std::size_t GAP = 4;
    static constexpr std::size_t MAX_BUTTONS = 3;
    static constexpr std::size_t PADDING = 5;

    // Using-declaration
    using FButtons = std::array<std::unique_ptr<FButton>, MAX_BUTTONS>;
    using FButtonsDigit = std::array<ButtonType, MAX_BUTTONS>;

    // Methods
    void init();
    void allocation();
    void initCallbacks();
    void calculateDimensions();
    void draw() override;
    auto getButtonLengthArray() const -> std::array<std::size_t, MAX_BUTTONS>;
    auto calculateMaxButtonSize() const -> std::size_t;
    void resizeButtons() const;
    void adjustButtons();
    auto calculateTotalButtonWidth() const -> std::size_t;
    void adjustWindowSize (std::size_t);
    void setButtonPositions (std::size_t);

    // Data members
    FString       headline_text{};
    FString       text{};
    FStringList   text_components{};
    FButtons      button{};
    std::size_t   max_line_width{0};
    FColor        emphasis_color{getColorTheme()->dialog.emphasis_fg};
    ButtonType    result_code{ButtonType::Reject};
    FButtonsDigit button_digit{};
    std::size_t   num_buttons{0};
    std::size_t   text_num_lines{0};
    bool          center_text{false};
};


// FMessageBox inline functions
//----------------------------------------------------------------------
inline auto FMessageBox::getClassName() const -> FString
{ return "FMessageBox"; }

//----------------------------------------------------------------------
inline auto FMessageBox::getTitlebarText() const -> FString
{
  const FString& title = FDialog::getText();  // initialize text
  return title;
}

//----------------------------------------------------------------------
inline auto FMessageBox::getHeadline() const -> FString
{ return headline_text; }

//----------------------------------------------------------------------
inline auto FMessageBox::getText() const -> FString
{ return text; }

//----------------------------------------------------------------------
inline void FMessageBox::setTitlebarText (const FString& txt)
{ return FDialog::setText(txt); }

//----------------------------------------------------------------------
inline void FMessageBox::setCenterText (bool enable)
{ center_text = enable; }

//----------------------------------------------------------------------
inline void FMessageBox::unsetCenterText()
{ setCenterText(false); }

//----------------------------------------------------------------------
template <typename messageType>
auto FMessageBox::info ( FWidget* parent
                       , const FString& caption
                       , const messageType& message
                       , ButtonType button0
                       , ButtonType button1
                       , ButtonType button2 ) -> ButtonType
{
  FMessageBox mbox ( caption
                   , std::move(FString() << message)
                   , button0, button1, button2
                   , parent );
  const ButtonType reply = mbox.exec();
  return reply;
}

//----------------------------------------------------------------------
template <typename messageType>
auto FMessageBox::error ( FWidget* parent
                        , const messageType& message
                        , ButtonType button0
                        , ButtonType button1
                        , ButtonType button2 ) -> ButtonType
{
  const FString caption{"Error message"};

  FMessageBox mbox ( caption
                   , std::move(FString() << message)
                   , button0, button1, button2
                   , parent );
  FVTerm::getFOutput()->beep();
  mbox.setHeadline("Warning:");
  mbox.setCenterText();
  const auto& wc_error_box = getColorTheme()->error_box;
  mbox.setForegroundColor(wc_error_box.fg);
  mbox.setBackgroundColor(wc_error_box.bg);
  mbox.emphasis_color = wc_error_box.emphasis_fg;
  const ButtonType reply = mbox.exec();
  return reply;
}

}  // namespace finalcut

#endif  // FMESSAGEBOX_H
