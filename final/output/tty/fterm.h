/***********************************************************************
* fterm.h - Base class for terminal control                            *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2012-2024 Markus Gans                                      *
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

/*  Base class
 *  ══════════
 *
 * ▕▔▔▔▔▔▔▔▏1       1▕▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTerm ▏-┬- - - -▕ FTermios ▏
 * ▕▁▁▁▁▁▁▁▏ :       ▕▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermcap ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermcapQuirks ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermDetection ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FTermXTerminal ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕   FTermLinux   ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕  FTermFreeBSD  ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕  FTermOpenBSD  ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FColorPalette ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FOptiAttr ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FOptiMove ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FKeyboard ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      1▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FMouseControl ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FString ▏
 *           :       ▕▁▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▔▏
 *           :- - - -▕ FPoint ▏
 *           :       ▕▁▁▁▁▁▁▁▁▏
 *           :
 *           :      *▕▔▔▔▔▔▔▔▏
 *           └- - - -▕ FRect ▏
 *                   ▕▁▁▁▁▁▁▁▏
 */

#ifndef FTERM_H
#define FTERM_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fconfig.h"

#include <sys/ioctl.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <langinfo.h>

#if defined(F_HAVE_GETTTYNAM) && defined(F_HAVE_TTYENT_H)
  #include <ttyent.h>
#endif

#include <clocale>
#include <cmath>
#include <csignal>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "final/fc.h"
#include "final/output/fcolorpalette.h"
#include "final/output/tty/ftermdata.h"
#include "final/output/tty/fterm_functions.h"
#include "final/util/fstring.h"
#include "final/util/fsystem.h"

namespace finalcut
{

// class forward declaration
class FColorPalette;
class FPoint;
class FStartOptions;
class FSize;
class FString;
class FVTermBuffer;


//----------------------------------------------------------------------
// class FTerm
//----------------------------------------------------------------------

class FTerm final
{
  public:
    // Using-declaration
    using FSetPalette = FColorPalette::FSetPalette;

    // Constructor
    FTerm();

    // Disable copy constructor
    FTerm (const FTerm&) = delete;

    // Disable move constructor
    FTerm (FTerm&&) noexcept = delete;

    // Destructor
    ~FTerm();

    // Disable copy assignment operator (=)
    auto operator = (const FTerm&) -> FTerm& = delete;

    // Disable move assignment operator (=)
    auto operator = (FTerm&&) noexcept -> FTerm& = delete;

    // Accessors
    static auto getClassName() -> FString;
    static auto getLineNumber() -> std::size_t;
    static auto getColumnNumber() -> std::size_t;
    static auto getKeyName (FKey) -> FString;
    auto        getCharSubstitutionMap() & -> FCharSubstitution&;
    static auto getTTYFileDescriptor() -> int;
    static auto getTermType() -> std::string;
    static auto getTermFileName() -> std::string;
    static auto getTabstop() -> int;
    static auto getMaxColor() -> int;

    // Inquiries
    static auto isRaw() -> bool;
    static auto hasUTF8() -> bool;
    static auto isMonochron() -> bool;
    static auto isNewFont() -> bool;
    static auto isInitialized() -> bool;
    static auto isCursorHideable() -> bool;
    static auto isEncodable (const wchar_t&) -> bool;
    static auto hasChangedTermSize() -> bool;
    static auto hasShadowCharacter() -> bool;
    static auto hasHalfBlockCharacter() -> bool;
    static auto hasAlternateScreen() -> bool;
    static auto canChangeColorPalette() -> bool;

    // Mutators
    static void setFSystem (std::unique_ptr<FSystem>&);
    static void setTermType (const std::string&);
    static void setInsertCursor (bool = true);
    static void unsetInsertCursor();
    static void redefineDefaultColors (bool = true);
    static void setDblclickInterval (const uInt64);
    static void useAlternateScreen (bool = true);
    static void setUTF8 (bool = true);
    static void unsetUTF8();

    // Methods
    static auto setVGAFont() -> bool;
    static auto setNewFont() -> bool;
    static auto resetFont() -> bool;
    static auto openConsole() -> int;
    static auto closeConsole() -> int;
    static auto moveCursorString (int, int, int, int) -> std::string;
    static auto cursorsVisibilityString (bool = true) -> std::string;
    static void detectTermSize();
    static void setTermSize (const FSize&);
    static void setTermTitle (const FString&);
    static void setKDECursor (KdeKonsoleCursorShape);
    static void saveColorMap();
    static void resetColorMap();
    static void clearTerminalAttributes();
    static void setPalette (FColor, int, int, int);
    static void setBeep (int, int);
    static void resetBeep();
    static void beep();

