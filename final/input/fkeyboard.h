/***********************************************************************
* fkeyboard.h - Read keyboard events                                   *
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
#include <string>
#include <utility>

#include "final/ftypes.h"
#include "final/input/fkey_hashmap.h"
#include "final/input/fkey_map.h"
#include "final/util/char_ringbuffer.h"
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
    inline void execute() const
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
    using keybuffer = CharRingBuffer<FIFO_BUF_SIZE>;

    // Constructor
    FKeyboard();

    // Disable copy constructor
    FKeyboard (const FKeyboard&) = delete;

    // Disable move constructor
    FKeyboard (FKeyboard&&) noexcept = delete;

    // Disable copy assignment operator (=)
    auto operator = (const FKeyboard&) -> FKeyboard& = delete;

    // Disable move assignment operator (=)
    auto operator = (FKeyboard&&) noexcept -> FKeyboard& = delete;

    // Accessors
    auto  getClassName() const -> FString;
    static auto getInstance() -> FKeyboard&;
    auto  getKey() const noexcept -> FKey;
    auto  getKeyName (const FKey) const -> FString;
    auto  getKeyBuffer() & noexcept -> keybuffer&;
    auto  getKeyPressedTime() const noexcept -> TimeValue;
    static auto  getKeypressTimeout() noexcept -> uInt64;
    static auto  getReadBlockingTime() noexcept -> uInt64;

    // Mutators
    template <typename T>
    void  setTermcapMap (const T&);
    void  setTermcapMap();
    static void  setKeypressTimeout (const uInt64) noexcept;
    static void  setReadBlockingTime (const uInt64) noexcept;
    static void  setNonBlockingInputSupport (bool = true) noexcept;
    void  setNonBlockingInput (bool = true);
    void  unsetNonBlockingInput() noexcept;
    void  enableUTF8() noexcept;
    void  disableUTF8() noexcept;
    void  enableMouseSequences() noexcept;
    void  disableMouseSequences() noexcept;
    void  setPressCommand (const FKeyboardCommand&);
    void  setReleaseCommand (const FKeyboardCommand&);
    void  setEscPressedCommand (const FKeyboardCommand&);
    void  setMouseTrackingCommand (const FKeyboardCommand&);

    // Inquiry
    auto  hasPendingInput() const noexcept -> bool;
    auto  hasDataInQueue() const -> bool;

    // Methods
    auto  hasUnprocessedInput() const noexcept -> bool;
    auto  isKeyPressed (uInt64 = read_blocking_time) -> bool;
    void  clearKeyBuffer() noexcept;
    void  clearKeyBufferOnTimeout();
    void  fetchKeyCode();
    void  escapeKeyHandling();
    void  processQueuedInput();

  private:
    // Constants
    static constexpr FKey NOT_SET = static_cast<FKey>(-2);
    static constexpr std::size_t MAX_QUEUE_SIZE = 32;

    // Using-declaration
    using FKeyMapPtr = std::shared_ptr<FKeyMap::KeyCapMapType>;
    using KeyMapEnd = FKeyMap::KeyCapMapType::const_iterator;
    using KeyQueue = FRingBuffer<FKey, MAX_QUEUE_SIZE>;

    // Accessors
    auto  getMouseProtocolKey() const -> FKey;
    auto  getTermcapKey() -> FKey;
    auto  getKnownKey() -> FKey;
    auto  getSingleKey() -> FKey;

    // Inquiry
    static auto isKeypressTimeout() -> bool;
    static auto isIntervalTimeout() -> bool;

    // Methods
    auto  UTF8decode (const std::size_t) const noexcept -> FKey;
    auto  readKey() -> ssize_t;
    void  parseKeyBuffer();
    auto  parseKeyString() -> FKey;
    auto  keyCorrection (const FKey&) const -> FKey;
    void  substringKeyHandling();
    void  keyPressedCommand() const;
    void  keyReleasedCommand() const;
    void  escapeKeyPressedCommand() const;
    void  mouseTrackingCommand() const;

    // Data members
    FKeyboardCommand  keypressed_cmd{};
    FKeyboardCommand  keyreleased_cmd{};
    FKeyboardCommand  escape_key_cmd{};
    FKeyboardCommand  mouse_tracking_cmd{};

    static TimeValue  time_keypressed;
    static uInt64     read_blocking_time;
    static uInt64     read_blocking_time_short;
    static uInt64     key_timeout;
    static bool       non_blocking_input_support;
    FKeyMapPtr        key_cap_ptr{};
    KeyMapEnd         key_cap_end{};
    keybuffer         fifo_buf{};
    KeyQueue          fkey_queue{};
    FKey              fkey{FKey::None};
    FKey              key{FKey::None};
    int               stdin_status_flags{0};
    char              read_character{};
    bool              has_pending_input{false};
    bool              fifo_in_use{false};
    bool              utf8_input{false};
    bool              mouse_support{true};
    bool              non_blocking_stdin{false};
};

// FKeyboard inline functions
//----------------------------------------------------------------------
inline auto FKeyboard::getClassName() const -> FString
{ return "FKeyboard"; }

//----------------------------------------------------------------------
inline auto FKeyboard::getKey() const noexcept -> FKey
{ return key; }

//----------------------------------------------------------------------
inline auto FKeyboard::getKeyBuffer() & noexcept -> keybuffer&
{ return fifo_buf; }

//----------------------------------------------------------------------
inline auto FKeyboard::getKeyPressedTime() const noexcept -> TimeValue
{ return time_keypressed; }

//----------------------------------------------------------------------
inline auto FKeyboard::getKeypressTimeout() noexcept -> uInt64
{ return key_timeout; }

//----------------------------------------------------------------------
inline auto FKeyboard::getReadBlockingTime() noexcept -> uInt64
{ return read_blocking_time; }

//----------------------------------------------------------------------
template <typename T>
inline void FKeyboard::setTermcapMap (const T& keymap)
{
  key_cap_ptr = std::make_shared<T>(keymap);
  key_cap_end = key_cap_ptr->cend();
  fkeyhashmap::setKeyCapMap<keybuffer>(key_cap_ptr->cbegin(), key_cap_end);
}

//----------------------------------------------------------------------
inline void FKeyboard::setTermcapMap()
{
  using type = FKeyMap::KeyCapMapType;
  key_cap_ptr = std::make_shared<type>(FKeyMap::getKeyCapMap());
  // Search for the first entry with a string length of 0 at the end
  key_cap_end = std::find_if ( key_cap_ptr->cbegin()
                             , key_cap_ptr->cend()
                             , [] (const FKeyMap::KeyCapMap& entry)
                               { return entry.length == 0; }
                             );
  fkeyhashmap::setKeyCapMap<keybuffer>(key_cap_ptr->cbegin(), key_cap_end);
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
inline void FKeyboard::unsetNonBlockingInput() noexcept
{ setNonBlockingInput(false); }

//----------------------------------------------------------------------
inline auto FKeyboard::hasPendingInput() const noexcept -> bool
{ return has_pending_input; }

//----------------------------------------------------------------------
inline auto FKeyboard::hasDataInQueue() const -> bool
{ return ! fkey_queue.isEmpty(); }

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
