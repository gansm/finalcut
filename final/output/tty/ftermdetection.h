/***********************************************************************
* ftermdetection.h - Detection of the terminal type                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2021 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermDetection ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMDETECTION_H
#define FTERMDETECTION_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "final/fconfig.h"  // Supplies F_HAVE_GETTTYNAM if available
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermDetection
//----------------------------------------------------------------------

class FTermDetection final
{
  public:
    // Constructors
    FTermDetection();

    // Disable copy constructor
    FTermDetection (const FTermDetection&) = delete;

    // Destructor
    ~FTermDetection();

    // Disable copy assignment operator (=)
    FTermDetection& operator = (const FTermDetection&) = delete;

    // Accessors
    FString               getClassName() const;
    static auto           getInstance() -> FTermDetection&;
    const FString&        getTermType() const;

#if DEBUG
    const FString&        getAnswerbackString() const;
    const FString&        getSecDAString() const;
    const FString&        getTermType_256color() const;
    const FString&        getTermType_Answerback() const;
    const FString&        getTermType_SecDA() const;
#endif

    // Inquiries
    bool                  canDisplay256Colors() const;
    bool                  hasTerminalDetection() const;
    bool                  hasSetCursorStyleSupport() const;

    // Mutators
    void                  setTerminalDetection (bool = true);
    void                  setTtyTypeFileName (const FString&);

    // Methods
    void                  detect();

  private:
    struct colorEnv
    {
      FString string1{};
      FString string2{};
      FString string3{};
      FString string4{};
      FString string5{};
      FString string6{};
      FString string7{};
      FString string8{};
    };

    struct secondaryDA
    {
      int terminal_id_type{-1};
      int terminal_id_version{-1};
      int terminal_id_hardware{-1};
    };

    // Methods
    void                  getSystemTermType();
    bool                  getTTYtype();
#if F_HAVE_GETTTYNAM
    bool                  getTTYSFileEntry();
#endif
    void                  termtypeAnalysis();
    void                  detectTerminal();
    FString               init_256colorTerminal();
    bool                  get256colorEnvString();
    FString               termtype_256color_quirks();
    FString               determineMaxColor (const FString&);
    FString               getXTermColorName (FColor) const;
    FString               parseAnswerbackMsg (const FString&);
    FString               getAnswerbackMsg() const;
    FString               parseSecDA (const FString&);
    int                   str2int (const FString&) const;
    FString               getSecDA() const;
    FString               secDA_Analysis (const FString&);
    FString               secDA_Analysis_0 (const FString&) const;
    FString               secDA_Analysis_1 (const FString&);
    FString               secDA_Analysis_24 (const FString&);
    FString               secDA_Analysis_32 () const;
    FString               secDA_Analysis_65 (const FString&);
    FString               secDA_Analysis_67 () const;
    FString               secDA_Analysis_77 ();
    FString               secDA_Analysis_82 () const;
    FString               secDA_Analysis_83 (const FString&) const;
    FString               secDA_Analysis_84 (const FString&) const;
    FString               secDA_Analysis_85 () const;
    FString               secDA_Analysis_vte (const FString&);
    FString               secDA_Analysis_kitty (const FString&);

    // Data members
#if DEBUG
    FString        termtype_256color{};
    FString        termtype_Answerback{};
    FString        termtype_SecDA{};
#endif
    FString        termtype{};
    FString        ttytypename{"/etc/ttytype"};  // Default ttytype file
    bool           decscusr_support{false};      // Preset to false
    bool           terminal_detection{true};     // Preset to true
    bool           color256{};
    FString        answer_back{};
    FString        sec_da{};
    colorEnv       color_env{};
    secondaryDA    secondary_da{};
};


// FTermDetection inline functions
//----------------------------------------------------------------------
inline FString FTermDetection::getClassName() const
{ return "FTermDetection"; }

//----------------------------------------------------------------------
inline const FString& FTermDetection::getTermType() const
{ return termtype; }

#if DEBUG
//----------------------------------------------------------------------
inline const FString& FTermDetection::getTermType_256color() const
{ return termtype_256color; }

//----------------------------------------------------------------------
inline const FString& FTermDetection::getTermType_Answerback() const
{ return termtype_Answerback; }

//----------------------------------------------------------------------
inline const FString& FTermDetection::getTermType_SecDA() const
{ return termtype_SecDA; }
#endif

//----------------------------------------------------------------------
inline bool FTermDetection::canDisplay256Colors() const
{ return color256; }

//----------------------------------------------------------------------
inline bool FTermDetection::hasSetCursorStyleSupport() const
{ return decscusr_support; }

//----------------------------------------------------------------------
inline bool FTermDetection::hasTerminalDetection() const
{ return terminal_detection; }

//----------------------------------------------------------------------
inline void FTermDetection::setTerminalDetection (bool enable)
{ terminal_detection = enable; }

}  // namespace finalcut

#endif  // FTERMDETECTION_H
