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

#include <atomic>
#include <bitset>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/util/frect.h"
#include "final/util/fstring.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FCharSubstitution
//----------------------------------------------------------------------

class FCharSubstitution
{
  public:
    struct Map
    {
      wchar_t from;
      wchar_t to;
    };

    wchar_t getMappedChar (wchar_t);
    void setCharMapping (const Map&);
    bool isEmpty();

  private:
    std::vector<Map> sub_map{};
};

// FTermData inline functions
//----------------------------------------------------------------------
inline wchar_t FCharSubstitution::getMappedChar (wchar_t c)
{
  const auto& end = sub_map.end();
  auto iter = std::find_if ( sub_map.begin(), end,
                             [&c] (const Map& map)
                             {
                               return map.from == c;
                             } );

  if ( iter == end )
    return L'\0';
  else
    return iter->to;
}

//----------------------------------------------------------------------
inline void FCharSubstitution::setCharMapping (const Map& m)
{
  const auto& end = sub_map.end();
  auto iter = std::find_if ( sub_map.begin(), end,
                             [&m] (const Map& map)
                             {
                               return map.from == m.from;
                             } );

  if ( iter == end )
    sub_map.push_back(m);
  else
    iter->to = m.to;
}

//----------------------------------------------------------------------
inline bool FCharSubstitution::isEmpty()
{
  return sub_map.empty();
}


//----------------------------------------------------------------------
// class FTermData
//----------------------------------------------------------------------

class FTermData final
{
  public:
    struct kittyVersion
    {
      int primary;
      int secondary;
    };

    // Using-declaration
    using EncodingMap = std::unordered_map<std::string, Encoding>;

    // Constructors
    FTermData () = default;

    // Accessors
    FString            getClassName() const;
    static auto        getInstance() -> FTermData&;
    EncodingMap&       getEncodingList() &;
    FCharSubstitution& getCharSubstitutionMap() &;
    Encoding           getTermEncoding() const;
    FRect&             getTermGeometry() &;
    int                getTTYFileDescriptor() const noexcept;
    uInt               getBaudrate() const noexcept;
    const std::string& getTermType() const & ;
    const std::string& getTermFileName() const &;
    int                getGnomeTerminalID() const noexcept;
    kittyVersion       getKittyVersion() const noexcept;
    const FString&     getXtermFont() const &;
    const FString&     getXtermTitle() const &;
#if DEBUG
    int                getFramebufferBpp() const noexcept;
#endif

    // Inquiries
    bool               hasShadowCharacter() const noexcept;
    bool               hasHalfBlockCharacter() const noexcept;
    bool               hasCursorOptimisation() const noexcept;
    bool               isCursorHidden() const noexcept;
    bool               hasAlternateScreen() const noexcept;
    bool               isInAlternateScreen() const noexcept;
    bool               hasASCIIConsole() const noexcept;
    bool               hasVT100Console() const noexcept;
    bool               hasUTF8Console() const noexcept;
    bool               isUTF8() const noexcept;
    bool               isNewFont() const noexcept;
    bool               isVGAFont() const noexcept;
    bool               isMonochron() const noexcept;
    bool               hasTermResized();
    bool               isTermType (FTermType) const;
    bool               isTermType (FTermTypeT) const;

    // Mutators
    void               setTermEncoding (Encoding) noexcept;
    void               setTTYFileDescriptor (int) noexcept;
    void               setBaudrate (uInt) noexcept;
    void               supportShadowCharacter (bool = true) noexcept;
    void               supportHalfBlockCharacter (bool = true) noexcept;
    void               supportCursorOptimisation (bool = true) noexcept;
    void               setCursorHidden (bool = true) noexcept;
    void               useAlternateScreen (bool = true) noexcept;
    void               setAlternateScreenInUse (bool = true) noexcept;
    void               setASCIIConsole (bool = true) noexcept;
    void               setVT100Console (bool = true) noexcept;
    void               setUTF8Console (bool = true) noexcept;
    void               setUTF8 (bool = true) noexcept;
    void               setNewFont (bool = true) noexcept;
    void               setVGAFont (bool = true) noexcept;
    void               setMonochron (bool = true) noexcept;
    void               setTermResized (bool = true);
    void               setTermType (const std::string&);
    void               setTermType (FTermType);
    void               unsetTermType (FTermType);
    void               setTermFileName (const std::string&);
    void               setGnomeTerminalID (int) noexcept;
    void               setKittyVersion (const kittyVersion&);
    void               setXtermFont (const FString&);
    void               setXtermTitle (const FString&);
#if DEBUG
    void               setFramebufferBpp (int) noexcept;
#endif

