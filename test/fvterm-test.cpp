/***********************************************************************
* fvterm-test.cpp - FVTerm unit tests                                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2025 Markus Gans                                      *
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

#include <queue>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>
#include <fvterm_check.h>

#undef F_PREPROC_HANDLER

#define F_PREPROC_HANDLER(i,h) \
    reinterpret_cast<finalcut::FVTerm*>((i)), \
    std::bind ( reinterpret_cast<finalcut::FVTerm::FPreprocessingHandler>((h)) \
              , reinterpret_cast<finalcut::FVTerm*>((i)) )

//----------------------------------------------------------------------
auto getBellState() -> bool&
{
  static bool bell = false;
  return bell;
}

//----------------------------------------------------------------------
void setBellState (bool state = true)
{
  getBellState() = state;
}


//----------------------------------------------------------------------
// class FTermOutputTest
//----------------------------------------------------------------------
class FTermOutputTest : public finalcut::FOutput
{
  public:
    // Constructor
    FTermOutputTest() = default;
    explicit FTermOutputTest (const finalcut::FVTerm&);

    // Destructor
    ~FTermOutputTest() noexcept override;

    // Accessors
    auto getClassName() const -> finalcut::FString override;
    auto getFTerm() & -> finalcut::FTerm&;
    auto getColumnNumber() const -> std::size_t override;
    auto getLineNumber() const -> std::size_t override;
    auto getTabstop() const -> int override;
    auto getMaxColor() const -> int override;
    auto getEncoding() const -> finalcut::Encoding override;
    auto getKeyName (finalcut::FKey) const -> finalcut::FString override;

    // Mutators
    void setCursor (finalcut::FPoint) override;
    void setCursor (finalcut::CursorMode) override;
    void hideCursor (bool = true) override;
    void showCursor() override;
    void setTerminalSize (finalcut::FSize) override;
    auto setVGAFont() -> bool override;
    auto setNewFont() -> bool override;
    void setNonBlockingRead (bool = true) override;
    static void setNoForce (bool = true);

    // Inquiries
    auto isCursorHideable() const -> bool override;
    auto isMonochron() const -> bool override;
    auto isNewFont() const -> bool override;
    auto isEncodable (const wchar_t&) const -> bool override;
    auto isFlushTimeout() const -> bool override;
    auto hasTerminalResized() const -> bool override;
    auto allowsTerminalSizeManipulation() const -> bool override;
    auto canChangeColorPalette() const -> bool override;
    auto hasHalfBlockCharacter() const -> bool override;
    auto hasShadowCharacter() const -> bool override;
    auto areMetaAndArrowKeysSupported() const -> bool override;

    // Methods
    void initTerminal (finalcut::FVTerm::FTermArea*) override;
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
    // Using-declaration
    using OutputBuffer = std::queue<char>;
    using FSetPalette  = std::function<void(finalcut::FColor, int, int, int)>;

    // Accessors
    auto getFSetPaletteRef() const & -> const FSetPalette& override;

    // Methods
    auto isDefaultPaletteTheme() const -> bool override;
    void redefineColorPalette() override;
    void restoreColorPalette() override;

    // Data member
    bool                                 bell{false};
    static bool                          no_force;
    finalcut::FTerm                      fterm{};
    static finalcut::FVTerm::FTermArea*  vterm;
    static finalcut::FTermData*          fterm_data;
    std::shared_ptr<OutputBuffer>        output_buffer{};
    std::shared_ptr<finalcut::FPoint>    term_pos{};
    finalcut::FChar                      term_attribute{};
    const FSetPalette                    set_palette_function{&finalcut::FTerm::setPalette};
};

// static class attributes
bool                         FTermOutputTest::no_force{false};
finalcut::FVTerm::FTermArea* FTermOutputTest::vterm{nullptr};
finalcut::FTermData*         FTermOutputTest::fterm_data{nullptr};


// constructors and destructor
//----------------------------------------------------------------------
FTermOutputTest::FTermOutputTest (const finalcut::FVTerm& t)  // constructor
  : finalcut::FOutput{t}
{ }

//----------------------------------------------------------------------
FTermOutputTest::~FTermOutputTest() noexcept = default;  // destructor


// public methods of FTermOutputTest
//----------------------------------------------------------------------
inline auto FTermOutputTest::getClassName() const -> finalcut::FString
{
  return "FTermOutputTest";
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::getColumnNumber() const -> std::size_t
{
  return 80;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::getLineNumber() const -> std::size_t
{
  return 24;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::getTabstop() const -> int
{
  return 8;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::getMaxColor() const -> int
{
  return 256;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::getEncoding() const -> finalcut::Encoding
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  return fterm_data.getTerminalEncoding();
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::getKeyName (finalcut::FKey keynum) const -> finalcut::FString
{
  static const auto& keyboard = finalcut::FKeyboard::getInstance();
  return keyboard.getKeyName (keynum);
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::isCursorHideable() const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::isMonochron() const -> bool
{
  return false;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::isNewFont() const -> bool
{
  return false;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::isEncodable (const wchar_t&) const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::isFlushTimeout() const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::hasTerminalResized() const -> bool
{
  return false;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::allowsTerminalSizeManipulation() const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::canChangeColorPalette() const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::hasHalfBlockCharacter() const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::hasShadowCharacter() const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::areMetaAndArrowKeysSupported() const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::setCursor (finalcut::FPoint)
{ }

//----------------------------------------------------------------------
inline void FTermOutputTest::setCursor (finalcut::CursorMode)
{ }

//----------------------------------------------------------------------
inline void FTermOutputTest::hideCursor (bool)
{ }

//----------------------------------------------------------------------
inline void FTermOutputTest::showCursor()
{
  return hideCursor(false);
}

//----------------------------------------------------------------------
inline void FTermOutputTest::setTerminalSize (finalcut::FSize)
{ }

//----------------------------------------------------------------------
inline auto FTermOutputTest::setVGAFont() -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::setNewFont() -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::setNonBlockingRead (bool enable)
{
  uInt64 blocking_time = enable ? 5000 : 100'000;  // 5 or 100 ms
  finalcut::FKeyboard::setReadBlockingTime (blocking_time);
}

//----------------------------------------------------------------------
inline void FTermOutputTest::setNoForce (bool state)
{
  no_force = state;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::initTerminal (finalcut::FVTerm::FTermArea* virtual_terminal)
{
  vterm         = virtual_terminal;
  output_buffer = std::make_shared<OutputBuffer>();
  term_pos      = std::make_shared<finalcut::FPoint>(-1, -1);

  hideCursor();

  // term_attribute stores the current state of the terminal
  term_attribute.ch = {{ L'\0' }};
  term_attribute.color.pair.fg = finalcut::FColor::Default;
  term_attribute.color.pair.bg = finalcut::FColor::Default;
  term_attribute.attr.data = 0;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::finishTerminal()
{ }

//----------------------------------------------------------------------
inline auto FTermOutputTest::updateTerminal() -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::detectTerminalSize()
{ }

//----------------------------------------------------------------------
inline void FTermOutputTest::commitTerminalResize()
{ }

//----------------------------------------------------------------------
inline void FTermOutputTest::initScreenSettings()
{ }

//----------------------------------------------------------------------
inline auto FTermOutputTest::scrollTerminalForward() -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::scrollTerminalReverse() -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::clearTerminalAttributes()
{

}

//----------------------------------------------------------------------
inline void FTermOutputTest::clearTerminalState()
{

}

//----------------------------------------------------------------------
inline auto FTermOutputTest::clearTerminal (wchar_t) -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::flush()
{
  if ( ! no_force )
    CPPUNIT_ASSERT ( finalcut::FVTerm::isTerminalUpdateForced() );
}

//----------------------------------------------------------------------
inline void FTermOutputTest::beep() const
{
  setBellState();
}

// public methods of FTermOutputTest
//----------------------------------------------------------------------
inline auto FTermOutputTest::getFSetPaletteRef() const & -> const FSetPalette&
{
  return set_palette_function;
}

//----------------------------------------------------------------------
inline auto FTermOutputTest::isDefaultPaletteTheme() const -> bool
{
  return true;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::redefineColorPalette()
{ }

//----------------------------------------------------------------------
inline void FTermOutputTest::restoreColorPalette()
{ }


//----------------------------------------------------------------------
// class FVTerm_protected
//----------------------------------------------------------------------

class FVTerm_protected : public finalcut::FVTerm
{
  public:
    // Using-declaration
    using finalcut::FVTerm::print;

    // Disable copy constructor
    FVTerm_protected (const FVTerm_protected&) = delete;

    // Disable move constructor
    FVTerm_protected (FVTerm_protected&&) noexcept = delete;

    // Constructor
    FVTerm_protected() = default;

    template <typename FOutputType>
    explicit FVTerm_protected (finalcut::outputClass<FOutputType>);

    // Disable copy assignment operator (=)
    auto operator = (const FVTerm_protected&) -> FVTerm_protected& = delete;

    // Disable move assignment operator (=)
    auto operator = (FVTerm_protected&&) noexcept -> FVTerm_protected& = delete;

    void clearArea (wchar_t = L' ') override;
    void addPreprocessingHandler (const finalcut::FVTerm*, finalcut::FVTerm::FPreprocessingFunction&&) override;
    void delPreprocessingHandler (const finalcut::FVTerm*) override;
    void print (const finalcut::FPoint&) override;
    auto getPrintArea() -> FTermArea* override;
    void initTerminal() override;

    auto p_getPrintArea() -> FTermArea*;
    auto p_getChildPrintArea() const -> FTermArea*;
    auto p_getCurrentPrintArea() const -> FTermArea*;
    auto p_getVirtualDesktop() const -> FTermArea*;
    auto p_getVirtualTerminal() const -> FTermArea*;

    // Mutators
    void p_setPrintArea (FTermArea*);
    void p_setChildPrintArea (FTermArea*);
    void p_setActiveArea (FTermArea*) const;

    // Inquiries
    auto p_isActive (const FTermArea*) const -> bool;
    auto p_hasPrintArea() const -> bool;
    auto p_hasChildPrintArea() const -> bool;
    auto p_isVirtualWindow() const -> bool;
    auto p_isCursorHideable() const -> bool;

    // Methods
    auto p_createArea (const finalcut::FVTerm::FShadowBox&) -> std::unique_ptr<finalcut::FVTerm::FTermArea>;
    auto p_createArea (const finalcut::FRect&) -> std::unique_ptr<finalcut::FVTerm::FTermArea>;
    void p_resizeArea (const finalcut::FVTerm::FShadowBox&, FTermArea*) const;
    void p_resizeArea (const finalcut::FRect&, FTermArea*) const;
    void p_restoreVTerm (const finalcut::FRect&) const;
    auto p_updateVTermCursor (const FTermArea*) const -> bool;
    void p_hideVTermCursor() const;
    void p_setAreaCursor (const finalcut::FPoint&, bool, FTermArea*);
    void p_getArea (const finalcut::FPoint&, FTermArea*) const;
    void p_getArea (const finalcut::FRect&, FTermArea*) const;
    void p_addLayer (FTermArea*) const;
    void p_putArea (const finalcut::FPoint&, FTermArea*) const;
    static auto p_getLayer (FVTerm&) -> int;
    static void p_determineWindowLayers();
    void p_scrollAreaForward (FTermArea*);
    void p_scrollAreaReverse (FTermArea*);
    void p_clearArea (FTermArea*, wchar_t = L' ');
    void p_forceTerminalUpdate() const;
    auto p_processTerminalUpdate() const -> bool;
    static void p_startDrawing();
    static void p_finishDrawing();
    void p_initTerminal();

    void Preprocessing()
    {
      value++;
    }

    auto value_ref() & noexcept -> int&
    {
      return value;
    }

  private:
    int value{0};
};

// public methods of FVTerm_protected
//----------------------------------------------------------------------
template <typename FOutputType>
FVTerm_protected::FVTerm_protected (finalcut::outputClass<FOutputType>)
  : finalcut::FVTerm{finalcut::outputClass<FOutputType>{}}
{ }

//----------------------------------------------------------------------
inline void FVTerm_protected::clearArea (wchar_t fillchar)
{
  finalcut::FVTerm::clearArea(fillchar);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::addPreprocessingHandler (const finalcut::FVTerm* instance, finalcut::FVTerm::FPreprocessingFunction&& function)
{
  finalcut::FVTerm::addPreprocessingHandler(instance, std::move(function));
}

//----------------------------------------------------------------------
inline void FVTerm_protected::delPreprocessingHandler (const finalcut::FVTerm* instance)
{
  finalcut::FVTerm::delPreprocessingHandler(instance);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::print (const finalcut::FPoint& pos)
{
  finalcut::FVTerm::print(pos);
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::getPrintArea() -> FTermArea*
{
  return finalcut::FVTerm::getPrintArea();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::initTerminal()
{
  finalcut::FVTerm::initTerminal();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_getPrintArea() -> FTermArea*
{
  return finalcut::FVTerm::getPrintArea();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_getChildPrintArea() const -> FTermArea*
{
  return finalcut::FVTerm::getChildPrintArea();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_getCurrentPrintArea() const -> FTermArea*
{
  return finalcut::FVTerm::getCurrentPrintArea();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_getVirtualDesktop() const -> FTermArea*
{
  return finalcut::FVTerm::getVirtualDesktop();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_getVirtualTerminal() const -> FTermArea*
{
  return finalcut::FVTerm::getVirtualTerminal();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_setPrintArea (FTermArea* area)
{
  finalcut::FVTerm::setPrintArea (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_setChildPrintArea (FTermArea* area)
{
  finalcut::FVTerm::setChildPrintArea (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_setActiveArea (FTermArea* area) const
{
  finalcut::FVTerm::setActiveArea (area);
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_isActive (const FTermArea* area) const -> bool
{
  return finalcut::FVTerm::isActive(area);
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_hasPrintArea() const -> bool
{
  return finalcut::FVTerm::hasPrintArea();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_hasChildPrintArea() const -> bool
{
  return finalcut::FVTerm::hasChildPrintArea();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_isVirtualWindow() const -> bool
{
  return finalcut::FVTerm::isVirtualWindow();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_isCursorHideable() const -> bool
{
  return finalcut::FVTerm::isCursorHideable();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_createArea (const finalcut::FVTerm::FShadowBox& shadowbox) -> std::unique_ptr<finalcut::FVTerm::FTermArea>
{
  return finalcut::FVTerm::createArea ({shadowbox.box, shadowbox.shadow});
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_createArea (const finalcut::FRect& box) -> std::unique_ptr<finalcut::FVTerm::FTermArea>
{
  return finalcut::FVTerm::createArea (box);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_resizeArea (const finalcut::FVTerm::FShadowBox& shadowbox, FTermArea* area) const
{
  finalcut::FVTerm::resizeArea ({shadowbox.box, shadowbox.shadow}, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_resizeArea (const finalcut::FRect& box, FTermArea* area) const
{
  finalcut::FVTerm::resizeArea (box, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_restoreVTerm (const finalcut::FRect& box) const
{
  finalcut::FVTerm::restoreVTerm (box);
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_updateVTermCursor (const FTermArea* area) const -> bool
{
  return finalcut::FVTerm::updateVTermCursor (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_hideVTermCursor() const
{
  finalcut::FVTerm::hideVTermCursor();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_setAreaCursor (const finalcut::FPoint& pos, bool visible, FTermArea* area)
{
  finalcut::FVTerm::setAreaCursor (pos, visible, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_getArea (const finalcut::FPoint& pos, FTermArea* area) const
{
  finalcut::FVTerm::getArea (pos, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_getArea (const finalcut::FRect& box, FTermArea* area) const
{
  finalcut::FVTerm::getArea (box, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_addLayer (FTermArea* area) const
{
  finalcut::FVTerm::addLayer (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_putArea (const finalcut::FPoint& pos, FTermArea* area) const
{
  finalcut::FVTerm::putArea (pos, area);
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_getLayer (FVTerm& obj) -> int
{
  return finalcut::FVTerm::getLayer (obj);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_determineWindowLayers()
{
  finalcut::FVTerm::determineWindowLayers();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_scrollAreaForward (FTermArea* area)
{
  finalcut::FVTerm::scrollAreaForward (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_scrollAreaReverse (FTermArea* area)
{
  finalcut::FVTerm::scrollAreaReverse (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_clearArea (FTermArea* area, wchar_t fillchar)
{
  finalcut::FVTerm::clearArea (area, fillchar);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_forceTerminalUpdate() const
{
  finalcut::FVTerm::forceTerminalUpdate();
}

//----------------------------------------------------------------------
inline auto FVTerm_protected::p_processTerminalUpdate() const -> bool
{
  return finalcut::FVTerm::processTerminalUpdate();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_startDrawing()
{
  finalcut::FVTerm::startDrawing();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_finishDrawing()
{
  finalcut::FVTerm::finishDrawing();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_initTerminal()
{
  finalcut::FVTerm::initTerminal();
}


//----------------------------------------------------------------------
// class FVTermTest
//----------------------------------------------------------------------

class FVTermTest : public CPPUNIT_NS::TestFixture
{
  public:
    FVTermTest();

  protected:
    void classNameTest();
    void noArgumentTest();
    void OwnFunctionsTest();
    void FVTermBasesTest();
    void FVTermPrintTest();
    void FVTermChildAreaPrintTest();
    void FVTermScrollTest();
    void FVTermOverlappingWindowsTest();
    void FVTermTranparencyTest();
    void FVTermReduceUpdatesTest();
    void getFVTermAreaTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FVTermTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (OwnFunctionsTest);
    CPPUNIT_TEST (FVTermBasesTest);
    CPPUNIT_TEST (FVTermPrintTest);
    CPPUNIT_TEST (FVTermChildAreaPrintTest);
    CPPUNIT_TEST (FVTermScrollTest);
    CPPUNIT_TEST (FVTermOverlappingWindowsTest);
    CPPUNIT_TEST (FVTermTranparencyTest);
    CPPUNIT_TEST (FVTermReduceUpdatesTest);
    CPPUNIT_TEST (getFVTermAreaTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
FVTermTest::FVTermTest()
{
  auto ret = std::setlocale (LC_CTYPE, "en_US.UTF-8");

  if ( ! ret )
    ret = std::setlocale (LC_CTYPE, "C.UTF-8");

  if ( ret )
    fwide(stdout, 1);  // Makes stream wide-character oriented
}

//----------------------------------------------------------------------
void FVTermTest::classNameTest()
{
  const finalcut::FVTerm vt;
  const finalcut::FString& classname = vt.getClassName();
  CPPUNIT_ASSERT ( classname == "FVTerm" );
}

//----------------------------------------------------------------------
void FVTermTest::noArgumentTest()
{
  finalcut::FVTerm fvterm_1{};

  CPPUNIT_ASSERT ( ! fvterm_1.getVWin() );
  CPPUNIT_ASSERT ( fvterm_1.getPrintCursor() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( fvterm_1.getWindowList() );
  CPPUNIT_ASSERT ( fvterm_1.getWindowList()->empty() );
  CPPUNIT_ASSERT ( fvterm_1.getWindowList()->size() == 0 );

  // With own output class
  finalcut::FVTerm fvterm_2(finalcut::outputClass<FTermOutputTest>{});

  const auto& vwin = fvterm_2.getVWin();
  CPPUNIT_ASSERT ( ! vwin );
  CPPUNIT_ASSERT ( fvterm_2.getPrintCursor() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( fvterm_2.getWindowList() );
  CPPUNIT_ASSERT ( fvterm_2.getWindowList()->empty() );
  CPPUNIT_ASSERT ( fvterm_2.getWindowList()->size() == 0 );
}

//----------------------------------------------------------------------
void FVTermTest::OwnFunctionsTest()
{
  finalcut::FChar shadow_char;
  shadow_char.ch            = { L'\0', L'\0', L'\0', L'\0', L'\0' };
  shadow_char.encoded_char  = { L'\0', L'\0', L'\0', L'\0', L'\0' };
  shadow_char.color.pair.fg = finalcut::FColor::Default;
  shadow_char.color.pair.bg = finalcut::FColor::Default;
  shadow_char.attr.byte[0]  = 0;
  shadow_char.attr.byte[1]  = 0;
  shadow_char.attr.bit.transparent = true;
  shadow_char.attr.byte[2]  = 0;
  shadow_char.attr.byte[3]  = 0;

  // FChar struct
  finalcut::FChar test_char =
  {
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Default, finalcut::FColor::Default } },
    { { 0x00, 0x00, 0x00, 0x00} }  // byte 0..3
  };

  test::RepeatFChar rep_fchar {80, test_char};
  test::RepeatFCharLine rep_fchar_line {25, {rep_fchar} };

  CPPUNIT_ASSERT( rep_fchar_line.data[0].data != shadow_char );
  test_char.attr.bit.transparent = true;
  CPPUNIT_ASSERT( test_char == shadow_char );
}

//----------------------------------------------------------------------
void FVTermTest::FVTermBasesTest()
{
  // With own output class
  FVTerm_protected p_fvterm(finalcut::outputClass<FTermOutputTest>{});

  auto vwin = p_fvterm.getVWin();
  CPPUNIT_ASSERT ( ! vwin );
  CPPUNIT_ASSERT ( p_fvterm.getPrintCursor() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( p_fvterm.getWindowList() );
  CPPUNIT_ASSERT ( p_fvterm.getWindowList()->empty() );
  CPPUNIT_ASSERT ( p_fvterm.getWindowList()->size() == 0 );
  CPPUNIT_ASSERT ( ! p_fvterm.areTerminalUpdatesPaused() );
  p_fvterm.setTerminalUpdates(finalcut::FVTerm::TerminalUpdate::Stop);
  CPPUNIT_ASSERT ( p_fvterm.areTerminalUpdatesPaused() );
  p_fvterm.setTerminalUpdates(finalcut::FVTerm::TerminalUpdate::Continue);
  CPPUNIT_ASSERT ( ! p_fvterm.areTerminalUpdatesPaused() );
  p_fvterm.setTerminalUpdates(finalcut::FVTerm::TerminalUpdate::Stop);
  CPPUNIT_ASSERT ( p_fvterm.areTerminalUpdatesPaused() );
  p_fvterm.setTerminalUpdates(finalcut::FVTerm::TerminalUpdate::Start);
  CPPUNIT_ASSERT ( ! p_fvterm.areTerminalUpdatesPaused() );

  // Create and check a virtual window for the p_fvterm object
  finalcut::FRect geometry {finalcut::FPoint{5, 5}, finalcut::FSize{20, 20}};
  finalcut::FSize Shadow(2, 1);
  auto vwin_ptr = p_fvterm.p_createArea ({geometry, Shadow});
  vwin = vwin_ptr.get();
  p_fvterm.setVWin(std::move(vwin_ptr));

  if ( ! (vwin && vwin->owner) || vwin->changes.empty() )
  {
    CPPUNIT_ASSERT ( false );
    return;
  }

  CPPUNIT_ASSERT ( vwin );
  CPPUNIT_ASSERT ( vwin == p_fvterm.getVWin() );
  CPPUNIT_ASSERT ( vwin->owner );
  CPPUNIT_ASSERT ( vwin->hasOwner() );
  CPPUNIT_ASSERT ( vwin->getOwner<FVTerm_protected*>() == &p_fvterm );
  CPPUNIT_ASSERT ( vwin->position.x == 5 );
  CPPUNIT_ASSERT ( vwin->position.y == 5 );
  CPPUNIT_ASSERT ( vwin->size.width == 20 );
  CPPUNIT_ASSERT ( vwin->size.height == 20 );
  CPPUNIT_ASSERT ( vwin->min_size.width == 20 );
  CPPUNIT_ASSERT ( vwin->min_size.height == 1 );
  CPPUNIT_ASSERT ( vwin->shadow.width == 2 );
  CPPUNIT_ASSERT ( vwin->shadow.height == 1 );
  CPPUNIT_ASSERT ( vwin->cursor.x == 0 );
  CPPUNIT_ASSERT ( vwin->cursor.y == 0 );
  CPPUNIT_ASSERT ( vwin->input_cursor.x == -1 );
  CPPUNIT_ASSERT ( vwin->input_cursor.y == -1 );
  vwin->setInputCursorPos (12, 4);
  CPPUNIT_ASSERT ( vwin->input_cursor.x == 12 );
  CPPUNIT_ASSERT ( vwin->input_cursor.y == 4 );
  p_fvterm.p_setAreaCursor ({3, 5}, true, vwin);
  CPPUNIT_ASSERT ( vwin->input_cursor_visible );
  p_fvterm.p_setAreaCursor ({12, 4}, false, vwin);
  CPPUNIT_ASSERT ( vwin->layer == -1 );
  CPPUNIT_ASSERT ( ! vwin->input_cursor_visible );
  CPPUNIT_ASSERT ( ! vwin->has_changes );
  CPPUNIT_ASSERT ( ! vwin->visible );
  CPPUNIT_ASSERT ( ! vwin->minimized );
  CPPUNIT_ASSERT ( vwin->preproc_list.empty() );
  CPPUNIT_ASSERT ( ! vwin->changes.empty() );
  CPPUNIT_ASSERT ( &(vwin->changes[0]) != nullptr );

  if ( &(vwin->changes[0]) )
  {
    CPPUNIT_ASSERT ( vwin->changes[0].xmin == 22 );
    CPPUNIT_ASSERT ( vwin->changes[0].xmax == 0 );
    CPPUNIT_ASSERT ( vwin->changes[0].trans_count == 0 );
  }

  CPPUNIT_ASSERT ( ! vwin->data.empty() );
  CPPUNIT_ASSERT ( ! p_fvterm.p_isActive(vwin) );
  CPPUNIT_ASSERT ( test::getAreaSize(vwin) == 462 );
  CPPUNIT_ASSERT ( vwin->contains({5, 5}) );
  CPPUNIT_ASSERT ( vwin->contains(5, 5) );
  CPPUNIT_ASSERT ( ! vwin->contains({4, 5}) );
  CPPUNIT_ASSERT ( ! vwin->contains(4, 5) );
  CPPUNIT_ASSERT ( ! vwin->contains({5, 4}) );
  CPPUNIT_ASSERT ( ! vwin->contains(5, 4) );
  CPPUNIT_ASSERT ( ! vwin->contains({4, 4}) );
  CPPUNIT_ASSERT ( ! vwin->contains(4, 4) );
  CPPUNIT_ASSERT ( vwin->contains({26, 25}) );    // {5 + 20 + 2 - 1 = 26, 5 + 20 + 1 - 1 = 25}
  CPPUNIT_ASSERT ( vwin->contains(26, 25) );    // {5 + 20 + 2 - 1 = 26, 5 + 20 + 1 - 1 = 25}
  CPPUNIT_ASSERT ( ! vwin->contains({27, 25}) );
  CPPUNIT_ASSERT ( ! vwin->contains(27, 25) );
  CPPUNIT_ASSERT ( ! vwin->contains({26, 26}) );
  CPPUNIT_ASSERT ( ! vwin->contains(26, 26) );
  CPPUNIT_ASSERT ( ! vwin->contains({27, 26}) );
  CPPUNIT_ASSERT ( ! vwin->contains(27, 26) );
  vwin->minimized = true;
  CPPUNIT_ASSERT ( vwin->contains({5, 5}) );
  CPPUNIT_ASSERT ( ! vwin->contains({4, 5}) );
  CPPUNIT_ASSERT ( ! vwin->contains({5, 4}) );
  CPPUNIT_ASSERT ( ! vwin->contains({4, 4}) );
  CPPUNIT_ASSERT ( vwin->contains({26, 5}) );    // {5 + 20 + 2 - 1 = 26, 5 + 1 - 1 = 5}
  CPPUNIT_ASSERT ( ! vwin->contains({27, 5}) );
  CPPUNIT_ASSERT ( ! vwin->contains({26, 6}) );
  CPPUNIT_ASSERT ( ! vwin->contains({27, 6}) );
  vwin->minimized = false;
  CPPUNIT_ASSERT ( ! vwin->isPrintPositionInsideArea() );
  vwin->setCursorPos(0, 1);
  CPPUNIT_ASSERT ( ! vwin->isPrintPositionInsideArea() );
  vwin->setCursorPos(1, 0);
  CPPUNIT_ASSERT ( ! vwin->isPrintPositionInsideArea() );
  vwin->setCursorPos(1, 1);
  CPPUNIT_ASSERT ( vwin->isPrintPositionInsideArea() );
  vwin->setCursorPos(22, 21);  // {20 + 2 = 22, 20 + 1 = 21}
  CPPUNIT_ASSERT ( vwin->isPrintPositionInsideArea() );
  vwin->setCursorPos(23, 21);
  CPPUNIT_ASSERT ( ! vwin->isPrintPositionInsideArea() );
  vwin->setCursorPos(22, 22);
  CPPUNIT_ASSERT ( ! vwin->isPrintPositionInsideArea() );
  vwin->setCursorPos(23, 22);
  CPPUNIT_ASSERT ( ! vwin->isPrintPositionInsideArea() );
  vwin->setCursorPos(0, 0);

  finalcut::FChar default_char;
  default_char.ch           = { L' ', L'\0', L'\0', L'\0', L'\0' };
  default_char.encoded_char = { L'\0', L'\0', L'\0', L'\0', L'\0' };
  default_char.color.pair.fg = finalcut::FColor::Default;
  default_char.color.pair.bg = finalcut::FColor::Default;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;
  default_char.attr.byte[2] = 8;  // char_width = 1
  default_char.attr.byte[3] = 0;

  for (std::size_t pos = 0; pos < test::getAreaSize(vwin); pos++)
    CPPUNIT_ASSERT ( test::isFCharEqual(vwin->data[pos], default_char) );

  CPPUNIT_ASSERT ( p_fvterm.getPrintCursor() == finalcut::FPoint(5, 5) );
  CPPUNIT_ASSERT ( p_fvterm.getWindowList() );
  CPPUNIT_ASSERT ( p_fvterm.getWindowList()->empty() );
  CPPUNIT_ASSERT ( p_fvterm.getWindowList()->size() == 0 );
  p_fvterm.getWindowList()->push_back(&p_fvterm);
  CPPUNIT_ASSERT ( ! p_fvterm.getWindowList()->empty() );
  CPPUNIT_ASSERT ( p_fvterm.getWindowList()->size() == 1 );
  CPPUNIT_ASSERT ( FVTerm_protected::p_getLayer(p_fvterm) == -1 );
  CPPUNIT_ASSERT ( vwin->layer == -1 );
  FVTerm_protected::p_determineWindowLayers();
  CPPUNIT_ASSERT ( FVTerm_protected::p_getLayer(p_fvterm) == 1 );
  CPPUNIT_ASSERT ( vwin->cursor.x == 0 );
  CPPUNIT_ASSERT ( vwin->cursor.y == 0 );
  p_fvterm.setCursor( {16, 8} );
  CPPUNIT_ASSERT ( vwin->cursor.x == 11 );
  CPPUNIT_ASSERT ( vwin->cursor.y == 3 );
  CPPUNIT_ASSERT ( finalcut::FKeyboard::getReadBlockingTime() == 100000 );
  p_fvterm.setNonBlockingRead();
  CPPUNIT_ASSERT ( finalcut::FKeyboard::getReadBlockingTime() == 5000 );
  p_fvterm.unsetNonBlockingRead();
  CPPUNIT_ASSERT ( finalcut::FKeyboard::getReadBlockingTime() == 100000 );
  p_fvterm.setNonBlockingRead(true);
  CPPUNIT_ASSERT ( finalcut::FKeyboard::getReadBlockingTime() == 5000 );
  p_fvterm.setNonBlockingRead(false);
  CPPUNIT_ASSERT ( finalcut::FKeyboard::getReadBlockingTime() == 100000 );
  CPPUNIT_ASSERT ( ! p_fvterm.isDrawingFinished() );
  FVTerm_protected::p_finishDrawing();
  CPPUNIT_ASSERT ( p_fvterm.isDrawingFinished() );
  FVTerm_protected::p_startDrawing();
  CPPUNIT_ASSERT ( ! p_fvterm.isDrawingFinished() );
  CPPUNIT_ASSERT ( ! finalcut::FVTerm::isTerminalUpdateForced() );
  CPPUNIT_ASSERT ( ! finalcut::FVTerm::hasPendingTerminalUpdates() );
  p_fvterm.p_forceTerminalUpdate();  // Call FTermOutputTest::flush()

  // Check the virtual desktop
  auto&& vdesktop = p_fvterm.p_getVirtualDesktop();
  CPPUNIT_ASSERT ( vdesktop->hasOwner() );
  CPPUNIT_ASSERT ( vdesktop->getOwner<FVTerm_protected*>() == &p_fvterm );
  CPPUNIT_ASSERT ( vdesktop->position.x == 0 );
  CPPUNIT_ASSERT ( vdesktop->position.y == 0 );
  CPPUNIT_ASSERT ( vdesktop->size.width == 80 );
  CPPUNIT_ASSERT ( vdesktop->size.height == 24 );
  CPPUNIT_ASSERT ( vdesktop->min_size.width == 80 );
  CPPUNIT_ASSERT ( vdesktop->min_size.height == 1 );
  CPPUNIT_ASSERT ( vdesktop->shadow.width == 0);
  CPPUNIT_ASSERT ( vdesktop->shadow.height == 0 );
  CPPUNIT_ASSERT ( vdesktop->cursor.x == 0 );
  CPPUNIT_ASSERT ( vdesktop->cursor.y == 0 );
  CPPUNIT_ASSERT ( vdesktop->input_cursor.x == -1 );
  CPPUNIT_ASSERT ( vdesktop->input_cursor.y == -1 );
  CPPUNIT_ASSERT ( vdesktop->layer == -1 );
  CPPUNIT_ASSERT ( ! vdesktop->input_cursor_visible );
  CPPUNIT_ASSERT ( ! vdesktop->has_changes );
  CPPUNIT_ASSERT ( vdesktop->visible );
  CPPUNIT_ASSERT ( ! vdesktop->minimized );
  CPPUNIT_ASSERT ( vdesktop->preproc_list.empty() );
  CPPUNIT_ASSERT ( ! vdesktop->changes.empty() );
  CPPUNIT_ASSERT ( vdesktop->changes[0].xmin == 80 );
  CPPUNIT_ASSERT ( vdesktop->changes[0].xmax == 0 );
  CPPUNIT_ASSERT ( vdesktop->changes[0].trans_count == 0 );
  CPPUNIT_ASSERT ( ! vdesktop->data.empty() );
  CPPUNIT_ASSERT ( test::getAreaSize(vdesktop) == 1920 );
  CPPUNIT_ASSERT ( p_fvterm.p_isActive(vdesktop) );
  p_fvterm.p_setActiveArea(vwin);
  CPPUNIT_ASSERT ( ! p_fvterm.p_isActive(vdesktop) );
  CPPUNIT_ASSERT ( p_fvterm.p_isActive(vwin) );
  p_fvterm.p_setActiveArea(vdesktop);
  CPPUNIT_ASSERT ( p_fvterm.p_isActive(vdesktop) );

  for (std::size_t pos = 0; pos < test::getAreaSize(vdesktop); pos++)
    CPPUNIT_ASSERT ( test::isFCharEqual(vdesktop->data[pos], default_char) );

  // Check the virtual terminal
  auto&& vterm = p_fvterm.p_getVirtualTerminal();
  CPPUNIT_ASSERT ( vterm->hasOwner() );
  CPPUNIT_ASSERT ( vterm->getOwner<FVTerm_protected*>() == &p_fvterm );
  CPPUNIT_ASSERT ( vterm->position.x == 0 );
  CPPUNIT_ASSERT ( vterm->position.y == 0 );
  CPPUNIT_ASSERT ( vterm->size.width == 80 );
  CPPUNIT_ASSERT ( vterm->size.height == 24 );
  CPPUNIT_ASSERT ( vterm->min_size.width == 80 );
  CPPUNIT_ASSERT ( vterm->min_size.height == 1 );
  CPPUNIT_ASSERT ( vterm->shadow.width == 0);
  CPPUNIT_ASSERT ( vterm->shadow.height == 0 );
  CPPUNIT_ASSERT ( vterm->cursor.x == 0 );
  CPPUNIT_ASSERT ( vterm->cursor.y == 0 );
  CPPUNIT_ASSERT ( vterm->input_cursor.x == -1 );
  CPPUNIT_ASSERT ( vterm->input_cursor.y == -1 );
  CPPUNIT_ASSERT ( vterm->layer == -1 );
  CPPUNIT_ASSERT ( ! vterm->input_cursor_visible );
  vterm->input_cursor_visible = true;
  CPPUNIT_ASSERT ( vterm->input_cursor_visible );
  p_fvterm.p_hideVTermCursor();
  CPPUNIT_ASSERT ( ! vterm->input_cursor_visible );
  CPPUNIT_ASSERT ( ! vterm->has_changes );
  CPPUNIT_ASSERT ( ! vterm->visible );
  CPPUNIT_ASSERT ( ! vterm->minimized );
  CPPUNIT_ASSERT ( vterm->preproc_list.empty() );
  CPPUNIT_ASSERT ( ! vterm->changes.empty() );
  CPPUNIT_ASSERT ( vterm->changes[0].xmin == 80 );
  CPPUNIT_ASSERT ( vterm->changes[0].xmax == 0 );
  CPPUNIT_ASSERT ( vterm->changes[0].trans_count == 0 );
  CPPUNIT_ASSERT ( ! vterm->data.empty() );
  CPPUNIT_ASSERT ( test::getAreaSize(vterm) == 1920 );

  for (std::size_t pos = 0; pos < test::getAreaSize(vterm); pos++)
    CPPUNIT_ASSERT ( test::isFCharEqual(vterm->data[pos], default_char) );

  // Create a vterm comparison area
  finalcut::FVTerm::FTermArea* test_vterm_area{};
  auto vterm_geometry = finalcut::FRect(finalcut::FPoint{1, 1}, finalcut::FSize{80, 24});
  auto test_vterm_area_ptr = p_fvterm.p_createArea (vterm_geometry);
  test_vterm_area = test_vterm_area_ptr.get();
  test::printOnArea (test_vterm_area, {test::getAreaSize(test_vterm_area), default_char});
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );

  for (auto i{0}; i < vwin->size.height; i++)
  {
    CPPUNIT_ASSERT ( vwin->changes[i].xmin == 22 );
    CPPUNIT_ASSERT ( vwin->changes[i].xmax == 0 );
    CPPUNIT_ASSERT ( vwin->changes[i].trans_count == 0 );
  }

  // Virtual windows fill test
  p_fvterm.clearArea(L'▒');  // Fill with '▒'
  CPPUNIT_ASSERT ( vwin->has_changes );

  for (auto i{0}; i < vwin->size.height; i++)
  {
    CPPUNIT_ASSERT ( vwin->changes[i].xmin == 0 );
    CPPUNIT_ASSERT ( vwin->changes[i].xmax == 21 );
    CPPUNIT_ASSERT ( vwin->changes[i].trans_count == 2 );
  }

  const auto full_height = vwin->size.height + vwin->shadow.height;

  for (auto i{vwin->size.height}; i < full_height; i++)
  {
    CPPUNIT_ASSERT ( vwin->changes[i].xmin == 0 );
    CPPUNIT_ASSERT ( vwin->changes[i].xmax == 21 );
    CPPUNIT_ASSERT ( vwin->changes[i].trans_count == 22 );
  }

  // Check area
  finalcut::FChar bg_char =
  {
    { L'▒', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Default, finalcut::FColor::Default } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  const auto full_width = std::size_t(vwin->size.width)
                        + std::size_t(vwin->shadow.width);

  for (auto y{0u}; y < std::size_t(vwin->size.height); y++)
  {
    for (auto x{0u}; x < std::size_t(vwin->size.width); x++)
    {
      CPPUNIT_ASSERT ( test::isFCharEqual(vwin->data[y * full_width + x], bg_char) );
    }
  }

  // Check shadow
  auto width = std::size_t(vwin->size.width);
  finalcut::FChar shadow_char =
  {
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Default, finalcut::FColor::Default } },
    { { 0x00, 0x00, 0x00, 0x00} }  // byte 0..3
  };
  shadow_char.attr.bit.transparent = true;

  test::showFCharData(vwin->data[width]);
  test::showFCharData(shadow_char);

  for (auto y{0u}; y < std::size_t(full_height); y++)  // Right side shadow
  {
    for (auto x{width}; x < full_width; x++)
    {
      CPPUNIT_ASSERT ( test::isFCharEqual(vwin->data[y * full_width + x], shadow_char) );
    }
  }

  for (auto x{0u}; x < full_width; x++)  // Bottom shadow
  {
    auto y = full_height - 1;
    CPPUNIT_ASSERT ( test::isFCharEqual(vwin->data[y * full_width + x], shadow_char) );
  }

  // Create a vwin comparison area
  finalcut::FVTerm::FTermArea* test_vwin_area{};
  auto test_vwin_area_ptr = p_fvterm.p_createArea ({geometry, Shadow});
  test_vwin_area = test_vwin_area_ptr.get();

  //                             .-------------------------- 20 line repetitions
  //                             |      .------------------- 20 column repetitions
  //                             |      |             .----- 2 column repetitions
  //                             |      |             |
  test::printOnArea (test_vwin_area, {20, { {20, bg_char}, {2, shadow_char} } });

  //                        .-------------------------- 22 column repetitions
  //                        |
  test::printOnArea (test_vwin_area, {22, shadow_char});
  test::printArea (vwin);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );

  // Makes the virtual window visible and thus displayable
  // on the virtual terminal
  vwin->visible = true;
  CPPUNIT_ASSERT ( p_fvterm.p_isCursorHideable() );
  vwin->input_cursor_visible = true;
  p_fvterm.p_setActiveArea(vwin);

  vwin->setInputCursorPos(4, 2);
  test::showFCharData(vwin->data[full_width + 1]);
  CPPUNIT_ASSERT ( vwin->has_changes );
  CPPUNIT_ASSERT ( vterm->input_cursor.x == -1 );
  CPPUNIT_ASSERT ( vterm->input_cursor.y == -1 );
  CPPUNIT_ASSERT ( ! vterm->input_cursor_visible );
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( vterm->input_cursor.x == 9 );
  CPPUNIT_ASSERT ( vterm->input_cursor.y == 7 );
  CPPUNIT_ASSERT ( vterm->input_cursor_visible );
  CPPUNIT_ASSERT ( ! vwin->has_changes );
  CPPUNIT_ASSERT ( ! test::isAreaEqual(test_vterm_area, vterm) );
  test::printOnArea (test_vterm_area, {5, { {80, default_char} } });
  test::printOnArea (test_vterm_area, {19, { {5, default_char}, {20, bg_char}, {55, default_char} } });
  test::printArea (vterm);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );

  // Move
  vwin->position.x = 0;
  vwin->position.y = 0;
  p_fvterm.p_putArea ({1, 1}, nullptr);
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  p_fvterm.p_putArea ({1, 1}, vwin);
  p_fvterm.p_processTerminalUpdate();
  test::printArea (vterm);
  test::printOnArea (test_vterm_area, {5, { {20, bg_char}, {60, default_char} } });
  test::printOnArea (test_vterm_area, {15, { {20, bg_char}, {2, default_char}, {3, bg_char}, {55, default_char} } });
  test::printOnArea (test_vterm_area, {1, { {22, default_char}, {3, bg_char}, {55, default_char} } });
  test::printOnArea (test_vterm_area, {3, { {5, default_char}, {20, bg_char}, {55, default_char} } });
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{21, 5}, finalcut::FSize{5, 21}});
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{5, 21}, finalcut::FSize{20, 4}});
  test::printOnArea (test_vterm_area, {20, { {20, bg_char}, {60, default_char} } });
  test::printOnArea (test_vterm_area, {4, { {80, default_char} } });
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  vwin->position.x = -10;
  vwin->position.y = -10;
  p_fvterm.p_putArea ({-9, -9}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{1, 1}, finalcut::FSize{22, 21}});
  test::printOnArea (test_vterm_area, {10, { {10, bg_char}, {70, default_char} } });
  test::printOnArea (test_vterm_area, {14, { {80, default_char} } });
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  vwin->position.x = 70;
  vwin->position.y = -10;
  p_fvterm.p_putArea ({71, -9}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{1, 1}, finalcut::FSize{12, 11}});
  test::printOnArea (test_vterm_area, {10, { {70, default_char}, {10, bg_char} } });
  test::printOnArea (test_vterm_area, {14, { {80, default_char} } });
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  vwin->position.x = -10;
  vwin->position.y = 14;
  p_fvterm.p_putArea ({-9, 15}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{71, 1}, finalcut::FSize{10, 11}});
  test::printOnArea (test_vterm_area, {14, { {80, default_char} } });
  test::printOnArea (test_vterm_area, {10, { {10, bg_char}, {70, default_char} } });
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  vwin->position.x = 70;
  vwin->position.y = 14;
  p_fvterm.p_putArea ({71, 15}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{1, 15}, finalcut::FSize{12, 10}});
  test::printOnArea (test_vterm_area, {14, { {80, default_char} } });
  test::printOnArea (test_vterm_area, {10, { {70, default_char}, {10, bg_char} } });
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  // Move outside
  vwin->position.x = -20;
  vwin->position.y = -20;
  p_fvterm.p_putArea ({-19, -19}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{71, 15}, finalcut::FSize{10, 10}});
  test::printOnArea (test_vterm_area, {24, { {80, default_char} } });
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  vwin->position.x = 80;
  vwin->position.y = -20;
  p_fvterm.p_putArea ({81, -19}, vwin);
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );

  vwin->position.x = -20;
  vwin->position.y = 24;
  p_fvterm.p_putArea ({-19, 25}, vwin);
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );

  vwin->position.x = 80;
  vwin->position.y = 24;
  p_fvterm.p_putArea ({81, 25}, vwin);
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );

  // Virtual window is larger than virtual terminal
  auto large_geometry = finalcut::FRect(finalcut::FPoint{1, 1}, finalcut::FSize{90, 34});
  p_fvterm.p_resizeArea (large_geometry, vwin);
  vwin->cursor.x = 1;
  vwin->cursor.y = 1;

  for (wchar_t i = 0; i < wchar_t(large_geometry.getHeight()); i++)
  {
    bg_char.ch[0] =  L'\\' + i;
    test::printOnArea (vwin, {large_geometry.getWidth(), bg_char});
  }

  vwin->position.x = -5;
  vwin->position.y = -5;
  p_fvterm.p_putArea ({-4, -4}, vwin);

  for (wchar_t i = 0; i < wchar_t(vterm->size.height); i++)
  {
    bg_char.ch[0] =  L'a' + i;
    test::printOnArea (test_vterm_area, {std::size_t(vterm->size.width), bg_char});
  }

  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  // Rezeize virtual terminal
  auto new_term_size = finalcut::FSize{70, 18};
  p_fvterm.resizeVTerm (new_term_size);
  auto new_term_geometry = finalcut::FRect(finalcut::FPoint{1, 1}, new_term_size);
  p_fvterm.p_resizeArea (new_term_geometry, test_vterm_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  CPPUNIT_ASSERT ( vterm->size.width == 70);
  CPPUNIT_ASSERT ( vterm->size.height == 18 );
  test::printArea (vterm);
  p_fvterm.p_putArea ({-4, -4}, vwin);

  for (wchar_t i = 0; i < wchar_t(vterm->size.height); i++)
  {
    bg_char.ch[0] =  L'a' + i;
    test::printOnArea (test_vterm_area, {std::size_t(vterm->size.width), bg_char});
  }

  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  // Reset line changes
  for (auto i{0}; i < vterm->size.height; i++)
  {
    vterm->changes[i].xmin = uInt(vterm->size.width - 1);
    vterm->changes[i].xmax = 0;
  }

  for (auto i{0}; i < vterm->size.height; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 69 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 0 );
  }

  // Force all lines of the virtual terminal to be output
  p_fvterm.putVTerm();

  for (auto i{0}; i < vterm->size.height; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 0 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 69 );
  }

  // Change the width only
  new_term_size = finalcut::FSize{75, 18};
  p_fvterm.resizeVTerm (new_term_size);
  new_term_geometry = finalcut::FRect(finalcut::FPoint{1, 1}, new_term_size);
  p_fvterm.p_resizeArea (new_term_geometry, test_vterm_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  CPPUNIT_ASSERT ( vterm->size.width == 75);
  CPPUNIT_ASSERT ( vterm->size.height == 18 );

  // No change
  new_term_size = finalcut::FSize{75, 18};
  p_fvterm.resizeVTerm (new_term_size);
  new_term_geometry = finalcut::FRect(finalcut::FPoint{1, 1}, new_term_size);
  p_fvterm.p_resizeArea (new_term_geometry, test_vterm_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  CPPUNIT_ASSERT ( vterm->size.width == 75);
  CPPUNIT_ASSERT ( vterm->size.height == 18 );

  // Deallocate area memory
  CPPUNIT_ASSERT ( test_vwin_area_ptr.get() );
  test_vwin_area_ptr.reset();
  CPPUNIT_ASSERT ( ! test_vwin_area_ptr.get() );

  CPPUNIT_ASSERT ( test_vterm_area_ptr.get() );
  test_vterm_area_ptr.reset();
  CPPUNIT_ASSERT ( ! test_vterm_area_ptr.get() );
}

//----------------------------------------------------------------------
void FVTermTest::FVTermPrintTest()
{
  for (const auto& enc : {finalcut::Encoding::VT100, finalcut::Encoding::UTF8})
  {
    if ( enc == finalcut::Encoding::VT100 )
    {
      if ( ! std::setlocale (LC_CTYPE, "C") )
        continue;

      std::wcout << L"\n-> Encoding: VT100\n";
    }
    else if ( enc == finalcut::Encoding::UTF8 )
    {
      auto ret = std::setlocale (LC_CTYPE, "en_US.UTF-8");

      if ( ! ret )
        ret = std::setlocale (LC_CTYPE, "C.UTF-8");

      if ( ! ret )
        continue;

      std::wcout << L"\n-> Encoding: UTF-8\n";
    }

    auto& fterm_data = finalcut::FTermData::getInstance();
    fterm_data.setTermEncoding (enc);

    FVTerm_protected p_fvterm(finalcut::outputClass<FTermOutputTest>{});
    p_fvterm.p_initTerminal();
    auto vwin = p_fvterm.getVWin();
    auto child_print_area = p_fvterm.p_getChildPrintArea();
    CPPUNIT_ASSERT ( child_print_area == nullptr );
    CPPUNIT_ASSERT ( ! p_fvterm.p_isVirtualWindow() );

    finalcut::FRect geometry {finalcut::FPoint{0, 0}, finalcut::FSize{15, 10}};
    finalcut::FSize Shadow(1, 1);
    auto vwin_ptr = p_fvterm.p_createArea ({geometry, Shadow});
    vwin = vwin_ptr.get();
    p_fvterm.setVWin(std::move(vwin_ptr));
    const finalcut::FVTerm::FTermArea* const_vwin = p_fvterm.getVWin();
    CPPUNIT_ASSERT ( p_fvterm.p_isVirtualWindow() );
    CPPUNIT_ASSERT ( static_cast<decltype(const_vwin)>(vwin) == const_vwin );
    CPPUNIT_ASSERT ( ! p_fvterm.p_hasPrintArea() );
    CPPUNIT_ASSERT ( p_fvterm.getPrintArea() == vwin );
    CPPUNIT_ASSERT ( p_fvterm.p_hasPrintArea() );
    auto print_area = p_fvterm.p_getCurrentPrintArea();
    CPPUNIT_ASSERT ( print_area == vwin );

    if ( ! vwin )
    {
      CPPUNIT_ASSERT ( false );
      return;
    }

    CPPUNIT_ASSERT ( vwin->cursor.x == 0 );  // First column is 1
    CPPUNIT_ASSERT ( vwin->cursor.y == 0 );  // First row is 1
    CPPUNIT_ASSERT ( vwin->position.x == 0 );
    CPPUNIT_ASSERT ( vwin->position.y == 0 );

    auto move_geometry = finalcut::FRect ( finalcut::FPoint{12, 37}
                                         , finalcut::FSize{15, 10} );
    p_fvterm.p_resizeArea ({move_geometry, Shadow}, vwin);
    CPPUNIT_ASSERT ( vwin->position.x == 12 );
    CPPUNIT_ASSERT ( vwin->position.y == 37 );

    // FChar struct
    finalcut::FChar default_char =
    {
      { L' ', L'\0', L'\0', L'\0', L'\0' },
      { L'\0', L'\0', L'\0', L'\0', L'\0' },
      { { finalcut::FColor::Default, finalcut::FColor::Default } },
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
    };

    // printf test

    // Create a vwin comparison area
    finalcut::FVTerm::FTermArea* test_vwin_area{};
    auto test_vwin_area_ptr = p_fvterm.p_createArea ({geometry, Shadow});
    test_vwin_area = test_vwin_area_ptr.get();

    CPPUNIT_ASSERT ( p_fvterm.getPrintArea() == vwin );
    p_fvterm.p_setPrintArea (test_vwin_area);
    CPPUNIT_ASSERT ( p_fvterm.getPrintArea() == test_vwin_area );
    p_fvterm.p_setPrintArea (vwin);
    CPPUNIT_ASSERT ( p_fvterm.getPrintArea() == vwin );

    test::printOnArea (test_vwin_area, {11, { {16, default_char} } });
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.printf("%s/%d = %4.2f...", "1", 3, 1.0f/3.0f);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.setCursor({1, 0});
    CPPUNIT_ASSERT ( vwin->cursor.x == -11 );
    CPPUNIT_ASSERT ( vwin->cursor.y == -37 );
    vwin->position.x = 0;
    vwin->position.y = 0;
    p_fvterm.setCursor({1, 0});
    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );  // First column is 1
    CPPUNIT_ASSERT ( vwin->cursor.y == 0 );  // First row is 1
    p_fvterm.printf("%s/%d = %4.2f...", "1", 3, 1.0f/3.0f);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.setCursor({0, 1});
    CPPUNIT_ASSERT ( vwin->cursor.x == 0 );  // First column is 1
    CPPUNIT_ASSERT ( vwin->cursor.y == 1 );  // First row is 1
    p_fvterm.printf("%s/%d = %4.2f...", "1", 3, 1.0f/3.0f);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.setCursor({1, 1});
    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );  // First column is 1
    CPPUNIT_ASSERT ( vwin->cursor.y == 1 );  // First row is 1
    std::setlocale(LC_NUMERIC, "C");
    p_fvterm.printf("%s/%d = %4.2f...", "1", 3, 1.0f/3.0f);
    CPPUNIT_ASSERT ( vwin->cursor.x == 14 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 1 );
    CPPUNIT_ASSERT ( ! test::isAreaEqual(test_vwin_area, vwin) );
    test_vwin_area->data[0].ch[0] = '1';
    test_vwin_area->data[1].ch[0] = '/';
    test_vwin_area->data[2].ch[0] = '3';
    test_vwin_area->data[3].ch[0] = ' ';
    test_vwin_area->data[4].ch[0] = '=';
    test_vwin_area->data[5].ch[0] = ' ';
    test_vwin_area->data[6].ch[0] = '0';
    test_vwin_area->data[7].ch[0] = '.';
    test_vwin_area->data[8].ch[0] = '3';
    test_vwin_area->data[9].ch[0] = '3';
    test_vwin_area->data[10].ch[0] = '.';
    test_vwin_area->data[11].ch[0] = '.';
    test_vwin_area->data[12].ch[0] = '.';

    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 14 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 1 );

    finalcut::FString string{};  // Empty string
    p_fvterm.print(vwin, string);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(string);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    finalcut::FVTermBuffer fvtermbuffer{}; // Empty FVTerm buffer
    p_fvterm.print(fvtermbuffer);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(vwin, fvtermbuffer);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(nullptr, fvtermbuffer);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    // Bell (BEL), Backspace (BS), Newline (LF) and Carriage Return (CR)
    string = "\nsetup\b\a\b  \rn";
    CPPUNIT_ASSERT ( ! getBellState() );
    p_fvterm.print(vwin, string);
    CPPUNIT_ASSERT ( getBellState() );
    setBellState(false);
    CPPUNIT_ASSERT ( ! getBellState() );

    test_vwin_area->data[16].ch[0] = 'n';
    test_vwin_area->data[17].ch[0] = 'e';
    test_vwin_area->data[18].ch[0] = 't';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 2 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 2 );
    p_fvterm.print(finalcut::FPoint{1, 3});
    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    // Horizontal tabulation (HT)
    p_fvterm.print(L"1\t2");
    test_vwin_area->data[32].ch[0] = '1';
    test_vwin_area->data[40].ch[0] = '2';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    p_fvterm.print(L"\r          ");  // Carriage Return + spaces
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"12\t2");
    test_vwin_area->data[33].ch[0] = '2';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"123\t2");
    test_vwin_area->data[34].ch[0] = '3';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"1234\t2");
    test_vwin_area->data[35].ch[0] = '4';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"12345\t2");
    test_vwin_area->data[36].ch[0] = '5';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"123456\t2");
    test_vwin_area->data[37].ch[0] = '6';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"1234567\t2");
    test_vwin_area->data[38].ch[0] = '7';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    p_fvterm.print(L"\r          \r12345678\t2");
    test_vwin_area->data[39].ch[0] = '8';
    test_vwin_area->data[40].ch[0] = ' ';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 17 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    // Cursor is outside the window
    p_fvterm.print(L"invisible");
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 17 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 3 );

    p_fvterm.print('\b');  // Backspace
    p_fvterm.print(L'…');
    test_vwin_area->data[47].ch[0] = L'…';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 4 );

    // std::vector<FChar>
    finalcut::FChar fchar =
    {
      { L'\0', L'\0', L'\0', L'\0', L'\0' },
      { L'\0', L'\0', L'\0', L'\0', L'\0' },
      { { finalcut::FColor::Red, finalcut::FColor::White } },
      { { 0x01, 0x00, 0x00, 0x00} }  // byte 0..3
    };

    std::vector<finalcut::FChar> term_string(7, fchar);
    term_string[0].ch[0] = 'V';
    term_string[1].ch[0] = 'e';
    term_string[2].ch[0] = 'c';
    term_string[3].ch[0] = 't';
    term_string[4].ch[0] = 'o';
    term_string[5].ch[0] = 'r';
    term_string[6].ch[0] = '\n';
    p_fvterm.print(term_string);
    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );

    for (std::size_t i{48}; i < 54; i++)
    {
      test_vwin_area->data[i] = fchar;
      test_vwin_area->data[i].attr.bit.char_width = 1 & 0x03;
    }

    test_vwin_area->data[48].ch[0] = 'V';
    test_vwin_area->data[49].ch[0] = 'e';
    test_vwin_area->data[50].ch[0] = 'c';
    test_vwin_area->data[51].ch[0] = 't';
    test_vwin_area->data[52].ch[0] = 'o';
    test_vwin_area->data[53].ch[0] = 'r';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    CPPUNIT_ASSERT ( term_string.size() == 7 );
    term_string.clear();
    CPPUNIT_ASSERT ( term_string.size() == 0 );
    CPPUNIT_ASSERT ( term_string.empty() );
    p_fvterm.print(term_string);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(vwin, term_string);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    // Full-Width Characters
    term_string.assign(1, fchar);
    term_string[0].ch[0] = L'🏠';
    term_string[0].attr.bit.char_width = 2 & 0x03;
    p_fvterm.print(vwin, term_string);
    CPPUNIT_ASSERT ( p_fvterm.print(nullptr, L'⌚') == -1 );
    p_fvterm.print(fchar);
    CPPUNIT_ASSERT ( vwin->changes[4].xmin == 0 );
    CPPUNIT_ASSERT ( vwin->changes[4].xmax == 1 );  // padding char or '.'

    if ( enc == finalcut::Encoding::VT100 )
    {
      test_vwin_area->data[64] = fchar;
      test_vwin_area->data[64].ch[0] = L'🏠';
      test_vwin_area->data[64].attr.bit.char_width = 2 & 0x03;
      test_vwin_area->data[65] = fchar;
      test_vwin_area->data[65].ch[0] = L'.';
      test_vwin_area->data[65].attr.bit.char_width = 1 & 0x03;
      test_vwin_area->data[66] = fchar;
    }
    else if ( enc == finalcut::Encoding::UTF8 )
    {
      test_vwin_area->data[64] = fchar;
      test_vwin_area->data[64].ch[0] = L'🏠';
      test_vwin_area->data[64].attr.bit.char_width = 2 & 0x03;
      test_vwin_area->data[65] = fchar;
      test_vwin_area->data[65].attr.bit.fullwidth_padding = true;
      test_vwin_area->data[66] = fchar;
    }

    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(nullptr, fchar);
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    // Printing text in the bottom row
    int last_row = vwin->size.height + vwin->shadow.height;
    CPPUNIT_ASSERT ( last_row == 11 );
    p_fvterm.print(finalcut::FPoint{1, last_row});

    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor.y == last_row );
    CPPUNIT_ASSERT ( p_fvterm.print("Disqualification") == 16 );
    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor.y == last_row );  // Scrolling up was prevented

    test_vwin_area->data[160].ch[0] = L'D';
    test_vwin_area->data[161].ch[0] = L'i';
    test_vwin_area->data[162].ch[0] = L's';
    test_vwin_area->data[163].ch[0] = L'q';
    test_vwin_area->data[164].ch[0] = L'u';
    test_vwin_area->data[165].ch[0] = L'a';
    test_vwin_area->data[166].ch[0] = L'l';
    test_vwin_area->data[167].ch[0] = L'i';
    test_vwin_area->data[168].ch[0] = L'f';
    test_vwin_area->data[169].ch[0] = L'i';
    test_vwin_area->data[170].ch[0] = L'c';
    test_vwin_area->data[171].ch[0] = L'a';
    test_vwin_area->data[172].ch[0] = L't';
    test_vwin_area->data[173].ch[0] = L'i';
    test_vwin_area->data[174].ch[0] = L'o';
    test_vwin_area->data[175].ch[0] = L'n';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print(finalcut::FPoint{1, 6});
    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 6 );

    // Combined unicode characters
    if ( enc == finalcut::Encoding::VT100 )
    {
      CPPUNIT_ASSERT ( p_fvterm.print(L"STARGΛ̊TE") == 9 );
      CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
      test_vwin_area->data[80].ch[0] = L'S';
      test_vwin_area->data[81].ch[0] = L'T';
      test_vwin_area->data[82].ch[0] = L'A';
      test_vwin_area->data[83].ch[0] = L'R';
      test_vwin_area->data[84].ch[0] = L'G';
      test_vwin_area->data[85].ch[0] = L'Λ';
      test_vwin_area->data[86].ch[0] = L'\U0000030a';
      test_vwin_area->data[87].ch[0] = L'T';
      test_vwin_area->data[88].ch[0] = L'E';
    }
    else if ( enc == finalcut::Encoding::UTF8 )
    {
      CPPUNIT_ASSERT ( p_fvterm.print(L"STARGΛ̊TE") == 8 );
      CPPUNIT_ASSERT ( vwin->cursor.x == 9 );
      test_vwin_area->data[80].ch[0] = L'S';
      test_vwin_area->data[81].ch[0] = L'T';
      test_vwin_area->data[82].ch[0] = L'A';
      test_vwin_area->data[83].ch[0] = L'R';
      test_vwin_area->data[84].ch[0] = L'G';
      test_vwin_area->data[85].ch[0] = L'Λ';
      test_vwin_area->data[85].ch[1] = L'\U0000030a';
      test_vwin_area->data[86].ch[0] = L'T';
      test_vwin_area->data[87].ch[0] = L'E';
    }

    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    // Line break
    p_fvterm.print(finalcut::FPoint{11, 7});
    CPPUNIT_ASSERT ( vwin->cursor.x == 11 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 7 );
    CPPUNIT_ASSERT ( p_fvterm.print(L"FINAL CUT") == 9 );
    CPPUNIT_ASSERT ( vwin->cursor.x == 4 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 8 );
    test_vwin_area->data[106].ch[0] = L'F';
    test_vwin_area->data[107].ch[0] = L'I';
    test_vwin_area->data[108].ch[0] = L'N';
    test_vwin_area->data[109].ch[0] = L'A';
    test_vwin_area->data[110].ch[0] = L'L';
    test_vwin_area->data[112].ch[0] = L'C';
    test_vwin_area->data[113].ch[0] = L'U';
    test_vwin_area->data[114].ch[0] = L'T';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    // Stream
    p_fvterm.print() << ' ';  // char
    CPPUNIT_ASSERT ( vwin->cursor.x == 5 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 8 );
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << 1.23;  // double
    CPPUNIT_ASSERT ( vwin->cursor.x == 13 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 8 );
    test_vwin_area->data[116].ch[0] = L'1';
    test_vwin_area->data[117].ch[0] = L'.';
    test_vwin_area->data[118].ch[0] = L'2';
    test_vwin_area->data[119].ch[0] = L'3';
    test_vwin_area->data[120].ch[0] = L'0';
    test_vwin_area->data[121].ch[0] = L'0';
    test_vwin_area->data[122].ch[0] = L'0';
    test_vwin_area->data[123].ch[0] = L'0';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << L' ';  // wchar_t
    CPPUNIT_ASSERT ( vwin->cursor.x == 14 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 8 );
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    wchar_t kilohertz[] = L"kHz";  // wchar_t*
    p_fvterm.print() << kilohertz;
    CPPUNIT_ASSERT ( vwin->cursor.x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 9 );
    test_vwin_area->data[125].ch[0] = L'k';
    test_vwin_area->data[126].ch[0] = L'H';
    test_vwin_area->data[127].ch[0] = L'z';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << finalcut::UniChar::BlackRightPointingPointer;  // UniChar
    CPPUNIT_ASSERT ( vwin->cursor.x == 2 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 9 );
    test_vwin_area->data[128].ch[0] = L'►';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << std::string("fan");  // std::string
    CPPUNIT_ASSERT ( vwin->cursor.x == 5 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 9 );
    test_vwin_area->data[129].ch[0] = L'f';
    test_vwin_area->data[130].ch[0] = L'a';
    test_vwin_area->data[131].ch[0] = L'n';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << std::wstring(L"tas");  // std::wstring
    CPPUNIT_ASSERT ( vwin->cursor.x == 8 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 9 );
    test_vwin_area->data[132].ch[0] = L't';
    test_vwin_area->data[133].ch[0] = L'a';
    test_vwin_area->data[134].ch[0] = L's';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << finalcut::FString(L"tic");  // FString
    CPPUNIT_ASSERT ( vwin->cursor.x == 11 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 9 );
    test_vwin_area->data[135].ch[0] = L't';
    test_vwin_area->data[136].ch[0] = L'i';
    test_vwin_area->data[137].ch[0] = L'c';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << finalcut::FPoint(3, 5);  // FPoint
    CPPUNIT_ASSERT ( vwin->cursor.x == 3 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );

    fchar.ch[0] = L'y';
    fchar.ch[1] = L'\U00000304';
    fchar.ch[2] = L'\0';
    fchar.attr.bit.char_width = 1 & 0x03;
    p_fvterm.print() << fchar;  // FChar
    CPPUNIT_ASSERT ( vwin->cursor.x == 4 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );
    test_vwin_area->data[66].ch[0] = L'y';
    test_vwin_area->data[66].ch[1] = L'\U00000304';
    test_vwin_area->data[66].attr.bit.char_width = 1 & 0x03;
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    fchar.ch[0] = L'-';
    fchar.ch[1] = L'\0';
    fchar.attr.byte[0] = 0;
    fchar.color.pair.fg = finalcut::FColor::Default;
    fchar.color.pair.bg = finalcut::FColor::Default;
    finalcut::FVTerm::FCharVector dash{2, fchar};
    p_fvterm.print() << dash;  // FCharVector
    CPPUNIT_ASSERT ( vwin->cursor.x == 6 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );
    test_vwin_area->data[67].ch[0] = L'-';
    test_vwin_area->data[68].ch[0] = L'-';
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << finalcut::FStyle ( finalcut::Style::Italic
                                         | finalcut::Style::DoubleUnderline );  // FStyle
    CPPUNIT_ASSERT ( vwin->cursor.x == 6 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );
    p_fvterm.print() << "F";
    CPPUNIT_ASSERT ( vwin->cursor.x == 7 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );
    test_vwin_area->data[69].ch[0] = L'F';
    test_vwin_area->data[69].attr.bit.italic = true;
    test_vwin_area->data[69].attr.bit.dbl_underline = true;
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    finalcut::FColorPair cpair{finalcut::FColor::Blue, finalcut::FColor::White};
    p_fvterm.print() << cpair;  // FColorPair
    CPPUNIT_ASSERT ( vwin->cursor.x == 7 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );
    p_fvterm.print() << "C";
    CPPUNIT_ASSERT ( vwin->cursor.x == 8 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );
    test_vwin_area->data[70].ch[0] = L'C';
    test_vwin_area->data[70].color.pair.fg = finalcut::FColor::Blue;
    test_vwin_area->data[70].color.pair.bg = finalcut::FColor::White;
    test_vwin_area->data[70].attr.bit.italic = true;
    test_vwin_area->data[70].attr.bit.dbl_underline = true;
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );

    fvtermbuffer.print("++");
    p_fvterm.print() << fvtermbuffer;  // FVTermBuffer
    CPPUNIT_ASSERT ( vwin->cursor.x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor.y == 5 );
    test_vwin_area->data[71] = test_vwin_area->data[70];
    test_vwin_area->data[71].ch[0] = L'+';
    test_vwin_area->data[72] = test_vwin_area->data[71];
    CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
    test::printArea (vwin);
  }  // Encoding loop
}

//----------------------------------------------------------------------
void FVTermTest::FVTermChildAreaPrintTest()
{
  FVTerm_protected p_fvterm(finalcut::outputClass<FTermOutputTest>{});

  // virtual window
  auto&& vwin = p_fvterm.getVWin();

  // virtual terminal
  auto&& vterm = p_fvterm.p_getVirtualTerminal();

  // Create the virtual window for the p_fvterm object
  finalcut::FRect geometry {finalcut::FPoint{34, 1}, finalcut::FSize{12, 12}};
  auto vwin_ptr = p_fvterm.p_createArea (geometry);
  vwin = vwin_ptr.get();
  p_fvterm.setVWin(std::move(vwin_ptr));

  // Create a child print area
  finalcut::FVTerm::FTermArea* child_print_area{nullptr};
  std::size_t w = 5;
  std::size_t h = 2;
  finalcut::FRect child_geometry{};
  child_geometry.setRect (0, 0, w, h);
  auto child_print_area_ptr = p_fvterm.p_createArea (child_geometry);
  child_print_area = child_print_area_ptr.get();
  CPPUNIT_ASSERT ( ! p_fvterm.p_hasChildPrintArea() );
  CPPUNIT_ASSERT ( p_fvterm.p_getChildPrintArea() == nullptr );
  p_fvterm.p_setChildPrintArea (child_print_area);
  CPPUNIT_ASSERT ( p_fvterm.p_hasChildPrintArea() );
  CPPUNIT_ASSERT ( p_fvterm.p_getChildPrintArea() != nullptr );
  CPPUNIT_ASSERT ( p_fvterm.p_getChildPrintArea() == child_print_area );

  CPPUNIT_ASSERT ( ! p_fvterm.hasPreprocessingHandler(&p_fvterm) );;
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 0 );
  p_fvterm.addPreprocessingHandler
  (
    F_PREPROC_HANDLER (&p_fvterm, &FVTerm_protected::Preprocessing)
  );

  CPPUNIT_ASSERT ( p_fvterm.hasPreprocessingHandler(&p_fvterm) );
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 0 );
  CPPUNIT_ASSERT ( vwin );

  if ( ! vwin )
  {
    CPPUNIT_ASSERT ( false );
    return;
  }

  CPPUNIT_ASSERT ( ! vwin->visible );
  vwin->visible = true;  // show()
  CPPUNIT_ASSERT ( vwin->visible );
  CPPUNIT_ASSERT ( ! vterm->has_changes );
  p_fvterm.p_addLayer(vwin);
  CPPUNIT_ASSERT ( vterm->has_changes );
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 1 );
  p_fvterm.p_addLayer(vwin);
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 2 );
  p_fvterm.value_ref() *= 5;
  p_fvterm.p_addLayer(vwin);
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 11 );
  vwin->visible = false;  // hide()
  p_fvterm.p_addLayer(vwin);
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 11 );
  p_fvterm.p_addLayer(nullptr);
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 11 );

  for (auto i{0}; i < vterm->size.height; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 80 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 0 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  CPPUNIT_ASSERT ( ! vwin->has_changes );
  p_fvterm.print() << finalcut::FColorPair(finalcut::FColor::Red, finalcut::FColor::White)
                   << finalcut::FPoint(36, 4)  << L"=========="
                   << finalcut::FPoint(36, 5)  << L"=        ="
                   << finalcut::FPoint(36, 6)  << L"=        ="
                   << finalcut::FPoint(36, 7)  << L"=        ="
                   << finalcut::FPoint(36, 8)  << L"=        ="
                   << finalcut::FPoint(36, 9)  << L"=        ="
                   << finalcut::FPoint(36, 10) << L"=        ="
                   << finalcut::FPoint(36, 11) << L"==========";
  CPPUNIT_ASSERT ( vwin->has_changes );
  test::printArea (vwin);

  p_fvterm.p_addLayer(vwin);
  vwin->visible = true;  // show()
  vterm->has_changes = false;
  p_fvterm.p_addLayer(vwin);
  CPPUNIT_ASSERT ( vterm->has_changes );

  for (auto i{0}; i < 3; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 80 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 0 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  for (auto i{3}; i < 11; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 35 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 44 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  for (auto i{11}; i < 24; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 80 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 0 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  finalcut::FChar space_char_1 =
  {
    { L' ', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Default, finalcut::FColor::Default } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar space_char_2 =
  {
    { L' ', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Red, finalcut::FColor::White } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar equal_sign_char =
  {
    { L'=', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Red, finalcut::FColor::White } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  // Create a vterm comparison area
  finalcut::FVTerm::FTermArea* test_vterm_area{};
  auto vterm_geometry = finalcut::FRect( finalcut::FPoint{0, 0}
                                       , finalcut::FSize{std::size_t(vterm->size.width), std::size_t(vterm->size.height)} );
  auto test_vterm_area_ptr = p_fvterm.p_createArea (vterm_geometry);
  test_vterm_area = test_vterm_area_ptr.get();

  // Check the virtual terminal
  //                                .--------------- line repetitions
  //                                |     .--------- column repetitions
  //                                |     |
  test::printOnArea (test_vterm_area, { { 3, { {80, space_char_1} } },
                                      { 1, { {35, space_char_1}, {10, equal_sign_char}, { 35, space_char_1} } },
                                      { 6, { {35, space_char_1}, { 1, equal_sign_char}, {  8, space_char_2}, { 1, equal_sign_char}, { 35, space_char_1} } },
                                      { 1, { {35, space_char_1}, {10, equal_sign_char}, { 35, space_char_1} } },
                                      {13, { {80, space_char_1} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vterm_area, vterm) );
  test::printArea (vterm);

  p_fvterm.delPreprocessingHandler(&p_fvterm);
  p_fvterm.p_setChildPrintArea (nullptr);
}

//----------------------------------------------------------------------
void FVTermTest::FVTermScrollTest()
{
  FVTerm_protected p_fvterm(finalcut::outputClass<FTermOutputTest>{});

  // virtual window
  auto vwin = p_fvterm.getVWin();

  // Create the virtual window for the p_fvterm object
  finalcut::FRect geometry {finalcut::FPoint{0, 0}, finalcut::FSize{5, 5}};
  auto vwin_ptr = p_fvterm.p_createArea (geometry);
  vwin = vwin_ptr.get();
  p_fvterm.setVWin(std::move(vwin_ptr));

  p_fvterm.print() << finalcut::FPoint{1, 1}
                   << "1111122222333334444455555";

  // Create a vwin comparison area
  finalcut::FVTerm::FTermArea* test_vwin_area{};
  auto vwin_geometry = finalcut::FRect( finalcut::FPoint{0, 0}, finalcut::FSize{5, 5} );
  auto test_vwin_area_ptr = p_fvterm.p_createArea (vwin_geometry);
  test_vwin_area = test_vwin_area_ptr.get();

  finalcut::FChar one_char =
  {
    { L'1', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Default, finalcut::FColor::Default } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar two_char = one_char;
  two_char.ch[0] = L'2';
  finalcut::FChar three_char = one_char;
  three_char.ch[0] = L'3';
  finalcut::FChar four_char = one_char;
  four_char.ch[0] = L'4';
  finalcut::FChar five_char = one_char;
  five_char.ch[0] = L'5';
  finalcut::FChar space_char = one_char;
  space_char.ch[0] = L' ';

  // Scroll forward

  test::printOnArea (test_vwin_area, { {5, one_char},
                                       {5, two_char},
                                       {5, three_char},
                                       {5, four_char},
                                       {5, five_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  test::printOnArea (test_vwin_area, { {5, two_char},
                                       {5, three_char},
                                       {5, four_char},
                                       {5, five_char},
                                       {5, space_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  test::printOnArea (test_vwin_area, { {5, three_char},
                                       {5, four_char},
                                       {5, five_char},
                                       {5, space_char},
                                       {5, space_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  test::printOnArea (test_vwin_area, { {5, four_char},
                                       {5, five_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, space_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  test::printOnArea (test_vwin_area, { {5, five_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, space_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  test::printOnArea (test_vwin_area, { {5, space_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, space_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  // Scroll reverse

  p_fvterm.print() << finalcut::FPoint{1, 1}
                   << "1111122222333334444455555";
  test::printOnArea (test_vwin_area, { {5, one_char},
                                       {5, two_char},
                                       {5, three_char},
                                       {5, four_char},
                                       {5, five_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  test::printOnArea (test_vwin_area, { {5, space_char},
                                       {5, one_char},
                                       {5, two_char},
                                       {5, three_char},
                                       {5, four_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  test::printOnArea (test_vwin_area, { {5, space_char},
                                       {5, space_char},
                                       {5, one_char},
                                       {5, two_char},
                                       {5, three_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  test::printOnArea (test_vwin_area, { {5, space_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, one_char},
                                       {5, two_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  test::printOnArea (test_vwin_area, { {5, space_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, one_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  test::printOnArea (test_vwin_area, { {5, space_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, space_char},
                                       {5, space_char} } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin) );
  test::printArea (vwin);

  // vdesktop scrolling

  auto&& vdesktop = p_fvterm.p_getVirtualDesktop();
  test::printOnArea (vdesktop, { { 5, { {80, space_char} } },
                                 { 1, { {80, one_char} } },
                                 { 5, { {80, space_char} } },
                                 { 1, { {80, one_char} } },
                                 { 5, { {80, space_char} } },
                                 { 1, { {80, one_char} } },
                                 { 6, { {80, space_char} } } } );

  // Create a vdesktop comparison area
  finalcut::FVTerm::FTermArea* test_vdesktop{};
  auto vdesktop_geometry = finalcut::FRect( finalcut::FPoint{0, 0}, finalcut::FSize{80, 24} );
  auto test_vdesktop_ptr = p_fvterm.p_createArea (vdesktop_geometry);
  test_vdesktop = test_vdesktop_ptr.get();

  test::printOnArea (test_vdesktop, { { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 6, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vdesktop, vdesktop) );
  test::printArea (vdesktop);

  FTermOutputTest::setNoForce(true);
  p_fvterm.p_scrollAreaForward (vdesktop);
  FTermOutputTest::setNoForce(false);
  test::printOnArea (test_vdesktop, { { 4, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 7, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vdesktop, vdesktop) );
  test::printArea (vdesktop);

  FTermOutputTest::setNoForce(true);
  p_fvterm.p_scrollAreaForward (vdesktop);
  p_fvterm.p_scrollAreaForward (vdesktop);
  FTermOutputTest::setNoForce(false);
  test::printOnArea (test_vdesktop, { { 2, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 9, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vdesktop, vdesktop) );
  test::printArea (vdesktop);

  FTermOutputTest::setNoForce(true);
  p_fvterm.p_scrollAreaReverse (vdesktop);
  FTermOutputTest::setNoForce(false);
  test::printOnArea (test_vdesktop, { { 3, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 8, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vdesktop, vdesktop) );
  test::printArea (vdesktop);

  for (auto i{0}; i < 6; i++)
  {
    FTermOutputTest::setNoForce(true);
    p_fvterm.p_scrollAreaReverse (vdesktop);
    FTermOutputTest::setNoForce(false);
  }

  test::printOnArea (test_vdesktop, { { 9, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 5, { {80, space_char} } },
                                      { 1, { {80, one_char} } },
                                      { 2, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vdesktop, vdesktop) );
  test::printArea (vdesktop);
}

//----------------------------------------------------------------------
void FVTermTest::FVTermOverlappingWindowsTest()
{
  //   1       2       3       4
  // ░░░░░░  ▒▒▒▒▒▒  ▓▓▓▓▓▓  ██████
  // ░░░░░░  ▒▒▒▒▒▒  ▓▓▓▓▓▓  ██████
  // ░░░░░░  ▒▒▒▒▒▒  ▓▓▓▓▓▓  ██████
  //
  // 1 : color overlay
  // 2 : inherit background
  // 3 : transparency
  // 4 : normal

  FVTerm_protected p_fvterm_1(finalcut::outputClass<FTermOutputTest>{});
  FVTerm_protected p_fvterm_2(finalcut::outputClass<FTermOutputTest>{});
  FVTerm_protected p_fvterm_3(finalcut::outputClass<FTermOutputTest>{});
  FVTerm_protected p_fvterm_4(finalcut::outputClass<FTermOutputTest>{});

  // unique virtual terminal
  auto&& vterm = p_fvterm_1.p_getVirtualTerminal();

  // virtual windows
  auto vwin_1 = p_fvterm_1.getVWin();
  auto vwin_2 = p_fvterm_2.getVWin();
  auto vwin_3 = p_fvterm_3.getVWin();
  auto vwin_4 = p_fvterm_4.getVWin();

  // Create the virtual windows for the p_fvterm_1..4 objects
  finalcut::FRect geometry_1 {finalcut::FPoint{0, 0}, finalcut::FSize{6, 3}};
  finalcut::FRect geometry_2 {finalcut::FPoint{8, 0}, finalcut::FSize{6, 3}};
  finalcut::FRect geometry_3 {finalcut::FPoint{16, 0}, finalcut::FSize{6, 3}};
  finalcut::FRect geometry_4 {finalcut::FPoint{24, 0}, finalcut::FSize{6, 3}};
  auto vwin_1_ptr = p_fvterm_1.p_createArea (geometry_1);
  auto vwin_2_ptr = p_fvterm_2.p_createArea (geometry_2);
  auto vwin_3_ptr = p_fvterm_3.p_createArea (geometry_3);
  auto vwin_4_ptr = p_fvterm_4.p_createArea (geometry_4);
  vwin_1 = vwin_1_ptr.get();
  vwin_2 = vwin_2_ptr.get();
  vwin_3 = vwin_3_ptr.get();
  vwin_4 = vwin_4_ptr.get();
  p_fvterm_1.setVWin(std::move(vwin_1_ptr));
  p_fvterm_2.setVWin(std::move(vwin_2_ptr));
  p_fvterm_3.setVWin(std::move(vwin_3_ptr));
  p_fvterm_4.setVWin(std::move(vwin_4_ptr));

  CPPUNIT_ASSERT ( p_fvterm_1.getWindowList()->empty() );
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_1);
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_2);
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_3);
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_4);
  CPPUNIT_ASSERT ( ! finalcut::FVTerm::getWindowList()->empty() );
  CPPUNIT_ASSERT ( finalcut::FVTerm::getWindowList()->size() == 4U );

  // Fill window with content
  p_fvterm_1.print() << finalcut::FPoint{1, 1}
                     << finalcut::FColorPair {finalcut::FColor::Black, finalcut::FColor::White}
                     << finalcut::FStyle {finalcut::Style::ColorOverlay}
                     << "░░░░░░░░░░░░░░░░░░"
                     << finalcut::FStyle {finalcut::Style::None};

  p_fvterm_2.print() << finalcut::FPoint{9, 1}
                     << finalcut::FColorPair {finalcut::FColor::Black, finalcut::FColor::White}
                     << finalcut::FStyle {finalcut::Style::InheritBackground}
                     << "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒"
                     << finalcut::FStyle {finalcut::Style::None};

  p_fvterm_3.print() << finalcut::FPoint{17, 1}
                     << finalcut::FColorPair {finalcut::FColor::Black, finalcut::FColor::White}
                     << finalcut::FStyle {finalcut::Style::Transparent}
                     << "▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓"
                     << finalcut::FStyle {finalcut::Style::None};

  p_fvterm_4.print() << finalcut::FPoint{25, 1}
                     << finalcut::FColorPair {finalcut::FColor::Black, finalcut::FColor::White}
                     << "██████████████████";
  test::printArea (vwin_1);
  test::printArea (vwin_2);
  test::printArea (vwin_3);
  test::printArea (vwin_4);
  vwin_1->visible = true;
  vwin_2->visible = true;
  vwin_3->visible = true;
  vwin_4->visible = true;

  CPPUNIT_ASSERT ( vwin_1->layer == -1 );
  CPPUNIT_ASSERT ( vwin_2->layer == -1 );
  CPPUNIT_ASSERT ( vwin_3->layer == -1 );
  CPPUNIT_ASSERT ( vwin_4->layer == -1 );
  p_fvterm_1.p_determineWindowLayers();
  CPPUNIT_ASSERT ( vwin_1->layer == 1 );
  CPPUNIT_ASSERT ( vwin_2->layer == 2 );
  CPPUNIT_ASSERT ( vwin_3->layer == 3 );
  CPPUNIT_ASSERT ( vwin_4->layer == 4 );

  CPPUNIT_ASSERT ( finalcut::FVTerm::getWindowList()->size() == 4U );

  // virtual desktop
  auto&& vdesktop = p_fvterm_1.p_getVirtualDesktop();
  p_fvterm_1.setColor (finalcut::FColor::DarkGray, finalcut::FColor::LightBlue);
  p_fvterm_1.p_clearArea (vdesktop, L'.');

  // Write changes to the virtual terminal
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  // Create a comparison area
  finalcut::FVTerm::FTermArea* test_area{};
  auto geometry = finalcut::FRect( finalcut::FPoint{0, 0}, finalcut::FSize{80, 24} );
  auto test_area_ptr = p_fvterm_1.p_createArea (geometry);
  test_area = test_area_ptr.get();

  finalcut::FChar bg_char =
  {
    { L'.', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::DarkGray, finalcut::FColor::LightBlue } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printOnArea (test_area, { 24, { {80, bg_char} } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vdesktop) );

  bg_char.attr.byte[2] = 10;

  finalcut::FChar vwin_1_char =  // with color overlay
  {
    { L'.', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Black, finalcut::FColor::White } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar vwin_2_char =  // with inherit background
  {
    { L'▒', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Black, finalcut::FColor::LightBlue } },
    { { 0x00, 0x80, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar vwin_3_char =  // with transparency
  {
    { L'.', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::DarkGray, finalcut::FColor::LightBlue } },
    { { 0x00, 0x00, 0x09, 0x00} }  // byte 0..3
  };

  finalcut::FChar vwin_4_char =
  {
    { L'█', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Black, finalcut::FColor::White } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printOnArea (test_area, { {  3, { {6, vwin_1_char}, {2, bg_char}, {6, vwin_2_char}, {2, bg_char}, {6, vwin_3_char}, {2, bg_char}, {6, vwin_4_char}, {50, bg_char} } },
                                  { 21, { {80, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  //     ░░░░░░
  //     ░░░░░░              1
  // ▒▒▒▒░░░░░░▓▓▓▓
  // ▒▒▒▒▒▒  ▓▓▓▓▓▓      2       3
  // ▒▒▒▒██████▓▓▓▓
  //     ██████              4
  //     ██████
  //
  // 1 : color overlay
  // 2 : inherit background
  // 3 : transparency
  // 4 : normal

  test::moveArea(vwin_1, finalcut::FPoint{4, 0});
  test::moveArea(vwin_2, finalcut::FPoint{0, 2});
  test::moveArea(vwin_3, finalcut::FPoint{8, 2});
  test::moveArea(vwin_4, finalcut::FPoint{4, 4});
  p_fvterm_1.p_restoreVTerm(geometry);
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  bg_char.attr.bit.printed = false;
  auto vwin_1_2_char = vwin_2_char;
  vwin_1_2_char.color.pair.bg = finalcut::FColor::White;
  vwin_3_char.attr.bit.no_changes = false;
  auto vwin_2_4_char = vwin_4_char;
  test::printOnArea (test_area, { {  2, { {4, bg_char}, {6, vwin_1_char}, {70, bg_char} } },
                                  {  1, { {4, vwin_2_char}, {2, vwin_1_2_char}, {2, vwin_1_char}, {2, vwin_1_char}, {4, vwin_3_char}, {66, bg_char} } },
                                  {  1, { {6, vwin_2_char}, {2, bg_char}, {6, vwin_3_char}, {66, bg_char} } },
                                  {  1, { {4, vwin_2_char}, {2, vwin_2_4_char}, {4, vwin_4_char}, {4, vwin_3_char}, {66, bg_char} } },
                                  {  2, { {4, bg_char}, {6, vwin_4_char}, {70, bg_char} } },
                                  { 17, { {80, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  //     ░░░░░░
  // ▒▒▒▒░░░░░░              1          4  top
  // ▒▒▒▒░░▓▓▓▓▓▓        2              3
  // ▒▒▒██████▓▓▓               3       1
  //    ██████▓▓▓           4           2  bottom
  //    ██████
  //
  // 1 : color overlay
  // 2 : inherit background
  // 3 : transparency
  // 4 : normal

  test::moveArea(vwin_1, finalcut::FPoint{4, 0});
  test::moveArea(vwin_2, finalcut::FPoint{0, 1});
  test::moveArea(vwin_3, finalcut::FPoint{6, 2});
  test::moveArea(vwin_4, finalcut::FPoint{3, 3});
  p_fvterm_1.p_restoreVTerm(geometry);
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  test::printOnArea (test_area, { {  1, { {4, bg_char}, {6, vwin_1_char}, {70, bg_char} } },
                                  {  1, { {4, vwin_2_char}, {2, vwin_1_2_char}, {4, vwin_1_char}, {70, bg_char} } },
                                  {  1, { {4, vwin_2_char}, {2, vwin_1_2_char}, {4, vwin_1_char}, {2, vwin_3_char}, {68, bg_char} } },
                                  {  1, { {3, vwin_2_char}, {6, vwin_4_char}, {3, vwin_3_char}, {68, bg_char} } },
                                  {  1, { {3, bg_char}, {6, vwin_4_char}, {3, vwin_3_char}, {68, bg_char} } },
                                  {  1, { {3, bg_char}, {6, vwin_4_char}, {71, bg_char} } },
                                  { 18, { {80, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  // ░░░░░▒▒▒▒▒▒                         4  top
  // ░░░░░▒▒▒▒▒▒          1     2        3
  // ██████▓▓▓▓▓                         2
  // ██████▓▓▓▓▓          4     3        1  botton
  // ██████▓▓▓▓▓
  //
  // 1 : color overlay
  // 2 : inherit background
  // 3 : transparency
  // 4 : normal

  test::moveArea(vwin_1, finalcut::FPoint{0, 0});
  test::moveArea(vwin_2, finalcut::FPoint{5, 0});
  test::moveArea(vwin_3, finalcut::FPoint{5, 2});
  test::moveArea(vwin_4, finalcut::FPoint{0, 2});
  p_fvterm_1.p_restoreVTerm(geometry);
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  test::printOnArea (test_area, { {  2, { {5, vwin_1_char}, {1, vwin_1_2_char}, {5, vwin_2_char}, {69, bg_char} } },
                                  {  1, { {6, vwin_4_char}, {5, vwin_2_char}, {69, bg_char} } },
                                  {  2, { {6, vwin_4_char}, {5, vwin_3_char}, {69, bg_char} } },
                                  { 19, { {80, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  // ░░░░░▒▒▒▒▒▒                         3  top
  // ░░░░░▒▒▒▒▒▒          1     2        4
  // █████▓▓▓▓▓▓                         2
  // █████▓▓▓▓▓▓          4     3        1  botton
  // █████▓▓▓▓▓▓
  //
  // 1 : color overlay
  // 2 : inherit background
  // 3 : transparency
  // 4 : normal
  auto window_list = finalcut::FVTerm::getWindowList();
  std::swap(window_list->at(2), window_list->at(3));
  p_fvterm_1.p_determineWindowLayers();
  p_fvterm_1.p_restoreVTerm(geometry);
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  test::printOnArea (test_area, { {  2, { {5, vwin_1_char}, {1, vwin_1_2_char}, {5, vwin_2_char}, {69, bg_char} } },
                                  {  1, { {6, vwin_4_char}, {5, vwin_2_char}, {69, bg_char} } },
                                  {  2, { {6, vwin_4_char}, {5, vwin_3_char}, {69, bg_char} } },
                                  { 19, { {80, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  // ░░░░░▒▒▒▒▒▒                         2  top
  // ░░░░░▒▒▒▒▒▒          1     2        3
  // █████▒▒▒▒▒▒                         4
  // █████▓▓▓▓▓▓          4     3        1  botton
  // █████▓▓▓▓▓▓
  //
  // 1 : color overlay
  // 2 : inherit background
  // 3 : transparency
  // 4 : normal
  std::swap(window_list->at(1), window_list->at(3));
  std::swap(window_list->at(1), window_list->at(2));
  p_fvterm_1.p_determineWindowLayers();
  p_fvterm_1.p_restoreVTerm(geometry);
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  test::printOnArea (test_area, { {  2, { {5, vwin_1_char}, {1, vwin_1_2_char}, {5, vwin_2_char}, {69, bg_char} } },
                                  {  1, { {5, vwin_4_char}, {1, vwin_1_2_char}, {5, vwin_2_char}, {69, bg_char} } },
                                  {  2, { {6, vwin_4_char}, {5, vwin_3_char}, {69, bg_char} } },
                                  { 19, { {80, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  // ░░░░░░▒▒▒▒▒                         1  top
  // ░░░░░░▒▒▒▒▒          1     2        2
  // ░░░░░░▒▒▒▒▒                         3
  // █████▓▓▓▓▓▓          4     3        4  botton
  // █████▓▓▓▓▓▓
  //
  // 1 : color overlay
  // 2 : inherit background
  // 3 : transparency
  // 4 : normal
  std::swap(window_list->at(0), window_list->at(3));
  std::swap(window_list->at(0), window_list->at(2));
  std::swap(window_list->at(0), window_list->at(1));
  p_fvterm_1.p_determineWindowLayers();
  p_fvterm_1.p_restoreVTerm(geometry);
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  auto vwin_2_1_char = vwin_1_char;
  vwin_2_1_char.ch[0] = L' ';
  auto vwin_4_1_char = vwin_1_char;
  vwin_4_1_char.ch[0] = L' ';
  test::printOnArea (test_area, { {  2, { {5, vwin_1_char}, {1, vwin_2_1_char}, {5, vwin_2_char}, {69, bg_char} } },
                                  {  1, { {5, vwin_4_1_char}, {1, vwin_2_1_char}, {5, vwin_2_char}, {69, bg_char} } },
                                  {  2, { {6, vwin_4_char}, {5, vwin_3_char}, {69, bg_char} } },
                                  { 19, { {80, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );
}

//----------------------------------------------------------------------
void FVTermTest::FVTermTranparencyTest()
{
  //     1          2          3          4          5
  // *********   *     *     *   *        │      ╲       ╱
  // *       *  *********    *   *        │       ╲     ╱
  // *       *   *     *   *********      │        ╲   ╱
  // *********   *     *     *   *        │         ╲ ╱
  // *       *  *********    *   *    ────┼────      ╳
  // *       *   *     *   *********      │         ╱ ╲
  // *********   *     *     *   *        │        ╱   ╲
  // *       *  *********    *   *        │       ╱     ╲
  // *       *   *     *   *********      │      ╱       ╲

  FVTerm_protected p_fvterm_1(finalcut::outputClass<FTermOutputTest>{});
  FVTerm_protected p_fvterm_2(finalcut::outputClass<FTermOutputTest>{});
  FVTerm_protected p_fvterm_3(finalcut::outputClass<FTermOutputTest>{});
  FVTerm_protected p_fvterm_4(finalcut::outputClass<FTermOutputTest>{});
  FVTerm_protected p_fvterm_5(finalcut::outputClass<FTermOutputTest>{});

  // unique virtual terminal
  auto&& vterm = p_fvterm_1.p_getVirtualTerminal();

  // virtual windows
  auto vwin_1 = p_fvterm_1.getVWin();
  auto vwin_2 = p_fvterm_2.getVWin();
  auto vwin_3 = p_fvterm_3.getVWin();
  auto vwin_4 = p_fvterm_4.getVWin();
  auto vwin_5 = p_fvterm_5.getVWin();

  // Create the virtual windows for the p_fvterm_1..5 objects
  finalcut::FRect geometry_1 {finalcut::FPoint{0, 0}, finalcut::FSize{9, 9}};
  finalcut::FRect geometry_2 {finalcut::FPoint{0, 0}, finalcut::FSize{9, 9}};
  finalcut::FRect geometry_3 {finalcut::FPoint{0, 0}, finalcut::FSize{9, 9}};
  finalcut::FRect geometry_4 {finalcut::FPoint{0, 0}, finalcut::FSize{9, 9}};
  finalcut::FRect geometry_5 {finalcut::FPoint{0, 0}, finalcut::FSize{9, 9}};
  auto vwin_1_ptr = p_fvterm_1.p_createArea (geometry_1);
  auto vwin_2_ptr = p_fvterm_2.p_createArea (geometry_2);
  auto vwin_3_ptr = p_fvterm_3.p_createArea (geometry_3);
  auto vwin_4_ptr = p_fvterm_4.p_createArea (geometry_4);
  auto vwin_5_ptr = p_fvterm_5.p_createArea (geometry_5);
  vwin_1 = vwin_1_ptr.get();
  vwin_2 = vwin_2_ptr.get();
  vwin_3 = vwin_3_ptr.get();
  vwin_4 = vwin_4_ptr.get();
  vwin_5 = vwin_5_ptr.get();
  p_fvterm_1.setVWin(std::move(vwin_1_ptr));
  p_fvterm_2.setVWin(std::move(vwin_2_ptr));
  p_fvterm_3.setVWin(std::move(vwin_3_ptr));
  p_fvterm_4.setVWin(std::move(vwin_4_ptr));
  p_fvterm_5.setVWin(std::move(vwin_5_ptr));

  CPPUNIT_ASSERT ( p_fvterm_1.getWindowList()->empty() );
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_1);
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_2);
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_3);
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_4);
  finalcut::FVTerm::getWindowList()->push_back(&p_fvterm_5);
  CPPUNIT_ASSERT ( ! finalcut::FVTerm::getWindowList()->empty() );
  CPPUNIT_ASSERT ( finalcut::FVTerm::getWindowList()->size() == 5U );

  // Fill window with content
  auto light_red_color = finalcut::FColorPair {finalcut::FColor::LightRed, finalcut::FColor::Black};
  auto transparent = finalcut::FStyle {finalcut::Style::Transparent};
  auto reset = finalcut::FStyle {finalcut::Style::None};
  p_fvterm_1.print() << finalcut::FPoint{1, 1}
                     << light_red_color
                     << "*********"
                     << "*" << transparent << "       " << reset << light_red_color << "*"
                     << "*" << transparent << "       " << reset << light_red_color << "*"
                     << "*********"
                     << "*" << transparent << "       " << reset << light_red_color << "*"
                     << "*" << transparent << "       " << reset << light_red_color << "*"
                     << "*********"
                     << "*" << transparent << "       " << reset << light_red_color << "*"
                     << "*" << transparent << "       " << reset << light_red_color << "*"
                     << reset;

  auto light_green_color = finalcut::FColorPair {finalcut::FColor::LightGreen, finalcut::FColor::Black};
  p_fvterm_2.print() << finalcut::FPoint{1, 1}
                     << transparent << " " << reset << light_green_color << "*" << transparent << "     " << reset << light_green_color << "*" << transparent << " "
                     << reset << light_green_color << "*********"
                     << transparent << " " << reset << light_green_color << "*" << transparent << "     " << reset << light_green_color << "*" << transparent << " "
                     << transparent << " " << reset << light_green_color << "*" << transparent << "     " << reset << light_green_color << "*" << transparent << " "
                     << reset << light_green_color << "*********"
                     << transparent << " " << reset << light_green_color << "*" << transparent << "     " << reset << light_green_color << "*" << transparent << " "
                     << transparent << " " << reset << light_green_color << "*" << transparent << "     " << reset << light_green_color << "*" << transparent << " "
                     << reset << light_green_color << "*********"
                     << transparent << " " << reset << light_green_color << "*" << transparent << "     " << reset << light_green_color << "*" << transparent << " "
                     << reset;

  auto light_blue_color = finalcut::FColorPair {finalcut::FColor::LightBlue, finalcut::FColor::Black};
  p_fvterm_3.print() << finalcut::FPoint{1, 1}
                     << finalcut::FColorPair {finalcut::FColor::LightBlue, finalcut::FColor::Black}
                     << transparent << "  " << reset << light_blue_color << "*" << transparent << "   " << reset << light_blue_color << "*" << transparent << "  "
                     << transparent << "  " << reset << light_blue_color << "*" << transparent << "   " << reset << light_blue_color << "*" << transparent << "  "
                     << reset << light_blue_color << "*********"
                     << transparent << "  " << reset << light_blue_color << "*" << transparent << "   " << reset << light_blue_color << "*" << transparent << "  "
                     << transparent << "  " << reset << light_blue_color << "*" << transparent << "   " << reset << light_blue_color << "*" << transparent << "  "
                     << reset << light_blue_color << "*********"
                     << transparent << "  " << reset << light_blue_color << "*" << transparent << "   " << reset << light_blue_color << "*" << transparent << "  "
                     << transparent << "  " << reset << light_blue_color << "*" << transparent << "   " << reset << light_blue_color << "*" << transparent << "  "
                     << reset << light_blue_color << "*********"
                     << reset;

    auto yellow_color = finalcut::FColorPair {finalcut::FColor::Yellow, finalcut::FColor::Black};
    p_fvterm_4.print() << finalcut::FPoint{1, 1}
                     << transparent << "    " << reset << yellow_color << "│"  << transparent <<"    "
                     << transparent << "    " << reset << yellow_color << "│"  << transparent <<"    "
                     << transparent << "    " << reset << yellow_color << "│"  << transparent <<"    "
                     << transparent << "    " << reset << yellow_color << "│"  << transparent <<"    "
                     << reset << yellow_color << "────┼────"
                     << transparent << "    " << reset << yellow_color << "│"  << transparent <<"    "
                     << transparent << "    " << reset << yellow_color << "│"  << transparent <<"    "
                     << transparent << "    " << reset << yellow_color << "│"  << transparent <<"    "
                     << transparent << "    " << reset << yellow_color << "│"  << transparent <<"    "
                     << reset;

  auto white_color = finalcut::FColorPair {finalcut::FColor::White, finalcut::FColor::Black};
  p_fvterm_5.print() << finalcut::FPoint{1, 1}
                     << white_color << "╲" << transparent << "       " << reset << white_color << "╱"
                     << transparent << " " << reset << white_color << "╲" << transparent << "     " << reset << white_color << "╱" << transparent << " "
                     << transparent << "  " << reset << white_color << "╲" << transparent << "   " << reset << white_color << "╱" << transparent << "  "
                     << transparent << "   " << reset << white_color << "╲" << transparent << " " << reset << white_color << "╱" << transparent << "   "
                     << transparent << "    " << reset << white_color << "╳" << transparent << "    "
                     << transparent << "   " << reset << white_color << "╱" << transparent << " " << reset << white_color << "╲" << transparent << "   "
                     << transparent << "  " << reset << white_color << "╱" << transparent << "   " << reset << white_color << "╲" << transparent << "  "
                     << transparent << " " << reset << white_color << "╱"<< transparent << "     " << reset << white_color << "╲" << transparent << " "
                     << reset << white_color << "╱" << transparent << "       " << reset << white_color << "╲"
                     << reset;

  test::printArea (vwin_1);
  test::printArea (vwin_2);
  test::printArea (vwin_3);
  test::printArea (vwin_4);
  test::printArea (vwin_5);
  vwin_1->visible = true;
  vwin_2->visible = false;
  vwin_3->visible = false;
  vwin_4->visible = false;
  vwin_5->visible = false;

  CPPUNIT_ASSERT ( vwin_1->layer == -1 );
  CPPUNIT_ASSERT ( vwin_2->layer == -1 );
  CPPUNIT_ASSERT ( vwin_3->layer == -1 );
  CPPUNIT_ASSERT ( vwin_4->layer == -1 );
  CPPUNIT_ASSERT ( vwin_5->layer == -1 );
  p_fvterm_1.p_determineWindowLayers();
  CPPUNIT_ASSERT ( vwin_1->layer == 1 );
  CPPUNIT_ASSERT ( vwin_2->layer == 2 );
  CPPUNIT_ASSERT ( vwin_3->layer == 3 );
  CPPUNIT_ASSERT ( vwin_4->layer == 4 );
  CPPUNIT_ASSERT ( vwin_5->layer == 5 );

  CPPUNIT_ASSERT ( finalcut::FVTerm::getWindowList()->size() == 5U );

  // virtual desktop
  auto&& vdesktop = p_fvterm_1.p_getVirtualDesktop();
  p_fvterm_1.setColor (finalcut::FColor::LightGray, finalcut::FColor::Black);
  p_fvterm_1.p_clearArea (vdesktop);

  // Write changes to the virtual terminal
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  // Create a comparison area
  finalcut::FVTerm::FTermArea* test_area{};
  auto geometry = finalcut::FRect( finalcut::FPoint{0, 0}, finalcut::FSize{80, 24} );
  auto test_area_ptr = p_fvterm_1.p_createArea (geometry);
  test_area = test_area_ptr.get();

  finalcut::FChar bg_char =
  {
    { L' ', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::LightGray, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printOnArea (test_area, { 24, { {80, bg_char} } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vdesktop) );

  finalcut::FChar vwin_1_char =
  {
    { L'*', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::LightRed, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printOnArea (test_area, { {  1, { {9, vwin_1_char}, {71, bg_char} } },
                                  {  2, { {1, vwin_1_char}, {7, bg_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {9, vwin_1_char}, {71, bg_char} } },
                                  {  2, { {1, vwin_1_char}, {7, bg_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {9, vwin_1_char}, {71, bg_char} } },
                                  {  2, { {1, vwin_1_char}, {7, bg_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  { 15, { {80, bg_char} } } } );

  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  vwin_2->visible = true;

  // Write changes to the virtual terminal
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  finalcut::FChar vwin_2_char =
  {
    { L'*', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::LightGreen, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printOnArea (test_area, { {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {5, vwin_1_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {9, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {5, bg_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {5, vwin_1_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {9, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {5, bg_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {5, vwin_1_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {9, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {5, bg_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  { 15, { {80, bg_char} } } } );

  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  vwin_3->visible = true;

  // Write changes to the virtual terminal
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  finalcut::FChar vwin_3_char =
  {
    { L'*', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::LightBlue, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printOnArea (test_area, { {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {1, vwin_3_char}, {3, vwin_1_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {2, vwin_2_char}, {1, vwin_3_char}, {3, vwin_2_char}, {1, vwin_3_char}, {2, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {9, vwin_3_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {1, vwin_3_char}, {3, vwin_1_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {2, vwin_2_char}, {1, vwin_3_char}, {3, vwin_2_char}, {1, vwin_3_char}, {2, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {9, vwin_3_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {1, vwin_3_char}, {3, vwin_1_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {2, vwin_2_char}, {1, vwin_3_char}, {3, vwin_2_char}, {1, vwin_3_char}, {2, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {9, vwin_3_char}, {71, bg_char} } },
                                  { 15, { {80, bg_char} } } } );

  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  vwin_4->visible = true;

  // Write changes to the virtual terminal
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  finalcut::FChar vertical_line_char =  // │
  {
    { L'│', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Yellow, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar horizontal_line_char =  // ─
  {
    { L'─', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Yellow, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar plus_line_char =  // ┼
  {
    { L'┼', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Yellow, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printOnArea (test_area, { {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {1, vwin_3_char}, {1, vwin_1_char}, {1, vertical_line_char}, {1, vwin_1_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {2, vwin_2_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vertical_line_char}, {1, vwin_2_char}, {1, vwin_3_char}, {2, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {4, vwin_3_char}, {1, vertical_line_char}, {4, vwin_3_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {1, vwin_3_char}, {1, vwin_1_char}, {1, vertical_line_char}, {1, vwin_1_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {4, horizontal_line_char}, {1, plus_line_char}, {4, horizontal_line_char},  {71, bg_char} } },
                                  {  1, { {4, vwin_3_char}, {1, vertical_line_char}, {4, vwin_3_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {1, vwin_3_char}, {1, vwin_1_char}, {1, vertical_line_char}, {1, vwin_1_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {2, vwin_2_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vertical_line_char}, {1, vwin_2_char}, {1, vwin_3_char}, {2, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {4, vwin_3_char}, {1, vertical_line_char}, {4, vwin_3_char}, {71, bg_char} } },
                                  { 15, { {80, bg_char} } } } );

  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );

  vwin_5->visible = true;

  // Write changes to the virtual terminal
  p_fvterm_1.p_processTerminalUpdate();
  test::printArea (vterm);

  finalcut::FChar diagonal_1_line_char =  // ╲
  {
    { L'╲', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::White, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar diagonal_2_line_char =  // ╱
  {
    { L'╱', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::White, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar cross_line_char =  // ╳
  {
    { L'╳', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::White, finalcut::FColor::Black } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printOnArea (test_area, { {  1, { {1, diagonal_1_line_char}, {1, vwin_2_char}, {1, vwin_3_char}, {1, vwin_1_char}, {1, vertical_line_char}, {1, vwin_1_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, diagonal_2_line_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_2_char}, {1, diagonal_1_line_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vertical_line_char}, {1, vwin_2_char}, {1, vwin_3_char}, {1, diagonal_2_line_char}, {1, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {2, vwin_3_char}, {1, diagonal_1_line_char}, {1, vwin_3_char}, {1, vertical_line_char}, {1, vwin_3_char}, {1, diagonal_2_line_char}, {2, vwin_3_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {1, vwin_3_char}, {1, diagonal_1_line_char}, {1, vertical_line_char}, {1, diagonal_2_line_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {4, horizontal_line_char}, {1, cross_line_char}, {4, horizontal_line_char},  {71, bg_char} } },
                                  {  1, { {3, vwin_3_char}, {1, diagonal_2_line_char}, {1, vertical_line_char}, {1, diagonal_1_line_char}, {3, vwin_3_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_1_char}, {1, vwin_2_char}, {1, diagonal_2_line_char}, {1, vwin_1_char}, {1, vertical_line_char}, {1, vwin_1_char}, {1, diagonal_1_line_char}, {1, vwin_2_char}, {1, vwin_1_char}, {71, bg_char} } },
                                  {  1, { {1, vwin_2_char}, {1, diagonal_2_line_char}, {1, vwin_3_char}, {1, vwin_2_char}, {1, vertical_line_char}, {1, vwin_2_char}, {1, vwin_3_char}, {1, diagonal_1_line_char}, {1, vwin_2_char}, {71, bg_char} } },
                                  {  1, { {1, diagonal_2_line_char}, {3, vwin_3_char}, {1, vertical_line_char}, {3, vwin_3_char}, {1, diagonal_1_line_char}, {71, bg_char} } },
                                  { 15, { {80, bg_char} } } } );

  CPPUNIT_ASSERT ( test::isAreaEqual(test_area, vterm) );
}

//----------------------------------------------------------------------
void FVTermTest::FVTermReduceUpdatesTest()
{
  FVTerm_protected p_fvterm(finalcut::outputClass<FTermOutputTest>{});

  // unique virtual terminal
  auto vterm = p_fvterm.p_getVirtualTerminal();

  // virtual windows
  auto vwin = p_fvterm.getVWin();

  // Create the virtual windows for the p_fvterm objects
  finalcut::FRect geometry {finalcut::FPoint{0, 0}, finalcut::FSize{15, 15}};
  auto vwin_ptr = p_fvterm.p_createArea (geometry);
  vwin = vwin_ptr.get();
  p_fvterm.setVWin(std::move(vwin_ptr));

  p_fvterm.print() << finalcut::FPoint{1, 1}
                   << finalcut::FColorPair { finalcut::FColor::Black
                                           , finalcut::FColor::White };
  p_fvterm.print() << "!\"#$%&'()*+,-./";  // Line 0
  p_fvterm.print() << "0123456789:;<=>";   // Line 1
  p_fvterm.print() << "?@ABCDEFGHIJKLM";   // Line 2
  p_fvterm.print() << "NOPQRSTUVWXYZ[\\";  // Line 3
  p_fvterm.print() << "]^_`abcdefghijk";   // Line 4
  p_fvterm.print() << "lmnopqrstuvwxyz";   // Line 5
  p_fvterm.print() << "!\"#$%&'()*+,-./";  // Line 6
  p_fvterm.print() << "0123456789:;<=>";   // Line 7
  p_fvterm.print() << "?@ABCDEFGHIJKLM";   // Line 8
  p_fvterm.print() << "NOPQRSTUVWXYZ[\\";  // Line 9
  p_fvterm.print() << "]^_`abcdefghijk";   // Line 10
  p_fvterm.print() << "lmnopqrstuvwxyz";   // Line 11
  p_fvterm.print() << "!\"#$%&'()*+,-./";  // Line 12
  p_fvterm.print() << "0123456789:;<=>";   // Line 13
  p_fvterm.print() << "?@ABCDEFGHIJKLM";   // Line 14
  vwin->visible = true;
  p_fvterm.p_addLayer(vwin);

  // Write changes to the virtual terminal
  p_fvterm.p_processTerminalUpdate();
  test::printArea (vterm);

  for (auto i{0}; i < 15; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 0 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 14 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  for (auto i{15}; i < vterm->size.height; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 80 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 0 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  finalcut::FApplication::start();
  finalcut::FApplication fapp(0, nullptr);
  p_fvterm.p_finishDrawing();
  p_fvterm.updateTerminal();

  // Simulate printing
  for (auto y{0}; y < vterm->size.height; y++)
  {
    for (auto x{vterm->changes[y].xmin}; x < vterm->changes[y].xmax; x++)
      vterm->getFChar(int(x), int(y)).attr.bit.printed = true;

    vterm->changes[y].xmin = uInt(vterm->size.width);
    vterm->changes[y].xmax = 0;
  }

  for (auto y{0}; y < vterm->size.height; y++)
  {
    for (auto x{0}; x < vterm->size.width; x++)
      CPPUNIT_ASSERT ( vterm->getFChar(x, y).attr.bit.no_changes == false );
  }

  std::cerr << '\n';

  p_fvterm.print() << finalcut::FPoint{1, 1}
                   << finalcut::FColorPair { finalcut::FColor::Black
                                           , finalcut::FColor::White };
  p_fvterm.print() << "!\"#$%&'()*+,-./";  // Line 0
  p_fvterm.print() << "0123456789:;<=>";   // Line 1
  p_fvterm.print() << "?@ABCDEFGHIJKLM";   // Line 2
  p_fvterm.print() << "NOPQRSTUVWXYZ[\\";  // Line 3
  p_fvterm.print() << "]^_`abcdefghijk";   // Line 4
  p_fvterm.print() << "lmnopqrstuvwxyz";   // Line 5
  p_fvterm.print() << "!\"#$%--------./";  // Line 6
  p_fvterm.print() << "0123456789:;★★★";   // Line 7
  p_fvterm.print() << "AAABCDEFGHIJKLM";   // Line 8
  p_fvterm.print() << "NOPQRSTUVWXYZZZ";   // Line 9
  p_fvterm.print() << "☺☺☺`abcdefghij☺";   // Line 10
  p_fvterm.print() << "lmnop☺☺☻☺☺vwxyz";   // Line 11
  p_fvterm.print() << "!\"#$%&'()*+,-./";  // Line 12
  p_fvterm.print() << "0123456789:;<=>";   // Line 13
  p_fvterm.print() << "?@ABCDEFGHIJKLM";   // Line 14
  p_fvterm.p_addLayer(vwin);


  for (auto y{0}; y < vterm->size.height; y++)
  {
    for (auto x{0}; x < vterm->size.width; x++)
      CPPUNIT_ASSERT ( vterm->getFChar(x, y).attr.bit.no_changes == false );
  }

  for (auto i{0}; i < 6; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 80 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 0 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  CPPUNIT_ASSERT ( vterm->changes[6].xmin == 5 );
  CPPUNIT_ASSERT ( vterm->changes[6].xmax == 11 );
  CPPUNIT_ASSERT ( vterm->changes[6].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[7].xmin == 12 );
  CPPUNIT_ASSERT ( vterm->changes[7].xmax == 14 );
  CPPUNIT_ASSERT ( vterm->changes[7].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[8].xmin == 0 );
  CPPUNIT_ASSERT ( vterm->changes[8].xmax == 1 );
  CPPUNIT_ASSERT ( vterm->changes[8].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[9].xmin == 13 );
  CPPUNIT_ASSERT ( vterm->changes[9].xmax == 14 );
  CPPUNIT_ASSERT ( vterm->changes[9].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[10].xmin == 0 );
  CPPUNIT_ASSERT ( vterm->changes[10].xmax == 14 );
  CPPUNIT_ASSERT ( vterm->changes[10].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[11].xmin == 5 );
  CPPUNIT_ASSERT ( vterm->changes[11].xmax == 9 );
  CPPUNIT_ASSERT ( vterm->changes[11].trans_count == 0 );

  for (auto i{12}; i < vterm->size.height; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 80 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 0 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  // Reset xmin and xmax values + reduceTerminalLineUpdates()
  for (auto i{0}; i < vterm->size.height; i++)
  {
    vterm->changes[i].xmin = 0;
    vterm->changes[i].xmax = 14;
    finalcut::FVTerm::reduceTerminalLineUpdates(i);
  }

  for (auto y{0}; y < 10; y++)
  {
    for (auto x{0}; x < vterm->size.width; x++)
      CPPUNIT_ASSERT ( vterm->getFChar(x, y).attr.bit.no_changes == false );
  }

  for (auto x{0}; x < 3; x++)
    CPPUNIT_ASSERT ( vterm->getFChar(x, 10).attr.bit.no_changes == false );

  for (auto x{3}; x < 14; x++)
    CPPUNIT_ASSERT ( vterm->getFChar(x, 10).attr.bit.no_changes == true );

  for (auto x{14}; x < vterm->size.width; x++)
    CPPUNIT_ASSERT ( vterm->getFChar(x, 10).attr.bit.no_changes == false );

  for (auto y{11}; y < vterm->size.height; y++)
  {
    for (auto x{0}; x < vterm->size.width; x++)
      CPPUNIT_ASSERT ( vterm->getFChar(x, y).attr.bit.no_changes == false );
  }

  for (auto i{0}; i < 6; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 14 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 13 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }

  CPPUNIT_ASSERT ( vterm->changes[6].xmin == 5 );
  CPPUNIT_ASSERT ( vterm->changes[6].xmax == 11 );
  CPPUNIT_ASSERT ( vterm->changes[6].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[7].xmin == 12 );
  CPPUNIT_ASSERT ( vterm->changes[7].xmax == 14 );
  CPPUNIT_ASSERT ( vterm->changes[7].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[8].xmin == 0 );
  CPPUNIT_ASSERT ( vterm->changes[8].xmax == 1 );
  CPPUNIT_ASSERT ( vterm->changes[8].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[9].xmin == 13 );
  CPPUNIT_ASSERT ( vterm->changes[9].xmax == 14 );
  CPPUNIT_ASSERT ( vterm->changes[9].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[10].xmin == 0 );
  CPPUNIT_ASSERT ( vterm->changes[10].xmax == 14 );
  CPPUNIT_ASSERT ( vterm->changes[10].trans_count == 0 );

  CPPUNIT_ASSERT ( vterm->changes[11].xmin == 5 );
  CPPUNIT_ASSERT ( vterm->changes[11].xmax == 9 );
  CPPUNIT_ASSERT ( vterm->changes[11].trans_count == 0 );

  for (auto i{12}; i < vterm->size.height; i++)
  {
    CPPUNIT_ASSERT ( vterm->changes[i].xmin == 14 );
    CPPUNIT_ASSERT ( vterm->changes[i].xmax == 13 );
    CPPUNIT_ASSERT ( vterm->changes[i].trans_count == 0 );
  }
}

//----------------------------------------------------------------------
void FVTermTest::getFVTermAreaTest()
{
  FVTerm_protected p_fvterm(finalcut::outputClass<FTermOutputTest>{});

  // unique virtual terminal
  auto vterm = p_fvterm.p_getVirtualTerminal();

  // virtual windows
  auto vwin = p_fvterm.getVWin();

  // Create the virtual windows for the p_fvterm objects
  finalcut::FRect geometry {finalcut::FPoint{0, 0}, finalcut::FSize{80, 24}};
  auto vwin_ptr = p_fvterm.p_createArea (geometry);
  vwin = vwin_ptr.get();
  p_fvterm.setVWin(std::move(vwin_ptr));

  p_fvterm.print() << finalcut::FPoint{1, 1}
                   << finalcut::FColorPair {finalcut::FColor::Black, finalcut::FColor::White};

  constexpr std::array<char[3], 256> hex_bytes =
  {{
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
    "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
    "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
    "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df",
    "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
  }};

  for (auto n{0}; n < 6; n++)
  {
    for (auto i{0}; i < 20; i++) p_fvterm.print() << " /\\ ";
    for (auto i{0}; i < 20; i++) p_fvterm.print() << "/" << hex_bytes[(n * 20) + i] << "\\";
    for (auto i{0}; i < 20; i++) p_fvterm.print() << "\\  /";
    for (auto i{0}; i < 20; i++) p_fvterm.print() << " \\/ ";
  }

  vwin->visible = true;
  p_fvterm.p_addLayer(vwin);

  // Write changes to the virtual terminal
  p_fvterm.p_processTerminalUpdate();
  test::printArea (vterm);

  // Creates another virtual window area
  finalcut::FVTerm::FTermArea* vwin_area{};
  finalcut::FVTerm::FTermArea* test_vwin_area{};
  auto vwin_geometry = finalcut::FRect( finalcut::FPoint{0, 0}, finalcut::FSize{6, 6} );
  auto vwin_area_ptr = p_fvterm.p_createArea (vwin_geometry);
  vwin_area = vwin_area_ptr.get();
  auto test_vwin_area_ptr = p_fvterm.p_createArea (vwin_geometry);
  test_vwin_area = test_vwin_area_ptr.get();

  finalcut::FChar bg_char =
  {
    { L' ', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { { finalcut::FColor::Default, finalcut::FColor::Default } },
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { 6, { {6, bg_char} } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  // Get rectangle block from virtual terminal

  // Test with x or y is < 1
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{0, 0}, finalcut::FSize{4, 4}), vwin_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{0, 1}, finalcut::FSize{4, 4}), vwin_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{1, 0}, finalcut::FSize{4, 4}), vwin_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  // Test with x and y is >= 1
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{1, 1}, finalcut::FSize{4, 4}), vwin_area);
  test::printArea (vwin_area);

  finalcut::FChar new_bg_char = bg_char;
  new_bg_char.color.pair.fg = finalcut::FColor::Black;
  new_bg_char.color.pair.bg = finalcut::FColor::White;
  finalcut::FChar forward_slash_char = new_bg_char;
  forward_slash_char.ch[0] = L'/';
  finalcut::FChar back_slash_char = new_bg_char;
  back_slash_char.ch[0] = L'\\';
  finalcut::FChar zero_char = new_bg_char;
  zero_char.ch[0] = L'0';
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char}, {2, bg_char} } },
                                       { 1, { {1, forward_slash_char}, {2, zero_char}, {1, back_slash_char}, {2, bg_char} } },
                                       { 1, { {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {2, bg_char} } },
                                       { 1, { {1, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char}, {2, bg_char} } },
                                       { 2, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{3, 3}, finalcut::FSize{4, 4}), vwin_area);
  test::printArea (vwin_area);
  const auto& fchar_pos1 = vwin_area->getFChar(0, 0);
  const auto& fchar_pos2 = vwin_area->getFChar(finalcut::FPoint{3, 3});
  auto& fchar_pos3 = vwin_area->getFChar(3, 4);
  auto& fchar_pos4 = vwin_area->getFChar(finalcut::FPoint{5, 5});
  CPPUNIT_ASSERT ( &fchar_pos1 == &vwin_area->data[0] );
  CPPUNIT_ASSERT ( &fchar_pos2 == &vwin_area->data[3 * 6 + 3] );  // 6 = width of area
  CPPUNIT_ASSERT ( &fchar_pos3 == &vwin_area->data[4 * 6 + 3] );  // 6 = width of area
  CPPUNIT_ASSERT ( &fchar_pos4 == &vwin_area->data[5 * 6 + 5] );  // 6 = width of area
  CPPUNIT_ASSERT ( &fchar_pos4 == &vwin_area->data[6 * 6 - 1] );  // 6 = width of area
  CPPUNIT_ASSERT ( &vwin_area->getFChar(10, 0) == &vwin_area->getFChar(4, 1) );

  finalcut::FChar one_char = new_bg_char;
  one_char.ch[0] = L'1';
  finalcut::FChar four_char = new_bg_char;
  four_char.ch[0] = L'4';
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char}, {2, bg_char} } },
                                       { 1, { {1, forward_slash_char}, {2, zero_char}, {1, back_slash_char}, {2, bg_char} } },
                                       { 1, { {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char},  {1, back_slash_char}, {1, new_bg_char} } },
                                       { 1, { {1, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {2, new_bg_char}, {1, back_slash_char} } },
                                       { 1, { {2, bg_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {2, bg_char}, {1, four_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, one_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{3, 3}, finalcut::FSize{4, 4}), vwin_area);
  test::printOnArea (test_vwin_area, { { 2, { {6, bg_char} } },
                                       { 1, { {2, bg_char}, {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char} } },
                                       { 1, { {2, bg_char}, {1, forward_slash_char}, {2, new_bg_char}, {1, back_slash_char} } },
                                       { 1, { {2, bg_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {2, bg_char}, {1, four_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, one_char} } } } );
  test::printArea (vwin_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{5, 5}, finalcut::FSize{4, 4}), vwin_area);
  test::printOnArea (test_vwin_area, { { 4, { {6, bg_char} } },
                                       { 1, { {4, bg_char}, {1, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {4, bg_char}, {1, forward_slash_char}, {1, one_char} } } } );
  test::printArea (vwin_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  // move area
  vwin_area->position.x += 2;
  vwin_area->position.y += 2;
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{5, 5}, finalcut::FSize{4, 4}), vwin_area);
  finalcut::FChar five_char = new_bg_char;
  five_char.ch[0] = L'5';
  test::printOnArea (test_vwin_area, { { 2, { {6, bg_char} } },
                                       { 1, { {2, bg_char}, {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char} } },
                                       { 1, { {2, bg_char}, {1, forward_slash_char}, {1, one_char}, {1, five_char}, {1, back_slash_char} } },
                                       { 1, { {2, bg_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {2, bg_char}, {1, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char} } } } );
  test::printArea (vwin_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  vwin_area->position.x += 2;
  vwin_area->position.y += 2;
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{5, 5}, finalcut::FSize{4, 4}), vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char}, {2, bg_char} } },
                                       { 1, { {1, forward_slash_char}, {1, one_char}, {1, five_char}, {1, back_slash_char}, {2, bg_char} } },
                                       { 1, { {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {2, bg_char} } },
                                       { 1, { {1, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char}, {2, bg_char} } },
                                       { 2, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  vwin_area->position.x += 1;
  vwin_area->position.y += 1;
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{5, 5}, finalcut::FSize{4, 4}), vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, one_char}, {1, five_char}, {1, back_slash_char}, {3, bg_char} } },
                                       { 1, { {2, new_bg_char}, {1, forward_slash_char}, {3, bg_char} } },
                                       { 1, { {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char}, {3, bg_char} } },
                                       { 3, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  vwin_area->position.x += 1;
  vwin_area->position.y += 1;
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{5, 5}, finalcut::FSize{4, 4}), vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {1, forward_slash_char}, {4, bg_char} } },
                                       { 1, { {1, forward_slash_char}, {1, new_bg_char}, {4, bg_char} } },
                                       { 4, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  vwin_area->position.x += 1;
  vwin_area->position.y += 1;
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{5, 5}, finalcut::FSize{4, 4}), vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {5, bg_char} } },
                                 { 5, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FRect(finalcut::FPoint{11, 9}, finalcut::FSize{4, 4}), vwin_area);
  test::printArea (vwin_area);
  finalcut::FChar a_char = new_bg_char;
  a_char.ch[0] = L'a';
  test::printOnArea (test_vwin_area, { { 1, { {6, bg_char} } },
                                       { 1, { {3, bg_char}, {1, back_slash_char}, {2, new_bg_char} } },
                                       { 1, { {3, bg_char}, {1, a_char}, {1, back_slash_char}, {1, forward_slash_char} } },
                                       { 1, { {3, bg_char}, {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char} } },
                                       { 1, { {3, bg_char}, {1, forward_slash_char}, {2, new_bg_char} } },
                                       { 1, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  // Get a block from a specified position of the virtual terminal

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );

  // Test with x or y is < 1
  p_fvterm.p_getArea (finalcut::FPoint{0, 0}, vwin_area);
  test::printOnArea (test_vwin_area, { { 6, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  p_fvterm.p_getArea (finalcut::FPoint{0, 1}, vwin_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  p_fvterm.p_getArea (finalcut::FPoint{1, 0}, vwin_area);
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  // Test with x and y is >= 1
  p_fvterm.p_getArea (finalcut::FPoint{1, 1}, vwin_area);
  CPPUNIT_ASSERT ( ! test::isAreaEqual(test_vwin_area, vwin_area) );
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {1, forward_slash_char}, {2, zero_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, zero_char} } },
                                       { 1, { {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char} } },
                                       { 1, { {1, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {2, new_bg_char}, {1, back_slash_char} } },
                                       { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {1, forward_slash_char}, {1, one_char}, {1, four_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, one_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );
  // Position independent
  vwin_area->position.x += 2;
  vwin_area->position.y += 2;
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );
  vwin_area->position.x = 512;
  vwin_area->position.y = 512;
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );
  vwin_area->position.x = -24;
  vwin_area->position.y = -100;
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );
  vwin_area->position.x = 0;
  vwin_area->position.y = 0;
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  // Outside
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{500, 400}, vwin_area);
  test::printOnArea (test_vwin_area, { { 6, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  // Top right
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{75, 1}, vwin_area);
  test::printArea (vwin_area);
  finalcut::FChar two_char = new_bg_char;
  two_char.ch[0] = L'2';
  finalcut::FChar three_char = new_bg_char;
  three_char.ch[0] = L'3';
  finalcut::FChar six_char = new_bg_char;
  six_char.ch[0] = L'6';
  finalcut::FChar seven_char = new_bg_char;
  seven_char.ch[0] = L'7';
  test::printOnArea (test_vwin_area, { { 1, { {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char} } },
                                       { 1, { {1, two_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, one_char}, {1, three_char}, {1, back_slash_char} } },
                                       { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {1, forward_slash_char}, {2, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char} } },
                                       { 1, { {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char} } },
                                       { 1, { {1, six_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, two_char}, {1, seven_char}, {1, back_slash_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{76, 1}, vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {2, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char}, {1, bg_char} } },
                                       { 1, { {1, back_slash_char}, {1, forward_slash_char}, {1, one_char}, {1, three_char}, {1, back_slash_char}, {1, bg_char} } },
                                       { 1, { {1, forward_slash_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {1, bg_char} } },
                                       { 1, { {2, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char}, {1, bg_char} } },
                                       { 1, { {2, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char}, {1, bg_char} } },
                                       { 1, { {1, back_slash_char}, {1, forward_slash_char}, {1, two_char}, {1, seven_char}, {1, back_slash_char}, {1, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  // Bottom right
  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{75, 19}, vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {1, forward_slash_char}, {2, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char} } },
                                       { 1, { {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char} } },
                                       { 1, { {1, six_char}, {1, back_slash_char}, {1, forward_slash_char}, {2, seven_char}, {1, back_slash_char} } },
                                       { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char} } },
                                       { 1, { {1, forward_slash_char}, {2, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{76, 20}, vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {2, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char}, {1, bg_char} } },
                                       { 1, { {2, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char}, {1, bg_char} } },
                                       { 1, { {1, back_slash_char}, {1, forward_slash_char}, {2, seven_char}, {1, back_slash_char}, {1, bg_char} } },
                                       { 1, { {1, forward_slash_char}, {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {1, bg_char} } },
                                       { 1, { {2, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char}, {1, bg_char} } },
                                       { 1, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{77, 21}, vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {1, forward_slash_char}, {1, back_slash_char}, {1, new_bg_char}, {2, bg_char} } },
                                       { 1, { {1, forward_slash_char}, {2, seven_char}, {1, back_slash_char}, {2, bg_char} } },
                                       { 1, { {1, back_slash_char}, {2, new_bg_char}, {1, forward_slash_char}, {2, bg_char} } },
                                       { 1, { {1, new_bg_char}, {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char}, {2, bg_char} } },
                                       { 2, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{78, 22}, vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {2, seven_char}, {1, back_slash_char}, {3, bg_char} } },
                                       { 1, { {2, new_bg_char}, {1, forward_slash_char}, {3, bg_char} } },
                                       { 1, { {1, back_slash_char}, {1, forward_slash_char}, {1, new_bg_char}, {3, bg_char} } },
                                       { 3, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{79, 23}, vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {1, forward_slash_char}, {4, bg_char} } },
                                       { 1, { {1, forward_slash_char}, {1, new_bg_char}, {4, bg_char} } },
                                       { 4, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{80, 24}, vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 1, { {1, new_bg_char}, {5, bg_char} } },
                                       { 5, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );

  test::printOnArea (vwin_area, { 6, { {6, bg_char} } } );
  p_fvterm.p_getArea (finalcut::FPoint{81, 25}, vwin_area);
  test::printArea (vwin_area);
  test::printOnArea (test_vwin_area, { { 6, { {6, bg_char} } } } );
  CPPUNIT_ASSERT ( test::isAreaEqual(test_vwin_area, vwin_area) );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FVTermTest);

// The general unit test main part
#include <main-test.inc>

