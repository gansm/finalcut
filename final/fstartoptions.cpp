/***********************************************************************
* fstartoptions.cpp - Contains the start options for initialization    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2026 Markus Gans                                      *
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

#include <memory>

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
{
  bits.cursor_optimisation = true;
  bits.mouse_support = true;
  bits.terminal_detection = true;
  bits.terminal_data_request = true;
  bits.terminal_focus_events = true;
  bits.sgr_optimizer = true;
  bits.vgafont = false;
  bits.newfont = false;
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  bits.meta_sends_escape = true;
  bits.change_cursorstyle = true;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  bits.meta_sends_escape = true;
#endif
  bits.dark_theme = false;
  bits.color_change = true;
  bits.is_being_initialized = false;
}


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
  bits.cursor_optimisation = true;
  bits.mouse_support = true;
  bits.terminal_detection = true;
  bits.color_change = true;
  bits.vgafont = false;
  bits.newfont = false;
  bits.dark_theme = false;
  bits.terminal_focus_events = true;
  bits.is_being_initialized = false;

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  bits.meta_sends_escape = true;
  bits.change_cursorstyle = true;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  bits.meta_sends_escape = true;
#endif

  encoding = Encoding::Unknown;
}

}  // namespace finalcut

