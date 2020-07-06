/***********************************************************************
* fbusyindicator.cpp - Shows background activity                       *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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

#include "final/fbusyindicator.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FBusyIndicator
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FBusyIndicator::FBusyIndicator (FWidget* parent)
  : FToolTip{parent}
{
  init();
}

//----------------------------------------------------------------------
FBusyIndicator::~FBusyIndicator()  // destructor
{ }


// public methods of FBusyIndicator
//----------------------------------------------------------------------
void FBusyIndicator::start()
{
  running = true;
  show();
  updateTerminal();
  flush();
  addTimer(TIMER);
}

//----------------------------------------------------------------------
void FBusyIndicator::stop()
{
  delOwnTimer();
  running = false;
  hide();
  updateTerminal();
  flush();
}


// private methods of FBusyIndicator
//----------------------------------------------------------------------
void FBusyIndicator::init()
{
  FToolTip::hide();
  createIndicatorText();
}

//----------------------------------------------------------------------
void FBusyIndicator::createIndicatorText()
{
  FString line[4]{};

  if ( FTerm::getEncoding() == fc::UTF8 )
  {
    const wchar_t (&p)[8] = uni_pattern;
    line[0] << "   " << p[7] << " " << p[0] << "   \n";
    line[1] << " " << p[6] << "     " << p[1] << " \n";
    line[2] << " " << p[5] << "     " << p[2] << " \n";
    line[3] << "   " << p[4] << " " << p[3] << "   ";
  }
  else
  {
    const char (&p)[8] = pattern;
    line[0] << "   " << p[7] << " " << p[0] << "   \n";
    line[1] << " " << p[6] << "     " << p[1] << " \n";
    line[2] << " " << p[5] << "     " << p[2] << " \n";
    line[3] << "   " << p[4] << " " << p[3] << "   ";
  }

  FString txt{line[0] + line[1] + line[2] + line[3]};
  FToolTip::setText(txt);
}

//----------------------------------------------------------------------
void FBusyIndicator::onTimer (finalcut::FTimerEvent*)
{
  // Rotate pattern
  if ( FTerm::getEncoding() == fc::UTF8 )
  {
    const wchar_t last = uni_pattern[7];
    std::memmove(uni_pattern + 1, uni_pattern , sizeof(wchar_t) * 7);
    uni_pattern[0] = last;
  }
  else
  {
    const char last = pattern[7];
    std::memmove(pattern + 1, pattern , sizeof(char) * 7);
    pattern[0] = last;
  }

  // Redraw the rotated pattern
  createIndicatorText();
  redraw();
  updateTerminal();
  flush();
}

}  // namespace finalcut

