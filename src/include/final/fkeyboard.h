/***********************************************************************
* fkeyboard.h - Read keyboard events                                   *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018-2019 Markus Gans                                      *
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
 * ▕ FKeyboard ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FKEYBOARD_H
#define FKEYBOARD_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include "final/fobject.h"
#include "final/ftypes.h"

#if defined(__linux__)
  #include "final/ftermlinux.h"
#endif

namespace finalcut
{

// class forward declaration
class FApplication;


//----------------------------------------------------------------------
// class FKeyboardCommand
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FKeyboardCommand final
{
  public:
    // Constructor
    explicit FKeyboardCommand ( FApplication* = nullptr
                              , void(FApplication::*)() = nullptr);

    // Method
    void execute();

  private:
    // Data Members
    FApplication* instance{nullptr};
    void (FApplication::*handler)(){nullptr};
};
#pragma pack(pop)


//----------------------------------------------------------------------
// class FKeyboard
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FKeyboard final
{
  public:
    // Constants
    static constexpr std::size_t FIFO_BUF_SIZE{512};

    // Typedef
    typedef char keybuffer[FIFO_BUF_SIZE];

    // Constructor
    FKeyboard();

    // Disable copy constructor
    FKeyboard (const FKeyboard&) = delete;

    // Destructor
    virtual ~FKeyboard();

    // Disable assignment operator (=)
    FKeyboard& operator = (const FKeyboard&) = delete;

    // Accessors
    virtual const char* getClassName() const;
    FKey                getKey();
    const FString       getKeyName (FKey);
    keybuffer&          getKeyBuffer();
    timeval*            getKeyPressedTime();

    // Mutators
    void                setTermcapMap (fc::fkeymap*);
    void                setKeypressTimeout (const uInt64);
    void                enableUTF8();
    void                disableUTF8();
    void                enableMouseSequences();
    void                disableMouseSequences();
    void                setPressCommand (FKeyboardCommand);
    void                setReleaseCommand (FKeyboardCommand);
    void                setEscPressedCommand (FKeyboardCommand);

    // Inquiry
    bool                isInputDataPending();

    // Methods
    static void         init();
    bool&               unprocessedInput();
    bool                isKeyPressed();
    void                clearKeyBuffer();
    void                clearKeyBufferOnTimeout();
    void                fetchKeyCode();
    void                escapeKeyHandling();

  private:
    // Constants
    static constexpr std::size_t READ_BUF_SIZE{1024};
    static constexpr FKey NOT_SET = static_cast<FKey>(-1);

    // Accessors
    FKey                getMouseProtocolKey();
    FKey                getTermcapKey();
    FKey                getMetaKey();
    FKey                getSingleKey();

    // Mutators
    bool                setNonBlockingInput (bool);
    bool                setNonBlockingInput();
    bool                unsetNonBlockingInput();

    // Inquiry
    static bool         isKeypressTimeout();

    // Methods
    FKey                UTF8decode (const char[]);
    ssize_t             readKey();
    void                parseKeyBuffer();
    FKey                parseKeyString();
    FKey                keyCorrection (const FKey&);
    void                substringKeyHandling();
    void                keyPressed();
    void                keyReleased();
    void                escapeKeyPressed();

    // Data Members
    FKey                key{0};
    char                read_buf[READ_BUF_SIZE]{'\0'};
    char                fifo_buf[FIFO_BUF_SIZE]{'\0'};
    int                 fifo_offset{0};
    bool                fifo_in_use{false};
    int                 stdin_status_flags{0};
    static uInt64       key_timeout;
    bool                input_data_pending{false};
    bool                utf8_input{false};
    bool                mouse_support{true};
    bool                non_blocking_stdin{false};
    FKeyboardCommand    keypressed_cmd{};
    FKeyboardCommand    keyreleased_cmd{};
    FKeyboardCommand    escape_key_cmd{};

    static timeval      time_keypressed;
    fc::fkeymap*        key_map{nullptr};

#if defined(__linux__)
    #undef linux
    static FTermLinux*  linux;
#endif
};
#pragma pack(pop)

// FKeyboard inline functions
//----------------------------------------------------------------------
inline const char* FKeyboard::getClassName() const
{ return "FKeyboard"; }

//----------------------------------------------------------------------
inline FKey FKeyboard::getKey()
{ return key; }

//----------------------------------------------------------------------
inline FKeyboard::keybuffer& FKeyboard::getKeyBuffer()
{ return fifo_buf; }

//----------------------------------------------------------------------
inline timeval* FKeyboard::getKeyPressedTime()
{ return &time_keypressed; }

//----------------------------------------------------------------------
inline void FKeyboard::setKeypressTimeout (const uInt64 timeout)
{ key_timeout = timeout; }

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
inline void FKeyboard::setPressCommand (FKeyboardCommand cmd)
{ keypressed_cmd = cmd; }

//----------------------------------------------------------------------
inline void FKeyboard::setReleaseCommand (FKeyboardCommand cmd)
{ keyreleased_cmd = cmd; }

//----------------------------------------------------------------------
inline void FKeyboard::setEscPressedCommand (FKeyboardCommand cmd)
{ escape_key_cmd = cmd; }

//----------------------------------------------------------------------
inline bool FKeyboard::isInputDataPending()
{ return input_data_pending; }
//----------------------------------------------------------------------
inline bool FKeyboard::setNonBlockingInput()
{ return setNonBlockingInput(true); }

//----------------------------------------------------------------------
inline bool FKeyboard::unsetNonBlockingInput()
{ return setNonBlockingInput(false); }

}  // namespace finalcut

#endif  // FKEYBOARD_H
