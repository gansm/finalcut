/***********************************************************************
* foutput.cpp - Abstract Virtual terminal output class                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021 Markus Gans                                           *
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

#include "final/output/foutput.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FOutput
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FOutput::~FOutput() noexcept = default;  // destructor

//----------------------------------------------------------------------
auto FOutput::getInstance() -> std::shared_ptr<FOutput>
{
  return shared_from_this();
}

}  // namespace finalcut

