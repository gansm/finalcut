/***********************************************************************
* ftermdetection.h - Detection of the terminal type                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2023 Markus Gans                                      *
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
    FTermDetection() = default;

    // Accessors
    auto  getClassName() const -> FString;
    static auto  getInstance() -> FTermDetection&;
    auto  getTermType() const & -> const FString&;

#if DEBUG
    auto  getAnswerbackString() const & -> const FString&;
    auto  getSecDAString() const & -> const FString&;
    auto  getTermType_256color() const & -> const FString&;
    auto  getTermType_Answerback() const & -> const FString&;
    auto  getTermType_SecDA() const & -> const FString&;
#endif

    // Inquiries
    auto  canDisplay256Colors() const noexcept -> bool;
    auto  hasTerminalDetection() const noexcept -> bool;
    auto  hasSetCursorStyleSupport() const noexcept -> bool;

    // Mutators
    void  setTerminalDetection (bool = true) noexcept;
    void  setTtyTypeFileName (const FString&);

    // Methods
    void  detect();

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

    // Using-declaration
    using TermTypeMap = std::vector<std::pair<std::wstring, FTermType>>;

    // Methods
    void  getSystemTermType();
    auto  getTTYtype() -> bool;
#if F_HAVE_GETTTYNAM
    auto  getTTYSFileEntry() -> bool;
#endif
    auto  getTermBasename() const -> const char*;
    template<typename StringT>
    auto  startsWithTermType (StringT&&) const -> bool;
    void  termtypeAnalysis();
    auto  findMatchingTerm (const TermTypeMap&) -> TermTypeMap::const_iterator;
    auto  isTerminalWithoutDetection() const -> bool;
    void  handleScreenAndTmux() const;
    void  detectTerminal();
    auto  init_256colorTerminal() -> FString;
    auto  get256colorEnvString() -> bool;
    auto  termtype_256color_quirks() -> FString;
    auto  determineMaxColor (const FString&) -> FString;
    auto  getXTermColorName (FColor) const -> FString;
    auto  parseAnswerbackMsg (const FString&) -> FString;
    auto  getAnswerbackMsg() const -> FString;
    auto  parseSecDA (const FString&) -> FString;
    auto  str2int (const FString&) const -> int;
    auto  getSecDA() const -> FString;
    auto  secDA_Analysis (const FString&) -> FString;
    auto  secDA_Analysis_0 (const FString&) const -> FString;
    auto  secDA_Analysis_1 (const FString&) -> FString;
    auto  secDA_Analysis_24 (const FString&) -> FString;
    auto  secDA_Analysis_32 () const -> FString;
    auto  secDA_Analysis_65 (const FString&) -> FString;
    auto  secDA_Analysis_67 () const -> FString;
    auto  secDA_Analysis_77 () -> FString;
    auto  secDA_Analysis_82 () const -> FString;
    auto  secDA_Analysis_83 (const FString&) const -> FString;
    auto  secDA_Analysis_84 (const FString&) const -> FString;
    auto  secDA_Analysis_85 () const -> FString;
    auto  secDA_Analysis_vte (const FString&) -> FString;
    auto  secDA_Analysis_kitty (const FString&) -> FString;
    void  correctFalseAssumptions (int) const;

    // Data members
#if DEBUG
    FString      termtype_256color{};
    FString      termtype_Answerback{};
    FString      termtype_SecDA{};
#endif
    FString      termtype{};
    FString      ttytypename{"/etc/ttytype"};  // Default ttytype file
    bool         decscusr_support{false};      // Preset to false
    bool         terminal_detection{true};     // Preset to true
    bool         color256{};
    FString      answer_back{};
    FString      sec_da{};
    colorEnv     color_env{};
    secondaryDA  secondary_da{};
};


// FTermDetection inline functions
//----------------------------------------------------------------------
inline auto FTermDetection::getClassName() const -> FString
{ return "FTermDetection"; }

//----------------------------------------------------------------------
inline auto FTermDetection::getTermType() const & -> const FString&
{ return termtype; }

#if DEBUG
//----------------------------------------------------------------------
inline auto FTermDetection::getTermType_256color() const & -> const FString&
{ return termtype_256color; }

//----------------------------------------------------------------------
inline auto FTermDetection::getTermType_Answerback() const & -> const FString&
{ return termtype_Answerback; }

//----------------------------------------------------------------------
inline auto FTermDetection::getTermType_SecDA() const & -> const FString&
{ return termtype_SecDA; }
#endif

//----------------------------------------------------------------------
inline auto FTermDetection::canDisplay256Colors() const noexcept -> bool
{ return color256; }

//----------------------------------------------------------------------
inline auto FTermDetection::hasSetCursorStyleSupport() const noexcept -> bool
{ return decscusr_support; }

//----------------------------------------------------------------------
inline auto FTermDetection::hasTerminalDetection() const noexcept -> bool
{ return terminal_detection; }

//----------------------------------------------------------------------
inline void FTermDetection::setTerminalDetection (bool enable) noexcept
{ terminal_detection = enable; }

//----------------------------------------------------------------------
template<typename StringT>
inline auto FTermDetection::startsWithTermType (StringT&& prefix) const -> bool
{
  return termtype.toWString().find(std::forward<StringT>(prefix)) == 0;
}

}  // namespace finalcut

#endif  // FTERMDETECTION_H
