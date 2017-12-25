/***********************************************************************
* ftermbuffer.h - Buffer for virtual terminal strings                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017 Markus Gans                                           *
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
#include <vector>

#include "final/fvterm.h"
#include "final/fstring.h"


//----------------------------------------------------------------------
// class FTermBuffer
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermBuffer
{
  public:
    // Typedef
    typedef FOptiAttr::char_data  char_data;

    // Constructor
    explicit FTermBuffer();

    // Destructor
    virtual ~FTermBuffer();

    // Overloaded operators
    template<class type> FTermBuffer& operator << (const type&);
    // Non-member operators
    friend std::vector<char_data>& operator << ( std::vector<char_data>&
                                               , const FTermBuffer& );

    // Accessors
    virtual const char*    getClassName() const;
    int                    getLength() const;

    // Inquiry
    bool                   isEmpty() const;

    // Methods
    void                   clear();
    int                    writef (const wchar_t[], ...);
    int                    writef (const char[], ...)
#if defined(__clang__)
      __attribute__((__format__ (__printf__, 2, 3)))
#elif defined(__GNUC__)
      __attribute__ ((format (printf, 2, 3)))
#endif
                           ;
    int                    write (const std::wstring&);
    int                    write (const wchar_t[]);
    int                    write (const char[]);
    int                    write (const std::string&);
    int                    write (const FString&);
    int                    write (int);
    FTermBuffer&           write ();
    std::vector<char_data> getBuffer();

  private:
    std::vector<char_data> data;
};

#pragma pack(pop)


// FTermBuffer inline functions
//----------------------------------------------------------------------
template<class type>
inline FTermBuffer& FTermBuffer::operator << (const type& s)
{
  std::wostringstream outstream;
  outstream << s;
  write (outstream.str());
  return *this;
}

//----------------------------------------------------------------------
inline const char* FTermBuffer::getClassName() const
{ return "FTermBuffer"; }

//----------------------------------------------------------------------
inline int FTermBuffer::getLength() const
{ return int(data.size()); }

//----------------------------------------------------------------------
inline bool FTermBuffer::isEmpty() const
{ return data.empty(); }

//----------------------------------------------------------------------
inline void FTermBuffer::clear()
{ data.clear(); }

//----------------------------------------------------------------------
inline FTermBuffer& FTermBuffer::write()
{ return *this; }

//----------------------------------------------------------------------
inline std::vector<FTermBuffer::char_data> FTermBuffer::getBuffer()
{ return data; }

#endif  // FTERMBUFFER_H
