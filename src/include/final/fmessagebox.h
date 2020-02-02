/***********************************************************************
* fmessagebox.h - Widget FMessageBox (a text message window)           *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2014-2020 Markus Gans                                      *
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

#include <cstring>

#include "final/fdialog.h"
#include "final/fwidgetcolors.h"

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
    enum ButtonType
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
    FMessageBox (const FMessageBox&);  // copy constructor
    FMessageBox ( const FString&, const FString&
                , int, int, int
                , FWidget* = nullptr );
    // Destructor
    virtual ~FMessageBox();

    // Assignment operator (=)
    FMessageBox& operator = (const FMessageBox&);

    // Accessor
    const FString       getClassName() const override;
    const FString       getTitlebarText() const;
    const FString       getHeadline() const;
    const FString       getText() const;

    // Mutator
    void                setTitlebarText (const FString&);
    void                setHeadline (const FString&);
    bool                setCenterText(bool);
    bool                setCenterText();
    bool                unsetCenterText();
    void                setText (const FString&);

    // Methods
    template <typename messageType>
    static int          info ( FWidget*
                             , const FString&
                             , const messageType&
                             , int = FMessageBox::Ok
                             , int = 0
                             , int = 0 );

    template <typename messageType>
    static int          error ( FWidget*
                              , const messageType&
                              , int = FMessageBox::Ok
                              , int = 0
                              , int = 0 );
   protected:
    // Method
    void                adjustSize() override;

    // Callback method
    void                cb_processClick (FWidget*, FDataPtr);

  private:
    // Methods
    void                init (int, int, int);
    void                allocation (int, int, int);
    void                deallocation();
    void                initCallbacks();
    void                calculateDimensions();
    void                draw() override;
    void                resizeButtons();
    void                adjustButtons();

    // Data members
    FString       headline_text{};
    FString       text{};
    FStringList   text_components{};
    FButton*      button[3]{nullptr};
    std::size_t   max_line_width{0};
    FColor        emphasis_color{getFWidgetColors().dialog_emphasis_fg};
    int           button_digit[3]{0};
    uInt          num_buttons{0};
    std::size_t   text_num_lines{0};
    bool          center_text{false};
};


// FMessageBox inline functions
//----------------------------------------------------------------------
inline const FString FMessageBox::getClassName() const
{ return "FMessageBox"; }

//----------------------------------------------------------------------
inline const FString FMessageBox::getTitlebarText() const
{ return FDialog::getText(); }

//----------------------------------------------------------------------
inline const FString FMessageBox::getHeadline() const
{ return headline_text; }

//----------------------------------------------------------------------
inline const FString FMessageBox::getText() const
{ return text; }

//----------------------------------------------------------------------
inline void FMessageBox::setTitlebarText (const FString& txt)
{ return FDialog::setText(txt); }

//----------------------------------------------------------------------
inline bool FMessageBox::setCenterText(bool enable)
{ return (center_text = enable); }

//----------------------------------------------------------------------
inline bool FMessageBox::setCenterText()
{ return setCenterText(true); }

//----------------------------------------------------------------------
inline bool FMessageBox::unsetCenterText()
{ return setCenterText(false); }

//----------------------------------------------------------------------
template <typename messageType>
int FMessageBox::info ( FWidget* parent
                      , const FString& caption
                      , const messageType& message
                      , int button0
                      , int button1
                      , int button2 )
{
  FMessageBox mbox ( caption
                   , FString() << message
                   , button0, button1, button2
                   , parent );
  const int reply = mbox.exec();
  return reply;
}

//----------------------------------------------------------------------
template <typename messageType>
int FMessageBox::error ( FWidget* parent
                       , const messageType& message
                       , int button0
                       , int button1
                       , int button2 )
{
  const FString caption{"Error message"};

  FMessageBox mbox ( caption
                   , FString() << message
                   , button0, button1, button2
                   , parent );
  mbox.beep();
  mbox.setHeadline("Warning:");
  mbox.setCenterText();
  const auto& wc = mbox.getFWidgetColors();
  mbox.setForegroundColor(wc.error_box_fg);
  mbox.setBackgroundColor(wc.error_box_bg);
  mbox.emphasis_color = wc.error_box_emphasis_fg;
  const int reply = mbox.exec();
  return reply;
}

}  // namespace finalcut

#endif  // FMESSAGEBOX_H
