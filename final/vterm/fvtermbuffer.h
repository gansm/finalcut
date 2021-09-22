/***********************************************************************
* fvtermbuffer.h - Buffer for virtual terminal strings                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2021 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FVTermBuffer ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FVTERMBUFFER_H
#define FVTERMBUFFER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "final/util/fstringstream.h"

namespace finalcut
{

// class forward declaration
class FStyle;
class FColorPair;

//----------------------------------------------------------------------
// class FVTermBuffer
//----------------------------------------------------------------------

class FVTermBuffer
{
  public:
    // Using-declarations
    using FCharVector     = std::vector<FChar>;
    using iterator        = FCharVector::iterator;
    using const_iterator  = FCharVector::const_iterator;
    using reference       = FCharVector::reference;
    using const_reference = FCharVector::const_reference;

    // Constructor
    FVTermBuffer() = default;
    template <typename Iterator>
    FVTermBuffer (Iterator, Iterator);

    // Destructor
    virtual ~FVTermBuffer() noexcept;

    // Overloaded operators
    template <typename typeT>
    FVTermBuffer& operator << (const typeT&);
    FVTermBuffer& operator << (const UniChar&);
    FVTermBuffer& operator << (const FCharVector&);
    FVTermBuffer& operator << (const std::string&);
    FVTermBuffer& operator << (const std::wstring&);
    FVTermBuffer& operator << (const FStyle&);
    FVTermBuffer& operator << (const FColorPair&);

    // Accessors
    virtual FString        getClassName() const;
    std::size_t            getLength() const;
    const FCharVector&     getBuffer() const;

    // Inquiry
    bool                   isEmpty() const;

    // Methods
    iterator               begin();
    iterator               end();
    const_iterator         begin() const;
    const_iterator         end() const;
    reference              front();
    reference              back();
    const_reference        front() const;
    const_reference        back() const;
    FString                toString() const;
    void                   clear();
    template <typename... Args>
    int                    writef (const FString&, Args&&...);
    int                    write (const FString&);
    int                    write (wchar_t);
    void                   write (const FStyle&) const;
    void                   write (const FColorPair&) const;
    FVTermBuffer&          write ();

  private:
    FCharVector            data{};
    void                   add ( FString::const_iterator&
                               , const FString::const_iterator&
                               , int& );

    // Non-member operators
    friend FCharVector& operator << ( FCharVector&
                                    , const FVTermBuffer& );
};


// FVTermBuffer inline functions
//----------------------------------------------------------------------
template <typename Iterator>
inline FVTermBuffer::FVTermBuffer(Iterator first, Iterator last)
{
  data.assign(first, last);
}

//----------------------------------------------------------------------
template <typename typeT>
inline FVTermBuffer& FVTermBuffer::operator << (const typeT& s)
{
  FStringStream outstream{std::ios_base::out};
  outstream << s;

  if ( ! outstream.str().isEmpty() )
    write (outstream.str());

  return *this;
}

//----------------------------------------------------------------------
inline FVTermBuffer& FVTermBuffer::operator << (const UniChar& c)
{
  write (static_cast<wchar_t>(c));
  return *this;
}

//----------------------------------------------------------------------
inline FVTermBuffer& FVTermBuffer::operator << (const FCharVector& vec)
{
  std::copy(vec.begin(), vec.end(), std::back_inserter(data));
  return *this;
}

//----------------------------------------------------------------------
inline FVTermBuffer& FVTermBuffer::operator << (const std::string& string)
{
  write (string);
  return *this;
}

//----------------------------------------------------------------------
inline FVTermBuffer& FVTermBuffer::operator << (const std::wstring& wstring)
{
  write (wstring);
  return *this;
}

//----------------------------------------------------------------------
inline FVTermBuffer& FVTermBuffer::operator << (const FStyle& style)
{
  write (style);
  return *this;
}

//----------------------------------------------------------------------
inline FVTermBuffer& FVTermBuffer::operator << (const FColorPair& pair)
{
  write (pair);
  return *this;
}

//----------------------------------------------------------------------
inline FString FVTermBuffer::getClassName() const
{ return "FVTermBuffer"; }

//----------------------------------------------------------------------
inline std::size_t FVTermBuffer::getLength() const
{ return data.size(); }

//----------------------------------------------------------------------
inline const FVTermBuffer::FCharVector& FVTermBuffer::getBuffer() const
{ return data; }

//----------------------------------------------------------------------
inline bool FVTermBuffer::isEmpty() const
{ return data.empty(); }

//----------------------------------------------------------------------
inline FVTermBuffer::iterator FVTermBuffer::begin()
{ return data.begin(); }

//----------------------------------------------------------------------
inline FVTermBuffer::iterator FVTermBuffer::end()
{ return data.end(); }

//----------------------------------------------------------------------
inline FVTermBuffer::const_iterator FVTermBuffer::begin() const
{ return data.begin(); }

//----------------------------------------------------------------------
inline FVTermBuffer::const_iterator FVTermBuffer::end() const
{ return data.end(); }

//----------------------------------------------------------------------
inline FVTermBuffer::reference FVTermBuffer::front()
{ return data.front(); }

//----------------------------------------------------------------------
inline FVTermBuffer::reference FVTermBuffer::back()
{ return data.back(); }

//----------------------------------------------------------------------
inline FVTermBuffer::const_reference FVTermBuffer::front() const
{ return data.front(); }

//----------------------------------------------------------------------
inline FVTermBuffer::const_reference FVTermBuffer::back() const
{ return data.back(); }

//----------------------------------------------------------------------
inline void FVTermBuffer::clear()
{
  data.clear();
  data.shrink_to_fit();
}

//----------------------------------------------------------------------
template <typename... Args>
inline int FVTermBuffer::writef (const FString& format, Args&&... args)
{
  FString str{};
  str.sprintf (format, std::forward<Args>(args)...);
  return write(str);
}

//----------------------------------------------------------------------
inline FVTermBuffer& FVTermBuffer::write()
{ return *this; }

}  // namespace finalcut

#endif  // FVTERMBUFFER_H