    static void setEncoding (Encoding);
    static auto getEncodingString() -> std::string;
    static auto charEncode (const wchar_t&) -> wchar_t;
    static auto charEncode (const wchar_t&, const Encoding&) -> wchar_t;

    static auto scrollTermForward() -> bool;
    static auto scrollTermReverse() -> bool;

    template <typename... Args>
    static void paddingPrintf (Args&&...);
    static void paddingPrint (const std::string&, int = 1);
    static void stringPrint (const std::string&);

    void initTerminal();
    static void initScreenSettings();
    static void changeTermSizeFinished();

  private:
    // Methods
    static auto getStartOptions() -> FStartOptions&;
    static void init_global_values();
    static void init_terminal_device_path();
    static void oscPrefix();
    static void oscPostfix();
    static void init_alt_charset();
    static void init_pc_charset();
    static void init_cygwin_charmap();
    static void updatePCEncodingForCygwin();
    static void updateCharMappingForCygwin();
    static void init_teraterm_charmap();
    static void init_fixed_max_color();
    static void init_termcap();
    static void init_quirks();
    static void init_optiMove();
    static void init_optiAttr();
    static auto init_font() -> bool;
    static void init_locale();
    static auto init_xterm_locale (const char*) ->  const char*;
    static auto init_tera_term_locale (const char*) ->  const char*;
    static auto init_kterm_locale (const char*) ->  const char*;
    static auto init_sun_locale (const char*) ->  const char*;
    static auto init_locale_if_not_found (const char*) ->  const char*;
    static void init_encoding();
    static void init_encoding_set();
    static void init_term_encoding();
    static void init_individual_term_encoding();
    static void init_force_vt100_encoding();
    static void init_utf8_without_alt_charset();
    static void init_tab_quirks();
    static void init_captureFontAndTitle();
    static auto hasNoFontSettingOption() -> bool;
    static auto canSetTerminalFont() -> bool;
    static void setInsertCursorStyle();
    static void setOverwriteCursorStyle();
    static auto enableCursorString() -> std::string;
    static auto disableCursorString() -> std::string;
    static void enableMouse();
    static void disableMouse();
    static void enableApplicationEscKey();
    static void disableApplicationEscKey();
    static void enableKeypad();
    static void disableKeypad();
    static void enableAlternateCharset();
    static void useAlternateScreenBuffer();
    static void useNormalScreenBuffer();
    void        init();
    auto        init_terminal() const -> bool;
    void        initOSspecifics() const;
    void        initTermspecifics() const;
    void        initBaudRate() const;
    void        finish() const;
    void        finishOSspecifics() const;
    void        finish_encoding() const;
    static void printExitMessage();
    static void terminalSizeChange();
    [[noreturn]] static void processTermination (int);
    static void setSignalHandler();
    static void resetSignalHandler();
    static void signal_handler (int);
};


// FTerm inline functions
//----------------------------------------------------------------------
inline auto FTerm::getClassName() -> FString
{ return "FTerm"; }

//----------------------------------------------------------------------
inline void FTerm::setFSystem (std::unique_ptr<FSystem>& fsystem)
{
  FSystem::getInstance().swap(fsystem);
}

//----------------------------------------------------------------------
inline void FTerm::unsetInsertCursor()
{ setInsertCursor(false); }

//----------------------------------------------------------------------
inline void FTerm::unsetUTF8()
{ setUTF8(false); }

//----------------------------------------------------------------------
template <typename... Args>
inline void FTerm::paddingPrintf (Args&&... args)
{
  const int size = std::snprintf (nullptr, 0, args...);

  if ( size <= 0 )
    return;

  std::string buffer{};
  auto buffer_size = std::size_t(size) + 1;
  buffer.resize(buffer_size);
  std::snprintf ( &*buffer.begin(), buffer_size
                , std::forward<Args>(args)... );
  buffer_size--;
  buffer.resize(buffer_size);
  paddingPrint (buffer, 1);
}

//----------------------------------------------------------------------
inline void FTerm::initTerminal()
{ init(); }

}  // namespace finalcut

//----------------------------------------------------------------------
inline auto operator << (std::ostream& os, finalcut::UniChar c) -> std::ostream&
{
  static const auto& data = finalcut::FTermData::getInstance();

  if ( data.getTerminalEncoding() == finalcut::Encoding::UTF8 )
    return os << finalcut::unicode_to_utf8(wchar_t(c));

  return os << static_cast<char>(uChar(c));
}

//----------------------------------------------------------------------
inline auto operator << (std::wostream& os, finalcut::UniChar c) -> std::wostream&
{
  return os << static_cast<wchar_t>(c);
}

#endif  // FTERM_H
