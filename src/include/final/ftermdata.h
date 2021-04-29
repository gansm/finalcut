/***********************************************************************
* ftermdata.h - Data class for FTerm                                   *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2021 Markus Gans                                      *
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
 * ▕▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermData ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMDATA_H
#define FTERMDATA_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <unordered_map>
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

class FTermData final
{
  public:
    // Using-declaration
    using EncodingMap = std::unordered_map<std::string, Encoding>;

    // Constructors
    FTermData () = default;

    // Disable copy constructor
    FTermData (const FTermData&) = delete;

    // Destructor
    ~FTermData() noexcept = default;

    // Disable copy assignment operator (=)
    FTermData& operator = (const FTermData&) = delete;

    // Accessors
    FString            getClassName() const;
    EncodingMap&       getEncodingList();
    charSubstitution&  getCharSubstitutionMap();
    Encoding           getTermEncoding() const;
    FRect&             getTermGeometry();
    int                getTTYFileDescriptor() const;
    uInt               getBaudrate() const;
    const std::string& getTermType() const;
    const std::string& getTermFileName() const;
    const FString&     getXtermFont() const;
    const FString&     getXtermTitle() const;
    const FString&     getExitMessage() const;
#if DEBUG
    int                getFramebufferBpp() const;
#endif

    // Inquiries
    bool               hasShadowCharacter() const;
    bool               hasHalfBlockCharacter() const;
    bool               hasCursorOptimisation() const;
    bool               isCursorHidden() const;
    bool               hasAlternateScreen() const;
    bool               isInAlternateScreen() const;
    bool               hasASCIIConsole() const;
    bool               hasVT100Console() const;
    bool               hasUTF8Console() const;
    bool               isUTF8() const;
    bool               isNewFont() const;
    bool               isVGAFont() const;
    bool               isMonochron() const;
    bool               hasTermResized() const;

    // Mutators
    void               setTermEncoding (Encoding);
    void               setTTYFileDescriptor (int);
    void               setBaudrate (uInt);
    void               supportShadowCharacter (bool = true);
    void               supportHalfBlockCharacter (bool = true);
    void               supportCursorOptimisation (bool = true);
    void               setCursorHidden (bool = true);
    void               useAlternateScreen (bool = true);
    void               setAlternateScreenInUse (bool = true);
    void               setASCIIConsole (bool = true);
    void               setVT100Console (bool = true);
    void               setUTF8Console (bool = true);
    void               setUTF8 (bool = true);
    void               setNewFont (bool = true);
    void               setVGAFont (bool = true);
    void               setMonochron (bool = true);
    void               setTermResized (bool = true);
    void               setTermType (const std::string&);
    void               setTermFileName (const std::string&);
    void               setXtermFont (const FString&);
    void               setXtermTitle (const FString&);
    void               setExitMessage (const FString&);
#if DEBUG
    void               setFramebufferBpp (int);
#endif

  private:
    // Data members
    EncodingMap        encoding_list{};
    charSubstitution   char_substitution_map{};
    FRect              term_geometry{};  // current terminal geometry
    FString            xterm_font{};
    FString            xterm_title{};
    FString            exit_message{};
    Encoding           term_encoding{Encoding::Unknown};
    int                fd_tty{-1};  // Teletype (tty) file descriptor
                                    // is still undefined
#if DEBUG
    int                framebuffer_bpp{-1};
#endif

    uInt               baudrate{0};
    std::string        termtype{};
    std::string        termfilename{};
    bool               shadow_character{true};
    bool               half_block_character{true};
    bool               cursor_optimisation{true};
    bool               hidden_cursor{false};  // Global cursor hidden state
    bool               use_alternate_screen{true};
    bool               alternate_screen{false};
    bool               ascii_console{false};
    bool               vt100_console{false};
    bool               utf8_console{false};
    bool               utf8_state{false};
    bool               new_font{false};
    bool               vga_font{false};
    bool               monochron{false};
    bool               resize_term{false};
};

// FTermData inline functions
//----------------------------------------------------------------------
inline FString FTermData::getClassName() const
{ return "FTermData"; }

//----------------------------------------------------------------------
inline FTermData::EncodingMap& FTermData::getEncodingList()
{ return encoding_list; }

//----------------------------------------------------------------------
inline charSubstitution& FTermData::getCharSubstitutionMap()
{ return char_substitution_map; }

//----------------------------------------------------------------------
inline Encoding FTermData::getTermEncoding() const
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
inline const std::string& FTermData::getTermType() const
{ return termtype; }

//----------------------------------------------------------------------
inline const std::string& FTermData::getTermFileName() const
{ return termfilename; }

//----------------------------------------------------------------------
inline const FString& FTermData::getXtermFont() const
{ return xterm_font; }

//----------------------------------------------------------------------
inline const FString& FTermData::getXtermTitle() const
{ return xterm_title; }

//----------------------------------------------------------------------
inline const FString& FTermData::getExitMessage() const
{ return exit_message; }

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
inline bool FTermData::isInAlternateScreen() const
{ return alternate_screen; }

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
inline void FTermData::setTermEncoding (Encoding enc)
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
inline void FTermData::setAlternateScreenInUse (bool in_use)
{ alternate_screen = in_use; }

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
inline void FTermData::setTermType (const std::string& name)
{
  if ( ! name.empty() )
    termtype = name;
}

//----------------------------------------------------------------------
inline void FTermData::setTermFileName (const std::string& file_name)
{
  if ( ! file_name.empty() )
    termfilename = file_name;
}

//----------------------------------------------------------------------
inline void FTermData::setXtermFont (const FString& font)
{ xterm_font = font; }

//----------------------------------------------------------------------
inline void FTermData::setXtermTitle (const FString& title)
{ xterm_title = title; }

//----------------------------------------------------------------------
inline void FTermData::setExitMessage (const FString& message)
{ exit_message = message; }

//----------------------------------------------------------------------
#if DEBUG && defined(__linux__)
inline void FTermData::setFramebufferBpp (int bpp)
{ framebuffer_bpp = bpp; }
#endif

}  // namespace finalcut

#endif  // FTERMDATA_H
