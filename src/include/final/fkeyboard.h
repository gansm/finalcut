/***********************************************************************
* fkeyboard.h - Read keyboard events                                   *
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
 * ▕ FKeyboard ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FKEYBOARD_H
#define FKEYBOARD_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <sys/time.h>

#include <array>
#include <functional>
#include <memory>
#include <queue>

#include "final/fkey_map.h"
#include "final/fstring.h"
#include "final/ftypes.h"

namespace finalcut
{

// class forward declaration
class FApplication;
class FString;

//----------------------------------------------------------------------
// class FKeyboardCommand
//----------------------------------------------------------------------

class FKeyboardCommand final
{
  public:
    // Constructors
    FKeyboardCommand () = default;
    explicit FKeyboardCommand (const std::function<void()>& fn)
      : handler(fn)
    { }

    // Method
    void execute() const
    {
      handler();
    }

  private:
    // Data members
    std::function<void()> handler{};
};


//----------------------------------------------------------------------
// class FKeyboard
//----------------------------------------------------------------------

class FKeyboard final
{
  public:
    // Constants
    static constexpr std::size_t FIFO_BUF_SIZE{512};

    // Using-declaration
    using keybuffer = char[FIFO_BUF_SIZE];

    // Constructor
    FKeyboard();

    // Disable copy constructor
    FKeyboard (const FKeyboard&) = delete;

    // Destructor
    ~FKeyboard() = default;

    // Disable copy assignment operator (=)
    FKeyboard& operator = (const FKeyboard&) = delete;

    // Accessors
    FString               getClassName() const;
    FKey                  getKey() const;
    FString               getKeyName (const FKey) const;
    keybuffer&            getKeyBuffer();
    timeval*              getKeyPressedTime();
    static uInt64         getKeypressTimeout();
    static uInt64         getReadBlockingTime();

    // Mutators
    template <typename T>
    void                  setTermcapMap (const T&);
    void                  setTermcapMap();
    static void           setKeypressTimeout (const uInt64);
    static void           setReadBlockingTime (const uInt64);
    static void           setNonBlockingInputSupport (bool = true);
    bool                  setNonBlockingInput (bool = true);
    bool                  unsetNonBlockingInput();
    void                  enableUTF8();
    void                  disableUTF8();
    void                  enableMouseSequences();
    void                  disableMouseSequences();
    void                  setPressCommand (const FKeyboardCommand&);
    void                  setReleaseCommand (const FKeyboardCommand&);
    void                  setEscPressedCommand (const FKeyboardCommand&);
    void                  setMouseTrackingCommand (const FKeyboardCommand&);

    // Inquiry
    bool                  hasPendingInput() const;
    bool                  hasDataInQueue() const;

    // Methods
    bool&                 hasUnprocessedInput();
    bool                  isKeyPressed (uInt64 = read_blocking_time);
    void                  clearKeyBuffer();
    void                  clearKeyBufferOnTimeout();
    void                  fetchKeyCode();
    void                  escapeKeyHandling();
    void                  processQueuedInput();

  private:
    // Using-declaration
    using FKeyMapPtr = std::shared_ptr<decltype(fc::fkey_cap_table)>;

    // Constants
    static constexpr FKey NOT_SET = static_cast<FKey>(-1);
    static constexpr std::size_t MAX_QUEUE_SIZE = 32;

    // Accessors
    FKey                  getMouseProtocolKey() const;
    FKey                  getTermcapKey();
    FKey                  getKnownKey();
    FKey                  getSingleKey();

    // Inquiry
    static bool           isKeypressTimeout();
    static bool           isIntervalTimeout();

    // Methods
    FKey                  UTF8decode (const char[]) const;
    ssize_t               readKey();
    void                  parseKeyBuffer();
    FKey                  parseKeyString();
    FKey                  keyCorrection (const FKey&) const;
    void                  substringKeyHandling();
    void                  keyPressed() const;
    void                  keyReleased() const;
    void                  escapeKeyPressed() const;
    void                  mouseTracking() const;

    // Data members
    FKeyboardCommand      keypressed_cmd{};
    FKeyboardCommand      keyreleased_cmd{};
    FKeyboardCommand      escape_key_cmd{};
    FKeyboardCommand      mouse_tracking_cmd{};

    static timeval        time_keypressed;
    static uInt64         read_blocking_time;
    static uInt64         read_blocking_time_short;
    static uInt64         key_timeout;
    static bool           non_blocking_input_support;
    FKeyMapPtr            key_map{};
    std::queue<FKey>      fkey_queue{};
    FKey                  fkey{FKey::None};
    FKey                  key{FKey::None};
    char                  read_character{};
    char                  fifo_buf[FIFO_BUF_SIZE]{'\0'};
    int                   fifo_offset{0};
    int                   stdin_status_flags{0};
    bool                  has_pending_input{false};
    bool                  fifo_in_use{false};
    bool                  unprocessed_buffer_data{false};
    bool                  utf8_input{false};
    bool                  mouse_support{true};
    bool                  non_blocking_stdin{false};
};

// FKeyboard inline functions
//----------------------------------------------------------------------
inline FString FKeyboard::getClassName() const
{ return "FKeyboard"; }

//----------------------------------------------------------------------
inline FKey FKeyboard::getKey() const
{ return key; }

//----------------------------------------------------------------------
inline FKeyboard::keybuffer& FKeyboard::getKeyBuffer()
{ return fifo_buf; }

//----------------------------------------------------------------------
inline timeval* FKeyboard::getKeyPressedTime()
{ return &time_keypressed; }

//----------------------------------------------------------------------
inline uInt64 FKeyboard::getKeypressTimeout()
{ return key_timeout; }

//----------------------------------------------------------------------
inline uInt64 FKeyboard::getReadBlockingTime()
{ return read_blocking_time; }

//----------------------------------------------------------------------
template <typename T>
inline void FKeyboard::setTermcapMap (const T& keymap)
{ key_map = std::make_shared<T>(keymap); }

//----------------------------------------------------------------------
inline void FKeyboard::setTermcapMap ()
{
  using type = decltype(fc::fkey_cap_table);
  key_map = std::make_shared<type>(fc::fkey_cap_table);
}

//----------------------------------------------------------------------
inline void FKeyboard::setKeypressTimeout (const uInt64 timeout)
{ key_timeout = timeout; }

//----------------------------------------------------------------------
inline void FKeyboard::setReadBlockingTime (const uInt64 blocking_time)
{ read_blocking_time = blocking_time; }

//----------------------------------------------------------------------
inline void FKeyboard::setNonBlockingInputSupport (bool enable)
{ non_blocking_input_support = enable; }

//----------------------------------------------------------------------
inline bool FKeyboard::unsetNonBlockingInput()
{ return setNonBlockingInput(false); }

//----------------------------------------------------------------------
inline bool FKeyboard::hasPendingInput() const
{ return has_pending_input; }

//----------------------------------------------------------------------
inline bool FKeyboard::hasDataInQueue() const
{ return ! fkey_queue.empty(); }

//----------------------------------------------------------------------
inline void FKeyboard::enableUTF8()
{ utf8_input = true; }

//----------------------------------------------------------------------
inline void FKeyboard::disableUTF8()
{ utf8_input = false; }

//----------------------------------------------------------------------
inline void FKeyboard::enableMouseSequences()
{ mouse_support = true; }

//----------------------------------------------------------------------
inline void FKeyboard::disableMouseSequences()
{ mouse_support = false; }

//----------------------------------------------------------------------
inline void FKeyboard::setPressCommand (const FKeyboardCommand& cmd)
{ keypressed_cmd = cmd; }

//----------------------------------------------------------------------
inline void FKeyboard::setReleaseCommand (const FKeyboardCommand& cmd)
{ keyreleased_cmd = cmd; }

//----------------------------------------------------------------------
inline void FKeyboard::setEscPressedCommand (const FKeyboardCommand& cmd)
{ escape_key_cmd = cmd; }

//----------------------------------------------------------------------
inline void FKeyboard::setMouseTrackingCommand (const FKeyboardCommand& cmd)
{ mouse_tracking_cmd = cmd; }

}  // namespace finalcut

#endif  // FKEYBOARD_H
