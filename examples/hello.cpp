/***********************************************************************
* hello.cpp - A simple hello world program                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2024 Markus Gans                                      *
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

#include <final/final.h>


auto main (int argc, char* argv[]) -> int
{
  // Create the application object
  finalcut::FApplication app{argc, argv};

  // Create a simple dialog box
  finalcut::FMessageBox mbox{&app};

  // New line
  const auto nl = finalcut::FString('\n');

  // Border spacing
  const auto spacing = finalcut::FString(5, ' ');

  // Line with unicode character "─" (U+2500/Box drawings horizontal)
  const auto line = finalcut::FString(13, L'\U00002500');

  // Set the message box text
  mbox.setText(nl + spacing + "Hello, World!" + spacing + nl + line);

  // Center the text in the message box
  mbox.setCenterText();

  // Start the application
  mbox.exec();
}
