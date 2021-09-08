/***********************************************************************
* ftermoutput.h - Implements the terminal output                       *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021 Markus Gans                                           *
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

#include "final/foutput.h"
#include "final/fterm.h"

namespace finalcut
{

// class forward declaration
class FStartOptions;

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
    ~FTermOutput() noexcept;

    // Accessors
    FString        getClassName() const override;
    FTerm&         getFTerm();
    std::size_t    getColumnNumber() const override;
    std::size_t    getLineNumber() const override;
    int            getTabstop() const override;
    int            getMaxColor() const override;
    Encoding       getEncoding() const override;
    FString        getKeyName (FKey) const override;

    // Mutators
    void           setCursor (FPoint) override;
    void           setCursor (CursorMode) override;
    void           hideCursor (bool = true) override;
    void           showCursor() override;
    void           setTerminalSize (FSize) override;
    bool           setVGAFont() override;
    bool           setNewFont() override;
    void           setNonBlockingRead (bool = true) override;

    // Inquiries
    bool           isCursorHideable() const override;
    bool           isMonochron() const override;
    bool           isNewFont() const override;
    bool           isEncodable (wchar_t) const override;
    bool           hasTerminalResized() const override;
    bool           allowsTerminalSizeManipulation() const override;
    bool           canChangeColorPalette() const override;
    bool           hasHalfBlockCharacter() const override;
    bool           hasShadowCharacter() const override;
    bool           areMetaAndArrowKeysSupported() const override;

    // Methods
    void           initTerminal (FVTerm::FTermArea*) override;
    void           finishTerminal() override;
    bool           updateTerminal() override;
    void           detectTerminalSize() override;
    void           commitTerminalResize() override;
    void           initScreenSettings() override;
    bool           scrollTerminalForward() override;
    bool           scrollTerminalReverse() override;
    bool           clearTerm (wchar_t = L' ') override;
    void           flush() override;
    void           beep() const override;

  private:
    // Constants
    struct FTermControl
    {
      std::string string;
    };

    struct FTermChar
    {
      wchar_t ch;
    };

    struct FTermString
    {
      std::wstring string;
    };

    // Enumerations
    enum class PrintState
    {
      NothingPrinted,
      RepeatCharacterPrinted,
      WhitespacesPrinted,
      LineCompletelyPrinted
    };

    enum class OutputType : uInt8  // Output data type of the terminal
    {
      String,
      Control
    };

    struct TermString
    {
      explicit TermString (const std::wstring& wstr)
        : wstring{wstr}
      { }

      explicit TermString (const std::string& str)
        : string{str}
      { }

      std::wstring wstring{};
      std::string string{};
    };

    // Using-declaration
    using OutputData = std::tuple<OutputType, TermString>;
    using OutputBuffer = std::queue<OutputData>;

    // Constants
    //   Buffer limit for character output on the terminal
    static constexpr std::size_t TERMINAL_OUTPUT_BUFFER_LIMIT = 1024;
    //   Upper and lower flush limit
    static constexpr uInt64 MIN_FLUSH_WAIT = 16667;   //   16.6 ms = 60 Hz
    static constexpr uInt64 MAX_FLUSH_WAIT = 200000;  //  200.0 ms = 5 Hz

    // Accessors
    const FSetPalette& getFSetPaletteRef() const override;

    // Methods
    FStartOptions& getStartOptions();
    bool           isInputCursorInsideTerminal();
    bool           isDefaultPaletteTheme() override;
    void           redefineColorPalette() override;
    void           restoreColorPalette() override;
    void           init_characterLengths();
    void           init_combined_character();
    bool           canClearToEOL (uInt, uInt) const;
    bool           canClearLeadingWS (uInt&, uInt) const;
    bool           canClearTrailingWS (uInt&, uInt) const;
    bool           skipUnchangedCharacters (uInt&, uInt, uInt);
    void           printRange (uInt, uInt, uInt, bool);
    void           replaceNonPrintableFullwidth (uInt, FChar&) const;
    void           printCharacter (uInt&, uInt, bool, FChar&);
    void           printFullWidthCharacter (uInt&, uInt, FChar&);
    void           printFullWidthPaddingCharacter (uInt&, uInt, FChar&);
    void           printHalfCovertFullWidthCharacter (uInt&, uInt, FChar&);
    void           skipPaddingCharacter (uInt&, uInt, const FChar&);
    PrintState     eraseCharacters (uInt&, uInt, uInt, bool);
    PrintState     repeatCharacter (uInt&, uInt, uInt);
    bool           isFullWidthChar (const FChar&) const;
    bool           isFullWidthPaddingChar (const FChar&) const;
    void           cursorWrap() const;
    bool           updateTerminalLine (uInt);
    bool           updateTerminalCursor();
    void           flushTimeAdjustment();
    bool           isFlushTimeout() const;
    void           markAsPrinted (uInt, uInt);
    void           markAsPrinted (uInt, uInt, uInt);
    void           newFontChanges (FChar&) const;
    void           charsetChanges (FChar&) const;
    void           appendCharacter (FChar&);
    void           appendChar (FChar&);
    void           appendAttributes (FChar&);
    void           appendLowerRight (FChar&);
    void           characterFilter (FChar&);
    bool           isOutputBufferLimitReached() const;
    void           appendOutputBuffer (const FTermControl&);
    void           appendOutputBuffer (const FTermChar&);
    void           appendOutputBuffer (const FTermString&);

    // Data members
    FTerm                         fterm{};
    static FVTerm::FTermArea*     vterm;
    std::shared_ptr<OutputBuffer> output_buffer{};
    std::shared_ptr<FPoint>       term_pos{};  // terminal cursor position
    TimeValue                     time_last_flush{};
    FChar                         term_attribute{};
    bool                          cursor_hideable{false};
    bool                          combined_char_support{false};
    uInt                          erase_char_length{};
    uInt                          repeat_char_length{};
    uInt                          clr_bol_length{};
    uInt                          clr_eol_length{};
    uInt                          cursor_address_length{};
    uInt64                        flush_wait{MIN_FLUSH_WAIT};
    uInt64                        flush_average{MIN_FLUSH_WAIT};
    uInt64                        flush_median{MIN_FLUSH_WAIT};
};

// FTermOutput inline functions
//----------------------------------------------------------------------
inline FString FTermOutput::getClassName() const
{ return "FTermOutput"; }

//----------------------------------------------------------------------
inline FTerm& FTermOutput::getFTerm()
{ return fterm; }

//----------------------------------------------------------------------
inline void FTermOutput::showCursor()
{ return hideCursor(false); }

//----------------------------------------------------------------------
inline bool FTermOutput::isCursorHideable() const
{ return cursor_hideable; }

//----------------------------------------------------------------------
inline const FTermOutput::FSetPalette& FTermOutput::getFSetPaletteRef() const
{
  static const FSetPalette& f = &FTerm::setPalette;
  return f;
}

}  // namespace finalcut

#endif  // FTERMOUTPUT_H

