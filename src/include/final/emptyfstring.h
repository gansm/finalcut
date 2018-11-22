/***********************************************************************
* emptyfstring.h - Creates an empty FString object                     *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2015-2018 Markus Gans                                      *
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

#ifndef EMPTYFSTRING_H
#define EMPTYFSTRING_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fstring.h"

namespace finalcut
{

namespace fc
{

//----------------------------------------------------------------------
// class emptyFString
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class emptyFString
{
public:
  emptyFString()
  { }

  static bool isNull();
  static const FString& get();
  static void clear();

private:
  // Disable copy constructor
  emptyFString (const emptyFString&);

  // Disable assignment operator (=)
  emptyFString& operator = (const emptyFString&);

  // Data Member
  static const FString* empty_string;
};
#pragma pack(pop)

// emptyFString inline functions
//----------------------------------------------------------------------
inline bool emptyFString::isNull()
{
  return ( empty_string ) ? false : true;
}

//----------------------------------------------------------------------
inline const FString& emptyFString::get()
{
  if ( ! empty_string )
  {
    try
    {
      empty_string = new FString("");
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << bad_alloc_str << ex.what() << std::endl;
    }
  }

  return *empty_string;
}

//----------------------------------------------------------------------
inline void emptyFString::clear()
{
  delete empty_string;
  empty_string = 0;
}

}  // namespace fc

}  // namespace finalcut

#endif  // EMPTYFSTRING_H
