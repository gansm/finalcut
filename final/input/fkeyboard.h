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

#include <algorithm>
#include <array>
#include <functional>
#include <memory>
#include <queue>
#include <string>

#include "final/ftypes.h"
#include "final/input/fkey_map.h"
#include "final/util/fstring.h"

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
    explicit FKeyboardCommand (std::function<void()>&& fn)
      : handler(std::move(fn))
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

    // Accessors
    FString               getClassName() const;
    static auto           getInstance() -> FKeyboard&;
    FKey                  getKey() const noexcept;
    FString               getKeyName (const FKey) const;
    keybuffer&            getKeyBuffer() & noexcept;
    TimeValue             getKeyPressedTime() const noexcept;
    static uInt64         getKeypressTimeout() noexcept;
    static uInt64         getReadBlockingTime() noexcept;

    // Mutators
    template <typename T>
    void                  setTermcapMap (const T&);
    void                  setTermcapMap();
    static void           setKeypressTimeout (const uInt64) noexcept;
    static void           setReadBlockingTime (const uInt64) noexcept;
    static void           setNonBlockingInputSupport (bool = true) noexcept;
    bool                  setNonBlockingInput (bool = true);
    bool                  unsetNonBlockingInput() noexcept;
    void                  enableUTF8() noexcept;
    void                  disableUTF8() noexcept;
    void                  enableMouseSequences() noexcept;
    void                  disableMouseSequences() noexcept;
    void                  setPressCommand (const FKeyboardCommand&);
    void                  setReleaseCommand (const FKeyboardCommand&);
    void                  setEscPressedCommand (const FKeyboardCommand&);
    void                  setMouseTrackingCommand (const FKeyboardCommand&);

    // Inquiry
    bool                  hasPendingInput() const noexcept;
    bool                  hasDataInQueue() const;

    // Methods
    bool&                 hasUnprocessedInput() & noexcept;
    bool                  isKeyPressed (uInt64 = read_blocking_time);
    void                  clearKeyBuffer() noexcept;
    void                  clearKeyBufferOnTimeout();
    void                  fetchKeyCode();
    void                  escapeKeyHandling();
    void                  processQueuedInput();

  private:
    // Using-declaration
    using FKeyMapPtr = std::shared_ptr<FKeyMap::KeyCapMapType>;
    using KeyMapEnd = FKeyMap::KeyCapMapType::const_iterator;

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
    FKey                  UTF8decode (const std::size_t) const noexcept;
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

    static TimeValue      time_keypressed;
    static uInt64         read_blocking_time;
    static uInt64         read_blocking_time_short;
    static uInt64         key_timeout;
    static bool           non_blocking_input_support;
    FKeyMapPtr            key_cap_ptr{};
    static KeyMapEnd      key_cap_end;
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
inline FKey FKeyboard::getKey() const noexcept
{ return key; }

//----------------------------------------------------------------------
inline FKeyboard::keybuffer& FKeyboard::getKeyBuffer() & noexcept
{ return fifo_buf; }

//----------------------------------------------------------------------
inline TimeValue FKeyboard::getKeyPressedTime() const noexcept
{ return time_keypressed; }

//----------------------------------------------------------------------
inline uInt64 FKeyboard::getKeypressTimeout() noexcept
{ return key_timeout; }

//----------------------------------------------------------------------
inline uInt64 FKeyboard::getReadBlockingTime() noexcept
{ return read_blocking_time; }

//----------------------------------------------------------------------
template <typename T>
inline void FKeyboard::setTermcapMap (const T& keymap)
{
  key_cap_ptr = std::make_shared<T>(keymap);
  key_cap_end = key_cap_ptr->cend();
}

//----------------------------------------------------------------------
inline void FKeyboard::setTermcapMap ()
{
  using type = FKeyMap::KeyCapMapType;
  key_cap_ptr = std::make_shared<type>(FKeyMap::getKeyCapMap());
  // Search for the first entry with a string length of 0 at the end
  key_cap_end = std::find_if ( key_cap_ptr->cbegin()
                             , key_cap_ptr->cend()
                             , [] (const FKeyMap::KeyCapMap& entry)
                               { return entry.length == 0; }
                             );
}

//----------------------------------------------------------------------
inline void FKeyboard::setKeypressTimeout (const uInt64 timeout) noexcept
{ key_timeout = timeout; }

//----------------------------------------------------------------------
inline void FKeyboard::setReadBlockingTime (const uInt64 blocking_time) noexcept
{ read_blocking_time = blocking_time; }

//----------------------------------------------------------------------
inline void FKeyboard::setNonBlockingInputSupport (bool enable) noexcept
{ non_blocking_input_support = enable; }

//----------------------------------------------------------------------
inline bool FKeyboard::unsetNonBlockingInput() noexcept
{ return setNonBlockingInput(false); }

//----------------------------------------------------------------------
inline bool FKeyboard::hasPendingInput() const noexcept
{ return has_pending_input; }

//----------------------------------------------------------------------
inline bool FKeyboard::hasDataInQueue() const
{ return ! fkey_queue.empty(); }

//----------------------------------------------------------------------
inline void FKeyboard::enableUTF8() noexcept
{ utf8_input = true; }

//----------------------------------------------------------------------
inline void FKeyboard::disableUTF8() noexcept
{ utf8_input = false; }

//----------------------------------------------------------------------
inline void FKeyboard::enableMouseSequences() noexcept
{ mouse_support = true; }

//----------------------------------------------------------------------
inline void FKeyboard::disableMouseSequences() noexcept
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
