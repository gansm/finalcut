/***********************************************************************
* conemu.h - Emulator for various consoles and terminals               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2025 Markus Gans                                      *
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

/*  Standalone test class
 *  ═════════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▏
 * ▕ ConEmu ▏
 * ▕▁▁▁▁▁▁▁▁▏
 */

#ifndef CONEMU_H
#define CONEMU_H

#include <sys/wait.h>
#include <sys/mman.h>

#include <chrono>
#include <thread>

#include <final/final.h>

using finalcut::C_STR;

namespace test
{

//----------------------------------------------------------------------
// class ConEmu
//----------------------------------------------------------------------

class ConEmu
{
  public:
    // Enumeration
    enum class console : std::size_t
    {
      ansi,
      xterm,
      rxvt,
      urxvt,
      kde_konsole,
      gnome_terminal,
      newer_vte_terminal,
      putty,
      win_terminal,
      tera_term,
      cygwin,
      mintty,
      stterm,
      linux_con,
      freebsd_con,
      netbsd_con,
      openbsd_con,
      sun_con,
      screen,
      tmux,
      kterm,
      mlterm,
      kitty,
      NUM_OF_CONSOLES  // Total number of console types
    };

    // Constructors
    ConEmu()
    {
      // Initialize buffer with zeros
      buffer.fill('\0');

      // create timer instance
      finalcut::FObjectTimer timer{};

      try
      {
        // Map shared memory
        shared_memory_ptr = mmap ( nullptr
                                 , SHARED_MEMORY_SIZE
                                 , PROT_READ | PROT_WRITE
                                 , MAP_SHARED | MAP_ANONYMOUS
                                 , INVALID_FD
                                 , 0 );

        if ( shared_memory_ptr == MAP_FAILED )
        {
          const int error_code = errno;
          throw std::system_error ( error_code, std::generic_category()
                                  , std::string("Failed to map shared memory: ") +
                                    std::strerror(error_code) );
        }

        shared_state = static_cast<bool*>(shared_memory_ptr);
        *shared_state = false;
      }
      catch (...)
      {
        cleanupResources();
        throw;
      }
    }

    // Disable copy constructor
    ConEmu (const ConEmu&) = delete;

    // Disable move constructor
    ConEmu (ConEmu&&) noexcept = delete;

    // Destructor
    ~ConEmu() noexcept
    {
      cleanupResources();
    }

    // Disable assignment operator (=)
    auto operator = (const ConEmu&) -> ConEmu& = delete;

    // Disable move assignment operator (=)
    auto operator = (ConEmu&&) noexcept -> ConEmu& = delete;

  protected:
    // Mutators
    void  enableConEmuDebug (bool) noexcept;

    // Inquiries
    auto  isConEmuChildProcess (pid_t) const noexcept -> bool;

    // Methods
    void  printConEmuDebug() noexcept;
    void  closeConEmuStdStreams();
    auto  forkConEmu() -> pid_t;
    void  startConEmuTerminal (console);

  private:
    // Constants
    static constexpr int INVALID_FD{-1};
    static constexpr int INVALID_PID{-1};
    static constexpr std::size_t COLOR_COUNT{256};
    static constexpr std::size_t BUFFER_SIZE{2048};
    static constexpr uInt64 TERMINAL_TIMEOUT{10'000'000};  // 10 sec
    static constexpr std::size_t SHARED_MEMORY_SIZE{sizeof(bool)};
    static constexpr auto NUM_OF_CONSOLES = std::size_t(console::NUM_OF_CONSOLES);
    static constexpr unsigned short DEFAULT_COLS{80};  // Terminal window character width
    static constexpr unsigned short DEFAULT_ROWS{25};  // Terminal window character height

    // Using-declaration
    using BufferType = std::array<char, BUFFER_SIZE>;
    using ColorTableType = const std::array<const char*, COLOR_COUNT>;
    using ConsoleStringTableType = const std::array<const char*, NUM_OF_CONSOLES>;

    // Accessors
    auto  getAnswerback (console) const noexcept -> const char*;
    auto  getDSR (console) const noexcept -> const char*;
    auto  getDECID (console) const noexcept -> const char*;
    auto  getDA (console) const noexcept -> const char*;
    auto  getDA1 (console) const noexcept -> const char*;
    auto  getSEC_DA (console) const noexcept -> const char*;

    // Methods
    auto  openMasterPTY() -> bool;
    auto  openSlavePTY() -> bool;
    void  closeMasterPTY() noexcept;
    void  closeSlavePTY() noexcept;
    void  cleanupResources() noexcept;
    void  validateConsoleType (console) const;
    auto  setupChildProcess() -> bool;
    auto  waitForChildReady() -> bool;
    auto  isValidFileDescriptor (int) const noexcept -> bool;
    void  writeToMaster (const char*, std::size_t) noexcept;
    void  writeToStdout (const char*, std::size_t) noexcept;
    void  parseTerminalBuffer (std::size_t, console) noexcept;

