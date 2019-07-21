/***********************************************************************
* ftermdebugdata.h - Debug data class for FTerm                        *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2019 Markus Gans                                      *
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

namespace finalcut
{

// class forward declaration
class FTerm;
class FTermData;
class FTermDetection;

#if DEBUG
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

    // Disable assignment operator (=)
    FTermDebugData& operator = (const FTermDebugData&) = delete;

    // Accessors
    const FString& getAnswerbackString();
    const FString& getSecDAString();
    const char*    getTermType_256color();
    const char*    getTermType_Answerback();
    const char*    getTermType_SecDA();
#if defined(__linux__)
    int            getFramebufferBpp();
#endif

    // Methods
    static void    init();

  private:
    // Data Members
    static FTermData*      data;
    static FTermDetection* term_detection;
};
#endif  // DEBUG

}  // namespace finalcut

#endif  // FTERMDEBUGDATA_H
