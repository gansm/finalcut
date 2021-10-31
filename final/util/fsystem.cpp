/***********************************************************************
* fsystem.cpp - Abstract class for system calls                        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2021 Markus Gans                                      *
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

#include "final/util/fsystem.h"
#include "final/util/fsystemimpl.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSystem
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FSystem::~FSystem() noexcept = default;  // destructor


// public methods of FSystem
//----------------------------------------------------------------------
auto FSystem::getInstance() -> std::unique_ptr<FSystem>&
{
  static const auto& fsys = make_unique<std::unique_ptr<FSystem>>(make_unique<FSystemImpl>());
  return *fsys;
}

}  // namespace finalcut

