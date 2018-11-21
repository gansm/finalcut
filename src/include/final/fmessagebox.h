/***********************************************************************
* fmessagebox.h - Widget FMessageBox (a text message window)           *
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

#include "final/fbutton.h"
#include "final/fdialog.h"
#include "final/fterm.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FMessageBox
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

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
    explicit FMessageBox (FWidget* = 0);
    FMessageBox (const FMessageBox&);  // copy constructor
    FMessageBox ( const FString&, const FString&
                , int, int, int
                , FWidget* = 0 );
    // Destructor
    virtual ~FMessageBox();

    // Assignment operator (=)
    FMessageBox& operator = (const FMessageBox&);

    // Accessor
    const char*   getClassName() const;
    const FString getTitlebarText() const;
    const FString getHeadline() const;
    const FString getText() const;

    // Mutator
    void          setTitlebarText (const FString&);
    void          setHeadline (const FString&);
    bool          setCenterText(bool);
    bool          setCenterText();
    bool          unsetCenterText();
    void          setText (const FString&);

    // Methods
    static int    info ( FWidget*
                       , const FString&
                       , const FString&
                       , int = FMessageBox::Ok
                       , int = 0
                       , int = 0 );

    static int    info ( FWidget*
                       , const FString&
                       , int
                       , int = FMessageBox::Ok
                       , int = 0
                       , int = 0 );

    static int    error ( FWidget*
                        , const FString&
                        , int = FMessageBox::Ok
                        , int = 0
                        , int = 0 );
   protected:
    // Method
    virtual void  adjustSize();

    // Callback method
    void          cb_processClick (FWidget*, data_ptr);

  private:
    // Methods
    void          init (int, int, int);
    void          allocation (int, int, int);
    void          deallocation();
    void          initCallbacks();
    void          calculateDimensions();
    virtual void  draw();
    void          resizeButtons();
    void          adjustButtons();

    // Data Members
    FString       headline_text;
    FString       text;
    FString*      text_components;
    FStringList   text_split;
    std::size_t   max_line_width;
    bool          center_text;
    FColor        emphasis_color;
    uInt          num_buttons;
    uInt          text_num_lines;
    int           button_digit[3];
    FButton*      button[3];
};
#pragma pack(pop)


// FMessageBox inline functions
//----------------------------------------------------------------------
inline const char* FMessageBox::getClassName() const
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
inline bool FMessageBox::setCenterText(bool on)
{ return (center_text = on); }

//----------------------------------------------------------------------
inline bool FMessageBox::setCenterText()
{ return setCenterText(true); }

//----------------------------------------------------------------------
inline bool FMessageBox::unsetCenterText()
{ return setCenterText(false); }

}  // namespace finalcut

#endif  // FMESSAGEBOX_H
