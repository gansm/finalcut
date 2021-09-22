/***********************************************************************
* fsystemimpl.cpp - FSystem implementation                             *
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

#if defined(__CYGWIN__)
  #include "final/fconfig.h"  // need for getpwuid_r and realpath
#endif

#include "final/util/fsystemimpl.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSystemImpl
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FSystemImpl::~FSystemImpl() noexcept = default;  // destructor


// public methods of FSystemImpl
//----------------------------------------------------------------------
int FSystemImpl::getpwuid_r ( uid_t uid, struct passwd* pwd
                            , char* buf, size_t buflen
                            , struct passwd** result )
{
  return ::getpwuid_r (uid, pwd, buf, buflen, result);
}

//----------------------------------------------------------------------
char* FSystemImpl::realpath (const char* path, char* resolved_path)
{
  return ::realpath(path, resolved_path);
}

}  // namespace finalcut

