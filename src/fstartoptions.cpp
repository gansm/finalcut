/***********************************************************************
* fstartoptions.cpp - Contains the start options for initialization    *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
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

#include "final/fstartoptions.h"

namespace finalcut
{

// static class attribute
FStartOptions* FStartOptions::start_options{};

//----------------------------------------------------------------------
// class FStartOptions
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FStartOptions::FStartOptions()
  : cursor_optimisation{true}
  , mouse_support{true}
  , terminal_detection{true}
  , color_change{true}
  , sgr_optimizer{true}
  , vgafont{false}
  , newfont{false}
  , encoding{fc::UNKNOWN}
#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  , meta_sends_escape{true}
  , change_cursorstyle{true}
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  , meta_sends_escape{true}
#endif
{ }

//----------------------------------------------------------------------
FStartOptions::~FStartOptions()  // destructor
{ }

// public methods of FStartOptions
//----------------------------------------------------------------------
FStartOptions& FStartOptions::getFStartOptions()
{
  if ( start_options == 0 )
  {
    try
    {
      start_options = new FStartOptions;
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << bad_alloc_str << ex.what() << std::endl;
      std::abort();
    }
  }

  return *start_options;
}

//----------------------------------------------------------------------
void FStartOptions::destroyObject()
{
  if ( start_options )
    delete start_options;
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
  encoding = fc::UNKNOWN;

#if defined(__FreeBSD__) || defined(__DragonFly__) || defined(UNIT_TEST)
  meta_sends_escape = true;
  change_cursorstyle = true;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
  meta_sends_escape = true;
#endif
}

}  // namespace finalcut

