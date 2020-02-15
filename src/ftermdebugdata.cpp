/***********************************************************************
* ftermdebugdata.cpp - Debug data class for FTerm                      *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019-2020 Markus Gans                                       *
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

#include "final/fc.h"
#include "final/fterm.h"
#include "final/ftermdata.h"
#include "final/ftermdetection.h"
#include "final/ftermdebugdata.h"

namespace finalcut
{

#if DEBUG
// static class attributes
FTermData*      FTermDebugData::data           {nullptr};
FTermDetection* FTermDebugData::term_detection {nullptr};

//----------------------------------------------------------------------
// class FTermDebugData
//----------------------------------------------------------------------

// public methods of FTermDebugData
//----------------------------------------------------------------------
void FTermDebugData::init()
{
  data = FTerm::getFTermData();
  term_detection = FTerm::getFTermDetection();
}

//----------------------------------------------------------------------
const FString& FTermDebugData::getAnswerbackString()
{
  return term_detection->getAnswerbackString();
}

//----------------------------------------------------------------------
const FString& FTermDebugData::getSecDAString()
{
  return term_detection->getSecDAString();
}

//----------------------------------------------------------------------
const char* FTermDebugData::getTermType_256color()
{
  return term_detection->getTermType_256color();
}

//----------------------------------------------------------------------
const char* FTermDebugData::getTermType_Answerback()
{
  return term_detection->getTermType_Answerback();
}

//----------------------------------------------------------------------
const char* FTermDebugData::getTermType_SecDA()
{
  return term_detection->getTermType_SecDA();
}

//----------------------------------------------------------------------
#if defined(__linux__)
int FTermDebugData::getFramebufferBpp()
{
  return data->getFramebufferBpp();
}
#endif  // defined(__linux__)

#endif  // DEBUG

}  // namespace finalcut