  private:
    // Data members
    EncodingMap           encoding_list{};
    FCharSubstitution     char_substitution_map{};
    FRect                 term_geometry{};  // current terminal geometry
    FString               xterm_font{};
    FString               xterm_title{};
    FString               exit_message{};
    FTermTypeT            terminal_type{};
    Encoding              term_encoding{Encoding::Unknown};

    // Teletype (tty) file descriptor is still undefined (-1)
    int                   fd_tty{-1};

    // Gnome terminal id from SecDA
    // Example: vte version 0.40.0 = 0 * 100 + 40 * 100 + 0 = 4000
    //                      a.b.c  = a * 100 +  b * 100 + c
    int                   gnome_terminal_id{0};
    kittyVersion          kitty_version{0, 0};

#if DEBUG
    int                   framebuffer_bpp{-1};
#endif

    uInt                  baudrate{0};
    std::string           termtype{};
    std::string           termfilename{};
    std::mutex            resize_mutex{};
    std::atomic<int>      resize_count{0};
    bool                  shadow_character{true};
    bool                  half_block_character{true};
    bool                  cursor_optimisation{true};
    bool                  hidden_cursor{false};  // Global cursor hidden state
    bool                  use_alternate_screen{true};
    bool                  alternate_screen{false};
    bool                  ascii_console{false};
    bool                  vt100_console{false};
    bool                  utf8_console{false};
    bool                  utf8_state{false};
    bool                  new_font{false};
    bool                  vga_font{false};
    bool                  monochron{false};
};

// FTermData inline functions
//----------------------------------------------------------------------
inline FString FTermData::getClassName() const
{ return "FTermData"; }

//----------------------------------------------------------------------
inline auto FTermData::getInstance() -> FTermData&
{
  static const auto& data = make_unique<FTermData>();
  return *data;
}

//----------------------------------------------------------------------
inline FTermData::EncodingMap& FTermData::getEncodingList() &
{ return encoding_list; }

//----------------------------------------------------------------------
inline FCharSubstitution& FTermData::getCharSubstitutionMap() &
{ return char_substitution_map; }

//----------------------------------------------------------------------
inline Encoding FTermData::getTermEncoding() const
{ return term_encoding; }

//----------------------------------------------------------------------
inline FRect& FTermData::getTermGeometry() &
{ return term_geometry; }

//----------------------------------------------------------------------
inline int FTermData::getTTYFileDescriptor() const noexcept
{ return fd_tty; }

//----------------------------------------------------------------------
inline uInt FTermData::getBaudrate() const noexcept
{ return baudrate; }

//----------------------------------------------------------------------
inline const std::string& FTermData::getTermType() const &
{ return termtype; }

//----------------------------------------------------------------------
inline const std::string& FTermData::getTermFileName() const &
{ return termfilename; }

//----------------------------------------------------------------------
inline int FTermData::getGnomeTerminalID() const noexcept
{ return gnome_terminal_id; }

//----------------------------------------------------------------------
inline FTermData::kittyVersion FTermData::getKittyVersion() const noexcept
{ return kitty_version; }

//----------------------------------------------------------------------
inline const FString& FTermData::getXtermFont() const &
{ return xterm_font; }

//----------------------------------------------------------------------
inline const FString& FTermData::getXtermTitle() const &
{ return xterm_title; }

//----------------------------------------------------------------------
#if DEBUG
inline int FTermData::getFramebufferBpp() const noexcept
{ return framebuffer_bpp; }
#endif

//----------------------------------------------------------------------
inline bool FTermData::hasShadowCharacter() const noexcept
{ return shadow_character; }

//----------------------------------------------------------------------
inline bool FTermData::hasHalfBlockCharacter() const noexcept
{ return half_block_character; }

//----------------------------------------------------------------------
inline bool FTermData::hasCursorOptimisation() const noexcept
{ return cursor_optimisation; }

//----------------------------------------------------------------------
inline bool FTermData::isCursorHidden() const noexcept
{ return hidden_cursor; }

//----------------------------------------------------------------------
inline bool FTermData::hasAlternateScreen() const noexcept
{ return use_alternate_screen; }

//----------------------------------------------------------------------
inline bool FTermData::isInAlternateScreen() const noexcept
{ return alternate_screen; }

//----------------------------------------------------------------------
inline bool FTermData::hasASCIIConsole() const noexcept
{ return ascii_console; }

//----------------------------------------------------------------------
inline bool FTermData::hasVT100Console() const noexcept
{ return vt100_console; }

//----------------------------------------------------------------------
inline bool FTermData::hasUTF8Console() const noexcept
{ return utf8_console; }

//----------------------------------------------------------------------
inline bool FTermData::isUTF8() const noexcept
{ return utf8_state; }

//----------------------------------------------------------------------
inline bool FTermData::isNewFont() const noexcept
{ return new_font; }

