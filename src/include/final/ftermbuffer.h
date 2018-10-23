/***********************************************************************
* ftermbuffer.h - Buffer for virtual terminal strings                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2017-2018 Markus Gans                                      *
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

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermBuffer
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermBuffer
{
  public:
    // Typedef
    typedef FOptiAttr::charData  charData;

    // Constructor
    FTermBuffer();

    // Destructor
    virtual ~FTermBuffer();

    // Overloaded operators
    template <typename type>
    FTermBuffer& operator << (const type&);
    // Non-member operators
    friend std::vector<charData>& operator << ( std::vector<charData>&
                                              , const FTermBuffer& );

    // Accessors
    virtual const char*    getClassName() const;
    std::size_t            getLength() const;

    // Inquiry
    bool                   isEmpty() const;

    // Methods
    void                   clear();
    int                    writef (const FString, ...);
    int                    write (const FString&);
    int                    write (int);
    FTermBuffer&           write ();
    std::vector<charData>  getBuffer();

  private:
    std::vector<charData> data;
};
#pragma pack(pop)


// FTermBuffer inline functions
//----------------------------------------------------------------------
template <typename type>
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
inline std::size_t FTermBuffer::getLength() const
{ return data.size(); }

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
inline std::vector<FTermBuffer::charData> FTermBuffer::getBuffer()
{ return data; }

}  // namespace finalcut

#endif  // FTERMBUFFER_H
