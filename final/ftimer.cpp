/***********************************************************************
* ftimer.cpp - Timer for executing recurring tasks                     *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2023 Markus Gans                                      *
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

#include <limits>

#include "final/fobject.h"
#include "final/ftimer.h"

namespace finalcut
{

namespace internal
{

static auto getInstance() -> FTimer<FObject>*
{
  static const auto& fobject_timer = std::make_unique<FTimer<FObject>>();
  return fobject_timer.get();
}

std::shared_timed_mutex timer_var::mutex{};

}  // namespace internal

// Static class attribute
FTimer<FObject>* FObjectTimer::timer{nullptr};


//----------------------------------------------------------------------
// class FObjectTimer
//----------------------------------------------------------------------

// Specialization for FObject
//----------------------------------------------------------------------
template <>
auto FTimer<FObject>::globalTimerList() -> const FTimerListUniquePtr&
{
  static const auto& timer_list = std::make_unique<FTimerList>();
  return timer_list;
}

// FTimer non-member functions
//----------------------------------------------------------------------
auto getNextId() -> int
{
  static int id{0};
  return ( id != std::numeric_limits<int>::max() ) ? ++id : 1;
}


//----------------------------------------------------------------------
// class FObjectTimer
//----------------------------------------------------------------------

FObjectTimer::FObjectTimer()
{
  if ( ! timer )
    timer = internal::getInstance();
}

// private methods of FTimer
//----------------------------------------------------------------------
void FObjectTimer::performTimerAction (FObject*, FEvent*)
{
  // This method must be reimplemented in a subclass
  // to process the passed object and timer event
}

}  // namespace finalcut

