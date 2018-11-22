/***********************************************************************
* ftermlinux.cpp - Contains the Linux terminal functions               *
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

#include "final/fterm.h"
#include "final/ftermlinux.h"

#if defined(__linux__)
  #include "../fonts/newfont.h"
  #include "../fonts/unicodemap.h"
  #include "../fonts/vgafont.h"
#endif

namespace finalcut
{

// static class attributes
#if defined(__linux__)
  FTermLinux::modifier_key  FTermLinux::mod_key;
  console_font_op           FTermLinux::screen_font;
  unimapdesc                FTermLinux::screen_unicode_map;

  bool   FTermLinux::new_font;
  bool   FTermLinux::vga_font;
  bool   FTermLinux::shadow_character = true;
  bool   FTermLinux::half_block_character = true;
  bool   FTermLinux::has_saved_palette = false;

  FTermDetection*             FTermLinux::term_detection = 0;
  fc::linuxConsoleCursorStyle FTermLinux::linux_console_cursor_style;
  FTermLinux::ColorMap        FTermLinux::saved_color_map;
  FTermLinux::ColorMap        FTermLinux::cmap;
  int                         FTermLinux::framebuffer_bpp = -1;
#endif  // defined(__linux__)


//----------------------------------------------------------------------
// class FTermLinux
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FTermLinux::FTermLinux()
{ }

//----------------------------------------------------------------------
FTermLinux::~FTermLinux()  // destructor
{
#if defined(__linux__)
  if ( screen_font.data )
    delete[] screen_font.data;

  if ( screen_unicode_map.entries )
    delete[] screen_unicode_map.entries;
#endif  // defined(__linux__)
}

// public methods of FTermLinux
//----------------------------------------------------------------------
#if defined(__linux__)
fc::linuxConsoleCursorStyle FTermLinux::getCursorStyle()
{
  // Get the current set cursor style

  return linux_console_cursor_style;
}

//----------------------------------------------------------------------
char* FTermLinux::setCursorStyle ( fc::linuxConsoleCursorStyle style
                                 , bool hidden )
{
  // Set cursor style in linux console

  static char buf[16] = { };

  if ( ! FTerm::isLinuxTerm() )
    return buf;

  linux_console_cursor_style = style;

  if ( hidden )
    return buf;

  std::sprintf (buf, CSI "?%dc", style);
  return buf;
}

//----------------------------------------------------------------------
void FTermLinux::setUTF8 (bool on)
{
  if ( ! FTerm::isLinuxTerm() )
    return;

  if ( on )
    FTerm::putstring (ESC "%G");
  else
    FTerm::putstring (ESC "%@");

  std::fflush(stdout);
}

//----------------------------------------------------------------------
bool FTermLinux::setPalette (FColor index, int r, int g, int b)
{
  if ( ! FTerm::isLinuxTerm() )
    return false;

#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
  return setVGAPalette (index, r, g, b);
#else
  return false;
#endif
}

//----------------------------------------------------------------------
bool FTermLinux::isLinuxConsole()
{
  // Check if it's a Linux console

  char arg = 0;
  int fd_tty = FTerm::getTTYFileDescriptor();

  // get keyboard type an compare
  return ( isatty (fd_tty)
        && ioctl(fd_tty, KDGKBTYPE, &arg) == 0
        && ((arg == KB_101) || (arg == KB_84)) );
}

//----------------------------------------------------------------------
void FTermLinux::init()
{
  // initialize Linux console

  screen_unicode_map.entries = 0;
  screen_font.data = 0;

  if ( FTerm::openConsole() == 0 )
  {
    term_detection->setLinuxTerm (isLinuxConsole());

    if ( FTerm::isLinuxTerm() )
    {
      getUnicodeMap();
      getScreenFont();

#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
      // Enable 16 background colors
      if ( setBlinkAsIntensity(true) == 0 )
        FTermcap::max_color = 16;
      else
        FTermcap::max_color = 8;
#endif
      // Underline cursor
      setCursorStyle (fc::underscore_cursor, true);

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
    std::cerr << "can not open the console.\n";
    std::abort();
  }
}

//----------------------------------------------------------------------
void FTermLinux::initCharMap (uInt char_map[][fc::NUM_OF_ENCODINGS])
{
  uInt c1, c2, c3, c4, c5;

  if ( new_font || vga_font )
    return;

  if ( screen_unicode_map.entry_ct != 0 )
  {
    for (int i = 0; i <= fc::lastCharItem; i++ )
    {
      bool known_unicode = false;

      for (uInt n = 0; n < screen_unicode_map.entry_ct; n++)
      {
        if ( char_map[i][fc::UTF8] == screen_unicode_map.entries[n].unicode )
        {
          if ( screen_unicode_map.entries[n].fontpos < 256 )
            known_unicode = true;

          break;
        }
      }

      if ( ! known_unicode )
        char_map[i][fc::PC] = char_map[i][fc::ASCII];
    }
  }

  c1 = fc::UpperHalfBlock;
  c2 = fc::LowerHalfBlock;
  c3 = fc::FullBlock;

  if ( FTerm::charEncode(c1, fc::PC) == FTerm::charEncode(c1, fc::ASCII)
    || FTerm::charEncode(c2, fc::PC) == FTerm::charEncode(c2, fc::ASCII)
    || FTerm::charEncode(c3, fc::PC) == FTerm::charEncode(c3, fc::ASCII) )
  {
    shadow_character = false;
  }

  c4 = fc::RightHalfBlock;
  c5 = fc::LeftHalfBlock;

  if ( FTerm::charEncode(c4, fc::PC) == FTerm::charEncode(c4, fc::ASCII)
    || FTerm::charEncode(c5, fc::PC) == FTerm::charEncode(c5, fc::ASCII) )
  {
    half_block_character = false;
  }
}

//----------------------------------------------------------------------
void FTermLinux::finish()
{
  if ( FTerm::isLinuxTerm() )
  {
#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
    setBlinkAsIntensity (false);
#endif
    setCursorStyle (fc::default_cursor, false);
  }
}

//----------------------------------------------------------------------
bool FTermLinux::loadVGAFont()
{
  vga_font = true;

  if ( FTerm::openConsole() == 0 )
  {
    if ( isLinuxConsole() )
    {
      // Set the standard vga font 8x16
      int ret = setScreenFont(fc::__8x16std, 256, 8, 16);

      if ( ret != 0 )
        vga_font = false;

      // unicode character mapping
      struct unimapdesc unimap;
      unimap.entry_ct = uInt16 ( sizeof(fc::unicode_cp437_pairs)
                               / sizeof(unipair) );
      unimap.entries = &fc::unicode_cp437_pairs[0];
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
    shadow_character = half_block_character = true;

  return vga_font;
}

//----------------------------------------------------------------------
bool FTermLinux::loadNewFont()
{
  new_font = true;

  if ( FTerm::openConsole() == 0 )
  {
    if ( isLinuxConsole() )
    {
      // Set the graphical font 8x16
      int ret = setScreenFont(fc::__8x16graph, 256, 8, 16);

      if ( ret != 0 )
        new_font = false;

      // unicode character mapping
      struct unimapdesc unimap;
      unimap.entry_ct = uInt16 ( sizeof(fc::unicode_cp437_pairs)
                               / sizeof(unipair) );
      unimap.entries = &fc::unicode_cp437_pairs[0];
      setUnicodeMap(&unimap);
    }
    else
      new_font = false;

    FTerm::detectTermSize();
    FTerm::closeConsole();
  }
  else
    new_font = false;

  if ( vga_font )
    shadow_character = half_block_character = true;

  return new_font;
}

//----------------------------------------------------------------------
bool FTermLinux::loadOldFont (uInt char_map[][fc::NUM_OF_ENCODINGS])
{
  bool retval = false;

  if ( FTerm::openConsole() == 0 )
  {
    if ( isLinuxConsole() )
    {
      if ( screen_font.data )
      {
        int ret = setScreenFont ( screen_font.data
                                , screen_font.charcount
                                , screen_font.width
                                , screen_font.height
                                , true );
        delete[] screen_font.data;
        screen_font.data = 0;

        if ( ret == 0 )
          retval = true;
      }

      if ( screen_unicode_map.entries )
      {
        setUnicodeMap (&screen_unicode_map);
        initCharMap(char_map);
        delete[] screen_unicode_map.entries;
        screen_unicode_map.entries = 0;
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
bool FTermLinux::saveColorMap()
{
  if ( ! FTerm::isLinuxTerm() )
    return false;

#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
  return saveVGAPalette();
#else
  return false;
#endif
}

//----------------------------------------------------------------------
bool FTermLinux::resetColorMap()
{
  if ( ! FTerm::isLinuxTerm() )
    return false;

#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
  return resetVGAPalette();
#else
  return false;
#endif
}

//----------------------------------------------------------------------
void FTermLinux::setBeep (int Hz, int ms)
{
  if ( ! FTerm::isLinuxTerm() )
    return;

  // range for frequency: 21-32766
  if ( Hz < 21 || Hz > 32766 )
    return;

  // range for duration:  0-1999
  if ( ms < 0 || ms > 1999 )
    return;

  FTerm::putstringf ( CSI "10;%d]"
                      CSI "11;%d]"
                    , Hz, ms );
  std::fflush(stdout);
}

//----------------------------------------------------------------------
void FTermLinux::resetBeep()
{
  if ( ! FTerm::isLinuxTerm() )
    return;

  // default frequency: 750 Hz
  // default duration:  125 ms
  FTerm::putstring ( CSI "10;750]"
                     CSI "11;125]" );
  std::fflush(stdout);
}

//----------------------------------------------------------------------
char* FTermLinux::restoreCursorStyle()
{
  // Reset to the last used Linux console cursor style

  return setCursorStyle (getCursorStyle(), false);
}

//----------------------------------------------------------------------
FKey FTermLinux::modifierKeyCorrection (const FKey& key_id)
{
  // Get the current modifier key state
  modifier_key& m = getModifierKey();

  if ( ! (m.shift || m.ctrl || m.alt) )
  {
    return key_id;
  }
  else if ( m.shift && ! m.ctrl && ! m.alt )
  {
    return shiftKeyCorrection(key_id);
  }
  else if ( ! m.shift && m.ctrl && ! m.alt )
  {
    return ctrlKeyCorrection(key_id);
  }
  else if ( ! m.shift && ! m.ctrl && m.alt )
  {
    return altKeyCorrection(key_id);
  }
  else if ( m.shift && m.ctrl && ! m.alt )
  {
    return shiftCtrlKeyCorrection(key_id);
  }
  else if ( m.shift && ! m.ctrl && m.alt )
  {
    return shiftAltKeyCorrection(key_id);
  }
  else if ( ! m.shift && m.ctrl && m.alt )
  {
    return ctrlAltKeyCorrection(key_id);
  }
  else if ( m.shift &&  m.ctrl && m.alt )
  {
    return shiftCtrlAltKeyCorrection(key_id);
  }

  return key_id;
}


// private methods of FTermLinux
//----------------------------------------------------------------------
int FTermLinux::getFramebuffer_bpp()
{
  int fd = -1;
  struct fb_var_screeninfo fb_var;
  struct fb_fix_screeninfo fb_fix;

  const char* fb = C_STR("/dev/fb/0");

  if ( (fd = open(fb, O_RDWR)) < 0 )
  {
    if ( errno != ENOENT && errno != ENOTDIR )
      return -1;

    fb = C_STR("/dev/fb0");

    if ( (fd = open(fb, O_RDWR)) < 0 )
      return -1;
  }

  if ( ! ioctl(fd, FBIOGET_VSCREENINFO, &fb_var)
    && ! ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix) )
  {
    ::close(fd);
    return int(fb_var.bits_per_pixel);
  }
  else
  {
    ::close(fd);
  }

  return -1;
}

//----------------------------------------------------------------------
bool FTermLinux::getScreenFont()
{
  static const std::size_t data_size = 4 * 32 * 512;
  struct console_font_op font;
  int fd_tty = FTerm::getTTYFileDescriptor();

  int ret;

  if ( fd_tty < 0 )
    return false;

  // initialize unused padding bytes in struct
  std::memset (&font, 0, sizeof(console_font_op));

  font.op = KD_FONT_OP_GET;
  font.flags = 0;
  font.width = 32;
  font.height = 32;
  font.charcount = 512;

  // initialize with 0
  try
  {
    font.data = new uChar[data_size]();
  }
  catch (const std::bad_alloc& ex)
  {
    std::cerr << bad_alloc_str << ex.what() << std::endl;
    return false;
  }

  // font operation
  ret = ioctl (fd_tty, KDFONTOP, &font);

  if ( ret == 0 )
  {
    screen_font.width = font.width;
    screen_font.height = font.height;
    screen_font.charcount = font.charcount;
    screen_font.data = font.data;
    return true;
  }
  else
    return false;
}

//----------------------------------------------------------------------
bool FTermLinux::getUnicodeMap()
{
  int ret;
  int fd_tty = FTerm::getTTYFileDescriptor();

  if ( fd_tty < 0 )
    return false;

  screen_unicode_map.entry_ct = 0;
  screen_unicode_map.entries = 0;

  // get count
  ret = ioctl (fd_tty, GIO_UNIMAP, &screen_unicode_map);

  if ( ret != 0 )
  {
    std::size_t count = screen_unicode_map.entry_ct;

    if ( errno != ENOMEM || count == 0 )
      return false;

    try
    {
      screen_unicode_map.entries = new struct unipair[count]();
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << bad_alloc_str << ex.what() << std::endl;
      return false;
    }

    // get unicode-to-font mapping from kernel
    ret = ioctl(fd_tty, GIO_UNIMAP, &screen_unicode_map);

    if ( ret != 0 )
      return false;
  }

  return true;
}

//----------------------------------------------------------------------
FTermLinux::modifier_key& FTermLinux::getModifierKey()
{
  // Get Linux console shift state

  char subcode = 6;  // Shift state command + return value

  // fill bit field with 0
  std::memset (&mod_key, 0x00, sizeof(mod_key));

  // TIOCLINUX, subcode = 6
  if ( ioctl(0, TIOCLINUX, &subcode) >= 0 )
  {
    if ( subcode & (1 << KG_SHIFT) )
      mod_key.shift = true;

    if ( subcode & (1 << KG_ALTGR) )
      mod_key.alt_gr = true;

    if ( subcode & (1 << KG_CTRL) )
      mod_key.ctrl = true;

    if ( subcode & (1 << KG_ALT) )
      mod_key.alt = true;
  }

  return mod_key;
}

//----------------------------------------------------------------------
int FTermLinux::setScreenFont ( uChar fontdata[], uInt count
                              , uInt fontwidth, uInt fontheight
                              , bool direct)
{
  struct console_font_op font;
  int ret;
  int fd_tty = FTerm::getTTYFileDescriptor();

  if ( fd_tty < 0 )
    return -1;

  // initialize unused padding bytes in struct
  std::memset (&font, 0x00, sizeof(console_font_op));

  font.op = KD_FONT_OP_SET;
  font.flags = 0;
  font.width = fontwidth;
  font.height = fontheight;
  font.charcount = count;

  if ( direct )
    font.data = fontdata;
  else
  {
    const std::size_t bytes_per_line = font.width / 8;
    const std::size_t data_size = bytes_per_line * 32 * font.charcount;

    try
    {
      font.data = new uChar[data_size]();  // initialize with 0
    }
    catch (const std::bad_alloc& ex)
    {
      std::cerr << bad_alloc_str << ex.what() << std::endl;
      return -1;
    }

    for (std::size_t i = 0; i < count; i++)
      std::memcpy ( const_cast<uChar*>(font.data + bytes_per_line * 32 * i)
                  , &fontdata[i * font.height]
                  , font.height);
  }

  // font operation
  ret = ioctl (fd_tty, KDFONTOP, &font);

  if ( ret != 0 && errno != ENOSYS && errno != EINVAL )
  {
    if ( ! direct )
      delete[] font.data;

    return -1;
  }

  if ( ! direct )
    delete[] font.data;

  if ( ret == 0 )
    return 0;
  else
    return -1;
}

//----------------------------------------------------------------------
int FTermLinux::setUnicodeMap (struct unimapdesc* unimap)
{
  struct unimapinit advice;
  int ret;
  int fd_tty = FTerm::getTTYFileDescriptor();

  if ( fd_tty < 0 )
    return -1;

  advice.advised_hashsize = 0;
  advice.advised_hashstep = 0;
  advice.advised_hashlevel = 0;

  do
  {
    // clear the unicode-to-font table
    ret = ioctl(fd_tty, PIO_UNIMAPCLR, &advice);

    if ( ret != 0 )
      return -1;

    // put the new unicode-to-font mapping in kernel
    ret = ioctl(fd_tty, PIO_UNIMAP, unimap);

    if ( ret != 0 )
      advice.advised_hashlevel++;
  }
  while ( ret != 0 && errno == ENOMEM && advice.advised_hashlevel < 100);

  if ( ret == 0 )
    return 0;
  else
    return -1;
}

#if defined(__x86_64__) || defined(__i386) || defined(__arm__)
//----------------------------------------------------------------------
inline uInt16 FTermLinux::getInputStatusRegisterOne()
{
  // Gets the VGA input-status-register-1

  // Miscellaneous output (read port)
  static const uInt16 misc_read = 0x3cc;
  const uInt16 io_base = ( inb(misc_read) & 0x01 ) ? 0x3d0 : 0x3b0;
  // 0x3ba : Input status 1 MDA (read port)
  // 0x3da : Input status 1 CGA (read port)
  return io_base + 0x0a;
}

//----------------------------------------------------------------------
uChar FTermLinux::readAttributeController (uChar index)
{
  // Reads a byte from the attribute controller from a given index

  uChar res;
  // Attribute controller (write port)
  static const uInt16 attrib_cntlr_write = 0x3c0;
  // Attribute controller (read port)
  static const uInt16 attrib_cntlr_read  = 0x3c1;
  const uInt16 input_status_1 = getInputStatusRegisterOne();

  inb (input_status_1);  // switch to index mode
  outb (index & 0x1f, attrib_cntlr_write);
  res = inb (attrib_cntlr_read);

  inb (input_status_1);  // switch to data mode
  index = (index & 0x1f) | 0x20;  // set bit 5 (enable display)
  outb (index, attrib_cntlr_write);
  inb (attrib_cntlr_read);
  return res;
}

//----------------------------------------------------------------------
void FTermLinux::writeAttributeController (uChar index, uChar data)
{
  // Writes a byte from the attribute controller from a given index

  // Attribute controller (write port)
  static const uInt16 attrib_cntlr_write = 0x3c0;
  const uInt16 input_status_1 = getInputStatusRegisterOne();

  inb (input_status_1);  // switch to index mode
  outb (index & 0x1f, attrib_cntlr_write);
  outb (data, attrib_cntlr_write);

  inb (input_status_1);  // switch to data mode
  index = (index & 0x1f) | 0x20;  // set bit 5 (enable display)
  outb (index, attrib_cntlr_write);
  outb (data, attrib_cntlr_write);
}

//----------------------------------------------------------------------
inline uChar FTermLinux::getAttributeMode()
{
  // Gets the attribute mode value from the vga attribute controller
  static const uChar attrib_mode = 0x10;
  return readAttributeController(attrib_mode);
}

//----------------------------------------------------------------------
inline void FTermLinux::setAttributeMode (uChar data)
{
  // Sets the attribute mode value from the vga attribute controller
  static const uChar attrib_mode = 0x10;
  writeAttributeController (attrib_mode, data);
}

//----------------------------------------------------------------------
int FTermLinux::setBlinkAsIntensity (bool on)
{
  // Uses blink-bit as background intensity.
  // That permits 16 colors for background

  int fd_tty = FTerm::getTTYFileDescriptor();

  // Test if the blink-bit is used by the screen font (512 characters)
  if ( screen_font.charcount > 256 )
    return -1;

  if ( getuid() != 0 )  // Direct hardware access requires root privileges
    return -2;

  if ( fd_tty < 0 )
    return -1;

  // Enable access to VGA I/O ports  (from 0x3B4 with num = 0x2C)
  if ( ioctl(fd_tty, KDENABIO, 0) < 0 )
    return -1;  // error on KDENABIO

  if ( on )
    setAttributeMode (getAttributeMode() & 0xF7);  // clear bit 3
  else
    setAttributeMode (getAttributeMode() | 0x08);  // set bit 3

  // Disable access to VGA I/O ports
  if ( ioctl(fd_tty, KDDISABIO, 0) < 0 )
    return -1;  // error on KDDISABIO

  return 0;
}

//----------------------------------------------------------------------
bool FTermLinux::setVGAPalette (FColor index, int r, int g, int b)
{
  // Set the vga color map

  if ( r >= 0 && r < 256
    && g >= 0 && g < 256
    && b >= 0 && b < 256 )
  {
    cmap.color[index].red   = uChar(r);
    cmap.color[index].green = uChar(g);
    cmap.color[index].blue  = uChar(b);
  }

  if ( ioctl (0, PIO_CMAP, &cmap) )
    return false;
  else
    return true;
}

//----------------------------------------------------------------------
bool FTermLinux::saveVGAPalette()
{
  // Save the current vga color map

  if ( ioctl (0, GIO_CMAP, &saved_color_map) )
    has_saved_palette = false;
  else
    has_saved_palette = true;

  return has_saved_palette;
}

//----------------------------------------------------------------------
bool FTermLinux::resetVGAPalette()
{
  // Reset the vga color map

  if ( has_saved_palette )
  {
    if ( ioctl (0, PIO_CMAP, &saved_color_map) )
      return false;
  }
  else
  {
    rgb defaultColor[16] =
    {
      {0x00, 0x00, 0x00}, {0xAA, 0x00, 0x00},
      {0x00, 0xAA, 0x00}, {0xAA, 0x55, 0x00},
      {0x00, 0x00, 0xAA}, {0xAA, 0x00, 0xAA},
      {0x00, 0xAA, 0xAA}, {0xAA, 0xAA, 0xAA},
      {0x55, 0x55, 0x55}, {0xFF, 0x55, 0x55},
      {0x55, 0xFF, 0x55}, {0xFF, 0xFF, 0x55},
      {0x55, 0x55, 0xFF}, {0xFF, 0x55, 0xFF},
      {0x55, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}
    };

    for (int index = 0; index < 16; index++)
    {
      cmap.color[index].red   = defaultColor[index].red;
      cmap.color[index].green = defaultColor[index].green;
      cmap.color[index].blue  = defaultColor[index].blue;
    }

    if ( ioctl (0, PIO_CMAP, &cmap) )
      return false;
  }

  return true;
}
#endif  // defined(__x86_64__) || defined(__i386) || defined(__arm__)

//----------------------------------------------------------------------
FKey FTermLinux::shiftKeyCorrection (const FKey& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fkey_sr;         // Shift+Up

    case fc::Fkey_down:
      return fc::Fkey_sf;         // Shift+Down

    case fc::Fkey_left:
      return fc::Fkey_sleft;      // Shift+Left

    case fc::Fkey_right:
      return fc::Fkey_sright;     // Shift+Right

    case fc::Fkey_ic:
      return fc::Fkey_sic;        // Shift+Ins

    case fc::Fkey_dc:
      return fc::Fkey_sdc;        // Shift+Del

    case fc::Fkey_home:
      return fc::Fkey_shome;      // Shift+Home

    case fc::Fkey_end:
      return fc::Fkey_send;       // Shift+End

    case fc::Fkey_ppage:
      return fc::Fkey_sprevious;  // Shift+PgUp

    case fc::Fkey_npage:
      return fc::Fkey_snext;      // Shift+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
FKey FTermLinux::ctrlKeyCorrection (const FKey& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fckey_up;     // Ctrl+Up

    case fc::Fkey_down:
      return fc::Fckey_down;   // Ctrl+Down

    case fc::Fkey_left:
      return fc::Fckey_left;   // Ctrl+Left

    case fc::Fkey_right:
      return fc::Fckey_right;  // Ctrl+Right

    case fc::Fkey_ic:
      return fc::Fckey_ic;     // Ctrl+Ins

    case fc::Fkey_dc:
      return fc::Fckey_dc;     // Ctrl+Del

    case fc::Fkey_home:
      return fc::Fckey_home;   // Ctrl+Home

    case fc::Fkey_end:
      return fc::Fckey_end;    // Ctrl+End

    case fc::Fkey_ppage:
      return fc::Fckey_ppage;  // Ctrl+PgUp

    case fc::Fkey_npage:
      return fc::Fckey_npage;  // Ctrl+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
FKey FTermLinux::altKeyCorrection (const FKey& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fmkey_up;     // Meta+Up

    case fc::Fkey_down:
      return fc::Fmkey_down;   // Meta+Down

    case fc::Fkey_left:
      return fc::Fmkey_left;   // Meta+Left

    case fc::Fkey_right:
      return fc::Fmkey_right;  // Meta+Right

    case fc::Fkey_ic:
      return fc::Fmkey_ic;     // Meta+Ins

    case fc::Fkey_dc:
      return fc::Fmkey_dc;     // Meta+Del

    case fc::Fkey_home:
      return fc::Fmkey_home;   // Meta+Home

    case fc::Fkey_end:
      return fc::Fmkey_end;    // Meta+End

    case fc::Fkey_ppage:
      return fc::Fmkey_ppage;  // Meta+PgUp

    case fc::Fkey_npage:
      return fc::Fmkey_npage;  // Meta+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
FKey FTermLinux::shiftCtrlKeyCorrection (const FKey& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fckey_sup;     // Shift+Ctrl+Up

    case fc::Fkey_down:
      return fc::Fckey_sdown;   // Shift+Ctrl+Down

    case fc::Fkey_left:
      return fc::Fckey_sleft;   // Shift+Ctrl+Left

    case fc::Fkey_right:
      return fc::Fckey_sright;  // Shift+Ctrl+Right

    case fc::Fkey_ic:
      return fc::Fckey_sic;     // Shift+Ctrl+Ins

    case fc::Fkey_dc:
      return fc::Fckey_sdc;     // Shift+Ctrl+Del

    case fc::Fkey_home:
      return fc::Fckey_shome;   // Shift+Ctrl+Home

    case fc::Fkey_end:
      return fc::Fckey_send;    // Shift+Ctrl+End

    case fc::Fkey_ppage:
      return fc::Fckey_sppage;  // Shift+Ctrl+PgUp

    case fc::Fkey_npage:
      return fc::Fckey_snpage;  // Shift+Ctrl+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
FKey FTermLinux::shiftAltKeyCorrection (const FKey& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fmkey_sup;     // Shift+Meta+Up

    case fc::Fkey_down:
      return fc::Fmkey_sdown;   // Shift+Meta+Down

    case fc::Fkey_left:
      return fc::Fmkey_sright;  // Shift+Meta+Left

    case fc::Fkey_right:
      return fc::Fmkey_sleft;   // Shift+Meta+Right

    case fc::Fkey_ic:
      return fc::Fmkey_sic;     // Shift+Meta+Ins

    case fc::Fkey_dc:
      return fc::Fmkey_sdc;     // Shift+Meta+Del

    case fc::Fkey_home:
      return fc::Fmkey_shome;   // Shift+Meta+Home

    case fc::Fkey_end:
      return fc::Fmkey_send;    // Shift+Meta+End

    case fc::Fkey_ppage:
      return fc::Fmkey_sppage;  // Shift+Meta+PgUp

    case fc::Fkey_npage:
      return fc::Fmkey_snpage;  // Shift+Meta+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
FKey FTermLinux::ctrlAltKeyCorrection (const FKey& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fcmkey_up;     // Ctrl+Meta+Up

    case fc::Fkey_down:
      return fc::Fcmkey_down;   // Ctrl+Meta+Down

    case fc::Fkey_left:
      return fc::Fcmkey_left;   // Ctrl+Meta+Left

    case fc::Fkey_right:
      return fc::Fcmkey_right;  // Ctrl+Meta+Right

    case fc::Fkey_ic:
      return fc::Fcmkey_ic;     // Ctrl+Meta+Ins

    case fc::Fkey_dc:
      return fc::Fcmkey_dc;     // Ctrl+Meta+Del

    case fc::Fkey_home:
      return fc::Fcmkey_home;   // Ctrl+Meta+Home

    case fc::Fkey_end:
      return fc::Fcmkey_end;    // Ctrl+Meta+End

    case fc::Fkey_ppage:
      return fc::Fcmkey_ppage;  // Ctrl+Meta+PgUp

    case fc::Fkey_npage:
      return fc::Fcmkey_npage;  // Ctrl+Meta+PgDn

    default:
      return key_id;
  }
}

//----------------------------------------------------------------------
FKey FTermLinux::shiftCtrlAltKeyCorrection (const FKey& key_id)
{
  switch ( key_id )
  {
    case fc::Fkey_up:
      return fc::Fcmkey_sup;     // Shift+Ctrl+Meta+Up

    case fc::Fkey_down:
      return fc::Fcmkey_sdown;   // Shift+Ctrl+Meta+Down

    case fc::Fkey_left:
      return fc::Fcmkey_sleft;   // Shift+Ctrl+Meta+Left

    case fc::Fkey_right:
      return fc::Fcmkey_sright;  // Shift+Ctrl+Meta+Right

    case fc::Fkey_ic:
      return fc::Fcmkey_sic;     // Shift+Ctrl+Meta+Ins

    case fc::Fkey_dc:
      return fc::Fcmkey_sdc;     // Shift+Ctrl+Meta+Del

    case fc::Fkey_home:
      return fc::Fcmkey_shome;   // Shift+Ctrl+Meta+Home

    case fc::Fkey_end:
      return fc::Fcmkey_send;    // Shift+Ctrl+Meta+End

    case fc::Fkey_ppage:
      return fc::Fcmkey_sppage;  // Shift+Ctrl+Meta+PgUp

    case fc::Fkey_npage:
      return fc::Fcmkey_snpage;  // Shift+Ctrl+Meta+PgDn

    default:
      return key_id;
  }
}
#endif  // defined(__linux__)

}  // namespace finalcut