//----------------------------------------------------------------------
inline bool FTermData::isVGAFont() const noexcept
{ return vga_font; }

//----------------------------------------------------------------------
inline bool FTermData::isMonochron() const noexcept
{ return monochron; }

//----------------------------------------------------------------------
inline bool FTermData::hasTermResized()
{
  std::lock_guard<std::mutex> resize_lock_guard(resize_mutex);
  return resize_count.load() > 0;
}

//----------------------------------------------------------------------
inline bool FTermData::isTermType (FTermType type) const
{ return terminal_type & static_cast<FTermTypeT>(type); }

//----------------------------------------------------------------------
inline bool FTermData::isTermType (FTermTypeT mask) const
{ return terminal_type & mask; }

//----------------------------------------------------------------------
inline void FTermData::setTermEncoding (Encoding enc) noexcept
{ term_encoding = enc; }

//----------------------------------------------------------------------
inline void FTermData::setTTYFileDescriptor (int fd) noexcept
{ fd_tty = fd; }

//----------------------------------------------------------------------
inline void FTermData::setBaudrate (uInt baud) noexcept
{ baudrate = baud; }

//----------------------------------------------------------------------
inline void FTermData::supportShadowCharacter (bool available) noexcept
{ shadow_character = available; }

//----------------------------------------------------------------------
inline void FTermData::supportHalfBlockCharacter (bool available) noexcept
{ half_block_character = available; }

//----------------------------------------------------------------------
inline void FTermData::supportCursorOptimisation (bool available) noexcept
{ cursor_optimisation = available; }

//----------------------------------------------------------------------
inline void FTermData::setCursorHidden (bool hidden_state) noexcept
{ hidden_cursor = hidden_state; }

//----------------------------------------------------------------------
inline void FTermData::useAlternateScreen (bool use) noexcept
{ use_alternate_screen = use; }

//----------------------------------------------------------------------
inline void FTermData::setAlternateScreenInUse (bool in_use) noexcept
{ alternate_screen = in_use; }

//----------------------------------------------------------------------
inline void FTermData::setASCIIConsole (bool ascii) noexcept
{ ascii_console = ascii; }

//----------------------------------------------------------------------
inline void FTermData::setVT100Console (bool vt100) noexcept
{ vt100_console = vt100; }

//----------------------------------------------------------------------
inline void FTermData::setUTF8Console (bool utf8) noexcept
{ utf8_console = utf8; }

//----------------------------------------------------------------------
inline void FTermData::setUTF8 (bool utf8) noexcept
{ utf8_state = utf8; }

//----------------------------------------------------------------------
inline void FTermData::setNewFont (bool nfont) noexcept
{ new_font = nfont; }

//----------------------------------------------------------------------
inline void FTermData::setVGAFont (bool vga) noexcept
{ vga_font = vga; }

//----------------------------------------------------------------------
inline void FTermData::setMonochron (bool mono) noexcept
{ monochron = mono; }

//----------------------------------------------------------------------
inline void FTermData::setTermResized (bool resize)
{
  std::lock_guard<std::mutex> resize_lock_guard(resize_mutex);

  if ( resize )
    ++resize_count;
  else if ( resize_count.load() > 0 )
    --resize_count;
}

//----------------------------------------------------------------------
inline void FTermData::setTermType (const std::string& name)
{
  if ( ! name.empty() )
    termtype = name;
}

//----------------------------------------------------------------------
inline void FTermData::setTermType (FTermType type)
{ terminal_type |= static_cast<FTermTypeT>(type); }

//----------------------------------------------------------------------
inline void FTermData::unsetTermType (FTermType type)
{ terminal_type &= ~(static_cast<FTermTypeT>(type)); }

//----------------------------------------------------------------------
inline void FTermData::setTermFileName (const std::string& file_name)
{
  if ( ! file_name.empty() )
    termfilename = file_name;
}

//----------------------------------------------------------------------
inline void FTermData::setGnomeTerminalID (int id) noexcept
{ gnome_terminal_id = id; }

//----------------------------------------------------------------------
inline void FTermData::setKittyVersion(const kittyVersion& version)
{
  kitty_version.primary = version.primary;
  kitty_version.secondary = version.secondary;
}

//----------------------------------------------------------------------
inline void FTermData::setXtermFont (const FString& font)
{ xterm_font = font; }

//----------------------------------------------------------------------
inline void FTermData::setXtermTitle (const FString& title)
{ xterm_title = title; }

//----------------------------------------------------------------------
#if DEBUG && defined(__linux__)
inline void FTermData::setFramebufferBpp (int bpp) noexcept
{ framebuffer_bpp = bpp; }
#endif

}  // namespace finalcut

#endif  // FTERMDATA_H
