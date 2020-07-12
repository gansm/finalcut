/***********************************************************************
* fdata.h - A general-purpose data wrapper                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2020 Markus Gans                                           *
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

#ifndef FDATA_H
#define FDATA_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

template<typename T>
struct FData
{
  explicit FData (T v)
    : value{v}
  { }

  FData (const FData& d)  // Copy constructor
    : value{d.value}
  { }

  FData& operator = (const FData& d)  // Copy assignment operator (=)
  {
    value = d.value;
    return *this;
  }

  FData (FData&& d) noexcept  // Move constructor
    : value{std::move(d.value)}
  { }

  FData& operator = (FData&& d) noexcept  // Move assignment operator (=)
  {
    value = std::move(d.value);
    return *this;
  }

  T operator () () const
  {
    return value;
  }

  template<typename... Args>
  T operator () (Args... args) const
  {
    return value(args...);
  }

  explicit operator T () const
  {
    return value;
  }

  T& get()
  {
    return value;
  }

  void set (const T& v)
  {
    value = v;
  }

  FData& operator << (const T& v)
  {
    value = v;
    return *this;
  }

  friend std::ostream& operator << (std::ostream &os, const FData& data)
  {
    os << data.value;
    return os;
  }

  T value;
};

#endif  // FDATA_H

