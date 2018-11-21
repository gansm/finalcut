/***********************************************************************
* ftermdebugdata.h - Debug data class for FTerm                        *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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

#if DEBUG
//----------------------------------------------------------------------
// class FTermDebugData
//----------------------------------------------------------------------

class FTermDebugData
{
  public:
    // Constructors
    FTermDebugData();

    // Destructor
    ~FTermDebugData();

    // Accessors
    const FString& getAnswerbackString();
    const FString& getSecDAString();
    const char*    getTermType_256color();
    const char*    getTermType_Answerback();
    const char*    getTermType_SecDA();
#if defined(__linux__)
    int            getFramebufferBpp();
#endif
    // Mutators
    void           setFTermDetection (FTermDetection*);
    void           setFTermData (FTermData*);

  private:
    // Disable copy constructor
    FTermDebugData (const FTermDebugData&);

    // Disable assignment operator (=)
    FTermDebugData& operator = (const FTermDebugData&);

    // Data Members
    FTermDetection* term_detection;
    FTermData*      data;
};

// FTermDebugData inline functions
//----------------------------------------------------------------------
inline FTermDebugData::FTermDebugData()
  : term_detection(0)
  , data(0)
{ }

//----------------------------------------------------------------------
inline FTermDebugData::~FTermDebugData()
{ }

//----------------------------------------------------------------------
inline void FTermDebugData::setFTermDetection (FTermDetection* obj)
{ term_detection = obj; }

//----------------------------------------------------------------------
inline void FTermDebugData::setFTermData (FTermData* obj)
{ data = obj; }

//----------------------------------------------------------------------
inline const FString& FTermDebugData::getAnswerbackString()
{ return term_detection->getAnswerbackString(); }

//----------------------------------------------------------------------
inline const FString& FTermDebugData::getSecDAString()
{ return term_detection->getSecDAString(); }

//----------------------------------------------------------------------
inline const char* FTermDebugData::getTermType_256color()
{ return term_detection->getTermType_256color(); }

//----------------------------------------------------------------------
inline const char* FTermDebugData::getTermType_Answerback()
{ return term_detection->getTermType_Answerback(); }

//----------------------------------------------------------------------
inline const char* FTermDebugData::getTermType_SecDA()
{ return term_detection->getTermType_SecDA(); }

//----------------------------------------------------------------------
#if defined(__linux__)
inline int FTermDebugData::getFramebufferBpp()
{ return data->getFramebufferBpp(); }
#endif  // defined(__linux__)

#endif  // DEBUG

}  // namespace finalcut

#endif  // FTERMDEBUGDATA_H
