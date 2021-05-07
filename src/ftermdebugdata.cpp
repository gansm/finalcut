/***********************************************************************
* ftermdebugdata.cpp - Debug data class for FTerm                      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2021 Markus Gans                                       *
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

#include "final/fc.h"
#include "final/fterm.h"
#include "final/ftermdata.h"
#include "final/ftermdetection.h"
#include "final/ftermdebugdata.h"

#if DEBUG

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermDebugData
//----------------------------------------------------------------------

// public methods of FTermDebugData
//----------------------------------------------------------------------
const FString& FTermDebugData::getAnswerbackString()
{
  auto& term_detection = FTerm::getFTermDetection();
  return term_detection.getAnswerbackString();
}

//----------------------------------------------------------------------
const FString& FTermDebugData::getSecDAString()
{
  auto& term_detection = FTerm::getFTermDetection();
  return term_detection.getSecDAString();
}

//----------------------------------------------------------------------
const char* FTermDebugData::getTermType_256color()
{
  auto& term_detection = FTerm::getFTermDetection();
  return term_detection.getTermType_256color();
}

//----------------------------------------------------------------------
const char* FTermDebugData::getTermType_Answerback()
{
  auto& term_detection = FTerm::getFTermDetection();
  return term_detection.getTermType_Answerback();
}

//----------------------------------------------------------------------
const char* FTermDebugData::getTermType_SecDA()
{
  auto& term_detection = FTerm::getFTermDetection();
  return term_detection.getTermType_SecDA();
}

//----------------------------------------------------------------------
#if defined(__linux__)
int FTermDebugData::getFramebufferBpp()
{
  auto& fterm_data = FTerm::getFTermData();
  return fterm_data.getFramebufferBpp();
}
#endif  // defined(__linux__)

}  // namespace finalcut

#endif  // DEBUG
