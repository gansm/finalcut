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
    finalcut::FTerm                      fterm{};
    static finalcut::FVTerm::FTermArea*  vterm;
    static finalcut::FTermData*          fterm_data;
    std::shared_ptr<OutputBuffer>        output_buffer{};
    std::shared_ptr<finalcut::FPoint>    term_pos{};
    finalcut::FChar                      term_attribute{};
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
  return finalcut::Encoding::UTF8;
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
{ }

// public methods of FTermOutputTest
//----------------------------------------------------------------------
inline const FTermOutputTest::FSetPalette& FTermOutputTest::getFSetPaletteRef() const &
{
  static const FSetPalette& f = &finalcut::FTerm::setPalette;
  return f;
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
    void p_resizeArea (const finalcut::FRect&, const finalcut::FSize&, FTermArea*) const;
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
inline void FVTerm_protected::p_resizeArea (const finalcut::FRect& box, const finalcut::FSize& shadow, FTermArea* area) const
{
  finalcut::FVTerm::resizeArea (box, shadow, area);
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
  finalcut::FVTerm::scrollAreaForward (area);
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
    void FVTermBasesTest();

  private:
    void         showFCharData (finalcut::FChar);
    std::size_t  getAreaSize (finalcut::FVTerm::FTermArea*);
    bool         isFCharEqual (const finalcut::FChar&, const finalcut::FChar&);

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FVTermTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (FVTermBasesTest);

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

  finalcut::FChar default_char;
  default_char.ch           = { L' ', L'\0', L'\0', L'\0', L'\0' };
  default_char.encoded_char = { L'\0', L'\0', L'\0', L'\0', L'\0' };
  default_char.fg_color     = finalcut::FColor::Default;
  default_char.bg_color     = finalcut::FColor::Default;
  default_char.attr.byte[0] = 0;
  default_char.attr.byte[1] = 0;
  default_char.attr.byte[2] = 0;
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
  CPPUNIT_ASSERT ( getAreaSize(vterm) == 1920 );

  for (std::size_t pos = 0; pos < getAreaSize(vdesktop); pos++)
    CPPUNIT_ASSERT ( isFCharEqual(vdesktop->data[pos], default_char) );

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

  finalcut::FChar bg_char;
  bg_char.ch           = { L'▒', L'\0', L'\0', L'\0', L'\0' };
  bg_char.encoded_char = { L'\0', L'\0', L'\0', L'\0', L'\0' };
  bg_char.fg_color     = finalcut::FColor::Default;
  bg_char.bg_color     = finalcut::FColor::Default;
  bg_char.attr.byte[0] = 0;
  bg_char.attr.byte[1] = 0;
  bg_char.attr.byte[2] = 0;
  bg_char.attr.byte[3] = 0;

  const auto full_width = std::size_t(vwin->width)
                        + std::size_t(vwin->right_shadow);

  for (auto y{0u}; y < std::size_t(vwin->height); y++)
  {
    for (auto x{0u}; x < std::size_t(vwin->width); x++)
    {
      CPPUNIT_ASSERT ( isFCharEqual(vwin->data[y * full_width + x], bg_char) );
    }
  }

  auto width = std::size_t(vwin->width);
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

  for (auto y{0u}; y < std::size_t(full_height); y++)
  {
    for (auto x{width}; x < full_width; x++)
    {
      CPPUNIT_ASSERT ( isFCharEqual(vwin->data[y * full_width + x], shadow_char) );
    }
  }

  for (auto x{0u}; x < full_width; x++)
  {
    auto y = full_height - 1;
    CPPUNIT_ASSERT ( isFCharEqual(vwin->data[y * full_width + x], shadow_char) );
  }

  p_fvterm.p_processTerminalUpdate();
  //CPPUNIT_ASSERT ( ! vwin->has_changes );

  showFCharData(vwin->data[vwin->width + 1]);

  std::cerr << "\n" << "offset_left: " << int(vwin->changes[full_height].trans_count) << '\n';
  std::cerr << "getAreaSize(vterm): " << int(getAreaSize(vdesktop)) << '\n';
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
  std::wcout << L"             attr.bit.blink: " << bool(fchar.attr.bit.bold) << L'\n';
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
std::size_t FVTermTest::getAreaSize (finalcut::FVTerm::FTermArea* vwin)
{
  const auto full_width = std::size_t(vwin->width) + std::size_t(vwin->right_shadow);
  const auto full_height = std::size_t(vwin->height) + std::size_t(vwin->bottom_shadow);
  return full_width * full_height;
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

