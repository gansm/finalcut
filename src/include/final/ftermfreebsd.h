/***********************************************************************
* ftermfreebsd.h - Contains the FreeBSD terminal functions             *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermFreeBSD ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMFREEBSD_H
#define FTERMFREEBSD_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fc.h"
#include "final/ftypes.h"

#if defined(__FreeBSD__) || defined(__DragonFly__)
  #undef mouse_info  // consio.h
  #undef buttons     // consio.h

  #include <sys/consio.h>
  #include <sys/kbio.h>
#endif

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermFreeBSD
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermFreeBSD
{
  public:
    // Typedef
    typedef fc::freebsdConsoleCursorStyle  CursorStyle;

    // Constructors
    FTermFreeBSD();

    // Destructor
    virtual ~FTermFreeBSD();

    // Accessors
    const char*        getClassName() const;
    static CursorStyle getCursorStyle();

    // Inquiry
    static bool        isFreeBSDConsole();

    // Mutators
    static void        setCursorStyle (CursorStyle, bool);
    static void        enableChangeCursorStyle();
    static void        disableChangeCursorStyle();
    static void        enableMetaSendsEscape();
    static void        disableMetaSendsEscape();

    // Methods
    static void        init();
    static void        initCharMap (uInt[][fc::NUM_OF_ENCODINGS]);
    static void        finish();
    static void        restoreCursorStyle();

  private:
    // Disable copy constructor
    FTermFreeBSD (const FTermFreeBSD&);

    // Disable assignment operator (=)
    FTermFreeBSD& operator = (const FTermFreeBSD&);

    // Methods
    static bool        saveFreeBSDAltKey();
    static bool        setFreeBSDAltKey (uInt);
    static bool        setFreeBSDAlt2Meta();
    static bool        resetFreeBSDAlt2Meta();

    // Data Members
    static uInt        bsd_alt_keymap;
    static CursorStyle cursor_style;
    static bool        change_cursorstyle;
    static bool        meta_sends_escape;
};
#pragma pack(pop)

// FTermFreeBSD inline functions
//----------------------------------------------------------------------
inline const char* FTermFreeBSD::getClassName() const
{ return "FTermFreeBSD"; }

//----------------------------------------------------------------------
#if defined(__FreeBSD__) || defined(__DragonFly__)
inline void FTermFreeBSD::enableChangeCursorStyle()
{ change_cursorstyle = true; }

//----------------------------------------------------------------------
inline void FTermFreeBSD::disableChangeCursorStyle()
{ change_cursorstyle = false; }

//----------------------------------------------------------------------
inline void FTermFreeBSD::enableMetaSendsEscape()
{ meta_sends_escape = true; }

//----------------------------------------------------------------------
inline void FTermFreeBSD::disableMetaSendsEscape()
{ meta_sends_escape = false; }
#endif  // defined(__FreeBSD__) || defined(__DragonFly__)

}  // namespace finalcut

#endif  // FTERMFREEBSD_H
