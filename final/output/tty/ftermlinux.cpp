/***********************************************************************
* ftermlinux.cpp - Contains the Linux terminal functions               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2018-2023 Markus Gans                                      *
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

#include <array>
#include <memory>
#include <vector>

#include "final/fapplication.h"
#include "final/fc.h"
#include "final/ftypes.h"
#include "final/output/tty/fcharmap.h"
#include "final/output/tty/ftermcap.h"
#include "final/output/tty/fterm.h"
#include "final/output/tty/ftermlinux.h"
#include "final/util/flog.h"
#include "final/util/fsystem.h"

#if defined(__linux__)

#include <linux/keyboard.h>  // need keyboard modifiers
#include "final/font/newfont.h"
#include "final/font/unicodemap.h"
#include "final/font/vgafont.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FTermLinux
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermLinux::~FTermLinux()  // destructor
{
  deleteFontData(screen_font);
  deleteUnicodeMapEntries(screen_unicode_map);
}

// public methods of FTermLinux
//----------------------------------------------------------------------
auto FTermLinux::getInstance() -> FTermLinux&
{
  static const auto& linux_console = std::make_unique<FTermLinux>();
  return *linux_console;
}

//----------------------------------------------------------------------
auto FTermLinux::getCursorStyle() const -> CursorStyle
{
  // Get the current set cursor style

  return linux_console_cursor_style;
}

//----------------------------------------------------------------------
auto FTermLinux::getCursorStyleString() -> char*
{
  // Gets the current cursor style string of the Linux console

  static std::array<char, 16> buf{};
  std::fill (std::begin(buf), std::end(buf), '\0');
  std::snprintf (buf.data(), buf.size(), CSI "?%dc", int(getCursorStyle()));
  return buf.data();
}

//----------------------------------------------------------------------
auto FTermLinux::setCursorStyle (CursorStyle style) -> bool
{
  // Set cursor style in linux console

  if ( ! isLinuxTerm() )
    return false;

  linux_console_cursor_style = style;
  static const auto& fterm_data = FTermData::getInstance();

  if ( fterm_data.isCursorHidden() )
    return false;

  setLinuxCursorStyle(style);
  return true;
}

//----------------------------------------------------------------------
void FTermLinux::setUTF8 (bool enable) const
{
  if ( ! isLinuxTerm() )
    return;

  if ( enable )
    FTerm::paddingPrint (ESC "%G");
  else
    FTerm::paddingPrint (ESC "%@");

  std::fflush(stdout);
}

//----------------------------------------------------------------------
#if defined(ISA_SYSCTL_SUPPORT)
auto FTermLinux::setPalette (FColor index, int r, int g, int b) -> bool
{
  if ( ! isLinuxTerm() )
    return false;

  return setVGAPalette (index, r, g, b);
}
#else
bool FTermLinux::setPalette (FColor, int, int, int)
{
  return false;
}
#endif

//----------------------------------------------------------------------
auto FTermLinux::isLinuxConsole() -> bool
{
  // Check if it's a Linux console

  char arg{0};
  int fd_tty = FTerm::getTTYFileDescriptor();
  static const auto& fsystem = FSystem::getInstance();

  if ( fd_tty < 0 )  // Undefined tty file descriptor
    fd_tty = 0;

  // Get keyboard type an compare
  return ( fsystem->isTTY(fd_tty)
        && fsystem->ioctl(fd_tty, KDGKBTYPE, &arg) == 0
        && ((arg == KB_101) || (arg == KB_84)) );
}

//----------------------------------------------------------------------
void FTermLinux::init()
{
  // Initialize Linux console

  static auto& fterm_data = FTermData::getInstance();
  screen_unicode_map.entries = nullptr;
  screen_font.data = nullptr;
  fterm_data.supportShadowCharacter (true);
  fterm_data.supportHalfBlockCharacter (true);
  initKeyMap();

#if defined(ISA_SYSCTL_SUPPORT)
  getVGAPalette();
#endif

  if ( FTerm::openConsole() == 0 )
  {
    if ( isLinuxConsole() )
    {
      fterm_data.setTermType(FTermType::linux_con);
      getUnicodeMap();
      getScreenFont();

#if defined(ISA_SYSCTL_SUPPORT)
      // Enable 16 background colors
      if ( setBlinkAsIntensity(true) == 0 )
        FTermcap::max_color = 16;
      else
        FTermcap::max_color = 8;
#endif
      // Underline cursor
      setCursorStyle (LinuxConsoleCursorStyle::Underscore);

      // Framebuffer color depth in bits per pixel
      framebuffer_bpp = getFramebuffer_bpp();

      // More than 4 bits per pixel and the font uses the blink-bit
      if ( framebuffer_bpp >= 4 && screen_font.charcount <= 256 )
        FTermcap::max_color = 16;
    }

    FTerm::detectTermSize();
    FTerm::closeConsole();
  }
  else
  {
    std::clog << FLog::LogLevel::Error << "Can not open the console." << std::endl;
    std::abort();
  }
}

//----------------------------------------------------------------------
void FTermLinux::initCharMap() const
{
  constexpr sInt16 NOT_FOUND = -1;

  if ( new_font || vga_font )
    return;

  if ( screen_unicode_map.entry_ct > 0 && screen_unicode_map.entries )
  {
    for (auto&& entry : FCharMap::getCharEncodeMap())
    {
      const auto ucs = entry.unicode;
      const sInt16 fontpos = getFontPos(ucs);

      // Fix for a non-cp437 Linux console with PC charset encoding
      if ( fontpos > 255 || fontpos == NOT_FOUND )
        entry.pc = entry.ascii;

      // Character substitutions for missing characters
      if ( fontpos == NOT_FOUND )
      {
        characterFallback (ucs, { L'▲', L'↑', L'^' });
        characterFallback (ucs, { L'▼', L'↓', L'v' });
        characterFallback (ucs, { L'►', L'▶', L'→', L'>' });
        characterFallback (ucs, { L'◄', L'◀', L'←', L'<' });
        characterFallback (ucs, { L'●', L'◆', L'⬤', L'*' });
        characterFallback (ucs, { L'•', L'●', L'◆', L'⬤', L'*' });
        characterFallback (ucs, { L'×', L'❌', L'x' });
        characterFallback (ucs, { L'÷', L'➗', L'/' });
        characterFallback (ucs, { L'√', L'✓', L'x' });
        characterFallback (ucs, { L'ˣ', L'ⁿ', L'ˆ', L'`' });
      }
    }
  }

  FTermData::getInstance().getCharSubstitutionMap().sort();
  initSpecialCharacter();
}

//----------------------------------------------------------------------
void FTermLinux::finish() const
{
  if ( isLinuxTerm() )
  {
#if defined(ISA_SYSCTL_SUPPORT)
    setBlinkAsIntensity (false);
#endif
    setLinuxCursorStyle (LinuxConsoleCursorStyle::Default);
  }
}

//----------------------------------------------------------------------
auto FTermLinux::loadVGAFont() -> bool
{
  vga_font = true;
  new_font = false;

  if ( FTerm::openConsole() == 0 )
  {
    if ( isLinuxConsole() )
    {
      // Set the standard vga font 8x16
      const int ret = setScreenFont(fc::F_8x16std, 256, 8, 16);
      vga_font = bool( ret == 0 );

      // Unicode character mapping
      struct unimapdesc unimap{};
      unimap.entry_ct = uInt16(fc::unicode_cp437_pairs.size());
      unimap.entries = const_cast<unipair*>(fc::unicode_cp437_pairs.data());
      setUnicodeMap(&unimap);
    }
    else
      vga_font = false;

    FTerm::detectTermSize();
    FTerm::closeConsole();
  }
  else
    vga_font = false;

  if ( vga_font )
  {
    static auto& fterm_data = FTermData::getInstance();
    fterm_data.supportShadowCharacter (true);
    fterm_data.supportHalfBlockCharacter (true);
  }

  return vga_font;
}

//----------------------------------------------------------------------
auto FTermLinux::loadNewFont() -> bool
{
  new_font = true;
  vga_font = false;

  if ( FTerm::openConsole() == 0 )
  {
    if ( isLinuxConsole() )
    {
      // Set the graphical font
      int ret;

#if defined(ISA_SYSCTL_SUPPORT)
      if ( has9BitCharacters() )
        ret = setScreenFont(fc::F_9x16graph, 256, 8, 16);  // set 9×16
      else
#endif
        ret = setScreenFont(fc::F_8x16graph, 256, 8, 16);  // set 8×16

      new_font = bool( ret == 0 );

      // Unicode character mapping
      struct unimapdesc unimap;
      unimap.entry_ct = uInt16(fc::unicode_newfont_pairs.size());
      unimap.entries = const_cast<unipair*>(fc::unicode_newfont_pairs.data());
      setUnicodeMap(&unimap);
    }
    else
      new_font = false;

    FTerm::detectTermSize();
    FTerm::closeConsole();
  }
  else
    new_font = false;

  if ( new_font )
  {
    static auto& fterm_data = FTermData::getInstance();
    fterm_data.supportShadowCharacter (true);
    fterm_data.supportHalfBlockCharacter (true);
  }

  return new_font;
}

//----------------------------------------------------------------------
auto FTermLinux::loadOldFont() -> bool
{
  bool retval{false};

  if ( FTerm::openConsole() == 0 )
  {
    if ( isLinuxConsole() )
    {
      if ( screen_font.data )
      {
        const int ret = setScreenFont ( screen_font.data
                                      , screen_font.charcount
                                      , screen_font.width
                                      , screen_font.height
                                      , true );
        retval = bool( ret == 0 );
        deleteFontData(screen_font);
      }

      if ( screen_unicode_map.entries )
      {
        setUnicodeMap (&screen_unicode_map);
        initCharMap();
        deleteUnicodeMapEntries(screen_unicode_map);
      }
    }

    FTerm::detectTermSize();
    FTerm::closeConsole();
  }

  if ( retval )
    vga_font = new_font = false;

  return retval;
}

//----------------------------------------------------------------------
auto FTermLinux::saveColorMap() -> bool
{
  if ( ! isLinuxTerm() )
    return false;

#if defined(ISA_SYSCTL_SUPPORT)
  return saveVGAPalette();
#else
  return false;
#endif
}

//----------------------------------------------------------------------
auto FTermLinux::resetColorMap() -> bool
{
  if ( ! isLinuxTerm() )
    return false;

#if defined(ISA_SYSCTL_SUPPORT)
  return resetVGAPalette();
#else
  return false;
#endif
}

//----------------------------------------------------------------------
void FTermLinux::setBeep (int Hz, int ms) const
{
  if ( ! isLinuxTerm() )
    return;

  // Range for frequency: 21-32766
  if ( Hz < 21 || Hz > 32766 )
    return;

  // Range for duration:  0-1999
  if ( ms < 0 || ms > 1999 )
    return;

  FTerm::paddingPrintf ( CSI "10;%d]"
                         CSI "11;%d]"
                       , Hz, ms );
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTermLinux::resetBeep() const
{
  if ( ! isLinuxTerm() )
    return;

  // Default frequency: 750 Hz
  // Default duration:  125 ms
  FTerm::paddingPrint ( CSI "10;750]"
                        CSI "11;125]" );
  std::fflush(stdout);
}

//----------------------------------------------------------------------
auto FTermLinux::modifierKeyCorrection (const FKey& key_id) -> FKey
{
  // Get the current modifier key state

  if ( key_map.empty() )
    return key_id;

  const Pair pair{getModifierKey(), key_id};
  const auto& iter = key_map.find(pair);

  if ( iter == key_map.end() )  // Not found
    return key_id;

  return iter->second;  // Found
}


// private methods of FTermLinux
//----------------------------------------------------------------------
auto FTermLinux::getFramebuffer_bpp() const -> int
{
  int fd{-1};
  const char* fb = "/dev/fb/0";
  struct fb_var_screeninfo fb_var{};
  struct fb_fix_screeninfo fb_fix{};
  static const auto& fsystem = FSystem::getInstance();

  if ( (fd = fsystem->open(fb, O_RDWR)) < 0 )
  {
    if ( errno != ENOENT && errno != ENOTDIR )
      return -1;

    fb = "/dev/fb0";

    if ( (fd = fsystem->open(fb, O_RDWR)) < 0 )
      return -1;
  }

  if ( fsystem->ioctl(fd, FBIOGET_VSCREENINFO, &fb_var) == 0
    && fsystem->ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix) == 0 )
  {
    fsystem->close(fd);
    return static_cast<int>(fb_var.bits_per_pixel);
  }

  fsystem->close(fd);
  return -1;
}

//----------------------------------------------------------------------
auto FTermLinux::getScreenFont() -> bool
{
  struct console_font_op font{};
  const int fd_tty = FTerm::getTTYFileDescriptor();
  int ret{-1};

  if ( fd_tty < 0 )
    return false;

  // Initialize unused padding bytes in struct
  std::memset (&font, 0, sizeof(console_font_op));

  font.op = KD_FONT_OP_GET;
  font.flags = 0;
  font.width = 32;
  font.height = 32;
  font.charcount = 512;

  // Initialize with 0
  static constexpr std::size_t data_size = 4 * 32 * 512;
  font_data.resize(data_size);
  font.data = font_data.data();

  // Font operation
  static const auto& fsystem = FSystem::getInstance();
  ret = fsystem->ioctl (fd_tty, KDFONTOP, &font);

  if ( ret != 0 )
  {
    deleteFontData(font);
    return false;
  }

  screen_font.width = font.width;
  screen_font.height = font.height;
  screen_font.charcount = font.charcount;
  screen_font.data = font.data;
  return true;
}

//----------------------------------------------------------------------
auto FTermLinux::getUnicodeMap() -> bool
{
  const int fd_tty = FTerm::getTTYFileDescriptor();
  int ret{-1};

  if ( fd_tty < 0 )
    return false;

  screen_unicode_map.entry_ct = 0;
  screen_unicode_map.entries = nullptr;

  // Get count
  static const auto& fsystem = FSystem::getInstance();
  ret = fsystem->ioctl (fd_tty, GIO_UNIMAP, &screen_unicode_map);

  if ( ret != 0 )
  {
    const std::size_t count = screen_unicode_map.entry_ct;

    if ( errno != ENOMEM || count == 0 )
      return false;

    unicode_entries.resize(count);
    screen_unicode_map.entries = unicode_entries.data();

    // Get unicode-to-font mapping from kernel
    ret = fsystem->ioctl (fd_tty, GIO_UNIMAP, &screen_unicode_map);

    if ( ret != 0 )
      return false;
  }

  return true;
}

//----------------------------------------------------------------------
auto FTermLinux::getModifierKey() & -> ModifierKey&
{
  // Get Linux console shift state

  char subcode{6};  // Shift state command + return value

  // Fill bit field with 0
  std::memset (&mod_key, 0x00, sizeof(mod_key));

  static const auto& fsystem = FSystem::getInstance();

  // TIOCLINUX, subcode = 6 (TIOCL_GETSHIFTSTATE)
  if ( fsystem->ioctl(0, TIOCLINUX, &subcode) >= 0 )
  {
    if ( uChar(subcode) & (1u << KG_SHIFT) )
      mod_key.shift = true;

    if ( uChar(subcode) & (1u << KG_ALTGR) )
      mod_key.alt_gr = true;

    if ( uChar(subcode) & (1u << KG_CTRL) )
      mod_key.ctrl = true;

    if ( uChar(subcode) & (1u << KG_ALT) )
      mod_key.alt = true;
  }

  return mod_key;
}

//----------------------------------------------------------------------
auto FTermLinux::isLinuxTerm() const -> bool
{
  static const auto& fterm_data = FTermData::getInstance();
  return fterm_data.isTermType(FTermType::linux_con);
}

//----------------------------------------------------------------------
auto FTermLinux::setScreenFont ( const uChar fontdata[], uInt count
                              , uInt fontwidth, uInt fontheight
                              , bool direct) -> int
{
  struct console_font_op font{};
  const int fd_tty = FTerm::getTTYFileDescriptor();
  int ret{-1};

  if ( fd_tty < 0 )
    return -1;

  // Initialize unused padding bytes in struct
  std::memset (&font, 0x00, sizeof(console_font_op));

  font.op = KD_FONT_OP_SET;
  font.flags = 0;
  font.width = fontwidth;
  font.height = fontheight;
  font.charcount = count;

  if ( direct )
    font.data = const_cast<uChar*>(fontdata);
  else
  {
    const std::size_t bytes_per_line = font.width / 8;
    const std::size_t data_size = bytes_per_line * 32 * font.charcount;
    font_data.resize(data_size);  // Initialize with 0
    font.data = font_data.data();

    for (std::size_t i{0}; i < count; i++)
      std::memcpy ( font.data + bytes_per_line * 32 * i
                  , &fontdata[i * font.height]
                  , font.height );
  }

  // Font operation
  static const auto& fsystem = FSystem::getInstance();
  ret = fsystem->ioctl (fd_tty, KDFONTOP, &font);

  if ( ret != 0 && errno != ENOSYS && errno != EINVAL )
  {
    if ( ! direct )
      deleteFontData(font);

    return -1;
  }

  if ( ! direct )
    deleteFontData(font);

  if ( ret == 0 )
    return 0;

  return -1;
}

//----------------------------------------------------------------------
auto FTermLinux::setUnicodeMap (struct unimapdesc* unimap) const -> int
{
  struct unimapinit advice{};
  const int fd_tty = FTerm::getTTYFileDescriptor();
  int ret{-1};

  if ( fd_tty < 0 )
    return -1;

  advice.advised_hashsize = 0;
  advice.advised_hashstep = 0;
  advice.advised_hashlevel = 0;

  do
  {
    // Clear the unicode-to-font table
    static const auto& fsystem = FSystem::getInstance();
    ret = fsystem->ioctl (fd_tty, PIO_UNIMAPCLR, &advice);

    if ( ret != 0 )
      return -1;

    // Put the new unicode-to-font mapping in kernel
    ret = fsystem->ioctl (fd_tty, PIO_UNIMAP, unimap);

    if ( ret != 0 )
      advice.advised_hashlevel++;
  }
  while ( ret != 0 && errno == ENOMEM && advice.advised_hashlevel < 100);

  if ( ret == 0 )
    return 0;

  return -1;
}

//----------------------------------------------------------------------
void FTermLinux::setLinuxCursorStyle (CursorStyle style) const
{
  FTerm::paddingPrintf (CSI "?%dc", style);
}

#if defined(ISA_SYSCTL_SUPPORT)
//----------------------------------------------------------------------
inline auto FTermLinux::getInputStatusRegisterOne() const -> uInt16
{
  // Gets the VGA input-status-register-1

  static const auto& fsystem = FSystem::getInstance();

  // Miscellaneous output (read port)
  static constexpr uInt16 misc_read = 0x3cc;
  const uChar misc_value = fsystem->inPortByte(misc_read);
  const uInt16 io_base = ( misc_value & uChar(0x01) ) ? 0x3d0 : 0x3b0;
  // 0x3ba : Input status 1 mono/MDA (read port)
  // 0x3da : Input status 1 color/CGA (read port)
  return io_base + 0x0a;
}

//----------------------------------------------------------------------
auto FTermLinux::readAttributeController (uChar index) const -> uChar
{
  // Reads a byte from the attribute controller from a given index

  static const auto& fsystem = FSystem::getInstance();

  // Attribute controller (write port)
  static constexpr uInt16 attrib_cntlr_write = 0x3c0;
  // Attribute controller (read port)
  static constexpr uInt16 attrib_cntlr_read  = 0x3c1;
  const uInt16 input_status_1 = getInputStatusRegisterOne();

  fsystem->inPortByte (input_status_1);  // switch to index mode
  fsystem->outPortByte (index & uChar(0x1f), attrib_cntlr_write);  // selects address register
  const uChar res = fsystem->inPortByte (attrib_cntlr_read);  // read from data register

  // Disable access to the palette and unblank the display
  fsystem->inPortByte (input_status_1);  // switch to index mode
  index = (index & uChar(0x1f)) | 0x20;  // set bit 5 (enable display)
  fsystem->outPortByte (index, attrib_cntlr_write);
  fsystem->inPortByte (attrib_cntlr_read);

  return res;
}

//----------------------------------------------------------------------
void FTermLinux::writeAttributeController (uChar index, uChar data) const
{
  // Writes a byte from the attribute controller from a given index

  static const auto& fsystem = FSystem::getInstance();

  // Attribute controller (write port)
  static constexpr uInt16 attrib_cntlr_write = 0x3c0;
  const uInt16 input_status_1 = getInputStatusRegisterOne();

  fsystem->inPortByte (input_status_1);  // switch to index mode
  fsystem->outPortByte (index & uChar(0x1f), attrib_cntlr_write);  // selects address register
  fsystem->outPortByte (data, attrib_cntlr_write);  // write to data register

  // Disable access to the palette and unblank the display
  fsystem->inPortByte (input_status_1);  // switch to index mode
  index = (index & uChar(0x1f)) | 0x20;  // set bit 5 (enable display)
  fsystem->outPortByte (index, attrib_cntlr_write);
  fsystem->outPortByte (data, attrib_cntlr_write);
}

//----------------------------------------------------------------------
inline auto FTermLinux::getAttributeMode() const -> uChar
{
  // Gets the attribute mode value from the vga attribute controller
  static constexpr uChar attrib_mode = 0x10;
  return readAttributeController(attrib_mode);
}

//----------------------------------------------------------------------
inline void FTermLinux::setAttributeMode (uChar data) const
{
  // Sets the attribute mode value from the vga attribute controller
  static constexpr uChar attrib_mode = 0x10;
  writeAttributeController (attrib_mode, data);
}

//----------------------------------------------------------------------
auto FTermLinux::setBlinkAsIntensity (bool enable) const -> int
{
  // Uses blink-bit as background intensity.
  // That permits 16 colors for background

  static const auto& fsystem = FSystem::getInstance();

  const int fd_tty = FTerm::getTTYFileDescriptor();

  // Test if the blink-bit is used by the screen font (512 characters)
  if ( screen_font.charcount > 256 )
    return -1;

  if ( fsystem->getuid() != 0 )  // Direct hardware access requires root privileges
    return -2;

  if ( fd_tty < 0 )
    return -1;

  // Enable access to VGA I/O ports  (from 0x3B4 with num = 0x2C)
  if ( fsystem->ioctl(fd_tty, KDENABIO, 0) < 0 )
    return -1;  // error on KDENABIO

  if ( enable )
    setAttributeMode (getAttributeMode() & uChar(0xF7));  // clear bit 3
  else
    setAttributeMode (getAttributeMode() | uChar(0x08));  // set bit 3

  // Disable access to VGA I/O ports
  if ( fsystem->ioctl(fd_tty, KDDISABIO, 0) < 0 )
    return -1;  // error on KDDISABIO

  return 0;
}

//----------------------------------------------------------------------
auto FTermLinux::has9BitCharacters() const -> bool
{
  // Are 9-bit wide characters used?

  // 9-bit character:
  //  0xc0...0xdf - copying the eighth pixel into the ninth pixel
  //     The rest - the ninth pixel has the background color

  static const auto& fsystem = FSystem::getInstance();
  const int fd_tty = FTerm::getTTYFileDescriptor();

  if ( fsystem->getuid() != 0 )  // Direct hardware access requires root privileges
    return false;

  if ( fd_tty < 0 )
    return false;

  // Enable access to VGA I/O ports  (from 0x3B4 with num = 0x2C)
  if ( fsystem->ioctl(fd_tty, KDENABIO, 0) < 0 )
    return false;  // error on KDENABIO

  const bool nine_bit_char( getAttributeMode() & uChar(0x04) );

    // Disable access to VGA I/O ports
  if ( fsystem->ioctl(fd_tty, KDDISABIO, 0) < 0 )
    return false;  // error on KDDISABIO

  return nine_bit_char;
}

//----------------------------------------------------------------------
void FTermLinux::getVGAPalette()
{
  static const auto& fsystem = FSystem::getInstance();

  if ( fsystem->ioctl(0, GIO_CMAP, cmap.color.data()) != 0 )
    setVGADefaultPalette();  // Fallback, if GIO_CMAP does not work
}

//----------------------------------------------------------------------
void FTermLinux::setVGADefaultPalette()
{
  constexpr std::array<RGB, 16> defaultColor =
  {{
    {0x00, 0x00, 0x00}, {0xaa, 0x00, 0x00},
    {0x00, 0xaa, 0x00}, {0xaa, 0x55, 0x00},
    {0x00, 0x00, 0xaa}, {0xaa, 0x00, 0xaa},
    {0x00, 0xaa, 0xaa}, {0xaa, 0xaa, 0xaa},
    {0x55, 0x55, 0x55}, {0xff, 0x55, 0x55},
    {0x55, 0xff, 0x55}, {0xff, 0xff, 0x55},
    {0x55, 0x55, 0xff}, {0xff, 0x55, 0xff},
    {0x55, 0xff, 0xff}, {0xff, 0xff, 0xff}
  }};

  std::transform ( defaultColor.cbegin()
                 , defaultColor.cend()
                 , cmap.color.begin()
                 , [] (const auto& rgb)
                   {
                     return rgb;
                   }
                 );
}

//----------------------------------------------------------------------
auto FTermLinux::setVGAPalette (FColor index, int r, int g, int b) -> bool
{
  // Set the vga color map

  if ( r >= 0 && r < 256
    && g >= 0 && g < 256
    && b >= 0 && b < 256 )
  {
    cmap.color[uInt16(index)].red   = uChar(r);
    cmap.color[uInt16(index)].green = uChar(g);
    cmap.color[uInt16(index)].blue  = uChar(b);
  }

  static const auto& fsystem = FSystem::getInstance();
  return fsystem->ioctl(0, PIO_CMAP, cmap.color.data()) == 0;
}

//----------------------------------------------------------------------
auto FTermLinux::saveVGAPalette() -> bool
{
  // Save the current vga color map

  static const auto& fsystem = FSystem::getInstance();

  if ( fsystem->ioctl(0, GIO_CMAP, saved_color_map.color.data()) == 0 )
    has_saved_palette = true;
  else
    has_saved_palette = false;

  return has_saved_palette;
}

//----------------------------------------------------------------------
auto FTermLinux::resetVGAPalette() -> bool
{
  // Reset the vga color map

  static const auto& fsystem = FSystem::getInstance();

  if ( has_saved_palette )
  {
    if ( fsystem->ioctl (0, PIO_CMAP, saved_color_map.color.data()) )
      return false;
  }
  else
  {
    setVGADefaultPalette();

    if ( fsystem->ioctl(0, PIO_CMAP, cmap.color.data()) != 0 )
      return false;
  }

  return true;
}
#endif  // defined(ISA_SYSCTL_SUPPORT)

//----------------------------------------------------------------------
void FTermLinux::initKeyMap()
{
  keyCorrection();
  shiftKeyCorrection();
  ctrlKeyCorrection();
  altKeyCorrection();
  shiftCtrlKeyCorrection();
  shiftAltKeyCorrection();
  ctrlAltKeyCorrection();
  shiftCtrlAltKeyCorrection();
}

//----------------------------------------------------------------------
inline void FTermLinux::keyCorrection()
{
  ModifierKey m{};
  std::memset (&m, 0x00, sizeof(m));
  key_map[{m, FKey::Up}]        = FKey::Up;         // Up
  key_map[{m, FKey::Down}]      = FKey::Down;       // Down
  key_map[{m, FKey::Left}]      = FKey::Left;       // Left
  key_map[{m, FKey::Right}]     = FKey::Right;      // Right
  key_map[{m, FKey::Insert}]    = FKey::Insert;     // Ins
  key_map[{m, FKey::Del_char}]  = FKey::Del_char;   // Del
  key_map[{m, FKey::Home}]      = FKey::Home;       // Home
  key_map[{m, FKey::End}]       = FKey::End;        // End
  key_map[{m, FKey::Page_up}]   = FKey::Page_up;    // PgUp
  key_map[{m, FKey::Page_down}] = FKey::Page_down;  // PgDn
}

//----------------------------------------------------------------------
inline void FTermLinux::shiftKeyCorrection()
{
  ModifierKey m{};
  std::memset (&m, 0x00, sizeof(m));
  m.shift = 1;
  key_map[{m, FKey::Up}]        = FKey::Shift_up;         // Shift+Up
  key_map[{m, FKey::Down}]      = FKey::Shift_down;       // Shift+Down
  key_map[{m, FKey::Left}]      = FKey::Shift_left;       // Shift+Left
  key_map[{m, FKey::Right}]     = FKey::Shift_right;      // Shift+Right
  key_map[{m, FKey::Insert}]    = FKey::Shift_insert;     // Shift+Ins
  key_map[{m, FKey::Del_char}]  = FKey::Shift_del_char;   // Shift+Del
  key_map[{m, FKey::Home}]      = FKey::Shift_home;       // Shift+Home
  key_map[{m, FKey::End}]       = FKey::Shift_end;        // Shift+End
  key_map[{m, FKey::Page_up}]   = FKey::Shift_page_up;    // Shift+PgUp
  key_map[{m, FKey::Page_down}] = FKey::Shift_page_down;  // Shift+PgDn
}

//----------------------------------------------------------------------
inline void FTermLinux::ctrlKeyCorrection()
{
  ModifierKey m{};
  std::memset (&m, 0x00, sizeof(m));
  m.ctrl = 1;
  key_map[{m, FKey::Up}]        = FKey::Ctrl_up;         // Ctrl+Up
  key_map[{m, FKey::Down}]      = FKey::Ctrl_down;       // Ctrl+Down
  key_map[{m, FKey::Left}]      = FKey::Ctrl_left;       // Ctrl+Left
  key_map[{m, FKey::Right}]     = FKey::Ctrl_right;      // Ctrl+Right
  key_map[{m, FKey::Insert}]    = FKey::Ctrl_insert;     // Ctrl+Ins
  key_map[{m, FKey::Del_char}]  = FKey::Ctrl_del_char;   // Ctrl+Del
  key_map[{m, FKey::Home}]      = FKey::Ctrl_home;       // Ctrl+Home
  key_map[{m, FKey::End}]       = FKey::Ctrl_end;        // Ctrl+End
  key_map[{m, FKey::Page_up}]   = FKey::Ctrl_page_up;    // Ctrl+PgUp
  key_map[{m, FKey::Page_down}] = FKey::Ctrl_page_down;  // Ctrl+PgDn
}

//----------------------------------------------------------------------
inline void FTermLinux::altKeyCorrection()
{
  ModifierKey m{};
  std::memset (&m, 0x00, sizeof(m));
  m.alt = 1;
  key_map[{m, FKey::Up}]        = FKey::Meta_up;         // Meta+Up
  key_map[{m, FKey::Down}]      = FKey::Meta_down;       // Meta+Down
  key_map[{m, FKey::Left}]      = FKey::Meta_left;       // Meta+Left
  key_map[{m, FKey::Right}]     = FKey::Meta_right;      // Meta+Right
  key_map[{m, FKey::Insert}]    = FKey::Meta_insert;     // Meta+Ins
  key_map[{m, FKey::Del_char}]  = FKey::Meta_del_char;   // Meta+Del
  key_map[{m, FKey::Home}]      = FKey::Meta_home;       // Meta+Home
  key_map[{m, FKey::End}]       = FKey::Meta_end;        // Meta+End
  key_map[{m, FKey::Page_up}]   = FKey::Meta_page_up;    // Meta+PgUp
  key_map[{m, FKey::Page_down}] = FKey::Meta_page_down;  // Meta+PgDn
}

//----------------------------------------------------------------------
inline void FTermLinux::shiftCtrlKeyCorrection()
{
  ModifierKey m{};
  std::memset (&m, 0x00, sizeof(m));
  m.shift = 1;
  m.ctrl = 1;
  key_map[{m, FKey::Up}]        = FKey::Shift_Ctrl_up;         // Shift+Ctrl+Up
  key_map[{m, FKey::Down}]      = FKey::Shift_Ctrl_down;       // Shift+Ctrl+Down
  key_map[{m, FKey::Left}]      = FKey::Shift_Ctrl_left;       // Shift+Ctrl+Left
  key_map[{m, FKey::Right}]     = FKey::Shift_Ctrl_right;      // Shift+Ctrl+Right
  key_map[{m, FKey::Insert}]    = FKey::Shift_Ctrl_insert;     // Shift+Ctrl+Ins
  key_map[{m, FKey::Del_char}]  = FKey::Shift_Ctrl_del_char;   // Shift+Ctrl+Del
  key_map[{m, FKey::Home}]      = FKey::Shift_Ctrl_home;       // Shift+Ctrl+Home
  key_map[{m, FKey::End}]       = FKey::Shift_Ctrl_end;        // Shift+Ctrl+End
  key_map[{m, FKey::Page_up}]   = FKey::Shift_Ctrl_page_up;    // Shift+Ctrl+PgUp
  key_map[{m, FKey::Page_down}] = FKey::Shift_Ctrl_page_down;  // Shift+Ctrl+PgDn
}

//----------------------------------------------------------------------
inline void FTermLinux::shiftAltKeyCorrection()
{
  ModifierKey m{};
  std::memset (&m, 0x00, sizeof(m));
  m.shift = 1;
  m.alt = 1;
  key_map[{m, FKey::Up}]        = FKey::Shift_Meta_up;         // Shift+Meta+Up
  key_map[{m, FKey::Down}]      = FKey::Shift_Meta_down;       // Shift+Meta+Down
  key_map[{m, FKey::Left}]      = FKey::Shift_Meta_left;       // Shift+Meta+Left
  key_map[{m, FKey::Right}]     = FKey::Shift_Meta_right;      // Shift+Meta+Right
  key_map[{m, FKey::Insert}]    = FKey::Shift_Meta_insert;     // Shift+Meta+Ins
  key_map[{m, FKey::Del_char}]  = FKey::Shift_Meta_del_char;   // Shift+Meta+Del
  key_map[{m, FKey::Home}]      = FKey::Shift_Meta_home;       // Shift+Meta+Home
  key_map[{m, FKey::End}]       = FKey::Shift_Meta_end;        // Shift+Meta+End
  key_map[{m, FKey::Page_up}]   = FKey::Shift_Meta_page_up;    // Shift+Meta+PgUp
  key_map[{m, FKey::Page_down}] = FKey::Shift_Meta_page_down;  // Shift+Meta+PgDn
}

//----------------------------------------------------------------------
inline void FTermLinux::ctrlAltKeyCorrection()
{
  ModifierKey m{};
  std::memset (&m, 0x00, sizeof(m));
  m.ctrl = 1;
  m.alt = 1;
  key_map[{m, FKey::Up}]        = FKey::Ctrl_Meta_up;         // Ctrl+Meta+Up
  key_map[{m, FKey::Down}]      = FKey::Ctrl_Meta_down;       // Ctrl+Meta+Down
  key_map[{m, FKey::Left}]      = FKey::Ctrl_Meta_left;       // Ctrl+Meta+Left
  key_map[{m, FKey::Right}]     = FKey::Ctrl_Meta_right;      // Ctrl+Meta+Right
  key_map[{m, FKey::Insert}]    = FKey::Ctrl_Meta_insert;     // Ctrl+Meta+Ins
  key_map[{m, FKey::Del_char}]  = FKey::Ctrl_Meta_del_char;   // Ctrl+Meta+Del
  key_map[{m, FKey::Home}]      = FKey::Ctrl_Meta_home;       // Ctrl+Meta+Home
  key_map[{m, FKey::End}]       = FKey::Ctrl_Meta_end;        // Ctrl+Meta+End
  key_map[{m, FKey::Page_up}]   = FKey::Ctrl_Meta_page_up;    // Ctrl+Meta+PgUp
  key_map[{m, FKey::Page_down}] = FKey::Ctrl_Meta_page_down;  // Ctrl+Meta+PgDn
}

//----------------------------------------------------------------------
inline void FTermLinux::shiftCtrlAltKeyCorrection()
{
  ModifierKey m{};
  std::memset (&m, 0x00, sizeof(m));
  m.shift = 1;
  m.ctrl = 1;
  m.alt = 1;
  key_map[{m, FKey::Up}]        = FKey::Shift_Ctrl_Meta_up;         // Shift+Ctrl+Meta+Up
  key_map[{m, FKey::Down}]      = FKey::Shift_Ctrl_Meta_down;       // Shift+Ctrl+Meta+Down
  key_map[{m, FKey::Left}]      = FKey::Shift_Ctrl_Meta_left;       // Shift+Ctrl+Meta+Left
  key_map[{m, FKey::Right}]     = FKey::Shift_Ctrl_Meta_right;      // Shift+Ctrl+Meta+Right
  key_map[{m, FKey::Insert}]    = FKey::Shift_Ctrl_Meta_insert;     // Shift+Ctrl+Meta+Ins
  key_map[{m, FKey::Del_char}]  = FKey::Shift_Ctrl_Meta_del_char;   // Shift+Ctrl+Meta+Del
  key_map[{m, FKey::Home}]      = FKey::Shift_Ctrl_Meta_home;       // Shift+Ctrl+Meta+Home
  key_map[{m, FKey::End}]       = FKey::Shift_Ctrl_Meta_end;        // Shift+Ctrl+Meta+End
  key_map[{m, FKey::Page_up}]   = FKey::Shift_Ctrl_Meta_page_up;    // Shift+Ctrl+Meta+PgUp
  key_map[{m, FKey::Page_down}] = FKey::Shift_Ctrl_Meta_page_down;  // Shift+Ctrl+Meta+PgDn
}

//----------------------------------------------------------------------
inline void FTermLinux::initSpecialCharacter() const
{
  static auto& fterm_data = FTermData::getInstance();
  const auto c1 = wchar_t(UniChar::UpperHalfBlock);  // ▀
  const auto c2 = wchar_t(UniChar::LowerHalfBlock);  // ▄
  const auto c3 = wchar_t(UniChar::FullBlock);  // █

  if ( FTerm::charEncode(c1, Encoding::PC) == FTerm::charEncode(c1, Encoding::ASCII)
    || FTerm::charEncode(c2, Encoding::PC) == FTerm::charEncode(c2, Encoding::ASCII)
    || FTerm::charEncode(c3, Encoding::PC) == FTerm::charEncode(c3, Encoding::ASCII) )
  {
    fterm_data.supportShadowCharacter (false);  // disable support
  }

  const auto c4 = wchar_t(UniChar::RightHalfBlock);
  const auto c5 = wchar_t(UniChar::LeftHalfBlock);

  if ( FTerm::charEncode(c4, Encoding::PC) == FTerm::charEncode(c4, Encoding::ASCII)
    || FTerm::charEncode(c5, Encoding::PC) == FTerm::charEncode(c5, Encoding::ASCII) )
  {
    fterm_data.supportHalfBlockCharacter (false);  // disable support
  }
}

//----------------------------------------------------------------------
auto FTermLinux::getFontPos (wchar_t ucs) const -> sInt16
{
  constexpr sInt16 NOT_FOUND = -1;
  auto& count = screen_unicode_map.entry_ct;
  const auto& begin = &screen_unicode_map.entries[0];
  const auto& end = &screen_unicode_map.entries[count];
  const auto& iter = std::find_if ( begin, end,
                                    [&ucs] (const auto& entry)
                                    {
                                      return ucs == wchar_t(entry.unicode);
                                    } );
  return ( iter != end )
       ? static_cast<sInt16>(iter->fontpos)
       : NOT_FOUND;
}

//----------------------------------------------------------------------
inline void FTermLinux::deleteFontData (console_font_op& font)
{
  font_data.clear();
  font.data = nullptr;
}

//----------------------------------------------------------------------
inline void FTermLinux::deleteUnicodeMapEntries (unimapdesc& unicode_map)
{
  unicode_entries.clear();
  unicode_map.entries = nullptr;
}

//----------------------------------------------------------------------
void FTermLinux::characterFallback ( wchar_t ucs
                                   , const std::vector<wchar_t>& fallback ) const
{
  if ( fallback.size() < 2 || ucs != fallback[0] )
    return;

  constexpr sInt16 NOT_FOUND = -1;
  static auto& fterm_data = FTermData::getInstance();
  auto& sub_map = fterm_data.getCharSubstitutionMap();

  for (auto iter = fallback.cbegin() + 1; iter != fallback.cend(); iter++)
  {
    sInt16 pos = getFontPos(*iter);

    if ( pos != NOT_FOUND )
    {
      sub_map.setCharMapping({ucs, *iter});
      return;
    }
  }
}

}  // namespace finalcut

#endif  // defined(__linux__)
