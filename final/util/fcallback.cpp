/***********************************************************************
* fcallback.cpp - Implements the callback functionality                *
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

#include "final/util/fcallback.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FCallback
//----------------------------------------------------------------------

// public methods of FCallback
//----------------------------------------------------------------------
void FCallback::delCallback (const FString& cb_signal)
{
  // Deletes entries with the given signal from the callback list

  if ( callback_objects.empty() )
    return;

  auto iter = callback_objects.begin();

  while ( iter != callback_objects.end() )
  {
    if ( iter->cb_signal == cb_signal )
      iter = callback_objects.erase(iter);
    else
      ++iter;
  }
}

//----------------------------------------------------------------------
void FCallback::delCallback()
{
  // Delete all callbacks from this widget

  callback_objects.clear();  // function pointer
}

//----------------------------------------------------------------------
void FCallback::emitCallback (const FString& emit_signal) const
{
  // Initiate callback for the given signal

  if ( callback_objects.empty() )
    return;

  for (auto&& cback : callback_objects)
  {
    if ( cback.cb_signal == emit_signal )
    {
      // Calling the stored function pointer
      cback.cb_function();
    }
  }
}

}  // namespace finalcut

