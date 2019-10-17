/***********************************************************************
* ftermbuffer.h - Buffer for virtual terminal strings                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2019 Markus Gans                                      *
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

#include <sstream>  // std::stringstream
#include <string>
#include <utility>
#include <vector>

namespace finalcut
{

// class forward declaration
class FColorPair;

//----------------------------------------------------------------------
// class FTermBuffer
//----------------------------------------------------------------------

class FTermBuffer
{
  public:
    // Typedef
    typedef std::vector<FChar>          FCharVector;
    typedef FCharVector::iterator       iterator;
    typedef FCharVector::const_iterator const_iterator;

    // Constructor
    FTermBuffer() = default;
    template<typename Iterator>
    FTermBuffer (Iterator, Iterator);

    // Destructor
    virtual ~FTermBuffer();

    // Overloaded operators
    template <typename typeT>
    FTermBuffer& operator << (const typeT&);
    FTermBuffer& operator << (const FCharVector&);
    FTermBuffer& operator << (const std::string&);
    FTermBuffer& operator << (const std::wstring&);
    FTermBuffer& operator << (const FColorPair&);

    // Non-member operators
    friend FCharVector& operator << ( FCharVector&
                                    , const FTermBuffer& );

    // Accessors
    virtual const FString  getClassName() const;
    std::size_t            getLength() const;
    const FCharVector&     getBuffer() const;

    // Inquiry
    bool                   isEmpty() const;

    // Methods
    iterator               begin();
    iterator               end();
    const_iterator         begin() const;
    const_iterator         end() const;
    FChar                  front() const;
    FChar                  back() const;
    const FString          toString() const;
    void                   clear();
    template<typename... Args>
    int                    writef (const FString&, Args&&...);
    int                    write (const FString&);
    int                    write (wchar_t);
    void                   write (const FColorPair&);
    FTermBuffer&           write ();

  private:
    FCharVector            data{};
};


// FTermBuffer inline functions
//----------------------------------------------------------------------
template<typename Iterator>
inline FTermBuffer::FTermBuffer(Iterator first, Iterator last)
{
  data.assign(first, last);
}

//----------------------------------------------------------------------
template <typename typeT>
inline FTermBuffer& FTermBuffer::operator << (const typeT& s)
{
  std::wostringstream outstream;
  outstream << s;

  if ( ! outstream.str().empty() )
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
inline FTermBuffer& FTermBuffer::operator << (const FColorPair& pair)
{
  write (pair);
  return *this;
}

//----------------------------------------------------------------------
inline const FString FTermBuffer::getClassName() const
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
inline FChar FTermBuffer::front() const
{ return data.front(); }

//----------------------------------------------------------------------
inline FChar FTermBuffer::back() const
{ return data.back(); }

//----------------------------------------------------------------------
inline void FTermBuffer::clear()
{
  data.clear();
  data.shrink_to_fit();
}

//----------------------------------------------------------------------
template<typename... Args>
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