    // Data members
    int                    fd_stdin{fileno(stdin)};
    int                    fd_stdout{fileno(stdout)};
    int                    fd_stderr{fileno(stderr)};
    int                    fd_master{INVALID_FD};
    int                    fd_slave{INVALID_FD};
    bool                   debug{false};
    BufferType             buffer{};
    void*                  shared_memory_ptr{};  // Shared memory management
    static bool*           shared_state;
    static ColorTableType  colorname;
};

// static class attributes
bool* ConEmu::shared_state = nullptr;


// private data member of ConEmu
//----------------------------------------------------------------------
const std::array<const char*, ConEmu::COLOR_COUNT> ConEmu::colorname
{{
  C_STR("0000/0000/0000"),  // 0
  C_STR("bbbb/0000/0000"),  // 1
  C_STR("0000/bbbb/0000"),  // 2
  C_STR("bbbb/bbbb/0000"),  // 3
  C_STR("0000/0000/bbbb"),  // 4
  C_STR("bbbb/0000/bbbb"),  // 5
  C_STR("0000/bbbb/bbbb"),  // 6
  C_STR("bbbb/bbbb/bbbb"),  // 7
  C_STR("5555/5555/5555"),  // 8
  C_STR("ffff/5555/5555"),  // 9
  C_STR("5555/ffff/5555"),  // 10
  C_STR("ffff/ffff/5555"),  // 11
  C_STR("5555/5555/ffff"),  // 12
  C_STR("ffff/5555/ffff"),  // 13
  C_STR("5555/ffff/ffff"),  // 14
  C_STR("ffff/ffff/ffff"),  // 15
  C_STR("0000/0000/0000"),  // 16
  C_STR("0000/0000/5f5f"),  // 17
  C_STR("0000/0000/8787"),  // 18
  C_STR("0000/0000/afaf"),  // 19
  C_STR("0000/0000/d7d7"),  // 20
  C_STR("0000/0000/ffff"),  // 21
  C_STR("0000/5f5f/0000"),  // 22
  C_STR("0000/5f5f/5f5f"),  // 23
  C_STR("0000/5f5f/8787"),  // 24
  C_STR("0000/5f5f/afaf"),  // 25
  C_STR("0000/5f5f/d7d7"),  // 26
  C_STR("0000/5f5f/ffff"),  // 27
  C_STR("0000/8787/0000"),  // 28
  C_STR("0000/8787/5f5f"),  // 29
  C_STR("0000/8787/8787"),  // 30
  C_STR("0000/8787/afaf"),  // 31
  C_STR("0000/8787/d7d7"),  // 32
  C_STR("0000/8787/ffff"),  // 33
  C_STR("0000/afaf/0000"),  // 34
  C_STR("0000/afaf/5f5f"),  // 35
  C_STR("0000/afaf/8787"),  // 36
  C_STR("0000/afaf/afaf"),  // 37
  C_STR("0000/afaf/d7d7"),  // 38
  C_STR("0000/afaf/ffff"),  // 39
  C_STR("0000/d7d7/0000"),  // 40
  C_STR("0000/d7d7/5f5f"),  // 41
  C_STR("0000/d7d7/8787"),  // 42
  C_STR("0000/d7d7/afaf"),  // 43
  C_STR("0000/d7d7/d7d7"),  // 44
  C_STR("0000/d7d7/ffff"),  // 45
  C_STR("0000/ffff/0000"),  // 46
  C_STR("0000/ffff/5f5f"),  // 47
  C_STR("0000/ffff/8787"),  // 48
  C_STR("0000/ffff/afaf"),  // 49
  C_STR("0000/ffff/d7d7"),  // 50
  C_STR("0000/ffff/ffff"),  // 51
  C_STR("5f5f/0000/0000"),  // 52
  C_STR("5f5f/0000/5f5f"),  // 53
  C_STR("5f5f/0000/8787"),  // 54
  C_STR("5f5f/0000/afaf"),  // 55
  C_STR("5f5f/0000/d7d7"),  // 56
  C_STR("5f5f/0000/ffff"),  // 57
  C_STR("5f5f/5f5f/0000"),  // 58
  C_STR("5f5f/5f5f/5f5f"),  // 59
  C_STR("5f5f/5f5f/8787"),  // 60
  C_STR("5f5f/5f5f/afaf"),  // 61
  C_STR("5f5f/5f5f/d7d7"),  // 62
  C_STR("5f5f/5f5f/ffff"),  // 63
  C_STR("5f5f/8787/0000"),  // 64
  C_STR("5f5f/8787/5f5f"),  // 65
  C_STR("5f5f/8787/8787"),  // 66
  C_STR("5f5f/8787/afaf"),  // 67
  C_STR("5f5f/8787/d7d7"),  // 68
  C_STR("5f5f/8787/ffff"),  // 69
  C_STR("5f5f/afaf/0000"),  // 70
  C_STR("5f5f/afaf/5f5f"),  // 71
  C_STR("5f5f/afaf/8787"),  // 72
  C_STR("5f5f/afaf/afaf"),  // 73
  C_STR("5f5f/afaf/d7d7"),  // 74
  C_STR("5f5f/afaf/ffff"),  // 75
  C_STR("5f5f/d7d7/0000"),  // 76
  C_STR("5f5f/d7d7/5f5f"),  // 77
  C_STR("5f5f/d7d7/8787"),  // 78
  C_STR("5f5f/d7d7/afaf"),  // 79
  C_STR("5f5f/d7d7/d7d7"),  // 80
  C_STR("5f5f/d7d7/ffff"),  // 81
  C_STR("5f5f/ffff/0000"),  // 82
  C_STR("5f5f/ffff/5f5f"),  // 83
  C_STR("5f5f/ffff/8787"),  // 84
  C_STR("5f5f/ffff/afaf"),  // 85
  C_STR("5f5f/ffff/d7d7"),  // 86
  C_STR("5f5f/ffff/ffff"),  // 87
  C_STR("8787/0000/0000"),  // 88
  C_STR("8787/0000/5f5f"),  // 89
  C_STR("8787/0000/8787"),  // 90
  C_STR("8787/0000/afaf"),  // 91
  C_STR("8787/0000/d7d7"),  // 92
  C_STR("8787/0000/ffff"),  // 93
  C_STR("8787/5f5f/0000"),  // 94
  C_STR("8787/5f5f/5f5f"),  // 95
  C_STR("8787/5f5f/8787"),  // 96
  C_STR("8787/5f5f/afaf"),  // 97
  C_STR("8787/5f5f/d7d7"),  // 98
  C_STR("8787/5f5f/ffff"),  // 99
  C_STR("8787/8787/0000"),  // 100
  C_STR("8787/8787/5f5f"),  // 101
  C_STR("8787/8787/8787"),  // 102
  C_STR("8787/8787/afaf"),  // 103
  C_STR("8787/8787/d7d7"),  // 104
  C_STR("8787/8787/ffff"),  // 105
  C_STR("8787/afaf/0000"),  // 106
  C_STR("8787/afaf/5f5f"),  // 107
  C_STR("8787/afaf/8787"),  // 108
  C_STR("8787/afaf/afaf"),  // 109
  C_STR("8787/afaf/d7d7"),  // 110
  C_STR("8787/afaf/ffff"),  // 111
  C_STR("8787/d7d7/0000"),  // 112
  C_STR("8787/d7d7/5f5f"),  // 113
  C_STR("8787/d7d7/8787"),  // 114
  C_STR("8787/d7d7/afaf"),  // 115
  C_STR("8787/d7d7/d7d7"),  // 116
  C_STR("8787/d7d7/ffff"),  // 117
  C_STR("8787/ffff/0000"),  // 118
  C_STR("8787/ffff/5f5f"),  // 119
  C_STR("8787/ffff/8787"),  // 120
  C_STR("8787/ffff/afaf"),  // 121
  C_STR("8787/ffff/d7d7"),  // 122
  C_STR("8787/ffff/ffff"),  // 123
  C_STR("afaf/0000/0000"),  // 124
  C_STR("afaf/0000/5f5f"),  // 125
  C_STR("afaf/0000/8787"),  // 126
  C_STR("afaf/0000/afaf"),  // 127
  C_STR("afaf/0000/d7d7"),  // 128
  C_STR("afaf/0000/ffff"),  // 129
  C_STR("afaf/5f5f/0000"),  // 130
  C_STR("afaf/5f5f/5f5f"),  // 131
  C_STR("afaf/5f5f/8787"),  // 132
  C_STR("afaf/5f5f/afaf"),  // 133
  C_STR("afaf/5f5f/d7d7"),  // 134
  C_STR("afaf/5f5f/ffff"),  // 135
  C_STR("afaf/8787/0000"),  // 136
  C_STR("afaf/8787/5f5f"),  // 137
  C_STR("afaf/8787/8787"),  // 138
  C_STR("afaf/8787/afaf"),  // 139
  C_STR("afaf/8787/d7d7"),  // 140
  C_STR("afaf/8787/ffff"),  // 141
  C_STR("afaf/afaf/0000"),  // 142
  C_STR("afaf/afaf/5f5f"),  // 143
  C_STR("afaf/afaf/8787"),  // 144
  C_STR("afaf/afaf/afaf"),  // 145
  C_STR("afaf/afaf/d7d7"),  // 146
  C_STR("afaf/afaf/ffff"),  // 147
  C_STR("afaf/d7d7/0000"),  // 148
  C_STR("afaf/d7d7/5f5f"),  // 149
  C_STR("afaf/d7d7/8787"),  // 150
  C_STR("afaf/d7d7/afaf"),  // 151
  C_STR("afaf/d7d7/d7d7"),  // 152
  C_STR("afaf/d7d7/ffff"),  // 153
  C_STR("afaf/ffff/0000"),  // 154
  C_STR("afaf/ffff/5f5f"),  // 155
  C_STR("afaf/ffff/8787"),  // 156
  C_STR("afaf/ffff/afaf"),  // 157
  C_STR("afaf/ffff/d7d7"),  // 158
  C_STR("afaf/ffff/ffff"),  // 159
  C_STR("d7d7/0000/0000"),  // 160
  C_STR("d7d7/0000/5f5f"),  // 161
  C_STR("d7d7/0000/8787"),  // 162
  C_STR("d7d7/0000/afaf"),  // 163
  C_STR("d7d7/0000/d7d7"),  // 164
  C_STR("d7d7/0000/ffff"),  // 165
  C_STR("d7d7/5f5f/0000"),  // 166
  C_STR("d7d7/5f5f/5f5f"),  // 167
  C_STR("d7d7/5f5f/8787"),  // 168
  C_STR("d7d7/5f5f/afaf"),  // 169
  C_STR("d7d7/5f5f/d7d7"),  // 170
  C_STR("d7d7/5f5f/ffff"),  // 171
  C_STR("d7d7/8787/0000"),  // 172
  C_STR("d7d7/8787/5f5f"),  // 173
  C_STR("d7d7/8787/8787"),  // 174
  C_STR("d7d7/8787/afaf"),  // 175
  C_STR("d7d7/8787/d7d7"),  // 176
  C_STR("d7d7/8787/ffff"),  // 177
  C_STR("d7d7/afaf/0000"),  // 178
  C_STR("d7d7/afaf/5f5f"),  // 179
  C_STR("d7d7/afaf/8787"),  // 180
  C_STR("d7d7/afaf/afaf"),  // 181
  C_STR("d7d7/afaf/d7d7"),  // 182
  C_STR("d7d7/afaf/ffff"),  // 183
  C_STR("d7d7/d7d7/0000"),  // 184
  C_STR("d7d7/d7d7/5f5f"),  // 185
  C_STR("d7d7/d7d7/8787"),  // 186
  C_STR("d7d7/d7d7/afaf"),  // 187
  C_STR("d7d7/d7d7/d7d7"),  // 188
  C_STR("d7d7/d7d7/ffff"),  // 189
  C_STR("d7d7/ffff/0000"),  // 190
  C_STR("d7d7/ffff/5f5f"),  // 191
  C_STR("d7d7/ffff/8787"),  // 192
  C_STR("d7d7/ffff/afaf"),  // 193
  C_STR("d7d7/ffff/d7d7"),  // 194
  C_STR("d7d7/ffff/ffff"),  // 195
  C_STR("ffff/0000/0000"),  // 196
  C_STR("ffff/0000/5f5f"),  // 197
  C_STR("ffff/0000/8787"),  // 198
  C_STR("ffff/0000/afaf"),  // 199
  C_STR("ffff/0000/d7d7"),  // 200
  C_STR("ffff/0000/ffff"),  // 201
  C_STR("ffff/5f5f/0000"),  // 202
  C_STR("ffff/5f5f/5f5f"),  // 203
  C_STR("ffff/5f5f/8787"),  // 204
  C_STR("ffff/5f5f/afaf"),  // 205
  C_STR("ffff/5f5f/d7d7"),  // 206
  C_STR("ffff/5f5f/ffff"),  // 207
  C_STR("ffff/8787/0000"),  // 208
  C_STR("ffff/8787/5f5f"),  // 209
  C_STR("ffff/8787/8787"),  // 210
  C_STR("ffff/8787/afaf"),  // 211
  C_STR("ffff/8787/d7d7"),  // 212
  C_STR("ffff/8787/ffff"),  // 213
  C_STR("ffff/afaf/0000"),  // 214
  C_STR("ffff/afaf/5f5f"),  // 215
  C_STR("ffff/afaf/8787"),  // 216
  C_STR("ffff/afaf/afaf"),  // 217
  C_STR("ffff/afaf/d7d7"),  // 218
  C_STR("ffff/afaf/ffff"),  // 219
  C_STR("ffff/d7d7/0000"),  // 220
  C_STR("ffff/d7d7/5f5f"),  // 221
  C_STR("ffff/d7d7/8787"),  // 222
  C_STR("ffff/d7d7/afaf"),  // 223
  C_STR("ffff/d7d7/d7d7"),  // 224
  C_STR("ffff/d7d7/ffff"),  // 225
  C_STR("ffff/ffff/0000"),  // 226
  C_STR("ffff/ffff/5f5f"),  // 227
  C_STR("ffff/ffff/8787"),  // 228
  C_STR("ffff/ffff/afaf"),  // 229
  C_STR("ffff/ffff/d7d7"),  // 230
  C_STR("ffff/ffff/ffff"),  // 231
  C_STR("0808/0808/0808"),  // 232
  C_STR("1212/1212/1212"),  // 233
  C_STR("1c1c/1c1c/1c1c"),  // 234
  C_STR("2626/2626/2626"),  // 235
  C_STR("3030/3030/3030"),  // 236
  C_STR("3a3a/3a3a/3a3a"),  // 237
  C_STR("4444/4444/4444"),  // 238
  C_STR("4e4e/4e4e/4e4e"),  // 239
  C_STR("5858/5858/5858"),  // 240
  C_STR("6262/6262/6262"),  // 241
  C_STR("6c6c/6c6c/6c6c"),  // 242
  C_STR("7676/7676/7676"),  // 243
  C_STR("8080/8080/8080"),  // 244
  C_STR("8a8a/8a8a/8a8a"),  // 245
  C_STR("9494/9494/9494"),  // 246
  C_STR("9e9e/9e9e/9e9e"),  // 247
  C_STR("a8a8/a8a8/a8a8"),  // 248
  C_STR("b2b2/b2b2/b2b2"),  // 249
  C_STR("bcbc/bcbc/bcbc"),  // 250
  C_STR("c6c6/c6c6/c6c6"),  // 251
  C_STR("d0d0/d0d0/d0d0"),  // 252
  C_STR("dada/dada/dada"),  // 253
  C_STR("e4e4/e4e4/e4e4"),  // 254
  C_STR("eeee/eeee/eeee")   // 255
}};


// ConEmu inline functions

// protected methods of ConEmu
//----------------------------------------------------------------------
inline void ConEmu::enableConEmuDebug (bool enable) noexcept
{
  debug = enable;
}

//----------------------------------------------------------------------
inline auto ConEmu::isConEmuChildProcess (pid_t pid) const noexcept -> bool
{
  return bool( pid == 0 );
}

//----------------------------------------------------------------------
inline void ConEmu::printConEmuDebug() noexcept
{
  // Printing terminal debug information for some escape sequences

  if ( ! debug )
    return;

  // Terminal Requests
  static_cast<void>(setenv ("DSR",        "\\033[5n", 1));
  static_cast<void>(setenv ("CURSOR_POS", "\\033[6n", 1));
  static_cast<void>(setenv ("DECID",      "\\033Z", 1));
  static_cast<void>(setenv ("DA",         "\\033[c", 1));
  static_cast<void>(setenv ("DA1",        "\\033[1c", 1));
  static_cast<void>(setenv ("SEC_DA",     "\\033[>c", 1));
  static_cast<void>(setenv ("ANSWERBACK", "\\005", 1));
  static_cast<void>(setenv ("TITLE",      "\\033[21t", 1));
  static_cast<void>(setenv ("COLOR16",    "\\033]4;15;?\\a", 1));
  static_cast<void>(setenv ("COLOR88",    "\\033]4;87;?\\a", 1));
  static_cast<void>(setenv ("COLOR256",   "\\033]4;254;?\\a", 1));
  // Cursor positioning
  static_cast<void>(setenv ("GO_MIDDLE",  "\\033[80D\\033[15C", 1));
  static_cast<void>(setenv ("GO_RIGHT",   "\\033[79D\\033[40C", 1));

  finalcut::FString line (69, '-');
  std::cout << std::endl << line << std::endl;
  std::cout << "Probe           Escape sequence          Reply";
  std::cout << std::endl << line << std::endl;

  // Command line
  static constexpr char debug_command[] = "/bin/bash -c ' \
      for i in DSR CURSOR_POS DECID DA DA1 SEC_DA ANSWERBACK \
               TITLE COLOR16 COLOR88 COLOR256; \
      do \
        eval \"echo -en \\\"$i${GO_MIDDLE}\\\"; \
              echo -n \\\"\\${$i}\\\"; \
              echo -en \\\"${GO_RIGHT}\\${$i}\\\"\"; \
        sleep 0.5; \
        echo -e \"\\r\"; \
      done'";
  static_cast<void>(system(debug_command));
  std::cout << std::flush;
  std::fflush (stdout);
}

//----------------------------------------------------------------------
inline void ConEmu::closeConEmuStdStreams()
{
  if ( isValidFileDescriptor(fd_stdin) )
    static_cast<void>(::close(fd_stdin));   // stdin

  if ( isValidFileDescriptor(fd_stdout) )
    static_cast<void>(::close(fd_stdout));  // stdout

  if ( isValidFileDescriptor(fd_stderr) )
    static_cast<void>(::close(fd_stderr));  // stderr
}

//----------------------------------------------------------------------
inline auto ConEmu::forkConEmu() -> pid_t
{
  try
  {
    // Initialize buffer with '\0'
    buffer.fill('\0');

    if ( ! openMasterPTY() || ! openSlavePTY() )
      return INVALID_PID;

    const pid_t pid = fork();  // Create a child process

    if ( pid < 0)  // Fork failed
    {
      cleanupResources();
      return INVALID_PID;
    }

    if ( isConEmuChildProcess(pid) )  // Child process
    {
      return setupChildProcess() ? 0 : INVALID_PID;
    }

    // Parent process
    return waitForChildReady() ? pid : INVALID_PID;
  }
  catch (const std::exception&)
  {
    cleanupResources();
    return INVALID_PID;
  }
}

//----------------------------------------------------------------------
inline void ConEmu::startConEmuTerminal (console con)
{
  validateConsoleType(con);

  if ( ! isValidFileDescriptor(fd_master) )
    return;

  closeSlavePTY();
  auto time_last_data{finalcut::FObjectTimer::getCurrentTime()};

  while ( ! finalcut::FObjectTimer::isTimeout (time_last_data, TERMINAL_TIMEOUT) )
  {
    fd_set ifds;
    struct timeval tv;

    FD_ZERO(&ifds);
    FD_SET(fd_stdin, &ifds);
    FD_SET(fd_master, &ifds);
    tv.tv_sec  = 0;
    tv.tv_usec = 750000;  // 750 ms

    // Wait for data from stdin or the master side of PTY
    const int select_result = select(fd_master + 1, &ifds, nullptr, nullptr, &tv);

    if ( select_result < 0 )
    {
      if ( errno == EINTR )
        continue;  // Interrupted by signal, retry

      break;  // Other error
    }

    if ( select_result == 0 )
      continue;  // Timeout, check for overall timeout

    // Data on standard input
    if ( FD_ISSET(fd_stdin, &ifds) )
    {
      const auto len = read (fd_stdin, buffer.data(), buffer.size() - 1);

      if ( len > 0 && std::size_t(len) < buffer.size() )
      {
        buffer[std::size_t(len)] = '\0';
        writeToMaster(buffer.data(), std::size_t(len));  // Send data to the master side
        time_last_data = finalcut::FObjectTimer::getCurrentTime();
      }
    }

    // Data on the master side of PTY
    if ( FD_ISSET(fd_master, &ifds) )
    {
      const auto len = read (fd_master, buffer.data(), buffer.size() - 1);

      if ( len == -1 || std::size_t(len) >= buffer.size() )
        break;

      if ( len > 0 )
      {
        buffer[std::size_t(len)] = '\0';
        parseTerminalBuffer (std::size_t(len), con);
        time_last_data = finalcut::FObjectTimer::getCurrentTime();
      }
    }
  }
}


// private methods of ConEmu
//----------------------------------------------------------------------
inline auto ConEmu::getAnswerback (console con) const noexcept -> const char*
{
  static ConsoleStringTableType answerback
  {{
    nullptr,         // Ansi,
    nullptr,         // XTerm
    nullptr,         // Rxvt
    nullptr,         // Urxvt
    nullptr,         // KDE Konsole
    nullptr,         // GNOME Terminal
    nullptr,         // VTE Terminal >= 0.53.0
    C_STR("PuTTY"),  // PuTTY
    nullptr,         // Windows Terminal
    nullptr,         // Tera Term
    nullptr,         // Cygwin
    nullptr,         // Mintty
    nullptr,         // st - simple terminal
    nullptr,         // Linux console
    nullptr,         // FreeBSD console
    nullptr,         // NetBSD console
    nullptr,         // OpenBSD console
    nullptr,         // Sun console
    nullptr,         // screen
    nullptr,         // tmux
    nullptr,         // kterm,
    nullptr,         // mlterm - Multi Lingual TERMinal
    nullptr          // kitty
  }};

  return answerback[static_cast<std::size_t>(con)];
}

//----------------------------------------------------------------------
inline auto ConEmu::getDSR (console con) const noexcept -> const char*
{
  static ConsoleStringTableType dsr
  {{
    nullptr,           // Ansi,
    C_STR("\033[0n"),  // XTerm
    C_STR("\033[0n"),  // Rxvt
    C_STR("\033[0n"),  // Urxvt
    C_STR("\033[0n"),  // KDE Konsole
    C_STR("\033[0n"),  // GNOME Terminal
    C_STR("\033[0n"),  // VTE Terminal >= 0.53.0
    C_STR("\033[0n"),  // PuTTY
    C_STR("\033[0n"),  // Windows Terminal >= 1.2
    C_STR("\033[0n"),  // Tera Term
    nullptr,           // Cygwin
    C_STR("\033[0n"),  // Mintty
    nullptr,           // st - simple terminal
    C_STR("\033[0n"),  // Linux console
    C_STR("\033[0n"),  // FreeBSD console
    C_STR("\033[0n"),  // NetBSD console
    C_STR("\033[0n"),  // OpenBSD console
    nullptr,           // Sun console
    C_STR("\033[0n"),  // screen
    C_STR("\033[0n"),  // tmux
    C_STR("\033[0n"),  // kterm
    C_STR("\033[0n"),  // mlterm - Multi Lingual TERMinal
    C_STR("\033[0n")   // kitty
  }};

  return dsr[static_cast<std::size_t>(con)];
}

//----------------------------------------------------------------------
inline auto ConEmu::getDECID (console con) const noexcept -> const char*
{
  static ConsoleStringTableType dec_id
  {{
    nullptr,                               // Ansi,
    C_STR("\033[?63;1;2;6;4;6;9;15;22c"),  // XTerm
    C_STR("\033[?1;2c"),                   // Rxvt
    C_STR("\033[?1;2c"),                   // Urxvt
    C_STR("\033[?1;2c"),                   // KDE Konsole
    C_STR("\033[?62;c"),                   // GNOME Terminal
    C_STR("\033[?65;1;9c"),                // VTE Terminal >= 0.53.0
    C_STR("\033[?6c"),                     // PuTTY
    nullptr,                               // Windows Terminal
    C_STR("\033[?1;2c"),                   // Tera Term
    nullptr,                               // Cygwin
    C_STR("\033[?1;2;6;22c"),              // Mintty
    C_STR("\033[?6c"),                     // st - simple terminal
    C_STR("\033[?6c"),                     // Linux console
    nullptr,                               // FreeBSD console
    nullptr,                               // NetBSD console
    nullptr,                               // OpenBSD console
    nullptr,                               // Sun console
    C_STR("\033[?1;2c"),                   // screen
    nullptr,                               // tmux
    C_STR("\033[?1;2c"),                   // kterm
    C_STR("\033[?63;1;2;3;4;7;29c"),       // mlterm - Multi Lingual TERMinal
    nullptr                                // kitty
  }};

  return dec_id[static_cast<std::size_t>(con)];
}

//----------------------------------------------------------------------
inline auto ConEmu::getDA (console con) const noexcept -> const char*
{
  static ConsoleStringTableType da
  {{
    nullptr,                               // Ansi,
    C_STR("\033[?63;1;2;6;4;6;9;15;22c"),  // XTerm
    C_STR("\033[?1;2c"),                   // Rxvt
    C_STR("\033[?1;2c"),                   // Urxvt
    C_STR("\033[?1;2c"),                   // KDE Konsole
    C_STR("\033[?62;c"),                   // GNOME Terminal
    C_STR("\033[?65;1;9c"),                // VTE Terminal >= 0.53.0
    C_STR("\033[?6c"),                     // PuTTY
    C_STR("\033[?1;0c"),                   // Windows Terminal >= 1.2
    C_STR("\033[?1;2c"),                   // Tera Term
    C_STR("\033[?6c"),                     // Cygwin
    C_STR("\033[?1;2;6;22c"),              // Mintty
    C_STR("\033[?6c"),                     // st - simple terminal
    C_STR("\033[?6c"),                     // Linux console
    C_STR("\033[?1;2c"),                   // FreeBSD console
    C_STR("\033[?62;6c"),                  // NetBSD console
    C_STR("\033[?62;6c"),                  // OpenBSD console
    nullptr,                               // Sun console
    C_STR("\033[?1;2c"),                   // screen
    C_STR("\033[?1;2c"),                   // tmux
    C_STR("\033[?1;2c"),                   // kterm
    C_STR("\033[?63;1;2;3;4;7;29c"),       // mlterm - Multi Lingual TERMinal
    C_STR("\033[?62;c")                    // kitty
  }};

  return da[static_cast<std::size_t>(con)];
}

//----------------------------------------------------------------------
inline auto ConEmu::getDA1 (console con) const noexcept -> const char*
{
  static ConsoleStringTableType da1
  {{
    nullptr,                          // Ansi,
    nullptr,                          // XTerm
    C_STR("\033[?1;2c"),              // Rxvt
    C_STR("\033[?1;2c"),              // Urxvt
    C_STR("\033[?1;2c"),              // KDE Konsole
    C_STR("\033[?62;c"),              // GNOME Terminal
    C_STR("\033[?65;1;9c"),           // VTE Terminal >= 0.53.0
    C_STR("\033[?6c"),                // PuTTY
    nullptr,                          // Windows Terminal
    C_STR("\033[?1;2c"),              // Tera Term
    C_STR("\033[?6c"),                // Cygwin
    C_STR("\033[?1;2;6;22c"),         // Mintty
    nullptr,                          // st - simple terminal
    nullptr,                          // Linux console
    nullptr,                          // FreeBSD console
    nullptr,                          // NetBSD console
    nullptr,                          // OpenBSD console
    nullptr,                          // Sun console
    nullptr,                          // screen
    nullptr,                          // tmux
    nullptr,                          // kterm
    C_STR("\033[?63;1;2;3;4;7;29c"),  // mlterm - Multi Lingual TERMinal
    nullptr                           // kitty
  }};

  return da1[static_cast<std::size_t>(con)];
}

//----------------------------------------------------------------------
inline auto ConEmu::getSEC_DA (console con) const noexcept -> const char*
{
  static ConsoleStringTableType sec_da
  {{
    nullptr,                      // Ansi,
    C_STR("\033[>19;312;0c"),     // XTerm
    C_STR("\033[>82;20710;0c"),   // Rxvt
    C_STR("\033[>85;95;0c"),      // Urxvt
    C_STR("\033[>0;115;0c"),      // KDE Konsole
    C_STR("\033[>1;5202;0c"),     // GNOME Terminal
    C_STR("\033[>65;5300;1c"),    // VTE Terminal >= 0.53.0
    C_STR("\033[>0;136;0c"),      // PuTTY
    C_STR("\033[>0;10;1c"),       // Windows Terminal >= 1.2
    C_STR("\033[>32;278;0c"),     // Tera Term
    C_STR("\033[>67;200502;0c"),  // Cygwin
    C_STR("\033[>77;20402;0c"),   // Mintty
    nullptr,                      // st - simple terminal
    nullptr,                      // Linux console
    C_STR("\033[>0;10;0c"),       // FreeBSD console
    C_STR("\033[>24;20;0c"),      // NetBSD console
    C_STR("\033[>24;20;0c"),      // OpenBSD console
    nullptr,                      // Sun console
    C_STR("\033[>83;40201;0c"),   // screen
    C_STR("\033[>84;0;0c"),       // tmux
    C_STR("\033[?1;2c"),          // kterm
    C_STR("\033[>24;279;0c"),     // mlterm - Multi Lingual TERMinal
    C_STR("\033[>1;4000;13c")     // kitty
  }};

  return sec_da[static_cast<std::size_t>(con)];
}

//----------------------------------------------------------------------
inline auto ConEmu::openMasterPTY() -> bool
{
  // Open a pseudoterminal device
  fd_master = posix_openpt(O_RDWR);

  if ( ! isValidFileDescriptor(fd_master) )
    return false;

  // Change the slave pseudoterminal access rights
  if ( grantpt(fd_master) != 0 )
  {
    closeMasterPTY();
    return false;
  }

  // Unlock the pseudoterminal master/slave pair
  if ( unlockpt(fd_master) != 0 )
  {
    closeMasterPTY();
    return false;
  }

  return true;
}

//----------------------------------------------------------------------
inline auto ConEmu::openSlavePTY() -> bool
{
  closeSlavePTY();

  // Get PTY filename
  const char* pty_name = ptsname(fd_master);

  if ( ! pty_name )
    return false;

  // Open the slave PTY
  fd_slave = ::open(pty_name, O_RDWR);

  return isValidFileDescriptor(fd_slave);
}

//----------------------------------------------------------------------
inline void ConEmu::closeMasterPTY() noexcept
{
  if ( isValidFileDescriptor(fd_master) )
  {
    static_cast<void>(::close (fd_master));
    fd_master = INVALID_FD;
  }
}

//----------------------------------------------------------------------
inline void ConEmu::closeSlavePTY() noexcept
{
  if ( isValidFileDescriptor(fd_slave) )
  {
    static_cast<void>(::close (fd_slave));
    fd_slave = INVALID_FD;
  }
}

//----------------------------------------------------------------------
inline void ConEmu::cleanupResources() noexcept
{
  closeMasterPTY();
  closeSlavePTY();

  // Unmap shared memory
  if ( shared_memory_ptr && shared_memory_ptr != MAP_FAILED )
  {
    static_cast<void>(munmap(shared_memory_ptr, SHARED_MEMORY_SIZE));
    shared_memory_ptr = nullptr;
    shared_state = nullptr;
  }
}

//----------------------------------------------------------------------
inline void ConEmu::validateConsoleType (console con) const
{
  if ( std::size_t(con) >= NUM_OF_CONSOLES )
  {
    throw std::invalid_argument( "Invalid console type: " +
                                 std::to_string(std::size_t(con)) );
  }
}

//----------------------------------------------------------------------
inline auto ConEmu::setupChildProcess() -> bool
{
  try
  {
    struct termios term_settings;
    closeMasterPTY();

    // Creates a session and makes the current process to the leader
    if ( setsid() == INVALID_PID )
    {
      *shared_state = true;
      return false;
    }

#ifdef TIOCSCTTY
    // Set controlling tty
    if ( ::ioctl(fd_slave, TIOCSCTTY, 0) == -1 )
    {
      *shared_state = true;
      return false;
    }
#endif

    // Get current terminal settings
    if ( tcgetattr(fd_slave, &term_settings) == -1 )
    {
      *shared_state = true;
      return false;
    }

    // Set raw mode on the slave side of the PTY
    cfmakeraw (&term_settings);

    if ( tcsetattr (fd_slave, TCSANOW, &term_settings) == -1 )
    {
      *shared_state = true;
      return false;
    }

#ifdef TIOCSWINSZ
    // Set slave tty window size
    struct winsize size{};
    size.ws_row = DEFAULT_ROWS;
    size.ws_col = DEFAULT_COLS;

    if ( ::ioctl(fd_slave, TIOCSWINSZ, &size) == -1 )
    {
      *shared_state = true;
      return INVALID_PID;
    }
#endif

    closeConEmuStdStreams();

    fd_stdin  = dup(fd_slave);  // PTY becomes stdin  (0)
    fd_stdout = dup(fd_slave);  // PTY becomes stdout (1)
    fd_stderr = dup(fd_slave);  // PTY becomes stderr (2)

    if ( fd_stdin < 0 || fd_stdout < 0 || fd_stderr < 0 )
    {
      *shared_state = true;
      return false;
    }

    closeSlavePTY();

    // The child process is now ready for input
    *shared_state = true;
    return true;
  }
  catch (...)
  {
    *shared_state = true;
    return false;
  }
}

//----------------------------------------------------------------------
inline auto ConEmu::waitForChildReady() -> bool
{
  static constexpr std::chrono::milliseconds CHILD_WAIT_TIME{10};  // 10 ms
  static constexpr int MAX_CHILD_ATTEMPTS{150};  // => 150 × 10 ms = 1.5 sec
  int count = 0;

  // Wait until the child process is ready for input
  while ( ! *shared_state && count < MAX_CHILD_ATTEMPTS )
  {
    std::this_thread::sleep_for(CHILD_WAIT_TIME);  // Wait 10 ms
    count++;
  }

  const bool child_ready = *shared_state;
  *shared_state = false;
  return child_ready;
}

//----------------------------------------------------------------------
inline auto ConEmu::isValidFileDescriptor (int fd) const noexcept -> bool
{
  return fd >= 0;
}

//----------------------------------------------------------------------
inline void ConEmu::writeToMaster (const char* data, std::size_t length) noexcept
{
  if ( isValidFileDescriptor(fd_master) && data && length > 0 )
  {
    static_cast<void>(::write(fd_master, data, length));
  }
}

//----------------------------------------------------------------------
inline void ConEmu::writeToStdout (const char* data, std::size_t length) noexcept
{
  if ( isValidFileDescriptor(fd_stdout) && data && length > 0 )
  {
    static_cast<void>(::write(fd_stdout, data, length));
  }
}

//----------------------------------------------------------------------
inline void ConEmu::parseTerminalBuffer (std::size_t length, console con) noexcept
{
  for (std::size_t i = 0; i < length; i++)
  {
    // Handle various terminal escape sequences
    if ( buffer[i] == ENQ[0] )  // Enquiry character (ENQ) - Ctrl-E
    {
      const char* answer = getAnswerback(con);

      if ( answer )
        writeToMaster(answer, std::strlen(answer));
    }
    else if ( i + 1 < length  // Terminal ID (DECID) - ESC Z
           && buffer[i] == '\033'
           && buffer[i + 1] == 'Z' )
    {
      const char* dec_id = getDECID(con);

      if ( dec_id )
        writeToMaster(dec_id, std::strlen(dec_id));

      i += 1;  // Skip the sequence
    }
    else if ( i + 3 < length  // Device status report (DSR) - ESC [ 5 n
           && buffer[i] == '\033'
           && buffer[i + 1] == '['
           && buffer[i + 2] == '5'
           && buffer[i + 3] == 'n' )
    {
      const char* dsr = getDSR(con);

      if ( dsr )
        writeToMaster(dsr, std::strlen(dsr));

      i += 3;  // Skip the sequence
    }
    else if ( i + 3 < length  // Report cursor position (CPR) - ESC [ 6 n
           && buffer[i] == '\033'
           && buffer[i + 1] == '['
           && buffer[i + 2] == '6'
           && buffer[i + 3] == 'n' )
    {
      writeToMaster("\033[25;80R", 8);  // row 25 ; column 80
      i += 3;  // Skip the sequence
    }
    else if ( i < length - 2  // Device attributes (DA) - ESC [ c
           && buffer[i] == '\033'
           && buffer[i + 1] == '['
           && buffer[i + 2] == 'c' )
    {
      const char* da = getDA(con);

      if ( da )
        writeToMaster(da, std::strlen(da));

      i += 2;  // Skip the sequence
    }
    else if ( i + 3 < length  // Device attributes (DA1) - ESC [ 1 c
           && buffer[i] == '\033'
           && buffer[i + 1] == '['
           && buffer[i + 2] == '1'
           && buffer[i + 3] == 'c' )
    {
      const char* da1 = getDA1(con);

      if ( da1 )
        writeToMaster(da1, std::strlen(da1));

      i += 3;  // Skip the sequence
    }
    else if ( i + 3 < length  // Secondary device attributes (SEC_DA) - ESC [ > c
           && buffer[i] == '\033'
           && buffer[i + 1] == '['
           && buffer[i + 2] == '>'
           && buffer[i + 3] == 'c' )
    {
      const char* sec_da = getSEC_DA(con);

      if ( sec_da )
        writeToMaster(sec_da, std::strlen(sec_da));

      i += 3;  // Skip the sequence
    }
    else if ( i + 4 < length  // Report xterm window's title - ESC [ 2 1 t
           && buffer[i] == '\033'
           && buffer[i + 1] == '['
           && buffer[i + 2] == '2'
           && buffer[i + 3] == '1'
           && buffer[i + 4] == 't' )
    {
      if ( con == console::urxvt )
        writeToMaster("\033]l", 3);
      else if ( con == console::tera_term )
        writeToMaster("\033]l\033\\", 5);
      else if ( con == console::screen )
        writeToMaster("\033]lbash\033\\", 9);
      else if ( con != console::ansi
             && con != console::rxvt
             && con != console::kde_konsole
             && con != console::cygwin
             && con != console::win_terminal
             && con != console::mintty
             && con != console::stterm
             && con != console::linux_con
             && con != console::freebsd_con
             && con != console::netbsd_con
             && con != console::openbsd_con
             && con != console::sun_con
             && con != console::tmux
             && con != console::kterm
             && con != console::mlterm
             && con != console::kitty )
        writeToMaster("\033]lTITLE\033\\", 10);

      i += 4;  // Skip the sequence
    }
    else if ( i + 7 < length  // Get xterm color name (0-9) - ESC ] 4 ; 0..9 ; ? BEL
           && buffer[i] == '\033'
           && buffer[i + 1] == ']'
           && buffer[i + 2] == '4'
           && buffer[i + 3] == ';'
           && buffer[i + 4] >= '0' && buffer[i + 4] <= '9'
           && buffer[i + 5] == ';'
           && buffer[i + 6] == '?'
           && buffer[i + 7] == '\a' )
    {
      // Check if this terminal supports color queries
      if ( con != console::ansi
        && con != console::rxvt
        && con != console::kde_konsole
        && con != console::cygwin
        && con != console::win_terminal
        && con != console::mintty
        && con != console::stterm
        && con != console::linux_con
        && con != console::freebsd_con
        && con != console::netbsd_con
        && con != console::openbsd_con
        && con != console::sun_con
        && con != console::screen
        && con != console::tmux
        && con != console::kterm )
      {
        const int color_index = buffer[i + 4] - '0';
        writeToMaster("\033]4;", 4);
        writeToMaster(&buffer[i + 4], 1);
        writeToMaster(";rgb:", 5);
        writeToMaster(colorname[color_index], 14);
        writeToMaster("\a", 1);
      }

      i += 7;  // Skip the sequence
    }
    else if ( i + 8 < length  // Get xterm color name (10-99) - ESC ] 4 ; 0..9 0..9 ; ? BEL
           && buffer[i] == '\033'
           && buffer[i + 1] == ']'
           && buffer[i + 2] == '4'
           && buffer[i + 3] == ';'
           && buffer[i + 4] >= '0' && buffer[i + 4] <= '9'
           && buffer[i + 5] >= '0' && buffer[i + 5] <= '9'
           && buffer[i + 6] == ';'
           && buffer[i + 7] == '?'
           && buffer[i + 8] == '\a' )
    {
      // Check if this terminal supports color queries
      if ( con != console::ansi
        && con != console::rxvt
        && con != console::kde_konsole
        && con != console::cygwin
        && con != console::win_terminal
        && con != console::mintty
        && con != console::stterm
        && con != console::linux_con
        && con != console::freebsd_con
        && con != console::netbsd_con
        && con != console::openbsd_con
        && con != console::sun_con
        && con != console::screen
        && con != console::tmux
        && con != console::kterm )
      {
        const int color_index = (buffer[i + 4] - '0') * 10
                              + (buffer[i + 5] - '0');
        writeToMaster("\033]4;", 4);
        writeToMaster(&buffer[i + 4], 1);
        writeToMaster(&buffer[i + 5], 1);
        writeToMaster(";rgb:", 5);
        writeToMaster(colorname[color_index], 14);
        writeToMaster("\a", 1);
      }

      i += 8;  // Skip the sequence
    }
    else if ( i + 9 < length  // Get xterm color name (100-255) - ESC ] 4 ; 0..9 0..9 0..9 ; ? BEL
           && buffer[i] == '\033'
           && buffer[i + 1] == ']'
           && buffer[i + 2] == '4'
           && buffer[i + 3] == ';'
           && buffer[i + 4] >= '0' && buffer[i + 4] <= '9'
           && buffer[i + 5] >= '0' && buffer[i + 5] <= '9'
           && buffer[i + 6] >= '0' && buffer[i + 6] <= '9'
           && buffer[i + 7] == ';'
           && buffer[i + 8] == '?'
           && buffer[i + 9] == '\a' )
    {
      // Check if this terminal supports color queries
      if ( con != console::ansi
        && con != console::rxvt
        && con != console::kde_konsole
        && con != console::cygwin
        && con != console::win_terminal
        && con != console::mintty
        && con != console::stterm
        && con != console::linux_con
        && con != console::freebsd_con
        && con != console::netbsd_con
        && con != console::openbsd_con
        && con != console::sun_con
        && con != console::screen
        && con != console::tmux
        && con != console::kterm )
      {
        const int color_index = (buffer[i + 4] - '0') * 100
                              + (buffer[i + 5] - '0') * 10
                              + (buffer[i + 6] - '0');

        if ( color_index < 256 )
        {
          writeToMaster("\033]4;", 4);
          writeToMaster(&buffer[i + 4], 1);
          writeToMaster(&buffer[i + 5], 1);
          writeToMaster(&buffer[i + 6], 1);
          writeToMaster(";rgb:", 5);
          writeToMaster(colorname[color_index], 14);
          writeToMaster("\a", 1);
        }
      }

      i += 9;  // Skip the sequence
    }
    else
    {
      writeToStdout(&buffer[i], 1);  // Send data to stdout
    }
  }
}

}  // namespace test

#endif  // CONEMU_H
