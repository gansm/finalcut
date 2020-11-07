/***********************************************************************
* ftermbuffer.h - Buffer for virtual terminal strings                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2020 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermBuffer ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMBUFFER_H
#define FTERMBUFFER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "final/fstringstream.h"

namespace finalcut
{

// class forward declaration
class FStyle;
class FColorPair;

//----------------------------------------------------------------------
// class FTermBuffer
//----------------------------------------------------------------------

class FTermBuffer
{
  public:
    // Using-declarations
    using FCharVector     = std::vector<FChar>;
    using iterator        = FCharVector::iterator;
    using const_iterator  = FCharVector::const_iterator;
    using reference       = FCharVector::reference;
    using const_reference = FCharVector::const_reference;

    // Constructor
    FTermBuffer() = default;
    template <typename Iterator>
    FTermBuffer (Iterator, Iterator);

    // Destructor
    virtual ~FTermBuffer();

    // Overloaded operators
    template <typename typeT>
    FTermBuffer& operator << (const typeT&);
    FTermBuffer& operator << (const FCharVector&);
    FTermBuffer& operator << (const std::string&);
    FTermBuffer& operator << (const std::wstring&);
    FTermBuffer& operator << (const FStyle&);
    FTermBuffer& operator << (const FColorPair&);

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
    FTermBuffer&           write ();

  private:
    FCharVector            data{};

    // Non-member operators
    friend FCharVector& operator << ( FCharVector&
                                    , const FTermBuffer& );
};


// FTermBuffer inline functions
//----------------------------------------------------------------------
template <typename Iterator>
inline FTermBuffer::FTermBuffer(Iterator first, Iterator last)
{
  data.assign(first, last);
}

//----------------------------------------------------------------------
template <typename typeT>
inline FTermBuffer& FTermBuffer::operator << (const typeT& s)
{
  FStringStream outstream{std::ios_base::out};
  outstream << s;

  if ( ! outstream.str().isEmpty() )
    write (outstream.str());

  return *this;
}

//----------------------------------------------------------------------
inline FTermBuffer& FTermBuffer::operator << (const FCharVector& vec)
{
  std::copy(vec.begin(), vec.end(), std::back_inserter(data));
  return *this;
}

//----------------------------------------------------------------------
inline FTermBuffer& FTermBuffer::operator << (const std::string& string)
{
  write (string);
  return *this;
}

//----------------------------------------------------------------------
inline FTermBuffer& FTermBuffer::operator << (const std::wstring& wstring)
{
  write (wstring);
  return *this;
}

//----------------------------------------------------------------------
inline FTermBuffer& FTermBuffer::operator << (const FStyle& style)
{
  write (style);
  return *this;
}

//----------------------------------------------------------------------
inline FTermBuffer& FTermBuffer::operator << (const FColorPair& pair)
{
  write (pair);
  return *this;
}

//----------------------------------------------------------------------
inline FString FTermBuffer::getClassName() const
{ return "FTermBuffer"; }

//----------------------------------------------------------------------
inline std::size_t FTermBuffer::getLength() const
{ return data.size(); }

//----------------------------------------------------------------------
inline const FTermBuffer::FCharVector& FTermBuffer::getBuffer() const
{ return data; }

//----------------------------------------------------------------------
inline bool FTermBuffer::isEmpty() const
{ return data.empty(); }

//----------------------------------------------------------------------
inline FTermBuffer::iterator FTermBuffer::begin()
{ return data.begin(); }

//----------------------------------------------------------------------
inline FTermBuffer::iterator FTermBuffer::end()
{ return data.end(); }

//----------------------------------------------------------------------
inline FTermBuffer::const_iterator FTermBuffer::begin() const
{ return data.begin(); }

//----------------------------------------------------------------------
inline FTermBuffer::const_iterator FTermBuffer::end() const
{ return data.end(); }

//----------------------------------------------------------------------
inline FTermBuffer::reference FTermBuffer::front()
{ return data.front(); }

//----------------------------------------------------------------------
inline FTermBuffer::reference FTermBuffer::back()
{ return data.back(); }

//----------------------------------------------------------------------
inline FTermBuffer::const_reference FTermBuffer::front() const
{ return data.front(); }

//----------------------------------------------------------------------
inline FTermBuffer::const_reference FTermBuffer::back() const
{ return data.back(); }

//----------------------------------------------------------------------
inline void FTermBuffer::clear()
{
  data.clear();
  data.shrink_to_fit();
}

//----------------------------------------------------------------------
template <typename... Args>
inline int FTermBuffer::writef (const FString& format, Args&&... args)
{
  FString str{};
  str.sprintf (format, std::forward<Args>(args)...);
  return write(str);
}

//----------------------------------------------------------------------
inline FTermBuffer& FTermBuffer::write()
{ return *this; }

}  // namespace finalcut

#endif  // FTERMBUFFER_H
