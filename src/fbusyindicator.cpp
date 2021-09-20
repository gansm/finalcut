/***********************************************************************
* fbusyindicator.cpp - Shows background activity                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020-2021 Markus Gans                                      *
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

#include <array>
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
FBusyIndicator::~FBusyIndicator() noexcept = default;  // destructor


// public methods of FBusyIndicator
//----------------------------------------------------------------------
void FBusyIndicator::start()
{
  running = true;
  createIndicatorText();
  show();
  addTimer(TIMER);
}

//----------------------------------------------------------------------
void FBusyIndicator::stop()
{
  delOwnTimers();
  running = false;
  hide();
}


// private methods of FBusyIndicator
//----------------------------------------------------------------------
void FBusyIndicator::init()
{
  FToolTip::hide();
  disableAutoTrim();
}

//----------------------------------------------------------------------
void FBusyIndicator::createIndicatorText()
{
  std::array<FString, 4> line{};

  if ( FVTerm::getFOutput()->getEncoding() == Encoding::UTF8 )
  {
    const auto& p = uni_pattern;
    line[0] << "   " << p[7] << " " << p[0] << "   \n";
    line[1] << " " << p[6] << "     " << p[1] << " \n";
    line[2] << " " << p[5] << "     " << p[2] << " \n";
    line[3] << "   " << p[4] << " " << p[3] << "   ";
  }
  else
  {
    const auto& p = pattern;
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
  if ( FVTerm::getFOutput()->getEncoding() == Encoding::UTF8 )
  {
    const wchar_t last = uni_pattern[7];
    std::memmove(&uni_pattern[1], &uni_pattern[0] , sizeof(wchar_t) * 7);
    uni_pattern[0] = last;
  }
  else
  {
    const char last = pattern[7];
    std::memmove(&pattern[1], &pattern[0] , sizeof(char) * 7);
    pattern[0] = last;
  }

  // Redraw the rotated pattern
  createIndicatorText();
  redraw();
}

}  // namespace finalcut

