/***********************************************************************
* fstartoptions.cpp - Contains the start options for initialization    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2023 Markus Gans                                      *
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

#include "final/fapplication.h"
#include "final/fstartoptions.h"
#include "final/util/flog.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FStartOptions
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FStartOptions::FStartOptions()
  : cursor_optimisation{true}
  , mouse_support{true}
  , terminal_detection{true}
  , terminal_data_request{true}
  , terminal_focus_events{true}
  , sgr_optimizer{true}
  , vgafont{false}
  , newfont{false}
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  , meta_sends_escape{true}
  , change_cursorstyle{true}
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  , meta_sends_escape{true}
#endif
  , dark_theme{false}
  , color_change{true}
{ }


// public methods of FStartOptions
//----------------------------------------------------------------------
auto FStartOptions::getInstance() -> FStartOptions&
{
  static const auto& start_options = std::make_unique<FStartOptions>();
  return *start_options;
}

//----------------------------------------------------------------------
void FStartOptions::setDefault()
{
  cursor_optimisation = true;
  mouse_support = true;
  terminal_detection = true;
  color_change = true;
  vgafont = false;
  newfont = false;
  encoding = Encoding::Unknown;
  dark_theme = false;
  terminal_focus_events = true;

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  meta_sends_escape = true;
  change_cursorstyle = true;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  meta_sends_escape = true;
#endif
}

}  // namespace finalcut

