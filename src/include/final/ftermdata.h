/***********************************************************************
* ftermdata.h - Data class for FTerm                                   *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermData ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMDATA_H
#define FTERMDATA_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <map>
#include <string>

#include "final/fc.h"
#include "final/frect.h"
#include "final/fstring.h"
#include "final/ftypes.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermData
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FTermData
{
  public:
    // Typedefs
    typedef std::map<std::string, fc::encoding> encodingMap;

    // Constructors
    FTermData();

    // Destructor
    ~FTermData();

    // Accessors
    const char*     getClassName() const;
    encodingMap&    getEncodingList();
    fc::encoding    getTermEncoding() const;
    FRect&          getTermGeometry();
    int             getTTYFileDescriptor() const;
    uInt            getBaudrate() const;
    char*           getTermType();
    char*           getTermFileName();
    const FString&  getXtermFont() const;
    const FString&  getXtermTitle() const;
#if DEBUG
    int             getFramebufferBpp() const;
#endif

    // Inquiries
    bool            hasShadowCharacter() const;
    bool            hasHalfBlockCharacter() const;
    bool            hasCursorOptimisation() const;
    bool            isCursorHidden() const;
    bool            hasAlternateScreen() const;
    bool            hasASCIIConsole() const;
    bool            hasVT100Console() const;
    bool            hasUTF8Console() const;
    bool            isUTF8() const;
    bool            isNewFont() const;
    bool            isVGAFont() const;
    bool            isMonochron() const;
    bool            hasTermResized() const;

    // Mutators
    void            setTermEncoding(fc::encoding);
    void            setTTYFileDescriptor (int);
    void            setBaudrate (uInt);
    void            supportShadowCharacter (bool);
    void            supportHalfBlockCharacter (bool);
    void            supportCursorOptimisation (bool);
    void            setCursorHidden (bool);
    void            useAlternateScreen (bool);
    void            setASCIIConsole (bool);
    void            setVT100Console (bool);
    void            setUTF8Console (bool);
    void            setUTF8 (bool);
    void            setNewFont (bool);
    void            setVGAFont (bool);
    void            setMonochron (bool);
    void            setTermResized (bool);
    void            setTermType (const char[]);
    void            setTermFileName (const char[]);
    void            setXtermFont (const FString&);
    void            setXtermTitle (const FString&);
#if DEBUG
    void            setFramebufferBpp (int);
#endif

  private:
    // Disable copy constructor
    FTermData (const FTermData&);

    // Disable assignment operator (=)
    FTermData& operator = (const FTermData&);

    // Data Members
    encodingMap     encoding_list;
    fc::encoding    term_encoding;
    FRect           term_geometry;  // current terminal geometry
    int             fd_tty;
    uInt            baudrate;
    bool            shadow_character;
    bool            half_block_character;
    bool            cursor_optimisation;
    bool            hidden_cursor;
    bool            use_alternate_screen;
    bool            ascii_console;
    bool            vt100_console;
    bool            utf8_console;
    bool            utf8_state;
    bool            new_font;
    bool            vga_font;
    bool            monochron;
    bool            resize_term;
    char            termtype[256];
    char            termfilename[256];
    FString         xterm_font;
    FString         xterm_title;

#if DEBUG
    int             framebuffer_bpp;
#endif
};
#pragma pack(pop)

// FTermData inline functions
//----------------------------------------------------------------------
inline FTermData::FTermData()
  : encoding_list()
  , term_encoding(fc::UNKNOWN)
  , term_geometry()
  , fd_tty(-1)  // Teletype (tty) file descriptor is still undefined
  , baudrate(0)
  , shadow_character(true)
  , half_block_character(true)
  , cursor_optimisation(true)
  , hidden_cursor(false)  // Global cursor hidden state
  , use_alternate_screen(true)
  , ascii_console(false)
  , vt100_console(false)
  , utf8_console(false)
  , utf8_state(false)
  , new_font(false)
  , vga_font(false)
  , monochron(false)
  , resize_term(false)
  , termtype()
  , termfilename()
  , xterm_font()
  , xterm_title()
#if DEBUG
  , framebuffer_bpp(-1)
#endif
{
  // Initialize arrays with '\0'
  std::fill_n (termtype, sizeof(termtype), '\0');
  std::fill_n (termfilename, sizeof(termfilename), '\0');
}

//----------------------------------------------------------------------
inline FTermData::~FTermData()
{ }

//----------------------------------------------------------------------
inline const char* FTermData::getClassName() const
{ return "FTermData"; }

//----------------------------------------------------------------------
inline FTermData::encodingMap& FTermData::getEncodingList()
{ return encoding_list; }

//----------------------------------------------------------------------
inline fc::encoding FTermData::getTermEncoding() const
{ return term_encoding; }

//----------------------------------------------------------------------
inline FRect& FTermData::getTermGeometry()
{ return term_geometry; }

//----------------------------------------------------------------------
inline int FTermData::getTTYFileDescriptor() const
{ return fd_tty; }

//----------------------------------------------------------------------
inline uInt FTermData::getBaudrate() const
{ return baudrate; }

//----------------------------------------------------------------------
inline char* FTermData::getTermType()
{ return termtype; }

//----------------------------------------------------------------------
inline char* FTermData::getTermFileName()
{ return termfilename; }

//----------------------------------------------------------------------
inline const FString& FTermData::getXtermFont() const
{ return xterm_font; }

//----------------------------------------------------------------------
inline const FString& FTermData::getXtermTitle() const
{ return xterm_title; }

//----------------------------------------------------------------------
#if DEBUG
inline int FTermData::getFramebufferBpp() const
{ return framebuffer_bpp; }
#endif

//----------------------------------------------------------------------
inline bool FTermData::hasShadowCharacter() const
{ return shadow_character; }

//----------------------------------------------------------------------
inline bool FTermData::hasHalfBlockCharacter() const
{ return half_block_character; }

//----------------------------------------------------------------------
inline bool FTermData::hasCursorOptimisation() const
{ return cursor_optimisation; }

//----------------------------------------------------------------------
inline bool FTermData::isCursorHidden() const
{ return hidden_cursor; }

//----------------------------------------------------------------------
inline bool FTermData::hasAlternateScreen() const
{ return use_alternate_screen; }

//----------------------------------------------------------------------
inline bool FTermData::hasASCIIConsole() const
{ return ascii_console; }

//----------------------------------------------------------------------
inline bool FTermData::hasVT100Console() const
{ return vt100_console; }

//----------------------------------------------------------------------
inline bool FTermData::hasUTF8Console() const
{ return utf8_console; }

//----------------------------------------------------------------------
inline bool FTermData::isUTF8() const
{ return utf8_state; }

//----------------------------------------------------------------------
inline bool FTermData::isNewFont() const
{ return new_font; }

//----------------------------------------------------------------------
inline bool FTermData::isVGAFont() const
{ return vga_font; }

//----------------------------------------------------------------------
inline bool FTermData::isMonochron() const
{ return monochron; }

//----------------------------------------------------------------------
inline bool FTermData::hasTermResized() const
{ return resize_term; }

//----------------------------------------------------------------------
inline void FTermData::setTermEncoding (fc::encoding enc)
{ term_encoding = enc; }

//----------------------------------------------------------------------
inline void FTermData::setTTYFileDescriptor (int fd)
{ fd_tty = fd; }

//----------------------------------------------------------------------
inline void FTermData::setBaudrate (uInt baud)
{ baudrate = baud; }

//----------------------------------------------------------------------
inline void FTermData::supportShadowCharacter (bool available)
{ shadow_character = available; }

//----------------------------------------------------------------------
inline void FTermData::supportHalfBlockCharacter (bool available)
{ half_block_character = available; }

//----------------------------------------------------------------------
inline void FTermData::supportCursorOptimisation (bool available)
{ cursor_optimisation = available; }

//----------------------------------------------------------------------
inline void FTermData::setCursorHidden (bool hidden_state)
{ hidden_cursor = hidden_state; }

//----------------------------------------------------------------------
inline void FTermData::useAlternateScreen (bool use)
{ use_alternate_screen = use; }

//----------------------------------------------------------------------
inline void FTermData::setASCIIConsole (bool ascii)
{ ascii_console = ascii; }

//----------------------------------------------------------------------
inline void FTermData::setVT100Console (bool vt100)
{ vt100_console = vt100; }

//----------------------------------------------------------------------
inline void FTermData::setUTF8Console (bool utf8)
{ utf8_console = utf8; }

//----------------------------------------------------------------------
inline void FTermData::setUTF8 (bool utf8)
{ utf8_state = utf8; }

//----------------------------------------------------------------------
inline void FTermData::setNewFont (bool nfont)
{ new_font = nfont; }

//----------------------------------------------------------------------
inline void FTermData::setVGAFont (bool vga)
{ vga_font = vga; }

//----------------------------------------------------------------------
inline void FTermData::setMonochron (bool mono)
{ monochron = mono; }

//----------------------------------------------------------------------
inline void FTermData::setTermResized (bool resize)
{ resize_term = resize; }

//----------------------------------------------------------------------
inline void FTermData::setTermType (const char name[])
{
  if ( ! name )
    return;

  std::strncpy (termtype, name, sizeof(termtype) - 1);
  termtype[sizeof(termtype) - 1] = '\0';
}

//----------------------------------------------------------------------
inline void FTermData::setTermFileName (const char file_name[])
{
  if ( ! file_name )
    return;

  std::strncpy (termfilename, file_name, sizeof(termfilename) - 1);
  termfilename[sizeof(termfilename) - 1] = '\0';
}

//----------------------------------------------------------------------
inline void FTermData::setXtermFont (const FString& font)
{ xterm_font = font; }

//----------------------------------------------------------------------
inline void FTermData::setXtermTitle (const FString& title)
{ xterm_title = title; }

//----------------------------------------------------------------------
#if DEBUG && defined(__linux__)
inline void FTermData::setFramebufferBpp (int bpp)
{ framebuffer_bpp = bpp; }
#endif

}  // namespace finalcut

#endif  // FTERMDATA_H


