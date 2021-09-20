/***********************************************************************
* fstringstream.h - I/O operations on FString based streams            *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ std::wiostream ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *          ▲
 *          │
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ FStringStream ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FSTRINGSTREAM_H
#define FSTRINGSTREAM_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <istream>
#include <iostream>
#include <sstream>

#include "final/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FStringStream
//----------------------------------------------------------------------

class FStringStream : public std::wiostream
{
  public:
    using std::ios_base::openmode;
    static constexpr openmode in_out = std::ios_base::out
                                     | std::ios_base::in;

    // Constructors
    explicit FStringStream (openmode = in_out);
    explicit FStringStream (const FString&, openmode = in_out);

    // Disable copy constructor
    FStringStream (const FStringStream&) = delete;

    // Move constructor
    FStringStream (FStringStream&&) noexcept;

    // Destructor
    ~FStringStream() noexcept override;

    // Disable copy assignment operator (=)
    FStringStream& operator = (const FStringStream&) = delete;

    // Move assignment operator (=)
    FStringStream& operator = (FStringStream&&) noexcept;

    virtual FString getClassName() const;
    void swap (FStringStream&) noexcept;
    void clear();
    std::wstringbuf* rdbuf();
    FString str() const;

  private:
    std::wstringbuf buffer{in_out};
};


// FStringStream inline functions
//----------------------------------------------------------------------
inline FString FStringStream::getClassName() const
{ return "FStringStream"; }

//----------------------------------------------------------------------
inline void FStringStream::clear()
{ buffer.str(L""); }

//----------------------------------------------------------------------
inline std::wstringbuf* FStringStream::rdbuf()
{ return &buffer; }

//----------------------------------------------------------------------
inline FString FStringStream::str() const
{ return FString{buffer.str()}; }


// FStringStream non-member function
//----------------------------------------------------------------------
inline void swap (FStringStream& a, FStringStream& b) noexcept
{ a.swap(b); }


}  // namespace finalcut

#endif  // FSTRINGSTREAM_H
