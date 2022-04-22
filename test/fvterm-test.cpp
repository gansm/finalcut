/***********************************************************************
* fvterm-test.cpp - FVTerm unit tests                                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2021-2022 Markus Gans                                      *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

#undef F_PREPROC_HANDLER

#define F_PREPROC_HANDLER(i,h) \
    reinterpret_cast<finalcut::FVTerm*>((i)), \
    std::bind ( reinterpret_cast<finalcut::FVTerm::FPreprocessingHandler>((h)) \
              , reinterpret_cast<finalcut::FVTerm*>((i)) )

//----------------------------------------------------------------------
bool& getBellState()
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
    finalcut::FString   getClassName() const override;
    finalcut::FTerm&    getFTerm() &;
    std::size_t         getColumnNumber() const override;
    std::size_t         getLineNumber() const override;
    int                 getTabstop() const override;
    int                 getMaxColor() const override;
    finalcut::Encoding  getEncoding() const override;
    finalcut::FString   getKeyName (finalcut::FKey) const override;

    // Mutators
    void                setCursor (finalcut::FPoint) override;
    void                setCursor (finalcut::CursorMode) override;
    void                hideCursor (bool = true) override;
    void                showCursor() override;
    void                setTerminalSize (finalcut::FSize) override;
    bool                setVGAFont() override;
    bool                setNewFont() override;
    void                setNonBlockingRead (bool = true) override;

    // Inquiries
    bool                isCursorHideable() const override;
    bool                isMonochron() const override;
    bool                isNewFont() const override;
    bool                isEncodable (const wchar_t&) const override;
    bool                hasTerminalResized() const override;
    bool                allowsTerminalSizeManipulation() const override;
    bool                canChangeColorPalette() const override;
    bool                hasHalfBlockCharacter() const override;
    bool                hasShadowCharacter() const override;
    bool                areMetaAndArrowKeysSupported() const override;

    // Methods
    void                initTerminal (finalcut::FVTerm::FTermArea*) override;
    void                finishTerminal() override;
    bool                updateTerminal() override;
    void                detectTerminalSize() override;
    void                commitTerminalResize() override;
    void                initScreenSettings() override;
    bool                scrollTerminalForward() override;
    bool                scrollTerminalReverse() override;
    void                clearTerminalAttributes() override;
    void                clearTerminalState() override;
    bool                clearTerminal (wchar_t = L' ') override;
    void                flush() override;
    void                beep() const override;

  private:
    // Using-declaration
    using OutputBuffer = std::queue<char>;
    using FSetPalette  = std::function<void(finalcut::FColor, int, int, int)>;

    // Accessors
    const FSetPalette& getFSetPaletteRef() const & override;

    // Methods
    bool                isDefaultPaletteTheme() override;
    void                redefineColorPalette() override;
    void                restoreColorPalette() override;

    // Data member
    bool                                 bell{false};
    finalcut::FTerm                      fterm{};
    static finalcut::FVTerm::FTermArea*  vterm;
    static finalcut::FTermData*          fterm_data;
    std::shared_ptr<OutputBuffer>        output_buffer{};
    std::shared_ptr<finalcut::FPoint>    term_pos{};
    finalcut::FChar                      term_attribute{};
    const FSetPalette                    set_palette_function{&finalcut::FTerm::setPalette};
};

// static class attributes
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
inline finalcut::FString FTermOutputTest::getClassName() const
{
  return "FTermOutputTest";
}

//----------------------------------------------------------------------
inline std::size_t FTermOutputTest::getColumnNumber() const
{
  return 80;
}

//----------------------------------------------------------------------
inline std::size_t FTermOutputTest::getLineNumber() const
{
  return 24;
}

//----------------------------------------------------------------------
inline int FTermOutputTest::getTabstop() const
{
  return 8;
}

//----------------------------------------------------------------------
inline int FTermOutputTest::getMaxColor() const
{
  return 256;
}

//----------------------------------------------------------------------
inline finalcut::Encoding FTermOutputTest::getEncoding() const
{
  auto& fterm_data = finalcut::FTermData::getInstance();
  return fterm_data.getTerminalEncoding();
}

//----------------------------------------------------------------------
inline finalcut::FString FTermOutputTest::getKeyName (finalcut::FKey keynum) const
{
  static const auto& keyboard = finalcut::FKeyboard::getInstance();
  return keyboard.getKeyName (keynum);
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::isCursorHideable() const
{
  return true;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::isMonochron() const
{
  return false;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::isNewFont() const
{
  return false;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::isEncodable (const wchar_t&) const
{
  return true;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::hasTerminalResized() const
{
  return false;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::allowsTerminalSizeManipulation() const
{
  return true;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::canChangeColorPalette() const
{
  return true;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::hasHalfBlockCharacter() const
{
  return true;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::hasShadowCharacter() const
{
  return true;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::areMetaAndArrowKeysSupported() const
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
inline bool FTermOutputTest::setVGAFont()
{
  return true;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::setNewFont()
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
inline void FTermOutputTest::initTerminal (finalcut::FVTerm::FTermArea* virtual_terminal)
{
  vterm         = virtual_terminal;
  output_buffer = std::make_shared<OutputBuffer>();
  term_pos      = std::make_shared<finalcut::FPoint>(-1, -1);

  hideCursor();

  // term_attribute stores the current state of the terminal
  term_attribute.ch           = {{ L'\0' }};
  term_attribute.fg_color     = finalcut::FColor::Default;
  term_attribute.bg_color     = finalcut::FColor::Default;
  term_attribute.attr.byte[0] = 0;
  term_attribute.attr.byte[1] = 0;
  term_attribute.attr.byte[2] = 0;
  term_attribute.attr.byte[3] = 0;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::finishTerminal()
{ }

//----------------------------------------------------------------------
inline bool FTermOutputTest::updateTerminal()
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
inline bool FTermOutputTest::scrollTerminalForward()
{
  return true;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::scrollTerminalReverse()
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
inline bool FTermOutputTest::clearTerminal (wchar_t)
{
  return true;
}

//----------------------------------------------------------------------
inline void FTermOutputTest::flush()
{
  CPPUNIT_ASSERT ( finalcut::FVTerm::isTerminalUpdateForced() );
}

//----------------------------------------------------------------------
inline void FTermOutputTest::beep() const
{
  setBellState();
}

// public methods of FTermOutputTest
//----------------------------------------------------------------------
inline const FTermOutputTest::FSetPalette& FTermOutputTest::getFSetPaletteRef() const &
{
  return set_palette_function;
}

//----------------------------------------------------------------------
inline bool FTermOutputTest::isDefaultPaletteTheme()
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
    FVTerm_protected& operator = (const FVTerm_protected&) = delete;

    // Disable move assignment operator (=)
    FVTerm_protected& operator = (FVTerm_protected&&) noexcept = delete;

    void clearArea (wchar_t = L' ') override;
    void addPreprocessingHandler (const finalcut::FVTerm*, finalcut::FVTerm::FPreprocessingFunction&&) override;
    void delPreprocessingHandler (const finalcut::FVTerm*) override;
    void print (const finalcut::FPoint&) override;
    FTermArea* getPrintArea() override;
    void initTerminal() override;

    FTermArea* p_getPrintArea();
    FTermArea* p_getChildPrintArea() const;
    FTermArea* p_getCurrentPrintArea() const;
    FTermArea* p_getVirtualDesktop() const;
    FTermArea* p_getVirtualTerminal() const;

    // Mutators
    void p_setPrintArea (FTermArea*);
    void p_setChildPrintArea (FTermArea*);
    void p_setActiveArea (FTermArea*) const;

    // Inquiries
    bool p_isActive (const FTermArea*) const;
    bool p_hasPrintArea() const;
    bool p_hasChildPrintArea() const;
    bool p_isVirtualWindow() const;
    bool p_isCursorHideable() const;

    // Methods
    void p_createArea (const finalcut::FRect&, const finalcut::FSize&, FTermArea*&);
    void p_createArea (const finalcut::FRect&, FTermArea*&);
    void p_resizeArea (const finalcut::FRect&, const finalcut::FSize&, FTermArea*) const;
    void p_resizeArea (const finalcut::FRect&, FTermArea*) const;
    static void p_removeArea (FTermArea*&);
    static void p_restoreVTerm (const finalcut::FRect&);
    bool p_updateVTermCursor (const FTermArea*) const;
    void p_hideVTermCursor() const;
    static void p_setAreaCursor (const finalcut::FPoint&, bool, FTermArea*);
    static void p_getArea (const finalcut::FPoint&, const FTermArea*);
    static void p_getArea (const finalcut::FRect&, const FTermArea*);
    void p_putArea (const FTermArea*) const;
    static void p_putArea (const finalcut::FPoint&, const FTermArea*);
    static int p_getLayer (FVTerm&);
    static void p_determineWindowLayers();
    void p_scrollAreaForward (FTermArea*) const;
    void p_scrollAreaReverse (FTermArea*) const;
    void p_clearArea (FTermArea*, wchar_t = L' ') const;
    void p_forceTerminalUpdate() const;
    bool p_processTerminalUpdate() const;
    static void p_startDrawing();
    static void p_finishDrawing();
    void p_initTerminal();

    void Preprocessing()
    {
      value++;
    }

    int& value_ref() & noexcept
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
inline FVTerm_protected::FTermArea* FVTerm_protected::getPrintArea()
{
  return finalcut::FVTerm::getPrintArea();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::initTerminal()
{
  finalcut::FVTerm::initTerminal();
}

//----------------------------------------------------------------------
inline FVTerm_protected::FTermArea* FVTerm_protected::p_getPrintArea()
{
  return finalcut::FVTerm::getPrintArea();
}

//----------------------------------------------------------------------
inline FVTerm_protected::FTermArea* FVTerm_protected::p_getChildPrintArea() const
{
  return finalcut::FVTerm::getChildPrintArea();
}

//----------------------------------------------------------------------
inline FVTerm_protected::FTermArea* FVTerm_protected::p_getCurrentPrintArea() const
{
  return finalcut::FVTerm::getCurrentPrintArea();
}

//----------------------------------------------------------------------
inline FVTerm_protected::FTermArea* FVTerm_protected::p_getVirtualDesktop() const
{
  return finalcut::FVTerm::getVirtualDesktop();
}

//----------------------------------------------------------------------
inline FVTerm_protected::FTermArea* FVTerm_protected::p_getVirtualTerminal() const
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
inline bool FVTerm_protected::p_isActive (const FTermArea* area) const
{
  return finalcut::FVTerm::isActive(area);
}

//----------------------------------------------------------------------
inline bool FVTerm_protected::p_hasPrintArea() const
{
  return finalcut::FVTerm::hasPrintArea();
}

//----------------------------------------------------------------------
inline bool FVTerm_protected::p_hasChildPrintArea() const
{
  return finalcut::FVTerm::hasChildPrintArea();
}

//----------------------------------------------------------------------
inline bool FVTerm_protected::p_isVirtualWindow() const
{
  return finalcut::FVTerm::isVirtualWindow();
}

//----------------------------------------------------------------------
inline bool FVTerm_protected::p_isCursorHideable() const
{
  return finalcut::FVTerm::isCursorHideable();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_createArea (const finalcut::FRect& box, const finalcut::FSize& shadow, FTermArea*& area)
{
  finalcut::FVTerm::createArea (box, shadow, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_createArea (const finalcut::FRect& box, FTermArea*& area)
{
  finalcut::FVTerm::createArea (box, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_resizeArea (const finalcut::FRect& box, const finalcut::FSize& shadow, FTermArea* area) const
{
  finalcut::FVTerm::resizeArea (box, shadow, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_resizeArea (const finalcut::FRect& box, FTermArea* area) const
{
  finalcut::FVTerm::resizeArea (box, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_removeArea (FTermArea*& area)
{
  finalcut::FVTerm::removeArea (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_restoreVTerm (const finalcut::FRect& box)
{
  finalcut::FVTerm::restoreVTerm (box);
}

//----------------------------------------------------------------------
inline bool FVTerm_protected::p_updateVTermCursor (const FTermArea* area) const
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
inline void FVTerm_protected::p_getArea (const finalcut::FPoint& pos, const FTermArea* area)
{
  finalcut::FVTerm::getArea (pos, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_getArea (const finalcut::FRect& box, const FTermArea* area)
{
  finalcut::FVTerm::getArea (box, area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_putArea (const FTermArea* area) const
{
  finalcut::FVTerm::putArea (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_putArea (const finalcut::FPoint& pos, const FTermArea* area)
{
  finalcut::FVTerm::putArea (pos, area);
}

//----------------------------------------------------------------------
inline int FVTerm_protected::p_getLayer (FVTerm& obj)
{
  return finalcut::FVTerm::getLayer (obj);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_determineWindowLayers()
{
  finalcut::FVTerm::determineWindowLayers();
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_scrollAreaForward (FTermArea* area) const
{
  finalcut::FVTerm::scrollAreaForward (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_scrollAreaReverse (FTermArea* area) const
{
  finalcut::FVTerm::scrollAreaReverse (area);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_clearArea (FTermArea* area, wchar_t fillchar) const
{
  finalcut::FVTerm::clearArea (area, fillchar);
}

//----------------------------------------------------------------------
inline void FVTerm_protected::p_forceTerminalUpdate() const
{
  finalcut::FVTerm::forceTerminalUpdate();
}

//----------------------------------------------------------------------
inline bool FVTerm_protected::p_processTerminalUpdate() const
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

  private:
    template <typename T>
    struct FRepeat
    {
      std::size_t repeat{0U};
      T data{};
    };

    using RepeatFChar = FRepeat<finalcut::FChar>;
    using RepeatFCharVector = std::vector<RepeatFChar>;
    using RepeatFCharLine = FRepeat<RepeatFCharVector>;
    using RepeatFCharLineVector = std::vector<RepeatFCharLine>;

    void         showFCharData (finalcut::FChar);
    std::size_t  getAreaSize (finalcut::FVTerm::FTermArea*);
    bool         isAreaEqual (finalcut::FVTerm::FTermArea*, finalcut::FVTerm::FTermArea*);
    bool         isFCharEqual (const finalcut::FChar&, const finalcut::FChar&);
    void         printOnArea (finalcut::FVTerm::FTermArea*, const finalcut::FChar&);
    void         printOnArea (finalcut::FVTerm::FTermArea*, const RepeatFChar&);
    void         printOnArea (finalcut::FVTerm::FTermArea*, const RepeatFCharVector&);
    void         printOnArea (finalcut::FVTerm::FTermArea*, const RepeatFCharLine&);
    void         printOnArea (finalcut::FVTerm::FTermArea*, const RepeatFCharLineVector&);
    void         printArea (finalcut::FVTerm::FTermArea*);

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
  finalcut::FVTerm fvterm_3(finalcut::outputClass<FTermOutputTest>{});

  const auto& vwin = fvterm_3.getVWin();
  CPPUNIT_ASSERT ( ! vwin );
  CPPUNIT_ASSERT ( fvterm_3.getPrintCursor() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( fvterm_3.getWindowList() );
  CPPUNIT_ASSERT ( fvterm_3.getWindowList()->empty() );
  CPPUNIT_ASSERT ( fvterm_3.getWindowList()->size() == 0 );
}

//----------------------------------------------------------------------
void FVTermTest::OwnFunctionsTest()
{
  finalcut::FChar shadow_char;
  shadow_char.ch           = { L'\0', L'\0', L'\0', L'\0', L'\0' };
  shadow_char.encoded_char = { L'\0', L'\0', L'\0', L'\0', L'\0' };
  shadow_char.fg_color     = finalcut::FColor::Default;
  shadow_char.bg_color     = finalcut::FColor::Default;
  shadow_char.attr.byte[0] = 0;
  shadow_char.attr.byte[1] = 0;
  shadow_char.attr.bit.transparent = true;
  shadow_char.attr.byte[2] = 0;
  shadow_char.attr.byte[3] = 0;

  // FChar struct
  finalcut::FChar test_char =
  {
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    finalcut::FColor::Default,
    finalcut::FColor::Default,
    { { 0x00, 0x00, 0x00, 0x00} }  // byte 0..3
  };

  using RepeatFChar = FRepeat<finalcut::FChar>;
  using RepeatFCharLine = FRepeat<std::vector<RepeatFChar>>;
  RepeatFChar rep_fchar {80, test_char};
  RepeatFCharLine rep_fchar_line {25, {rep_fchar} };

  CPPUNIT_ASSERT( rep_fchar_line.data[0].data != shadow_char );
  test_char.attr.bit.transparent = true;
  CPPUNIT_ASSERT( test_char == shadow_char );
}

//----------------------------------------------------------------------
void FVTermTest::FVTermBasesTest()
{
  // With own output class
  FVTerm_protected p_fvterm(finalcut::outputClass<FTermOutputTest>{});

  auto&& vwin = p_fvterm.getVWin();
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
  p_fvterm.p_createArea (geometry, Shadow, vwin);

  if ( ! (vwin && vwin->owner && vwin->changes) )
  {
    CPPUNIT_ASSERT ( false );
    return;
  }

  CPPUNIT_ASSERT ( vwin );
  CPPUNIT_ASSERT ( vwin == p_fvterm.getVWin() );
  CPPUNIT_ASSERT ( vwin->owner );
  CPPUNIT_ASSERT ( vwin->hasOwner() );
  CPPUNIT_ASSERT ( vwin->getOwner<FVTerm_protected*>() == &p_fvterm );
  CPPUNIT_ASSERT ( vwin->offset_left == 5 );
  CPPUNIT_ASSERT ( vwin->offset_top == 5 );
  CPPUNIT_ASSERT ( vwin->width == 20 );
  CPPUNIT_ASSERT ( vwin->height == 20 );
  CPPUNIT_ASSERT ( vwin->min_width == 20 );
  CPPUNIT_ASSERT ( vwin->min_height == 1 );
  CPPUNIT_ASSERT ( vwin->right_shadow == 2 );
  CPPUNIT_ASSERT ( vwin->bottom_shadow == 1 );
  CPPUNIT_ASSERT ( vwin->cursor_x == 0 );
  CPPUNIT_ASSERT ( vwin->cursor_y == 0 );
  CPPUNIT_ASSERT ( vwin->input_cursor_x == -1 );
  CPPUNIT_ASSERT ( vwin->input_cursor_y == -1 );
  vwin->setInputCursorPos (12, 4);
  CPPUNIT_ASSERT ( vwin->input_cursor_x == 12 );
  CPPUNIT_ASSERT ( vwin->input_cursor_y == 4 );
  CPPUNIT_ASSERT ( vwin->layer == -1 );
  CPPUNIT_ASSERT ( ! vwin->input_cursor_visible );
  CPPUNIT_ASSERT ( ! vwin->has_changes );
  CPPUNIT_ASSERT ( ! vwin->visible );
  CPPUNIT_ASSERT ( ! vwin->minimized );
  CPPUNIT_ASSERT ( vwin->preproc_list.empty() );
  CPPUNIT_ASSERT ( vwin->changes );
  CPPUNIT_ASSERT ( &(vwin->changes[0]) != nullptr );
  CPPUNIT_ASSERT ( vwin->changes[0].xmin == 22 );
  CPPUNIT_ASSERT ( vwin->changes[0].xmax == 0 );
  CPPUNIT_ASSERT ( vwin->changes[0].trans_count == 0 );
  CPPUNIT_ASSERT ( vwin->data );
  CPPUNIT_ASSERT ( getAreaSize(vwin) == 462 );
  CPPUNIT_ASSERT ( vwin->contains({5, 5}) );
  CPPUNIT_ASSERT ( ! vwin->contains({4, 5}) );
  CPPUNIT_ASSERT ( ! vwin->contains({5, 4}) );
  CPPUNIT_ASSERT ( ! vwin->contains({4, 4}) );
  CPPUNIT_ASSERT ( vwin->contains({26, 25}) );    // {5 + 20 + 2 - 1 = 26, 5 + 20 + 1 - 1 = 25}
  CPPUNIT_ASSERT ( ! vwin->contains({27, 25}) );
  CPPUNIT_ASSERT ( ! vwin->contains({26, 26}) );
  CPPUNIT_ASSERT ( ! vwin->contains({27, 26}) );
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
  CPPUNIT_ASSERT ( ! vwin->checkPrintPos() );
  vwin->setCursorPos(0, 1);
  CPPUNIT_ASSERT ( ! vwin->checkPrintPos() );
  vwin->setCursorPos(1, 0);
  CPPUNIT_ASSERT ( ! vwin->checkPrintPos() );
  vwin->setCursorPos(1, 1);
  CPPUNIT_ASSERT ( vwin->checkPrintPos() );
  vwin->setCursorPos(22, 21);  // {20 + 2 = 22, 20 + 1 = 21}
  CPPUNIT_ASSERT ( vwin->checkPrintPos() );
  vwin->setCursorPos(23, 21);
  CPPUNIT_ASSERT ( ! vwin->checkPrintPos() );
  vwin->setCursorPos(22, 22);
  CPPUNIT_ASSERT ( ! vwin->checkPrintPos() );
  vwin->setCursorPos(23, 22);
  CPPUNIT_ASSERT ( ! vwin->checkPrintPos() );
  vwin->setCursorPos(0, 0);

  finalcut::FChar default_char;
  default_char.ch           = { L' ', L'\0', L'\0', L'\0', L'\0' };
  default_char.encoded_char = { L'\0', L'\0', L'\0', L'\0', L'\0' };
  default_char.fg_color     = finalcut::FColor::Default;
  default_char.bg_color     = finalcut::FColor::Default;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;
  default_char.attr.byte[2] = 8;  // char_width = 1
  default_char.attr.byte[3] = 0;

  for (std::size_t pos = 0; pos < getAreaSize(vwin); pos++)
    CPPUNIT_ASSERT ( isFCharEqual(vwin->data[pos], default_char) );

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
  CPPUNIT_ASSERT ( vwin->cursor_x == 0 );
  CPPUNIT_ASSERT ( vwin->cursor_y == 0 );
  p_fvterm.setCursor( {16, 8} );
  CPPUNIT_ASSERT ( vwin->cursor_x == 11 );
  CPPUNIT_ASSERT ( vwin->cursor_y == 3 );
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
  CPPUNIT_ASSERT ( vdesktop->offset_left == 0 );
  CPPUNIT_ASSERT ( vdesktop->offset_top == 0 );
  CPPUNIT_ASSERT ( vdesktop->width == 80 );
  CPPUNIT_ASSERT ( vdesktop->height == 24 );
  CPPUNIT_ASSERT ( vdesktop->min_width == 80 );
  CPPUNIT_ASSERT ( vdesktop->min_height == 1 );
  CPPUNIT_ASSERT ( vdesktop->right_shadow == 0);
  CPPUNIT_ASSERT ( vdesktop->bottom_shadow == 0 );
  CPPUNIT_ASSERT ( vdesktop->cursor_x == 0 );
  CPPUNIT_ASSERT ( vdesktop->cursor_y == 0 );
  CPPUNIT_ASSERT ( vdesktop->input_cursor_x == -1 );
  CPPUNIT_ASSERT ( vdesktop->input_cursor_y == -1 );
  CPPUNIT_ASSERT ( vdesktop->layer == -1 );
  CPPUNIT_ASSERT ( ! vdesktop->input_cursor_visible );
  CPPUNIT_ASSERT ( ! vdesktop->has_changes );
  CPPUNIT_ASSERT ( vdesktop->visible );
  CPPUNIT_ASSERT ( ! vdesktop->minimized );
  CPPUNIT_ASSERT ( vdesktop->preproc_list.empty() );
  CPPUNIT_ASSERT ( vdesktop->changes );
  CPPUNIT_ASSERT ( vdesktop->changes[0].xmin == 80 );
  CPPUNIT_ASSERT ( vdesktop->changes[0].xmax == 0 );
  CPPUNIT_ASSERT ( vdesktop->changes[0].trans_count == 0 );
  CPPUNIT_ASSERT ( vdesktop->data );
  CPPUNIT_ASSERT ( getAreaSize(vdesktop) == 1920 );

  for (std::size_t pos = 0; pos < getAreaSize(vdesktop); pos++)
    CPPUNIT_ASSERT ( isFCharEqual(vdesktop->data[pos], default_char) );

  // Check the virtual terminal
  auto&& vterm = p_fvterm.p_getVirtualTerminal();
  CPPUNIT_ASSERT ( vterm->hasOwner() );
  CPPUNIT_ASSERT ( vterm->getOwner<FVTerm_protected*>() == &p_fvterm );
  CPPUNIT_ASSERT ( vterm->offset_left == 0 );
  CPPUNIT_ASSERT ( vterm->offset_top == 0 );
  CPPUNIT_ASSERT ( vterm->width == 80 );
  CPPUNIT_ASSERT ( vterm->height == 24 );
  CPPUNIT_ASSERT ( vterm->min_width == 80 );
  CPPUNIT_ASSERT ( vterm->min_height == 1 );
  CPPUNIT_ASSERT ( vterm->right_shadow == 0);
  CPPUNIT_ASSERT ( vterm->bottom_shadow == 0 );
  CPPUNIT_ASSERT ( vterm->cursor_x == 0 );
  CPPUNIT_ASSERT ( vterm->cursor_y == 0 );
  CPPUNIT_ASSERT ( vterm->input_cursor_x == -1 );
  CPPUNIT_ASSERT ( vterm->input_cursor_y == -1 );
  CPPUNIT_ASSERT ( vterm->layer == -1 );
  CPPUNIT_ASSERT ( ! vterm->input_cursor_visible );
  CPPUNIT_ASSERT ( ! vterm->has_changes );
  CPPUNIT_ASSERT ( ! vterm->visible );
  CPPUNIT_ASSERT ( ! vterm->minimized );
  CPPUNIT_ASSERT ( vterm->preproc_list.empty() );
  CPPUNIT_ASSERT ( vterm->changes );
  CPPUNIT_ASSERT ( vterm->changes[0].xmin == 80 );
  CPPUNIT_ASSERT ( vterm->changes[0].xmax == 0 );
  CPPUNIT_ASSERT ( vterm->changes[0].trans_count == 0 );
  CPPUNIT_ASSERT ( vterm->data );
  CPPUNIT_ASSERT ( getAreaSize(vterm) == 1920 );

  for (std::size_t pos = 0; pos < getAreaSize(vterm); pos++)
    CPPUNIT_ASSERT ( isFCharEqual(vterm->data[pos], default_char) );

  // Create a vterm comparison area
  finalcut::FVTerm::FTermArea* test_vterm_area{};
  auto vterm_geometry = finalcut::FRect(finalcut::FPoint{1, 1}, finalcut::FSize{80, 24});
  p_fvterm.p_createArea (vterm_geometry, test_vterm_area);
  printOnArea (test_vterm_area, {getAreaSize(test_vterm_area), default_char});
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );

  // Virtual windows fill test
  p_fvterm.clearArea(L'▒');  // Fill with '▒'
  CPPUNIT_ASSERT ( vwin->has_changes );

  for (auto i{0}; i < vwin->height; i++)
  {
    CPPUNIT_ASSERT ( vwin->changes[i].xmin == 0 );
    CPPUNIT_ASSERT ( vwin->changes[i].xmax == 21 );
    CPPUNIT_ASSERT ( vwin->changes[i].trans_count == 2 );
  }

  const auto full_height = vwin->height + vwin->bottom_shadow;

  for (auto i{vwin->height}; i < full_height; i++)
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
    finalcut::FColor::Default,
    finalcut::FColor::Default,
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  const auto full_width = std::size_t(vwin->width)
                        + std::size_t(vwin->right_shadow);

  for (auto y{0u}; y < std::size_t(vwin->height); y++)
  {
    for (auto x{0u}; x < std::size_t(vwin->width); x++)
    {
      CPPUNIT_ASSERT ( isFCharEqual(vwin->data[y * full_width + x], bg_char) );
    }
  }

  // Check shadow
  auto width = std::size_t(vwin->width);
  finalcut::FChar shadow_char =
  {
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    finalcut::FColor::Default,
    finalcut::FColor::Default,
    { { 0x00, 0x00, 0x00, 0x00} }  // byte 0..3
  };
  shadow_char.attr.bit.transparent = true;

  showFCharData(vwin->data[width]);
  showFCharData(shadow_char);

  for (auto y{0u}; y < std::size_t(full_height); y++)  // Right side shadow
  {
    for (auto x{width}; x < full_width; x++)
    {
      CPPUNIT_ASSERT ( isFCharEqual(vwin->data[y * full_width + x], shadow_char) );
    }
  }

  for (auto x{0u}; x < full_width; x++)  // Bottom shadow
  {
    auto y = full_height - 1;
    CPPUNIT_ASSERT ( isFCharEqual(vwin->data[y * full_width + x], shadow_char) );
  }

  // Create a vwin comparison area
  finalcut::FVTerm::FTermArea* test_vwin_area{};
  p_fvterm.p_createArea (geometry, Shadow, test_vwin_area);

  //                             .-------------------------- 20 line repetitions
  //                             |      .------------------- 20 column repetitions
  //                             |      |             .----- 2 column repetitions
  //                             |      |             |
  printOnArea (test_vwin_area, {20, { {20, bg_char}, {2, shadow_char} } });

  //                        .-------------------------- 22 column repetitions
  //                        |
  printOnArea (test_vwin_area, {22, shadow_char});
  printArea (vwin);
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );

  // Makes the virtual window visible and thus displayable
  // on the virtual terminal
  vwin->visible = true;
  showFCharData(vwin->data[full_width + 1]);
  CPPUNIT_ASSERT ( vwin->has_changes );
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( ! vwin->has_changes );
  CPPUNIT_ASSERT ( ! isAreaEqual(test_vterm_area, vterm) );
  printOnArea (test_vterm_area, {5, { {80, default_char} } });
  printOnArea (test_vterm_area, {19, { {5, default_char}, {20, bg_char}, {55, default_char} } });
  printArea (vterm);
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );

  // Move
  vwin->offset_left = 0;
  vwin->offset_top = 0;
  p_fvterm.p_putArea ({1, 1}, nullptr);
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  p_fvterm.p_putArea ({1, 1}, vwin);
  p_fvterm.p_processTerminalUpdate();
  printArea (vterm);
  printOnArea (test_vterm_area, {5, { {20, bg_char}, {60, default_char} } });
  printOnArea (test_vterm_area, {15, { {20, bg_char}, {2, default_char}, {3, bg_char}, {55, default_char} } });
  printOnArea (test_vterm_area, {1, { {22, default_char}, {3, bg_char}, {55, default_char} } });
  printOnArea (test_vterm_area, {3, { {5, default_char}, {20, bg_char}, {55, default_char} } });
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{21, 5}, finalcut::FSize{5, 21}});
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{5, 21}, finalcut::FSize{20, 4}});
  printOnArea (test_vterm_area, {20, { {20, bg_char}, {60, default_char} } });
  printOnArea (test_vterm_area, {4, { {80, default_char} } });
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  vwin->offset_left = -10;
  vwin->offset_top = -10;
  p_fvterm.p_putArea ({-9, -9}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{1, 1}, finalcut::FSize{22, 21}});
  printOnArea (test_vterm_area, {10, { {10, bg_char}, {70, default_char} } });
  printOnArea (test_vterm_area, {14, { {80, default_char} } });
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  vwin->offset_left = 70;
  vwin->offset_top = -10;
  p_fvterm.p_putArea ({71, -9}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{1, 1}, finalcut::FSize{12, 11}});
  printOnArea (test_vterm_area, {10, { {70, default_char}, {10, bg_char} } });
  printOnArea (test_vterm_area, {14, { {80, default_char} } });
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  vwin->offset_left = -10;
  vwin->offset_top = 14;
  p_fvterm.p_putArea ({-9, 15}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{71, 1}, finalcut::FSize{10, 11}});
  printOnArea (test_vterm_area, {14, { {80, default_char} } });
  printOnArea (test_vterm_area, {10, { {10, bg_char}, {70, default_char} } });
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  vwin->offset_left = 70;
  vwin->offset_top = 14;
  p_fvterm.p_putArea ({71, 15}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{1, 15}, finalcut::FSize{12, 10}});
  printOnArea (test_vterm_area, {14, { {80, default_char} } });
  printOnArea (test_vterm_area, {10, { {70, default_char}, {10, bg_char} } });
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  // Move outside
  vwin->offset_left = -20;
  vwin->offset_top = -20;
  p_fvterm.p_putArea ({-19, -19}, vwin);
  p_fvterm.p_processTerminalUpdate();
  p_fvterm.p_restoreVTerm ({finalcut::FPoint{71, 15}, finalcut::FSize{10, 10}});
  printOnArea (test_vterm_area, {24, { {80, default_char} } });
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  vwin->offset_left = 80;
  vwin->offset_top = -20;
  p_fvterm.p_putArea ({81, -19}, vwin);
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );

  vwin->offset_left = -20;
  vwin->offset_top = 24;
  p_fvterm.p_putArea ({-19, 25}, vwin);
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );

  vwin->offset_left = 80;
  vwin->offset_top = 24;
  p_fvterm.p_putArea ({81, 25}, vwin);
  p_fvterm.p_processTerminalUpdate();
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );

  // Virtual window is larger than virtual terminal
  auto large_geometry = finalcut::FRect(finalcut::FPoint{1, 1}, finalcut::FSize{90, 34});
  p_fvterm.p_resizeArea (large_geometry, vwin);
  vwin->cursor_x = 1;
  vwin->cursor_y = 1;

  for (wchar_t i = 0; i < wchar_t(large_geometry.getHeight()); i++)
  {
    bg_char.ch[0] =  L'\\' + i;
    printOnArea (vwin, {large_geometry.getWidth(), bg_char});
  }

  vwin->offset_left = -5;
  vwin->offset_top = -5;
  p_fvterm.p_putArea ({-4, -4}, vwin);

  for (wchar_t i = 0; i < wchar_t(vterm->height); i++)
  {
    bg_char.ch[0] =  L'a' + i;
    printOnArea (test_vterm_area, {std::size_t(vterm->width), bg_char});
  }

  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  // Rezeize virtual terminal
  auto new_term_size = finalcut::FSize{70, 18};
  p_fvterm.resizeVTerm (new_term_size);
  auto new_term_geometry = finalcut::FRect(finalcut::FPoint{1, 1}, new_term_size);
  p_fvterm.p_resizeArea (new_term_geometry, test_vterm_area);
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  CPPUNIT_ASSERT ( vterm->width == 70);
  CPPUNIT_ASSERT ( vterm->height == 18 );
  printArea (vterm);
  p_fvterm.p_putArea ({-4, -4}, vwin);

  for (wchar_t i = 0; i < wchar_t(vterm->height); i++)
  {
    bg_char.ch[0] =  L'a' + i;
    printOnArea (test_vterm_area, {std::size_t(vterm->width), bg_char});
  }

  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  // Deallocate area memory
  CPPUNIT_ASSERT ( test_vwin_area );
  p_fvterm.p_removeArea (test_vwin_area);
  CPPUNIT_ASSERT ( ! test_vwin_area );

  CPPUNIT_ASSERT ( test_vterm_area );
  p_fvterm.p_removeArea (test_vterm_area);
  CPPUNIT_ASSERT ( ! test_vterm_area );

  finalcut::FVTerm::FTermArea* null_area{nullptr};
  p_fvterm.p_removeArea (null_area);  // Do nothing
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
    auto&& vwin = p_fvterm.getVWin();
    auto child_print_area = p_fvterm.p_getChildPrintArea();
    CPPUNIT_ASSERT ( child_print_area == nullptr );
    CPPUNIT_ASSERT ( ! p_fvterm.p_isVirtualWindow() );

    finalcut::FRect geometry {finalcut::FPoint{0, 0}, finalcut::FSize{15, 10}};
    finalcut::FSize Shadow(1, 1);
    p_fvterm.p_createArea (geometry, Shadow, vwin);
    const finalcut::FVTerm::FTermArea* const_vwin = p_fvterm.getVWin();
    CPPUNIT_ASSERT ( p_fvterm.p_isVirtualWindow() );
    CPPUNIT_ASSERT ( static_cast<decltype(const_vwin)>(vwin) == const_vwin );
    CPPUNIT_ASSERT ( ! p_fvterm.p_hasPrintArea() );
    CPPUNIT_ASSERT ( p_fvterm.getPrintArea() == vwin );
    CPPUNIT_ASSERT ( p_fvterm.p_hasPrintArea() );
    auto print_area = p_fvterm.p_getCurrentPrintArea();
    CPPUNIT_ASSERT ( print_area == vwin );
    CPPUNIT_ASSERT ( vwin->cursor_x == 0 );  // First column is 1
    CPPUNIT_ASSERT ( vwin->cursor_y == 0 );  // First row is 1
    CPPUNIT_ASSERT ( vwin->offset_left == 0 );
    CPPUNIT_ASSERT ( vwin->offset_top == 0 );

    auto move_geometry = finalcut::FRect ( finalcut::FPoint{12, 37}
                                         , finalcut::FSize{15, 10} );
    p_fvterm.p_resizeArea (move_geometry, Shadow, vwin);
    CPPUNIT_ASSERT ( vwin->offset_left == 12 );
    CPPUNIT_ASSERT ( vwin->offset_top == 37 );

    // FChar struct
    finalcut::FChar default_char =
    {
      { L' ', L'\0', L'\0', L'\0', L'\0' },
      { L'\0', L'\0', L'\0', L'\0', L'\0' },
      finalcut::FColor::Default,
      finalcut::FColor::Default,
      { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
    };

    // printf test

    // Create a vwin comparison area
    finalcut::FVTerm::FTermArea* test_vwin_area{};
    p_fvterm.p_createArea (geometry, Shadow, test_vwin_area);
    printOnArea (test_vwin_area, {11, { {16, default_char} } });
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.printf("%s/%d = %4.2f...", "1", 3, 1.0f/3.0f);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.setCursor({1, 0});
    CPPUNIT_ASSERT ( vwin->cursor_x == -11 );
    CPPUNIT_ASSERT ( vwin->cursor_y == -37 );
    vwin->offset_left = 0;
    vwin->offset_top = 0;
    p_fvterm.setCursor({1, 0});
    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );  // First column is 1
    CPPUNIT_ASSERT ( vwin->cursor_y == 0 );  // First row is 1
    p_fvterm.printf("%s/%d = %4.2f...", "1", 3, 1.0f/3.0f);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.setCursor({0, 1});
    CPPUNIT_ASSERT ( vwin->cursor_x == 0 );  // First column is 1
    CPPUNIT_ASSERT ( vwin->cursor_y == 1 );  // First row is 1
    p_fvterm.printf("%s/%d = %4.2f...", "1", 3, 1.0f/3.0f);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.setCursor({1, 1});
    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );  // First column is 1
    CPPUNIT_ASSERT ( vwin->cursor_y == 1 );  // First row is 1
    std::setlocale(LC_NUMERIC, "C");
    p_fvterm.printf("%s/%d = %4.2f...", "1", 3, 1.0f/3.0f);
    CPPUNIT_ASSERT ( vwin->cursor_x == 14 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 1 );
    CPPUNIT_ASSERT ( ! isAreaEqual(test_vwin_area, vwin) );
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

    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 14 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 1 );

    finalcut::FString string{};  // Empty string
    p_fvterm.print(vwin, string);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(string);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    finalcut::FVTermBuffer fvtermbuffer{}; // Empty FVTerm buffer
    p_fvterm.print(fvtermbuffer);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(vwin, fvtermbuffer);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(nullptr, fvtermbuffer);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

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
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 2 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 2 );
    p_fvterm.print(finalcut::FPoint{1, 3});
    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    // Horizontal tabulation (HT)
    p_fvterm.print(L"1\t2");
    test_vwin_area->data[32].ch[0] = '1';
    test_vwin_area->data[40].ch[0] = '2';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    p_fvterm.print(L"\r          ");  // Carriage Return + spaces
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"12\t2");
    test_vwin_area->data[33].ch[0] = '2';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"123\t2");
    test_vwin_area->data[34].ch[0] = '3';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"1234\t2");
    test_vwin_area->data[35].ch[0] = '4';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"12345\t2");
    test_vwin_area->data[36].ch[0] = '5';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"123456\t2");
    test_vwin_area->data[37].ch[0] = '6';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    p_fvterm.print(L"\r          ");
    p_fvterm.print(finalcut::FPoint{1, 3});
    p_fvterm.print(L"1234567\t2");
    test_vwin_area->data[38].ch[0] = '7';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    p_fvterm.print(L"\r          \r12345678\t2");
    test_vwin_area->data[39].ch[0] = '8';
    test_vwin_area->data[40].ch[0] = ' ';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 17 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    // Cursor is outside the window
    p_fvterm.print(L"invisible");
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 17 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 3 );

    p_fvterm.print('\b');  // Backspace
    p_fvterm.print(L'…');
    test_vwin_area->data[47].ch[0] = L'…';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 4 );

    // std::vector<FChar>
    finalcut::FChar fchar =
    {
      { L'\0', L'\0', L'\0', L'\0', L'\0' },
      { L'\0', L'\0', L'\0', L'\0', L'\0' },
      finalcut::FColor::Red,
      finalcut::FColor::White,
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
    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );

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
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    CPPUNIT_ASSERT ( term_string.size() == 7 );
    term_string.clear();
    CPPUNIT_ASSERT ( term_string.size() == 0 );
    CPPUNIT_ASSERT ( term_string.empty() );
    p_fvterm.print(term_string);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(vwin, term_string);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    // Full-Width Characters
    term_string.assign(1, fchar);
    term_string[0].ch[0] = L'🏠';
    term_string[0].attr.bit.char_width = 2 & 0x03;
    p_fvterm.print(vwin, term_string);
    CPPUNIT_ASSERT ( p_fvterm.print(nullptr, L'⌚') == -1 );
    p_fvterm.print(fchar);

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

    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    p_fvterm.print(nullptr, fchar);
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    // Printing text in the bottom row
    int last_row = vwin->height + vwin->bottom_shadow;
    CPPUNIT_ASSERT ( last_row == 11 );
    p_fvterm.print(finalcut::FPoint{1, last_row});

    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor_y == last_row );
    CPPUNIT_ASSERT ( p_fvterm.print("Disqualification") == 16 );
    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor_y == last_row );  // Scrolling up was prevented

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
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print(finalcut::FPoint{1, 6});
    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 6 );

    // Combined unicode characters
    if ( enc == finalcut::Encoding::VT100 )
    {
      CPPUNIT_ASSERT ( p_fvterm.print(L"STARGΛ̊TE") == 9 );
      CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
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
      CPPUNIT_ASSERT ( vwin->cursor_x == 9 );
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

    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    // Line break
    p_fvterm.print(finalcut::FPoint{11, 7});
    CPPUNIT_ASSERT ( vwin->cursor_x == 11 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 7 );
    CPPUNIT_ASSERT ( p_fvterm.print(L"FINAL CUT") == 9 );
    CPPUNIT_ASSERT ( vwin->cursor_x == 4 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 8 );
    test_vwin_area->data[106].ch[0] = L'F';
    test_vwin_area->data[107].ch[0] = L'I';
    test_vwin_area->data[108].ch[0] = L'N';
    test_vwin_area->data[109].ch[0] = L'A';
    test_vwin_area->data[110].ch[0] = L'L';
    test_vwin_area->data[112].ch[0] = L'C';
    test_vwin_area->data[113].ch[0] = L'U';
    test_vwin_area->data[114].ch[0] = L'T';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    // Stream
    p_fvterm.print() << ' ';  // char
    CPPUNIT_ASSERT ( vwin->cursor_x == 5 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 8 );
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << 1.23;  // double
    CPPUNIT_ASSERT ( vwin->cursor_x == 13 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 8 );
    test_vwin_area->data[116].ch[0] = L'1';
    test_vwin_area->data[117].ch[0] = L'.';
    test_vwin_area->data[118].ch[0] = L'2';
    test_vwin_area->data[119].ch[0] = L'3';
    test_vwin_area->data[120].ch[0] = L'0';
    test_vwin_area->data[121].ch[0] = L'0';
    test_vwin_area->data[122].ch[0] = L'0';
    test_vwin_area->data[123].ch[0] = L'0';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << L' ';  // wchar_t
    CPPUNIT_ASSERT ( vwin->cursor_x == 14 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 8 );
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    wchar_t kilohertz[] = L"kHz";  // wchar_t*
    p_fvterm.print() << kilohertz;
    CPPUNIT_ASSERT ( vwin->cursor_x == 1 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 9 );
    test_vwin_area->data[125].ch[0] = L'k';
    test_vwin_area->data[126].ch[0] = L'H';
    test_vwin_area->data[127].ch[0] = L'z';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << finalcut::UniChar::BlackRightPointingPointer;  // UniChar
    CPPUNIT_ASSERT ( vwin->cursor_x == 2 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 9 );
    test_vwin_area->data[128].ch[0] = L'►';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << std::string("fan");  // std::string
    CPPUNIT_ASSERT ( vwin->cursor_x == 5 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 9 );
    test_vwin_area->data[129].ch[0] = L'f';
    test_vwin_area->data[130].ch[0] = L'a';
    test_vwin_area->data[131].ch[0] = L'n';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << std::wstring(L"tas");  // std::wstring
    CPPUNIT_ASSERT ( vwin->cursor_x == 8 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 9 );
    test_vwin_area->data[132].ch[0] = L't';
    test_vwin_area->data[133].ch[0] = L'a';
    test_vwin_area->data[134].ch[0] = L's';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << finalcut::FString(L"tic");  // FString
    CPPUNIT_ASSERT ( vwin->cursor_x == 11 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 9 );
    test_vwin_area->data[135].ch[0] = L't';
    test_vwin_area->data[136].ch[0] = L'i';
    test_vwin_area->data[137].ch[0] = L'c';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << finalcut::FPoint(3, 5);  // FPoint
    CPPUNIT_ASSERT ( vwin->cursor_x == 3 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );

    fchar.ch[0] = L'y';
    fchar.ch[1] = L'\U00000304';
    fchar.ch[2] = L'\0';
    fchar.attr.bit.char_width = 1 & 0x03;
    p_fvterm.print() << fchar;  // FChar
    CPPUNIT_ASSERT ( vwin->cursor_x == 4 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );
    test_vwin_area->data[66].ch[0] = L'y';
    test_vwin_area->data[66].ch[1] = L'\U00000304';
    test_vwin_area->data[66].attr.bit.char_width = 1 & 0x03;
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    fchar.ch[0] = L'-';
    fchar.ch[1] = L'\0';
    fchar.attr.byte[0] = 0;
    fchar.fg_color = finalcut::FColor::Default;
    fchar.bg_color = finalcut::FColor::Default;
    finalcut::FVTerm::FCharVector dash{2, fchar};
    p_fvterm.print() << dash;  // FCharVector
    CPPUNIT_ASSERT ( vwin->cursor_x == 6 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );
    test_vwin_area->data[67].ch[0] = L'-';
    test_vwin_area->data[68].ch[0] = L'-';
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    p_fvterm.print() << finalcut::FStyle ( finalcut::Style::Italic
                                         | finalcut::Style::DoubleUnderline );  // FStyle
    CPPUNIT_ASSERT ( vwin->cursor_x == 6 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );
    p_fvterm.print() << "F";
    CPPUNIT_ASSERT ( vwin->cursor_x == 7 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );
    test_vwin_area->data[69].ch[0] = L'F';
    test_vwin_area->data[69].attr.bit.italic = true;
    test_vwin_area->data[69].attr.bit.dbl_underline = true;
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    finalcut::FColorPair cpair{finalcut::FColor::Blue, finalcut::FColor::White};
    p_fvterm.print() << cpair;  // FColorPair
    CPPUNIT_ASSERT ( vwin->cursor_x == 7 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );
    p_fvterm.print() << "C";
    CPPUNIT_ASSERT ( vwin->cursor_x == 8 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );
    test_vwin_area->data[70].ch[0] = L'C';
    test_vwin_area->data[70].fg_color = finalcut::FColor::Blue;
    test_vwin_area->data[70].bg_color = finalcut::FColor::White;
    test_vwin_area->data[70].attr.bit.italic = true;
    test_vwin_area->data[70].attr.bit.dbl_underline = true;
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );

    fvtermbuffer.print("++");
    p_fvterm.print() << fvtermbuffer;  // FVTermBuffer
    CPPUNIT_ASSERT ( vwin->cursor_x == 10 );
    CPPUNIT_ASSERT ( vwin->cursor_y == 5 );
    test_vwin_area->data[71] = test_vwin_area->data[70];
    test_vwin_area->data[71].ch[0] = L'+';
    test_vwin_area->data[72] = test_vwin_area->data[71];
    CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
    printArea (vwin);

    // Deallocate area memory
    p_fvterm.p_removeArea (test_vwin_area);
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
  p_fvterm.p_createArea (geometry, vwin);

  // Create a child print area
  finalcut::FVTerm::FTermArea* child_print_area{nullptr};
  std::size_t w = 5;
  std::size_t h = 2;
  finalcut::FRect child_geometry{};
  child_geometry.setRect (0, 0, w, h);
  p_fvterm.p_createArea (child_geometry, child_print_area);
  CPPUNIT_ASSERT ( ! p_fvterm.p_hasChildPrintArea() );
  CPPUNIT_ASSERT ( p_fvterm.p_getChildPrintArea() == nullptr );
  p_fvterm.p_setChildPrintArea (child_print_area);
  CPPUNIT_ASSERT ( p_fvterm.p_hasChildPrintArea() );
  CPPUNIT_ASSERT ( p_fvterm.p_getChildPrintArea() != nullptr );
  CPPUNIT_ASSERT ( p_fvterm.p_getChildPrintArea() == child_print_area );

  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 0 );
  p_fvterm.addPreprocessingHandler
  (
    F_PREPROC_HANDLER (&p_fvterm, &FVTerm_protected::Preprocessing)
  );

  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 0 );
  CPPUNIT_ASSERT ( vwin );
  CPPUNIT_ASSERT ( ! vwin->visible );
  vwin->visible = true;  // show()
  CPPUNIT_ASSERT ( vwin->visible );
  CPPUNIT_ASSERT ( ! vterm->has_changes );
  p_fvterm.p_putArea(vwin);
  CPPUNIT_ASSERT ( vterm->has_changes );
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 1 );
  p_fvterm.p_putArea(vwin);
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 2 );
  p_fvterm.value_ref() *= 5;
  p_fvterm.p_putArea(vwin);
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 11 );
  vwin->visible = false;  // hide()
  p_fvterm.p_putArea(vwin);
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 11 );
  p_fvterm.p_putArea(nullptr);
  CPPUNIT_ASSERT ( p_fvterm.value_ref() == 11 );

  for (auto i{0}; i < vterm->height; i++)
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
  printArea (vwin);

  p_fvterm.p_putArea(vwin);
  vwin->visible = true;  // show()
  vterm->has_changes = false;
  p_fvterm.p_putArea(vwin);
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
    finalcut::FColor::Default,
    finalcut::FColor::Default,
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar space_char_2 =
  {
    { L' ', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    finalcut::FColor::Red,
    finalcut::FColor::White,
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  finalcut::FChar equal_sign_char =
  {
    { L'=', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    finalcut::FColor::Red,
    finalcut::FColor::White,
    { { 0x00, 0x00, 0x08, 0x00} }  // byte 0..3
  };

  // Create a vterm comparison area
  finalcut::FVTerm::FTermArea* test_vterm_area{};
  auto vterm_geometry = finalcut::FRect( finalcut::FPoint{0, 0}
                                       , finalcut::FSize{std::size_t(vterm->width), std::size_t(vterm->height)} );
  p_fvterm.p_createArea (vterm_geometry, test_vterm_area);

  // Check the virtual terminal
  //                                .--------------- line repetitions
  //                                |     .--------- column repetitions
  //                                |     |
  printOnArea (test_vterm_area, { { 3, { {80, space_char_1} } },
                                  { 1, { {35, space_char_1}, {10, equal_sign_char}, { 35, space_char_1} } },
                                  { 6, { {35, space_char_1}, { 1, equal_sign_char}, {  8, space_char_2}, { 1, equal_sign_char}, { 35, space_char_1} } },
                                  { 1, { {35, space_char_1}, {10, equal_sign_char}, { 35, space_char_1} } },
                                  {13, { {80, space_char_1} } } } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vterm_area, vterm) );
  printArea (vterm);

  // Deallocate area memory
  CPPUNIT_ASSERT ( test_vterm_area );
  p_fvterm.p_removeArea (test_vterm_area);
  CPPUNIT_ASSERT ( ! test_vterm_area );

  p_fvterm.delPreprocessingHandler(&p_fvterm);
  p_fvterm.p_setChildPrintArea (nullptr);
  p_fvterm.p_removeArea (child_print_area);
}

//----------------------------------------------------------------------
void FVTermTest::FVTermScrollTest()
{
  FVTerm_protected p_fvterm(finalcut::outputClass<FTermOutputTest>{});

  // virtual window
  auto&& vwin = p_fvterm.getVWin();

  // Create the virtual window for the p_fvterm object
  finalcut::FRect geometry {finalcut::FPoint{0, 0}, finalcut::FSize{5, 5}};
  p_fvterm.p_createArea (geometry, vwin);

  p_fvterm.print() << finalcut::FPoint{1, 1}
                   << "1111122222333334444455555";

  // Create a vwin comparison area
  finalcut::FVTerm::FTermArea* test_vwin_area{};
  auto vwin_geometry = finalcut::FRect( finalcut::FPoint{0, 0}, finalcut::FSize{5, 5} );
  p_fvterm.p_createArea (vwin_geometry, test_vwin_area);

  finalcut::FChar one_char =
  {
    { L'1', L'\0', L'\0', L'\0', L'\0' },
    { L'\0', L'\0', L'\0', L'\0', L'\0' },
    finalcut::FColor::Default,
    finalcut::FColor::Default,
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

  printOnArea (test_vwin_area, { {5, one_char},
                                 {5, two_char},
                                 {5, three_char},
                                 {5, four_char},
                                 {5, five_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  printOnArea (test_vwin_area, { {5, two_char},
                                 {5, three_char},
                                 {5, four_char},
                                 {5, five_char},
                                 {5, space_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  printOnArea (test_vwin_area, { {5, three_char},
                                 {5, four_char},
                                 {5, five_char},
                                 {5, space_char},
                                 {5, space_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  printOnArea (test_vwin_area, { {5, four_char},
                                 {5, five_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, space_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  printOnArea (test_vwin_area, { {5, five_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, space_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaForward (vwin);
  printOnArea (test_vwin_area, { {5, space_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, space_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  // Scroll reverse

  p_fvterm.print() << finalcut::FPoint{1, 1}
                   << "1111122222333334444455555";
  printOnArea (test_vwin_area, { {5, one_char},
                                 {5, two_char},
                                 {5, three_char},
                                 {5, four_char},
                                 {5, five_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  printOnArea (test_vwin_area, { {5, space_char},
                                 {5, one_char},
                                 {5, two_char},
                                 {5, three_char},
                                 {5, four_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  printOnArea (test_vwin_area, { {5, space_char},
                                 {5, space_char},
                                 {5, one_char},
                                 {5, two_char},
                                 {5, three_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  printOnArea (test_vwin_area, { {5, space_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, one_char},
                                 {5, two_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  printOnArea (test_vwin_area, { {5, space_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, one_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  p_fvterm.p_scrollAreaReverse (vwin);
  printOnArea (test_vwin_area, { {5, space_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, space_char},
                                 {5, space_char} } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vwin_area, vwin) );
  printArea (vwin);

  // vdesktop scrolling

  auto&& vdesktop = p_fvterm.p_getVirtualDesktop();
  printOnArea (vdesktop, { { 5, { {80, space_char} } },
                           { 1, { {80, one_char} } },
                           { 5, { {80, space_char} } },
                           { 1, { {80, one_char} } },
                           { 5, { {80, space_char} } },
                           { 1, { {80, one_char} } },
                           { 6, { {80, space_char} } } } );

  // Create a vdesktop comparison area
  finalcut::FVTerm::FTermArea* test_vdesktop{};
  auto vdesktop_geometry = finalcut::FRect( finalcut::FPoint{0, 0}, finalcut::FSize{80, 24} );
  p_fvterm.p_createArea (vdesktop_geometry, test_vdesktop);

  printOnArea (test_vdesktop, { { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 6, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vdesktop, vdesktop) );
  printArea (vdesktop);

  p_fvterm.p_scrollAreaForward (vdesktop);
  printOnArea (test_vdesktop, { { 4, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 7, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vdesktop, vdesktop) );
  printArea (vdesktop);

  p_fvterm.p_scrollAreaForward (vdesktop);
  p_fvterm.p_scrollAreaForward (vdesktop);
  printOnArea (test_vdesktop, { { 2, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 9, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vdesktop, vdesktop) );
  printArea (vdesktop);

  p_fvterm.p_scrollAreaReverse (vdesktop);
  printOnArea (test_vdesktop, { { 3, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 8, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vdesktop, vdesktop) );
  printArea (vdesktop);

  for (auto i{0}; i < 6; i++)
  {
    p_fvterm.p_scrollAreaReverse (vdesktop);
  }
  
  printOnArea (test_vdesktop, { { 9, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 5, { {80, space_char} } },
                                { 1, { {80, one_char} } },
                                { 2, { {80, space_char} } } } );
  CPPUNIT_ASSERT ( isAreaEqual(test_vdesktop, vdesktop) );
  printArea (vdesktop);
}

//----------------------------------------------------------------------
void FVTermTest::FVTermOverlappingWindowsTest()
{
  //   1       2       3       4
  // ░░░░░░  ▒▒▒▒▒▒  ▓▓▓▓▓▓  ██████
  // ░░░░░░  ▒▒▒▒▒▒  ▓▓▓▓▓▓  ██████
  // ░░░░░░  ▒▒▒▒▒▒  ▓▓▓▓▓▓  ██████

  //     ░░░░░░
  //     ░░░░░░              1
  // ▒▒▒▒░░░░░░▓▓▓▓
  // ▒▒▒▒▒▒  ▓▓▓▓▓▓      2       3
  // ▒▒▒▒██████▓▓▓▓
  //     ██████              4
  //     ██████

  //     ░░░░░░
  // ▒▒▒▒░░░░░░              1           4  top
  // ▒▒▒▒░░▓▓▓▓▓▓                        3
  // ▒▒▒██████▓▓▓        2       3       1
  //    ██████▓▓▓                        2  bottom
  //    ██████               4

  // ░░░░░▒▒▒▒▒▒                         4  top
  // ░░░░░▒▒▒▒▒▒          1     2        3
  // ██████▓▓▓▓▓                         2
  // ██████▓▓▓▓▓          4     3        1  botton
  // ██████▓▓▓▓▓

}

//----------------------------------------------------------------------
void FVTermTest::printOnArea ( finalcut::FVTerm::FTermArea* area
                             , const finalcut::FChar& fchar )
{
  if ( area->cursor_x < 1 )
    area->cursor_x = 1;

  if ( area->cursor_y < 1 )
    area->cursor_y = 1;

  int ax = area->cursor_x - 1;
  int ay = area->cursor_y - 1;
  const int line_length = area->width + area->right_shadow;
  const int line_height = area->height + area->bottom_shadow;
  const int size = line_length * line_height;

  if ( ay * line_length + ax > size )
  {
    auto tmp = ay * line_length + ax % size;
    ax = tmp % line_length;
    ay = tmp / line_height;
    area->cursor_x = ax + 1;
    area->cursor_y = ay + 1;
  }

  auto& ac = area->data[ay * line_length + ax];  // area character
  std::memcpy (&ac, &fchar, sizeof(ac));  // copy character to area
  area->cursor_x = ((ax + 1) % line_length) + 1;
  area->cursor_y = ((ax + 1) / line_length) + area->cursor_y;

  if ( area->cursor_y > line_height )
    area->cursor_y = ((area->cursor_y - 1) % line_height) + 1;
}

//----------------------------------------------------------------------
void FVTermTest::printOnArea ( finalcut::FVTerm::FTermArea* area
                             , const RepeatFChar& rep_fchar )
{
  for (std::size_t i{0U}; i < rep_fchar.repeat; i++)
  {
    printOnArea (area, rep_fchar.data);
  }
}

//----------------------------------------------------------------------
void FVTermTest::printOnArea ( finalcut::FVTerm::FTermArea* area
                             , const RepeatFCharVector& rep_fchar_vec )
{
  for (const auto& rep_fchar : rep_fchar_vec)
  {
    printOnArea (area, rep_fchar);
  }
}

//----------------------------------------------------------------------
void FVTermTest::printOnArea ( finalcut::FVTerm::FTermArea* area
                             , const RepeatFCharLine& rep_fchar_line )
{
  for (std::size_t i{0U}; i < rep_fchar_line.repeat; i++)
  {
    printOnArea (area, rep_fchar_line.data);
  }
}

//----------------------------------------------------------------------
void FVTermTest::printOnArea ( finalcut::FVTerm::FTermArea* area
                             , const RepeatFCharLineVector& rep_fchar_line_vec )
{
  for (const auto& rep_fchar_line : rep_fchar_line_vec)
  {
    printOnArea (area, rep_fchar_line);
  }
}

//----------------------------------------------------------------------
void FVTermTest::printArea ( finalcut::FVTerm::FTermArea* area )
{
  auto width = area->width + area->right_shadow;
  auto height = area->height + area->bottom_shadow;
  auto size = getAreaSize(area);
  std::wcout << L'┌' << std::wstring(width, L'─') << L"┐\n";

  for (std::size_t i{0U}; i < size; i++)
  {
    if ( area->data[i].attr.bit.fullwidth_padding )
      continue;

    auto col = (i + 1) % width ;
    auto line = (i + 1) / width;

    if ( col == 1 && line < std::size_t(height) )
      std::wcout << L"│";

    auto ch = area->data[i].ch;

    if ( ch[0] == L'\0' )
      ch[0] = L' ';

    std::wcout << ch.data();

    if ( col == 0  )
      std::wcout << L"│" << line << L"\n";
  }

  std::wcout << L'└' << std::wstring(width, L'─') << L"┘\n ";

  if ( width > 9 )
  {
    for (int i{1}; i <= width; i++)
      std::wcout << i / 10;

    std::wcout << L"\n ";
  }

  for (int i{1}; i <= width; i++)
    std::wcout << i % 10;

  std::wcout << L"\n (" << size << L" character) \n";
}

//----------------------------------------------------------------------
void FVTermTest::showFCharData (finalcut::FChar fchar)
{
  std::wcout << L"FChar data\n" << std::boolalpha;
  std::wcout << L"                         ch: '" <<  fchar.ch.data();
  std::wcout << L"' {" << uInt32(fchar.ch[0]) << L", " <<
                          uInt32(fchar.ch[1]) << L", " <<
                          uInt32(fchar.ch[2]) << L", " <<
                          uInt32(fchar.ch[3]) << L", " <<
                          uInt32(fchar.ch[4]) << L"}\n";
  std::wcout << L"               encoded_char: '" <<  fchar.encoded_char.data();
  std::wcout << L"' {" << uInt32(fchar.encoded_char[0]) << L", " <<
                          uInt32(fchar.encoded_char[1]) << L", " <<
                          uInt32(fchar.encoded_char[2]) << L", " <<
                          uInt32(fchar.encoded_char[3]) << L", " <<
                          uInt32(fchar.encoded_char[4]) << L"}\n";
  std::wcout << L"                   fg_color: " << int(fchar.fg_color) << L'\n';
  std::wcout << L"                   bg_color: " << int(fchar.bg_color) << L'\n';
  std::wcout << L"                    attr[0]: " << int(fchar.attr.byte[0]) << L'\n';
  std::wcout << L"              attr.bit.bold: " << bool(fchar.attr.bit.bold) << L'\n';
  std::wcout << L"               attr.bit.dim: " << bool(fchar.attr.bit.dim) << L'\n';
  std::wcout << L"            attr.bit.italic: " << bool(fchar.attr.bit.italic) << L'\n';
  std::wcout << L"         attr.bit.underline: " << bool(fchar.attr.bit.underline) << L'\n';
  std::wcout << L"             attr.bit.blink: " << bool(fchar.attr.bit.blink) << L'\n';
  std::wcout << L"           attr.bit.reverse: " << bool(fchar.attr.bit.reverse) << L'\n';
  std::wcout << L"          attr.bit.standout: " << bool(fchar.attr.bit.standout) << L'\n';
  std::wcout << L"         attr.bit.invisible: " << bool(fchar.attr.bit.invisible) << L'\n';
  std::wcout << L"                    attr[1]: " << int(fchar.attr.byte[1]) << L'\n';
  std::wcout << L"           attr.bit.protect: " << bool(fchar.attr.bit.protect) << L'\n';
  std::wcout << L"       attr.bit.crossed_out: " << bool(fchar.attr.bit.crossed_out) << L'\n';
  std::wcout << L"     attr.bit.dbl_underline: " << bool(fchar.attr.bit.dbl_underline) << L'\n';
  std::wcout << L"       attr.bit.alt_charset: " << bool(fchar.attr.bit.alt_charset) << L'\n';
  std::wcout << L"        attr.bit.pc_charset: " << bool(fchar.attr.bit.pc_charset) << L'\n';
  std::wcout << L"       attr.bit.transparent: " << bool(fchar.attr.bit.transparent) << L'\n';
  std::wcout << L"     attr.bit.color_overlay: " << bool(fchar.attr.bit.color_overlay) << L'\n';
  std::wcout << L"attr.bit.inherit_background: " << bool(fchar.attr.bit.inherit_background) << L'\n';
  std::wcout << L"                    attr[2]: " << int(fchar.attr.byte[2]) << L'\n';
  std::wcout << L"        attr.bit.no_changes: " << bool(fchar.attr.bit.no_changes) << L'\n';
  std::wcout << L"           attr.bit.printed: " << bool(fchar.attr.bit.printed) << L'\n';
  std::wcout << L" attr.bit.fullwidth_padding: " << bool(fchar.attr.bit.fullwidth_padding) << L'\n';
  std::wcout << L"        attr.bit.char_width: " << int(fchar.attr.bit.char_width) << L'\n';
  std::wcout << L"                    attr[3]: " << int(fchar.attr.byte[3]) << L'\n'
             << std::noboolalpha;
}

//----------------------------------------------------------------------
std::size_t FVTermTest::getAreaSize (finalcut::FVTerm::FTermArea* area)
{
  const auto full_width = std::size_t(area->width) + std::size_t(area->right_shadow);
  const auto full_height = std::size_t(area->height) + std::size_t(area->bottom_shadow);
  return full_width * full_height;
}

//----------------------------------------------------------------------
bool FVTermTest::isAreaEqual ( finalcut::FVTerm::FTermArea* area1
                             , finalcut::FVTerm::FTermArea* area2 )
{
  auto size1 = getAreaSize(area1);
  auto size2 = getAreaSize(area2);

  if ( size1 != size2
    || area1->width != area2->width
    || area1->right_shadow != area2->right_shadow
    || area1->height != area2->height
    || area1->bottom_shadow != area2->bottom_shadow )
    return false;

  for (std::size_t i{0U}; i < size1; i++)
  {
    if ( ! isFCharEqual (area1->data[i], area2->data[i]) )
    {
      std::wcout << L"differ: char " << i << L" '"
                 << area1->data[i].ch[0] << L"' != '"
                 << area2->data[i].ch[0] << L"'\n";
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------
bool FVTermTest::isFCharEqual ( const finalcut::FChar& lhs
                              , const finalcut::FChar& rhs )
{
  return finalcut::isFUnicodeEqual(lhs.ch, rhs.ch)
      && finalcut::isFUnicodeEqual(lhs.encoded_char, rhs.encoded_char)
      && lhs.fg_color     == rhs.fg_color
      && lhs.bg_color     == rhs.bg_color
      && lhs.attr.byte[0] == rhs.attr.byte[0]
      && lhs.attr.byte[1] == rhs.attr.byte[1]
      && lhs.attr.byte[2] == rhs.attr.byte[2]
      && lhs.attr.byte[3] == rhs.attr.byte[3];
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FVTermTest);

// The general unit test main part
#include <main-test.inc>

