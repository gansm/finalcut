/***********************************************************************
* ftermdebugdata.h - Debug data class for FTerm                        *
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
 * ▕ FTermDebugData ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMDEBUGDATA_H
#define FTERMDEBUGDATA_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fstring.h"

#if DEBUG

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermDebugData
//----------------------------------------------------------------------

class FTermDebugData final
{
  public:
    // Constructors
    FTermDebugData() = default;

    // Disable copy constructor
    FTermDebugData (const FTermDebugData&) = delete;

    // Destructor
    ~FTermDebugData() = default;

    // Disable copy assignment operator (=)
    FTermDebugData& operator = (const FTermDebugData&) = delete;

    // Accessors
    static FString getClassName();
    static auto    getInstance() -> FTermDebugData&;
    const FString& getAnswerbackString();
    const FString& getSecDAString();
    const FString& getTermType_256color();
    const FString& getTermType_Answerback();
    const FString& getTermType_SecDA();
#if defined(__linux__)
    int            getFramebufferBpp();
#endif
};

inline FString FTermDebugData::getClassName()
{ return "FTermDebugData"; }

}  // namespace finalcut

#endif  // DEBUG

#endif  // FTERMDEBUGDATA_H
