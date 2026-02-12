/***********************************************************************
* ftermoutput.h - Implements the terminal output                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2026 Markus Gans                                      *
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
 *   ▕▔▔▔▔▔▔▔▔▔▏
 *   ▕ FOutput ▏
 *   ▕▁▁▁▁▁▁▁▁▁▏
 *        ▲
 *        │
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ FTermOutput ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef FTERMOUTPUT_H
#define FTERMOUTPUT_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include "final/output/foutput.h"
#include "final/output/tty/fterm.h"
#include "final/util/char_ringbuffer.h"

namespace finalcut
{

// class forward declaration
class FStartOptions;
class FTermData;
template <typename T, std::size_t Capacity>
class FRingBuffer;


//----------------------------------------------------------------------
// struct FOutputBuffer
//----------------------------------------------------------------------

struct FOutputBuffer
{
  // Constant - output buffer size
  static constexpr std::size_t BUFFER_SIZE = 32'768;   // 32 KB

  // Enumerations
  enum class OutputType : uInt8  // Output data type of the terminal
  {
    String,
    Control
  };

  struct TypeSlice
  {
    OutputType type{OutputType::String};
    uInt32 length{0};

    TypeSlice() = default;

    TypeSlice (OutputType t, uInt32 len)
      : type{t}
      , length{len}
    { }
  };

  // Using-declaration
  using TypeSliceBuffer = FRingBuffer<TypeSlice, BUFFER_SIZE>;

  FOutputBuffer() = default;

  FOutputBuffer (OutputType t, std::string str, uInt32 s)
    : data{std::move(str)}
  {
    slices.push({t, s});
  }

  inline auto isEmpty() const noexcept -> bool
  {
    return slices.isEmpty();
  }

  TypeSliceBuffer slices{};
  std::string data{};
};


//----------------------------------------------------------------------
// class FTermOutput
//----------------------------------------------------------------------

class FTermOutput final : public FOutput
{
  public:
    // Constructor
    FTermOutput() = default;

    explicit FTermOutput (const FVTerm&);

    // Destructor
    ~FTermOutput() noexcept override;

    // Accessors
    auto getClassName() const -> FString override;
    auto getFTerm() noexcept -> FTerm&;
    auto getColumnNumber() const -> std::size_t override;
    auto getLineNumber() const -> std::size_t override;
    auto getTabstop() const -> int override;
    auto getMaxColor() const -> int override;
    auto getEncoding() const -> Encoding override;
    auto getKeyName (FKey) const -> FString override;

    // Mutators
    void setCursor (FPoint) override;
    void setCursor (CursorMode) override;
    void hideCursor (bool = true) override;
    void showCursor() noexcept override;
    void setTerminalSize (FSize) override;
    auto setVGAFont() -> bool override;
    auto setNewFont() -> bool override;
    void setNonBlockingRead (bool = true) override;

    // Inquiries
    auto isCursorHideable() const noexcept -> bool override;
    auto isMonochron() const -> bool override;
    auto isNewFont() const -> bool override;
    auto isEncodable (const wchar_t&) const -> bool override;
    auto isFlushTimeout() const noexcept -> bool override;
    auto hasTerminalResized() const -> bool override;
    auto allowsTerminalSizeManipulation() const -> bool override;
    auto canChangeColorPalette() const -> bool override;
    auto hasHalfBlockCharacter() const -> bool override;
    auto hasShadowCharacter() const -> bool override;
    auto areMetaAndArrowKeysSupported() const -> bool override;

    // Methods
    void initTerminal (FVTerm::FTermArea*) override;
    void finishTerminal() override;
    auto updateTerminal() -> bool override;
    void detectTerminalSize() override;
    void commitTerminalResize() override;
    void initScreenSettings() override;
    auto scrollTerminalForward() -> bool override;
    auto scrollTerminalReverse() -> bool override;
    void clearTerminalAttributes() override;
    void clearTerminalState() override;
    auto clearTerminal (wchar_t = L' ') -> bool override;
    void flush() override;
    void beep() const override;

  private:
    // Constants
    struct FTermControl
    {
      std::string string;
    };

    // Enumerations
    enum class PrintState : uInt8
    {
      NothingPrinted,
      RepeatCharacterPrinted,
      WhitespacesPrinted,
      LineCompletelyPrinted
    };

    enum class Repetition : uInt8
    {
      ASCII,
      UTF8,
      NotOptimized
    };

    enum class CursorMoved : bool { No, Yes };

    // Constants
    //   Upper and lower flush limit
    static constexpr uInt64 MIN_FLUSH_WAIT   = 16'667;   //  16.6 ms = 60 Hz
    static constexpr uInt64 MAX_FLUSH_WAIT   = 200'000;  // 200.0 ms = 5 Hz
    static constexpr uInt64 RESET_THRESHOLD  = 400'000;  // 400.0 ms = 2.5 Hz

    // Using-declaration
    using clock = std::chrono::steady_clock;
    using FChar_iterator = FVTerm::FTermArea::FCharVec::iterator;
    using FChar_const_iterator = FVTerm::FTermArea::FCharVec::const_iterator;

    // Accessors
    auto getFSetPaletteRef() const & -> const FSetPalette& override;

    // Methods
    auto getStartOptions() & -> FStartOptions&;
    auto isInputCursorInsideTerminal() const noexcept -> bool;
    auto isDefaultPaletteTheme() const -> bool override;
    void redefineColorPalette() override;
    void restoreColorPalette() override;
    void init_characterLengths();
    void init_combined_character();
    auto canClearToEOL (uInt, uInt) const -> bool;
    auto canClearLeadingWS (uInt&, uInt) const -> bool;
    auto canClearTrailingWS (uInt&, uInt) const -> bool;
    auto skipUnchangedCharacters (uInt&, uInt, uInt, FChar_iterator) -> bool;
    void printRange (uInt, uInt, uInt);
    void replaceNonPrintableFullwidth (uInt, uInt, FChar&) const noexcept;
    void printCharacter (uInt&, uInt, bool, FChar_iterator&);
    void printFullWidthCharacter (uInt&, uInt, const FChar_iterator&);
    void printFullWidthPaddingCharacter (uInt&, uInt, const FChar_iterator&);
    void printHalfCovertFullWidthCharacter (uInt, uInt, const FChar_iterator&);
    void printEllipsis (uInt, uInt, FChar&);
    void skipPaddingCharacter (uInt&, uInt, const FChar&) const noexcept;
    auto eraseCharacters (uInt&, uInt, uInt, FChar_iterator&) -> PrintState;
    auto repeatCharacter (uInt&, uInt, uInt, FChar_iterator&) -> PrintState;
    auto countRepetitions (FChar_const_iterator, uInt, uInt) const noexcept -> uInt;
    auto canUseEraseCharacters (const FChar&, uInt) const noexcept -> bool;
    auto canUseCharacterRepetitions (const FChar&, uInt) const noexcept -> bool;
    auto getRepetitionType (const FChar&, uInt) const noexcept -> Repetition;
    auto isFullWidthChar (const FChar&) const noexcept -> bool;
    auto isFullWidthPaddingChar (const FChar&) const noexcept -> bool;
    void cursorWrap() const noexcept;
    void adjustCursorPosition (FPoint&) const noexcept;
    auto updateTerminalLine (uInt) -> bool;
    auto updateTerminalCursor() -> bool;
    void flushTimeAdjustment() noexcept;
    void markAsPrinted (uInt, uInt) const noexcept;
    void markAsPrinted (uInt, uInt, uInt) const noexcept;
    void newFontChanges (FChar&) const;
    void charsetChanges (FChar&) const;
    void appendCharacter (const FChar_iterator&);
    void appendCharacter_n (const FChar_iterator&, uInt);
    void appendChar (FChar&);
    void appendAttributes (FChar&);
    void appendLowerRight (const FChar_iterator&);
    void characterFilter (FChar&);
    auto moveCursorLeft() -> CursorMoved;
    void checkFreeBufferSize();
    void appendOutputBuffer (const FTermControl&);
    void appendOutputBuffer (const UniChar&);
    void appendOutputBuffer (const UTF8_Char&);
    void appendOutputBuffer (FOutputBuffer::OutputType, const char*, uInt32);

    // Data members
    FTerm                          fterm{};
    static FVTerm::FTermArea*      vterm;
    static FTermData*              fterm_data;
    std::shared_ptr<FOutputBuffer> output_buffer{};
    std::shared_ptr<FPoint>        term_pos{};  // terminal cursor position
    FChar                          term_attribute{};
    bool                           cursor_hideable{false};
    bool                           combined_char_support{false};
    uInt                           erase_char_length{};
    uInt                           repeat_char_length{};
    uInt                           clr_bol_length{};
    uInt                           clr_eol_length{};
    uInt                           cursor_address_length{};
    uInt64                         time_last_flush_us{};
    uInt64                         flush_wait{MIN_FLUSH_WAIT};
    uInt64                         flush_average{MIN_FLUSH_WAIT};
    uInt64                         flush_median{MIN_FLUSH_WAIT};
};

// FTermOutput inline functions
//----------------------------------------------------------------------
inline auto FTermOutput::getClassName() const -> FString
{ return "FTermOutput"; }

//----------------------------------------------------------------------
inline auto FTermOutput::getFTerm() noexcept -> FTerm&
{ return fterm; }

//----------------------------------------------------------------------
inline void FTermOutput::showCursor() noexcept
{ return hideCursor(false); }

//----------------------------------------------------------------------
inline auto FTermOutput::isCursorHideable() const noexcept -> bool
{ return cursor_hideable; }

//----------------------------------------------------------------------
inline auto FTermOutput::getFSetPaletteRef() const & -> const FSetPalette&
{
  static const FSetPalette& f = &FTerm::setPalette;
  return f;
}

}  // namespace finalcut

#endif  // FTERMOUTPUT_H

