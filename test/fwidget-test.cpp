/***********************************************************************
* fwidget-test.cpp - FWidget unit tests                                *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2023 Markus Gans                                      *
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

#include <limits>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>
#include <fvterm_check.h>

//----------------------------------------------------------------------
auto getLastEvent() -> finalcut::Event&
{
  static finalcut::Event event = finalcut::Event::None;
  return event;
}

//----------------------------------------------------------------------
void setLastEvent (finalcut::Event event)
{
  getLastEvent() = event;
}

//----------------------------------------------------------------------
void resetLastEvent()
{
  getLastEvent() = finalcut::Event::None;
}

//----------------------------------------------------------------------
// class FWidget_protected
//----------------------------------------------------------------------

class FWidget_protected : public finalcut::FWidget
{
  public:
    // Using-declarations
    using FWidgetList = std::vector<finalcut::FWidget*>;

    // Disable copy constructor
    FWidget_protected (const FWidget_protected&) = delete;

    // Disable move constructor
    FWidget_protected (FWidget_protected&&) noexcept = delete;

    // Constructor
    explicit FWidget_protected (finalcut::FWidget* = nullptr);

    // Accessor
    auto  getPrintArea() -> finalcut::FWidget::FTermArea* override;
    static auto p_getModalDialogCounter() -> uInt;
    static auto p_getDialogList() -> FWidgetList*&;
    static auto p_getAlwaysOnTopList() -> FWidgetList*&;
    static auto p_getWidgetCloseList() -> FWidgetList*&;
    void  addPreprocessingHandler ( const finalcut::FVTerm*
                                  , FPreprocessingFunction&& ) override;
    void  delPreprocessingHandler (const finalcut::FVTerm*) override;

    // Inquiry
    auto  p_isChildPrintArea() const -> bool;

    // Mutators
    void setStatusBar (finalcut::FStatusBar*) override;
    void setMenuBar (finalcut::FMenuBar*) override;
    static auto  p_setModalDialogCounter() -> uInt&;
    void  p_setParentOffset();
    void  p_setTermOffset();
    void  p_setTermOffsetWithPadding();

    // Methods
    void  initTerminal() override;
    void  p_initDesktop();
    void  initLayout() override;
    void  adjustSize() override;
    void  p_adjustSizeGlobal();
    void  p_hideArea (const finalcut::FSize&);

    // Event handlers
    auto event (finalcut::FEvent*) -> bool override;
    void onKeyPress (finalcut::FKeyEvent*) override;
    void onKeyUp (finalcut::FKeyEvent*) override;
    void onKeyDown (finalcut::FKeyEvent*) override;
    void onMouseDown (finalcut::FMouseEvent*) override;
    void onMouseUp (finalcut::FMouseEvent*) override;
    void onMouseDoubleClick (finalcut::FMouseEvent*) override;
    void onWheel (finalcut::FWheelEvent*) override;
    void onMouseMove (finalcut::FMouseEvent*) override;
    void onFocusIn (finalcut::FFocusEvent*) override;
    void onFocusOut (finalcut::FFocusEvent*) override;
    void onChildFocusIn (finalcut::FFocusEvent*) override;
    void onChildFocusOut (finalcut::FFocusEvent*) override;
    void onFailAtChildFocus (finalcut::FFocusEvent*) override;
    void onAccel (finalcut::FAccelEvent*) override;
    void onResize (finalcut::FResizeEvent*) override;
    void onShow (finalcut::FShowEvent*) override;
    void onHide (finalcut::FHideEvent*) override;
    void onClose (finalcut::FCloseEvent*) override;

    // From FObject
    void p_setWidgetProperty (bool);
};

//----------------------------------------------------------------------
inline FWidget_protected::FWidget_protected (finalcut::FWidget* parent)
  : finalcut::FWidget{parent}
{ }

//----------------------------------------------------------------------
inline auto FWidget_protected::getPrintArea() -> finalcut::FWidget::FTermArea*
{
  return finalcut::FWidget::getPrintArea();
}

//----------------------------------------------------------------------
inline auto FWidget_protected::p_getModalDialogCounter() -> uInt
{
  return finalcut::FWidget::getModalDialogCounter();
}

//----------------------------------------------------------------------
inline auto FWidget_protected::p_getDialogList() -> FWidgetList*&
{
  return finalcut::FWidget::getDialogList();
}

//----------------------------------------------------------------------
inline auto FWidget_protected::p_getAlwaysOnTopList() -> FWidgetList*&
{
  return finalcut::FWidget::getAlwaysOnTopList();
}

//----------------------------------------------------------------------
inline auto FWidget_protected::p_getWidgetCloseList() -> FWidgetList*&
{
  return finalcut::FWidget::getWidgetCloseList();
}

//----------------------------------------------------------------------
inline void FWidget_protected::addPreprocessingHandler ( const finalcut::FVTerm* instance
                                                       , FPreprocessingFunction&& function )
{
  finalcut::FWidget::addPreprocessingHandler (instance, std::move(function));
}

//----------------------------------------------------------------------
inline void FWidget_protected::delPreprocessingHandler (const finalcut::FVTerm* instance)
{
  finalcut::FWidget::delPreprocessingHandler (instance);
}

//----------------------------------------------------------------------
inline auto FWidget_protected::p_isChildPrintArea() const -> bool
{
  return finalcut::FWidget::isChildPrintArea();
}

//----------------------------------------------------------------------
inline void FWidget_protected::setStatusBar (finalcut::FStatusBar* sbar)
{
  finalcut::FWidget::setStatusBar (sbar);
}

//----------------------------------------------------------------------
inline void FWidget_protected::setMenuBar (finalcut::FMenuBar* mbar)
{
  finalcut::FWidget::setMenuBar (mbar);
}

//----------------------------------------------------------------------
inline auto FWidget_protected::p_setModalDialogCounter() -> uInt&
{
  return finalcut::FWidget::setModalDialogCounter();
}

//----------------------------------------------------------------------
inline void FWidget_protected::p_setParentOffset()
{
  finalcut::FWidget::setParentOffset();
}

//----------------------------------------------------------------------
inline void FWidget_protected::p_setTermOffset()
{
  finalcut::FWidget::setTermOffset();
}

//----------------------------------------------------------------------
inline void FWidget_protected::p_setTermOffsetWithPadding()
{
  finalcut::FWidget::setTermOffsetWithPadding();
}

//----------------------------------------------------------------------
inline void FWidget_protected::initTerminal()
{
  finalcut::FWidget::initTerminal();
}

//----------------------------------------------------------------------
inline void FWidget_protected::p_initDesktop()
{
  finalcut::FWidget::initDesktop();
}

//----------------------------------------------------------------------
inline void FWidget_protected::initLayout()
{
  finalcut::FWidget::initLayout();
}

//----------------------------------------------------------------------
inline void FWidget_protected::adjustSize()
{
  finalcut::FWidget::adjustSize();
}

//----------------------------------------------------------------------
inline void FWidget_protected::p_adjustSizeGlobal()
{
  finalcut::FWidget::adjustSizeGlobal();
}

//----------------------------------------------------------------------
inline void FWidget_protected::p_hideArea (const finalcut::FSize& size)
{
  finalcut::FWidget::hideArea (size);
}

//----------------------------------------------------------------------
inline auto FWidget_protected::event (finalcut::FEvent* ev) -> bool
{
  return finalcut::FWidget::event(ev);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onKeyPress (finalcut::FKeyEvent*)
{
  setLastEvent (finalcut::Event::KeyPress);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onKeyUp (finalcut::FKeyEvent*)
{
  setLastEvent (finalcut::Event::KeyUp);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onKeyDown (finalcut::FKeyEvent*)
{
  setLastEvent (finalcut::Event::KeyDown);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onMouseDown (finalcut::FMouseEvent*)
{
  setLastEvent (finalcut::Event::MouseDown);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onMouseUp (finalcut::FMouseEvent*)
{
  setLastEvent (finalcut::Event::MouseUp);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onMouseDoubleClick (finalcut::FMouseEvent*)
{
  setLastEvent (finalcut::Event::MouseDoubleClick);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onWheel (finalcut::FWheelEvent*)
{
  setLastEvent (finalcut::Event::MouseWheel);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onMouseMove (finalcut::FMouseEvent*)
{
  setLastEvent (finalcut::Event::MouseMove);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onFocusIn (finalcut::FFocusEvent*)
{
  setLastEvent (finalcut::Event::FocusIn);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onFocusOut (finalcut::FFocusEvent*)
{
  setLastEvent (finalcut::Event::FocusOut);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onChildFocusIn (finalcut::FFocusEvent*)
{
  setLastEvent (finalcut::Event::ChildFocusIn);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onChildFocusOut (finalcut::FFocusEvent*)
{
  setLastEvent (finalcut::Event::ChildFocusOut);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onFailAtChildFocus (finalcut::FFocusEvent*)
{
  setLastEvent (finalcut::Event::FailAtChildFocus);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onAccel (finalcut::FAccelEvent*)
{
  setLastEvent (finalcut::Event::Accelerator);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onResize (finalcut::FResizeEvent*)
{
  setLastEvent (finalcut::Event::Resize);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onShow (finalcut::FShowEvent*)
{
  setLastEvent (finalcut::Event::Show);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onHide (finalcut::FHideEvent*)
{
  setLastEvent (finalcut::Event::Hide);
}

//----------------------------------------------------------------------
inline void FWidget_protected::onClose (finalcut::FCloseEvent*)
{
  setLastEvent (finalcut::Event::Close);
}

//----------------------------------------------------------------------
void FWidget_protected::p_setWidgetProperty (bool property)
{
  finalcut::FObject::setWidgetProperty (property);
}

//----------------------------------------------------------------------
// class FWidgetTest
//----------------------------------------------------------------------

class FWidgetTest : public CPPUNIT_NS::TestFixture
{
  public:
    FWidgetTest() = default;

  protected:
    void classNameTest();
    void noArgumentTest();
    void colorThemeTest();
    void resetColorsTest();
    void acceleratorTest();
    void PosAndSizeTest();
    void focusableChildrenTest();
    void closeWidgetTest();
    void adjustSizeTest();
    void callbackTest();

  private:
    class FSystemTest;

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FWidgetTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (colorThemeTest);
    CPPUNIT_TEST (resetColorsTest);
    CPPUNIT_TEST (acceleratorTest);
    CPPUNIT_TEST (PosAndSizeTest);
    CPPUNIT_TEST (focusableChildrenTest);
    CPPUNIT_TEST (closeWidgetTest);
    CPPUNIT_TEST (adjustSizeTest);
    CPPUNIT_TEST (callbackTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
// class FWidgetTest::FSystemTest
//----------------------------------------------------------------------

class FWidgetTest::FSystemTest : public finalcut::FSystem
{
  public:
    // Constructor
    FSystemTest()
    { }

    // Methods
    auto inPortByte (uShort) -> uChar override
    {
      return 0;
    }

    void outPortByte (uChar, uShort) override
    { }

    auto isTTY (int) const -> int override
    {
      return 1;
    }

    auto ioctl (int, uLong request, ...) -> int override
    {
      va_list args{};
      void* argp{};
      int ret_val{-1};

      va_start (args, request);
      argp = va_arg (args, void*);

      switch ( request )
      {
        case TIOCGWINSZ:
          auto win_size = static_cast<winsize*>(argp);
          win_size->ws_col = screen_size.getWidth();
          win_size->ws_row = screen_size.getHeight();
          ret_val = 0;
          break;
      }

      va_end (args);
      return ret_val;
    }

    auto pipe (finalcut::PipeData&) -> int override
    {
      return 0;
    }

    auto open (const char*, int, ...) -> int override
    {
      return 0;
    }

    auto close (int) -> int override
    {
      return 0;
    }

    auto fopen (const char*, const char*) -> FILE* override
    {
      return nullptr;
    }

    auto fputs (const char* str, FILE* stream) -> int override
    {
      return std::fputs(str, stream);
    }

    auto fclose (FILE*) -> int override
    {
      return 0;
    }

    auto putchar (int c) -> int override
    {
#if defined(__sun) && defined(__SVR4)
      return std::putchar(char(c));
#else
      return std::putchar(c);
#endif
    }

    auto sigaction (int, const struct sigaction*, struct sigaction*) -> int override
    {
      return 0;
    }

    auto timer_create (clockid_t, struct sigevent*, timer_t*) -> int override
    {
      return 0;
    }

    auto timer_settime ( timer_t, int
                       , const struct itimerspec*
                       , struct itimerspec* ) -> int override
    {
      return 0;
    }

    auto timer_delete (timer_t) -> int override
    {
      return 0;
    }

    auto kqueue() -> int override
    {
      return 0;
    }

    auto kevent ( int, const struct kevent*
                , int, struct kevent*
                , int, const struct timespec* ) -> int override
    {
      return 0;
    }

    auto getuid() -> uid_t override
    {
      return 0;
    }

    auto geteuid() -> uid_t override
    {
      return 0;
    }

    auto getpwuid_r ( uid_t, struct passwd*, char*
                   , size_t, struct passwd** ) -> int override
    {
      return 0;
    }

    auto realpath (const char*, char*) -> char* override
    {
      return const_cast<char*>("");
    }

    void setScreenSize (finalcut::FSize size)
    {
      screen_size = size;
    }

  private:
    static finalcut::FSize screen_size;
};

// static class attribute
//----------------------------------------------------------------------
finalcut::FSize FWidgetTest::FSystemTest::screen_size = finalcut::FSize(80, 24);

//----------------------------------------------------------------------
void FWidgetTest::classNameTest()
{
  finalcut::FWidget w{nullptr};
  const finalcut::FString& classname = w.getClassName();
  CPPUNIT_ASSERT ( classname == "FWidget" );
  CPPUNIT_ASSERT ( w.getRootWidget() == &w );
}

//----------------------------------------------------------------------
void FWidgetTest::noArgumentTest()
{
  std::unique_ptr<finalcut::FSystem> fsys = std::make_unique<FSystemTest>();
  finalcut::FTerm::setFSystem(fsys);

  {
    // First root object
    finalcut::FWidget wdgt1{};
    CPPUNIT_ASSERT ( wdgt1.getRootWidget() == &wdgt1 );
    CPPUNIT_ASSERT ( wdgt1.getParentWidget() == nullptr );

    // Second root object
    finalcut::FWidget wdgt2{};
    CPPUNIT_ASSERT ( wdgt2.getRootWidget() == &wdgt2 );

    std::cout << "\n-> An exit error message should be displayed:\n";
  }

  // Root widget
  finalcut::FWidget root_wdgt{};
  CPPUNIT_ASSERT ( root_wdgt.getRootWidget() == &root_wdgt );
  CPPUNIT_ASSERT ( root_wdgt.getParentWidget() == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.getMainWidget() == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.getActiveWindow() == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.getFocusWidget() == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.getClickedWidget() == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.getOpenMenu() == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.getMoveResizeWidget() == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.getMenuBar() == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.getStatusBar() == nullptr );

  // Child widget
  finalcut::FWidget wdgt{&root_wdgt};
  CPPUNIT_ASSERT ( wdgt.getRootWidget() == &root_wdgt );
  CPPUNIT_ASSERT ( wdgt.getParentWidget() == &root_wdgt );
  CPPUNIT_ASSERT ( wdgt.getMainWidget() == nullptr );

  // Set active window
  CPPUNIT_ASSERT ( wdgt.getActiveWindow() == nullptr );
  wdgt.setActiveWindow(&wdgt);
  CPPUNIT_ASSERT ( wdgt.getActiveWindow() == &wdgt );

  // Set focus widget
  CPPUNIT_ASSERT ( wdgt.getFocusWidget() == nullptr );
  wdgt.setFocusWidget(&wdgt);
  CPPUNIT_ASSERT ( wdgt.getFocusWidget() == &wdgt );

  // Set clicked widget
  CPPUNIT_ASSERT ( wdgt.getClickedWidget() == nullptr );
  wdgt.setClickedWidget(&wdgt);
  CPPUNIT_ASSERT ( wdgt.getClickedWidget() == &wdgt );

  // Set open menu
  CPPUNIT_ASSERT ( wdgt.getOpenMenu() == nullptr );
  wdgt.setOpenMenu(&wdgt);
  CPPUNIT_ASSERT ( wdgt.getOpenMenu() == &wdgt );

  // Set the moving or resizing widget
  CPPUNIT_ASSERT ( wdgt.getMoveResizeWidget() == nullptr );
  wdgt.setMoveSizeWidget(&wdgt);
  CPPUNIT_ASSERT ( wdgt.getMoveResizeWidget() == &wdgt );

  // Menu bar and status bar
  CPPUNIT_ASSERT ( wdgt.getMenuBar() == nullptr );
  CPPUNIT_ASSERT ( wdgt.getStatusBar() == nullptr );

  // Set main widget
  wdgt.setMainWidget(&wdgt);
  CPPUNIT_ASSERT ( wdgt.getMainWidget() != nullptr );
  CPPUNIT_ASSERT ( wdgt.getMainWidget() == &wdgt );
  CPPUNIT_ASSERT ( root_wdgt.getMainWidget() == &wdgt );

  // Test the status bar message
  CPPUNIT_ASSERT ( wdgt.getStatusbarMessage().isEmpty() );
  wdgt.setStatusbarMessage("message");
  CPPUNIT_ASSERT ( ! wdgt.getStatusbarMessage().isEmpty() );
  CPPUNIT_ASSERT ( wdgt.getStatusbarMessage() == "message" );
  wdgt.clearStatusbarMessage();
  CPPUNIT_ASSERT ( wdgt.getStatusbarMessage().isEmpty() );
  CPPUNIT_ASSERT ( wdgt.getStatusbarMessage().isEmpty() );

  // Test the primary widget colors
  CPPUNIT_ASSERT ( wdgt.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( wdgt.getBackgroundColor() == finalcut::FColor::Default );
  wdgt.setColor();
  CPPUNIT_ASSERT ( wdgt.getTermForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( wdgt.getTermBackgroundColor() == finalcut::FColor::Default );
  wdgt.setForegroundColor(finalcut::FColor::White);
  wdgt.setBackgroundColor(finalcut::FColor::DarkSeaGreen5);
  CPPUNIT_ASSERT ( wdgt.getForegroundColor() == finalcut::FColor::White );
  CPPUNIT_ASSERT ( wdgt.getBackgroundColor() == finalcut::FColor::DarkSeaGreen5 );
  wdgt.setColor();
  CPPUNIT_ASSERT ( wdgt.getTermForegroundColor() == finalcut::FColor::White );
  CPPUNIT_ASSERT ( wdgt.getTermBackgroundColor() == finalcut::FColor::DarkSeaGreen5 );
  wdgt.setForegroundColor(finalcut::FColor(256));  // FColor value > 255
  wdgt.setBackgroundColor(finalcut::FColor(257));  // FColor value > 255
  CPPUNIT_ASSERT ( wdgt.getForegroundColor() == finalcut::FColor::White );
  CPPUNIT_ASSERT ( wdgt.getBackgroundColor() == finalcut::FColor::DarkSeaGreen5 );

  // Parent widget color
  CPPUNIT_ASSERT ( root_wdgt.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( root_wdgt.getBackgroundColor() == finalcut::FColor::Default );
  root_wdgt.useParentWidgetColor();
  CPPUNIT_ASSERT ( root_wdgt.getForegroundColor() == finalcut::FColor::Black );
  CPPUNIT_ASSERT ( root_wdgt.getBackgroundColor() == finalcut::FColor::LightGray );
  CPPUNIT_ASSERT ( root_wdgt.getTermForegroundColor() == finalcut::FColor::Black );
  CPPUNIT_ASSERT ( root_wdgt.getTermBackgroundColor() == finalcut::FColor::LightGray );
  root_wdgt.setForegroundColor(finalcut::FColor::Yellow);
  root_wdgt.setBackgroundColor(finalcut::FColor::DarkBlue);
  wdgt.useParentWidgetColor();
  CPPUNIT_ASSERT ( wdgt.getForegroundColor() == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( wdgt.getBackgroundColor() == finalcut::FColor::DarkBlue );
  CPPUNIT_ASSERT ( wdgt.getTermForegroundColor() == finalcut::FColor::Yellow );
  CPPUNIT_ASSERT ( wdgt.getTermBackgroundColor() == finalcut::FColor::DarkBlue );

  // Get and set double flat line
  auto& top = wdgt.doubleFlatLine_ref(finalcut::Side::Top);
  auto& right = wdgt.doubleFlatLine_ref(finalcut::Side::Right);
  auto& bottom = wdgt.doubleFlatLine_ref(finalcut::Side::Bottom);
  auto& left = wdgt.doubleFlatLine_ref(finalcut::Side::Left);
  CPPUNIT_ASSERT_THROW ( wdgt.doubleFlatLine_ref(static_cast<finalcut::Side>(99))
                       , std::invalid_argument );
  CPPUNIT_ASSERT ( top.size() == 1 );
  CPPUNIT_ASSERT ( right.size() == 1 );
  CPPUNIT_ASSERT ( bottom.size() == 1 );
  CPPUNIT_ASSERT ( left.size() == 1 );
  CPPUNIT_ASSERT ( top[0] == false );
  CPPUNIT_ASSERT ( right[0] == false );
  CPPUNIT_ASSERT ( bottom[0] == false );
  CPPUNIT_ASSERT ( left[0] == false );
  wdgt.setDoubleFlatLine(finalcut::Side::Top);
  CPPUNIT_ASSERT ( top[0] == true );
  wdgt.setDoubleFlatLine(finalcut::Side::Right);
  CPPUNIT_ASSERT ( right[0] == true );
  wdgt.setDoubleFlatLine(finalcut::Side::Bottom);
  CPPUNIT_ASSERT ( bottom[0] == true );
  wdgt.setDoubleFlatLine(finalcut::Side::Left);
  CPPUNIT_ASSERT ( left[0] == true );
  top[0] = false;
  right[0] = false;
  bottom[0] = false;
  left[0] = false;
  CPPUNIT_ASSERT ( top[0] == false );
  CPPUNIT_ASSERT ( right[0] == false );
  CPPUNIT_ASSERT ( bottom[0] == false );
  CPPUNIT_ASSERT ( left[0] == false );
  CPPUNIT_ASSERT_THROW ( wdgt.setDoubleFlatLine(static_cast<finalcut::Side>(99))
                       , std::invalid_argument );
  CPPUNIT_ASSERT ( top[0] == false );
  CPPUNIT_ASSERT ( right[0] == false );
  CPPUNIT_ASSERT ( bottom[0] == false );
  CPPUNIT_ASSERT ( left[0] == false );

  // Positioning and sizes
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getClientWidth() == 0 );
  CPPUNIT_ASSERT ( wdgt.getClientHeight() == 0 );
  CPPUNIT_ASSERT ( wdgt.getClientSize() == finalcut::FSize(0, 0) );
  CPPUNIT_ASSERT ( wdgt.getMaxWidth() == 80 );
  CPPUNIT_ASSERT ( wdgt.getMaxHeight() == 24 );
  CPPUNIT_ASSERT ( wdgt.getShadow() == finalcut::FSize(0, 0) );
  CPPUNIT_ASSERT ( wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( wdgt.getGeometryWithShadow() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( wdgt.getTermGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( wdgt.getTermGeometryWithShadow() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( wdgt.getDesktopWidth() == 80 );
  CPPUNIT_ASSERT ( wdgt.getDesktopHeight() == 24 );
  CPPUNIT_ASSERT ( wdgt.getCursorPos() == finalcut::FPoint(-1, -1) );
  CPPUNIT_ASSERT ( wdgt.getPrintPos() == finalcut::FPoint(0, 0) );
  wdgt.setPrintPos(finalcut::FPoint(30, 10));
  CPPUNIT_ASSERT ( wdgt.getPrintPos() == finalcut::FPoint(30, 10) );
  wdgt.setPrintPos(finalcut::FPoint(-3, -10));
  CPPUNIT_ASSERT ( wdgt.getPrintPos() == finalcut::FPoint(-3, -10) );

  // Widget flags
  CPPUNIT_ASSERT ( wdgt.getFlags().shadow.shadow == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().shadow.trans_shadow == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.active == true );
  CPPUNIT_ASSERT ( wdgt.isEnabled() );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible == true );
  CPPUNIT_ASSERT ( wdgt.isVisible() );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.shown == false );
  CPPUNIT_ASSERT ( ! wdgt.isShown() );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.hidden == false );
  CPPUNIT_ASSERT ( ! wdgt.isHidden() );
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focus == false );
  CPPUNIT_ASSERT ( ! wdgt.hasFocus() );
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focusable == true );
  CPPUNIT_ASSERT ( wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.scrollable == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.resizeable == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.minimizable == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.modal == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible_cursor == false );
  CPPUNIT_ASSERT ( ! wdgt.hasVisibleCursor() );
  CPPUNIT_ASSERT ( wdgt.getFlags().type.window_widget == false );
  CPPUNIT_ASSERT ( ! wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt.getFlags().type.dialog_widget == false );
  CPPUNIT_ASSERT ( ! wdgt.isDialogWidget() );
  CPPUNIT_ASSERT ( wdgt.getFlags().type.menu_widget == false );
  CPPUNIT_ASSERT ( ! wdgt.isMenuWidget() );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.always_on_top == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.flat == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.no_border == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.no_underline == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.ignore_padding == false );

  wdgt.setVisible(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible == false );
  CPPUNIT_ASSERT ( ! wdgt.isVisible() );
  wdgt.setVisible(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible == true );
  CPPUNIT_ASSERT ( wdgt.isVisible() );
  wdgt.unsetVisible();
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible == false );
  CPPUNIT_ASSERT ( ! wdgt.isVisible() );
  wdgt.setVisible();
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible == true );
  CPPUNIT_ASSERT ( wdgt.isVisible() );

  wdgt.setEnable(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.active == false );
  wdgt.setEnable(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.active == true );
  wdgt.unsetEnable();
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.active == false );
  wdgt.setEnable();
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.active == true );
  wdgt.setDisable();
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.active == false );

  wdgt.setVisibleCursor(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible_cursor == false );
  wdgt.setVisibleCursor(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible_cursor == true );
  wdgt.unsetVisibleCursor();
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible_cursor == false );
  wdgt.setVisibleCursor();
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.visible_cursor == true );

  wdgt.setFocus(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focus == false );
  wdgt.setFocus(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focus == false );
  wdgt.setEnable();
  wdgt.setFocus(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focus == true );
  wdgt.unsetFocus();
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focus == false );
  wdgt.setFocus();
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focus == true );

  wdgt.setFocusable(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focusable == false );
  wdgt.setFocusable(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focusable == true );
  wdgt.unsetFocusable();
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focusable == false );
  wdgt.setFocusable();
  CPPUNIT_ASSERT ( wdgt.getFlags().focus.focusable == true );

  wdgt.ignorePadding(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.ignore_padding == false );
  CPPUNIT_ASSERT ( ! wdgt.isPaddingIgnored() );
  wdgt.ignorePadding(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.ignore_padding == true );
  CPPUNIT_ASSERT ( wdgt.isPaddingIgnored() );
  wdgt.acceptPadding();
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.ignore_padding == false );
  CPPUNIT_ASSERT ( ! wdgt.isPaddingIgnored() );
  wdgt.ignorePadding();
  CPPUNIT_ASSERT ( wdgt.getFlags().feature.ignore_padding == true );
  CPPUNIT_ASSERT ( wdgt.isPaddingIgnored() );
}

//----------------------------------------------------------------------
void FWidgetTest::colorThemeTest()
{
  {
    finalcut::FWidget root_wdgt{};  // Root widget

    // Get color theme
    const auto& color_theme = root_wdgt.getColorTheme();
    CPPUNIT_ASSERT ( finalcut::FVTerm::getFOutput()->getMaxColor() == 1 );
    // Less than 16 colors -> default8ColorTheme
    CPPUNIT_ASSERT ( color_theme->term.fg == finalcut::FColor::LightGray );
    CPPUNIT_ASSERT ( color_theme->term.bg == finalcut::FColor::Blue );
  }

  {
    finalcut::FTermcap::max_color = 8;
    finalcut::FWidget root_wdgt{};  // Root widget

    // Get color theme
    const auto& color_theme = root_wdgt.getColorTheme();
    CPPUNIT_ASSERT ( finalcut::FVTerm::getFOutput()->getMaxColor() == 8 );
    // Less than 16 colors -> default8ColorTheme
    CPPUNIT_ASSERT ( color_theme->term.fg == finalcut::FColor::LightGray );
    CPPUNIT_ASSERT ( color_theme->term.bg == finalcut::FColor::Blue );
  }

  {
    finalcut::FTermcap::max_color = 16;
    finalcut::FWidget root_wdgt{};  // Root widget

    // Get color theme
    const auto& color_theme = root_wdgt.getColorTheme();
    CPPUNIT_ASSERT ( finalcut::FVTerm::getFOutput()->getMaxColor() == 16 );
    // At least 16 colors or more -> default16ColorTheme
    CPPUNIT_ASSERT ( color_theme->term.fg == finalcut::FColor::Black );
    CPPUNIT_ASSERT ( color_theme->term.bg == finalcut::FColor::LightBlue );
  }
}

//----------------------------------------------------------------------
void FWidgetTest::resetColorsTest()
{
  finalcut::FWidget root_wdgt{};  // Root widget

  class TestWidget : public finalcut::FWidget
  {
    public:
      explicit TestWidget (finalcut::FWidget* parent = nullptr)
        : finalcut::FWidget{parent}
      { }

      TestWidget (const TestWidget&) = delete;

      TestWidget (TestWidget&&) noexcept = delete;

      ~TestWidget() override
      { }

      void resetColors() override
      {
        const auto& wc = getColorTheme();
        setForegroundColor (wc->dialog.fg);
        setBackgroundColor (wc->dialog.bg);
        FWidget::resetColors();
      }
  };

  TestWidget wdgt{&root_wdgt};  // Child widget

  CPPUNIT_ASSERT ( root_wdgt.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( root_wdgt.getBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( wdgt.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( wdgt.getBackgroundColor() == finalcut::FColor::Default );

  root_wdgt.resetColors();
  CPPUNIT_ASSERT ( root_wdgt.getForegroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( root_wdgt.getBackgroundColor() == finalcut::FColor::Default );
  CPPUNIT_ASSERT ( wdgt.getForegroundColor() == finalcut::FColor::Black );
  CPPUNIT_ASSERT ( wdgt.getBackgroundColor() == finalcut::FColor::White );
}

//----------------------------------------------------------------------
void FWidgetTest::acceleratorTest()
{
  finalcut::FWidget root_wdgt{};  // Root widget
  finalcut::FWidget wdgt{&root_wdgt};  // Child widget
  auto& accelerator_list = root_wdgt.getAcceleratorList();
  CPPUNIT_ASSERT ( accelerator_list.size() == 0 );

  CPPUNIT_ASSERT ( root_wdgt.getAcceleratorList().size() == 0 );
  finalcut::FWidget::FAcceleratorList new_accelerator_list =
      { {finalcut::FKey::Escape, &root_wdgt} };
  root_wdgt.setAcceleratorList() = new_accelerator_list;
  CPPUNIT_ASSERT ( root_wdgt.getAcceleratorList().size() == 1 );
  CPPUNIT_ASSERT ( root_wdgt.getAcceleratorList()[0].key == finalcut::FKey::Escape );
  CPPUNIT_ASSERT ( root_wdgt.getAcceleratorList()[0].object == &root_wdgt );

  wdgt.addAccelerator(finalcut::FKey::F1);
  CPPUNIT_ASSERT ( accelerator_list.size() == 2 );
  CPPUNIT_ASSERT ( accelerator_list[1].key == finalcut::FKey::F1 );
  CPPUNIT_ASSERT ( accelerator_list[1].object == &wdgt );

  wdgt.addAccelerator(finalcut::FKey::Menu, &root_wdgt);
  CPPUNIT_ASSERT ( accelerator_list.size() == 3 );
  CPPUNIT_ASSERT ( accelerator_list[2].key == finalcut::FKey::Menu );
  CPPUNIT_ASSERT ( accelerator_list[2].object == &root_wdgt );

  // Delete accelerator
  CPPUNIT_ASSERT ( accelerator_list.size() == 3 );
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 0 );
  root_wdgt.delAccelerator(&wdgt);
  CPPUNIT_ASSERT ( accelerator_list.size() == 2 );
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 0 );
  root_wdgt.delAccelerator(&wdgt);
  CPPUNIT_ASSERT ( accelerator_list.size() == 2 );
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 0 );
  root_wdgt.delAccelerator();
  CPPUNIT_ASSERT ( accelerator_list.size() == 0 );
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 0 );
  wdgt.setAcceleratorList() = std::move(new_accelerator_list);
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 1 );
  wdgt.delAccelerator();
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 1 );
  wdgt.setFlags().type.window_widget = true;
  wdgt.delAccelerator(&wdgt);
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 1 );
  wdgt.delAccelerator(&root_wdgt);
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 0 );
}

//----------------------------------------------------------------------
void FWidgetTest::PosAndSizeTest()
{
  std::unique_ptr<finalcut::FSystem> fsys = std::make_unique<FSystemTest>();
  finalcut::FTerm::setFSystem(fsys);

  finalcut::FWidget root_wdgt{};  // Root widget
  finalcut::FWidget wdgt{&root_wdgt};  // Child widget

  CPPUNIT_ASSERT ( root_wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( root_wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( root_wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( root_wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( root_wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( root_wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( root_wdgt.getWidth() == 80 );
  CPPUNIT_ASSERT ( root_wdgt.getHeight() == 24 );
  CPPUNIT_ASSERT ( root_wdgt.getSize() == finalcut::FSize(80, 24) );

  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );

  wdgt.setX(1);
  wdgt.setY(1);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(1, 1) );

  wdgt.setX(-1);
  wdgt.setY(-1);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(1, 1) );

  wdgt.setFlags().type.window_widget = true;
  wdgt.setX(-1);
  wdgt.setY(-1);
  CPPUNIT_ASSERT ( wdgt.getX() == -1 );
  CPPUNIT_ASSERT ( wdgt.getY() == -1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(-1, -1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == -1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == -1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(-1, -1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );

  wdgt.setX(5);
  wdgt.setY(3);
  CPPUNIT_ASSERT ( wdgt.getX() == 5 );
  CPPUNIT_ASSERT ( wdgt.getY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(5, 3) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 5 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(5, 3) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(3, 3)) == finalcut::FPoint(-1, 1) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(2, 2)) == finalcut::FPoint(-2, 0) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(-3, -1) );

  wdgt.setPos(finalcut::FPoint(5, 3));
  CPPUNIT_ASSERT ( wdgt.getX() == 5 );
  CPPUNIT_ASSERT ( wdgt.getY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(5, 3) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 5 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(5, 3) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(3, 3)) == finalcut::FPoint(-1, 1) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(2, 2)) == finalcut::FPoint(-2, 0) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(-3, -1) );

  wdgt.setPos(finalcut::FPoint(20, 15));
  CPPUNIT_ASSERT ( wdgt.getX() == 20 );
  CPPUNIT_ASSERT ( wdgt.getY() == 15 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(20, 15) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 20 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 15 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(20, 15) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );

  wdgt.setPos(finalcut::FPoint(-5, -7));
  CPPUNIT_ASSERT ( wdgt.getX() == -5 );
  CPPUNIT_ASSERT ( wdgt.getY() == -7 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(-5, -7) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == -5 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == -7 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(-5, -7) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );

  wdgt.setFlags().type.window_widget = false;
  wdgt.setPos(finalcut::FPoint(-9, -6));
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 1 );

  wdgt.setWidth(1);
  wdgt.setHeight(1);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 1 );

  wdgt.setFlags().type.window_widget = true;
  wdgt.setWidth(0);
  wdgt.setHeight(0);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 1 );

  wdgt.setWidth(400);
  wdgt.setHeight(100);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 400 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 100 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(400, 100) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 400 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 100 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 400 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 100 );

  wdgt.setSize(finalcut::FSize(400, 100));
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 400 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 100 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(400, 100) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 400 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 100 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 400 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 100 );

  wdgt.setSize(finalcut::FSize(0, 0));
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 1 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 1 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 1 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 1 );

  wdgt.setSize(finalcut::FSize(64, 13));
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 64 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 13 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(64, 13) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 64 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 13 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 64 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 13 );

  // Tests with minimum and maximum
  wdgt.setMinimumWidth(40);
  wdgt.setMinimumHeight(10);
  wdgt.setMaximumWidth(80);
  wdgt.setMaximumHeight(24);
  wdgt.setSize(finalcut::FSize(800, 600));
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 80 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 24 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(80, 24) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 80 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 24 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 80 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 24 );

  wdgt.setSize(finalcut::FSize(14, 7));
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 40 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 10 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(40, 10) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 40 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 10 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 40 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 10 );

  wdgt.setWidth(1000);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 80 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 10 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(80, 10) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 80 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 10 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 80 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 10 );

  wdgt.setHeight(1000);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 80 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 24 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(80, 24) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 80 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 24 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 80 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 24 );

  wdgt.setMinimumSize( finalcut::FSize(60, 20) );
  wdgt.setMaximumSize( finalcut::FSize(100, 40) );
  wdgt.setWidth(999);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 100 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 24 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(100, 24) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 100 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 24 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 100 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 24 );

  wdgt.setWidth(59);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 60 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 24 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(60, 24) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 60 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 24 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 60 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 24 );

  wdgt.setHeight(999);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 60 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 40 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(60, 40) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 60 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 40 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 60 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 40 );

  wdgt.setHeight(19);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 60 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 20 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(60, 20) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 60 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 20 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 60 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 20 );

  wdgt.setFixedSize( finalcut::FSize(45, 16) );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(60, 20) );
  wdgt.setWidth(10);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 45 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 20 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(45, 20) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 45 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 20 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 45 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 20 );

  wdgt.setFixedSize( finalcut::FSize(48, 16) );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(45, 20) );
  wdgt.setWidth(50);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 48 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 20 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(48, 20) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 48 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 20 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 48 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 20 );

  wdgt.setHeight(5);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 48 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 16 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(48, 16) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 48 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 16 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 48 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 16 );

  wdgt.setFixedSize( finalcut::FSize(50, 20) );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(48, 16) );
  wdgt.setHeight(25);
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 48 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 20 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(48, 20) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 48 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 20 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 48 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 20 );

  wdgt.setFixedSize( finalcut::FSize(30, 15) );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(48, 20) );
  wdgt.setSize( finalcut::FSize(3, 5) );
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 30 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 15 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(30, 15) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 30 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 15 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 30 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 15 );

  wdgt.setFixedSize( finalcut::FSize(34, 12) );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(30, 15) );
  wdgt.setSize( finalcut::FSize(35, 13) );
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 34 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 12 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(34, 12) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 34 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 12 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 34 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 12 );

  // Reset minimum and maximum
  wdgt.setMinimumSize(finalcut::FSize(0, 0));
  wdgt.setMaximumSize(finalcut::FSize(INT_MAX, INT_MAX));

  auto box = finalcut::FRect(finalcut::FPoint(7, 7), finalcut::FSize(10, 8) );
  wdgt.setGeometry (box);
  CPPUNIT_ASSERT ( wdgt.getX() == 7 );
  CPPUNIT_ASSERT ( wdgt.getY() == 7 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(7, 7) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 7 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 7 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(7, 7) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 10 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 8 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(10, 8) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 10 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 8 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 10 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 8 );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(3, 3)) == finalcut::FPoint(-3, -3) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(2, 2)) == finalcut::FPoint(-4, -4) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(-5, -5) );

  // Same position and size
  wdgt.setGeometry (box);
  CPPUNIT_ASSERT ( wdgt.getX() == 7 );
  CPPUNIT_ASSERT ( wdgt.getY() == 7 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(7, 7) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 7 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 7 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(7, 7) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 10 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 8 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(10, 8) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 10 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 8 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 10 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 8 );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(3, 3)) == finalcut::FPoint(-3, -3) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(2, 2)) == finalcut::FPoint(-4, -4) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(-5, -5) );

  wdgt.setGeometry(finalcut::FPoint(22, 33), finalcut::FSize(560, 130) );
  CPPUNIT_ASSERT ( wdgt.getX() == 22 );
  CPPUNIT_ASSERT ( wdgt.getY() == 33 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(22, 33) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 22 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 33 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(22, 33) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 560 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 130 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(560, 130) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 560 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 130 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 560 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 130 );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(3, 3)) == finalcut::FPoint(-18, -29) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(2, 2)) == finalcut::FPoint(-19, -30) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(-20, -31) );

  wdgt.setGeometry(finalcut::FPoint(-5, -3), finalcut::FSize(50, 30) );
  CPPUNIT_ASSERT ( wdgt.getX() == -5 );
  CPPUNIT_ASSERT ( wdgt.getY() == -3 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(-5, -3) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == -5 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == -3 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(-5, -3) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 50 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 30 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(50, 30) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 50 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 30 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 50 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 30 );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(3, 3)) == finalcut::FPoint(9, 7) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(2, 2)) == finalcut::FPoint(8, 6) );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(7, 5) );

  wdgt.setFlags().type.window_widget = false;
  wdgt.setFlags().visibility.shown = true;
  wdgt.setGeometry(finalcut::FPoint(-4, -2), finalcut::FSize(30, 20) );
  CPPUNIT_ASSERT ( wdgt.getX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 1 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 30 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 20 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(30, 20) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 30 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 20 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 30 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 20 );
  CPPUNIT_ASSERT ( wdgt.childWidgetAt({0, 0}) == nullptr );
  CPPUNIT_ASSERT ( wdgt.childWidgetAt({0, 1}) == nullptr );
  CPPUNIT_ASSERT ( wdgt.childWidgetAt({1, 0}) == nullptr );
  CPPUNIT_ASSERT ( wdgt.childWidgetAt({1, 1}) == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.childWidgetAt({0, 0}) == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.childWidgetAt({0, 1}) == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.childWidgetAt({1, 0}) == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.childWidgetAt({1, 1}) == &wdgt );
  CPPUNIT_ASSERT ( root_wdgt.childWidgetAt({30, 20}) == &wdgt );
  CPPUNIT_ASSERT ( root_wdgt.childWidgetAt({30, 21}) == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.childWidgetAt({31, 20}) == nullptr );
  CPPUNIT_ASSERT ( root_wdgt.childWidgetAt({31, 21}) == nullptr );

  // Double flat line
  wdgt.setDoubleFlatLine (finalcut::Side::Top, -6, true);  // ignore
  wdgt.setDoubleFlatLine (finalcut::Side::Top, -2, true);  // ignore
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 0, true);  // ignore
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 2, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 3, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 5, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 7, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 11, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 13, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 17, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 19, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 23, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Top, 29, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Right, 12, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Bottom, 5, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Bottom, 15, true);
  wdgt.setDoubleFlatLine (finalcut::Side::Left, 12, true);
  CPPUNIT_ASSERT_THROW ( wdgt.setDoubleFlatLine (static_cast<finalcut::Side>(99), 1, true)
                       , std::invalid_argument );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[0] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[1] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[2] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[3] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[4] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[5] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[6] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[7] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[8] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[9] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[10] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[11] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[12] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[13] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[14] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[15] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[16] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[17] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[18] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[19] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[20] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[21] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[22] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[23] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[24] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[25] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[26] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[27] == false );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[28] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top)[29] == false );

  for (int i{0}; i < 11; i++)
  {
    CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right)[i] == false );
    CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left)[i] == false );
  }

  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right)[11] == true );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left)[11] == true );

  for (int i{12}; i < 20; i++)
  {
    CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right)[i] == false );
    CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left)[i] == false );
  }

  for (int i{0}; i < 4; i++)
  {
    CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom)[i] == false );
  }

  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom)[4] == true );

  for (int i{5}; i < 14; i++)
  {
    CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom)[i] == false );
  }

  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom)[14] == true );

  for (int i{15}; i < 30; i++)
  {
    CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom)[i] == false );
  }

  // Test with shadow size
  wdgt.setGeometry(finalcut::FPoint(3, 3), finalcut::FSize(5, 5) );
  wdgt.setShadowSize(finalcut::FSize(2, 2));
  CPPUNIT_ASSERT ( wdgt.getX() == 3 );
  CPPUNIT_ASSERT ( wdgt.getY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(3, 3) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 3 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(3, 3) );
  CPPUNIT_ASSERT ( wdgt.getWidth() == 5 );
  CPPUNIT_ASSERT ( wdgt.getHeight() == 5 );
  CPPUNIT_ASSERT ( wdgt.getSize() == finalcut::FSize(5, 5) );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Top).size() == 5 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Right).size() == 5 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Bottom).size() == 5 );
  CPPUNIT_ASSERT ( wdgt.doubleFlatLine_ref(finalcut::Side::Left).size() == 5 );
  CPPUNIT_ASSERT ( wdgt.getShadow() == finalcut::FSize(2, 2) );
  CPPUNIT_ASSERT ( wdgt.getGeometryWithShadow() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(7, 7)) );
  CPPUNIT_ASSERT ( wdgt.getTermGeometryWithShadow() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(7, 7)) );

  // Cursor position
  CPPUNIT_ASSERT ( wdgt.getCursorPos() == finalcut::FPoint(-1, -1) );
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(0, 0)) == false );
  CPPUNIT_ASSERT ( wdgt.getCursorPos() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(30, 3)) == false );
  CPPUNIT_ASSERT ( wdgt.getCursorPos() == finalcut::FPoint(30, 3) );
  wdgt.unsetCursorPos();
  CPPUNIT_ASSERT ( wdgt.getCursorPos() == finalcut::FPoint(-1, -1) );
  wdgt.setFlags().visibility.hidden = false;
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(10, 3)) == false );
  wdgt.setFlags().focus.focus = true;
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(11, 3)) == false );
  wdgt.setFlags().type.window_widget = false;
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(12, 3)) == false );
  root_wdgt.setFlags().type.window_widget = true;
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(13, 3)) == true );
  CPPUNIT_ASSERT ( wdgt.getCursorPos() == finalcut::FPoint(13, 3) );

  // Padding
  CPPUNIT_ASSERT ( wdgt.getClientSize() == wdgt.getSize() );
  CPPUNIT_ASSERT ( wdgt.getClientSize() == finalcut::FSize(5, 5) );
  CPPUNIT_ASSERT ( wdgt.getClientWidth() == 5 );
  CPPUNIT_ASSERT ( wdgt.getClientHeight() == 5 );
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(3, 3)) == finalcut::FPoint(1, 1));
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(2, 2)) == finalcut::FPoint(0, 0));
  CPPUNIT_ASSERT ( wdgt.termToWidgetPos(finalcut::FPoint(1, 1)) == finalcut::FPoint(-1, -1));

  wdgt.setTopPadding(1);
  CPPUNIT_ASSERT ( wdgt.getClientSize() == finalcut::FSize(5, 4) );
  CPPUNIT_ASSERT ( wdgt.getClientWidth() == 5 );
  CPPUNIT_ASSERT ( wdgt.getClientHeight() == 4 );
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == 1 );
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 0 );
  wdgt.setTopPadding(1);
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == 1 );

  wdgt.setBottomPadding(2);
  CPPUNIT_ASSERT ( wdgt.getClientSize() == finalcut::FSize(5, 2) );
  CPPUNIT_ASSERT ( wdgt.getClientWidth() == 5 );
  CPPUNIT_ASSERT ( wdgt.getClientHeight() == 2 );
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == 1 );
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 2 );
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 0 );
  wdgt.setBottomPadding(2);
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 2 );

  wdgt.setRightPadding(2);
  CPPUNIT_ASSERT ( wdgt.getClientSize() == finalcut::FSize(3, 2) );
  CPPUNIT_ASSERT ( wdgt.getClientWidth() == 3 );
  CPPUNIT_ASSERT ( wdgt.getClientHeight() == 2 );
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == 1 );
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == 2 );
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 2 );
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 0 );
  wdgt.setRightPadding(2);
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == 2 );

  wdgt.setLeftPadding(1);
  CPPUNIT_ASSERT ( wdgt.getClientSize() == finalcut::FSize(2, 2) );
  CPPUNIT_ASSERT ( wdgt.getClientWidth() == 2 );
  CPPUNIT_ASSERT ( wdgt.getClientHeight() == 2 );
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == 1 );
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == 2 );
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 2 );
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 1 );
  wdgt.setLeftPadding(1);
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 1 );

  wdgt.setRightPadding(-2);
  CPPUNIT_ASSERT ( wdgt.getClientSize() == finalcut::FSize(6, 2) );
  CPPUNIT_ASSERT ( wdgt.getClientWidth() == 6 );
  CPPUNIT_ASSERT ( wdgt.getClientHeight() == 2 );
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == 1 );
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == -2 );
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 2 );
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 1 );

  wdgt.setTopPadding(-3);
  CPPUNIT_ASSERT ( wdgt.getClientSize() == finalcut::FSize(6, 6) );
  CPPUNIT_ASSERT ( wdgt.getClientWidth() == 6 );
  CPPUNIT_ASSERT ( wdgt.getClientHeight() == 6 );
  CPPUNIT_ASSERT ( wdgt.getTopPadding() == -3 );
  CPPUNIT_ASSERT ( wdgt.getRightPadding() == -2 );
  CPPUNIT_ASSERT ( wdgt.getBottomPadding() == 2 );
  CPPUNIT_ASSERT ( wdgt.getLeftPadding() == 1 );

  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(80, 24) );
  CPPUNIT_ASSERT ( root_wdgt.getClientWidth() == 80 );
  CPPUNIT_ASSERT ( root_wdgt.getClientHeight() == 24 );
  CPPUNIT_ASSERT ( root_wdgt.getTopPadding() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getRightPadding() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getBottomPadding() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getLeftPadding() == 0 );
  CPPUNIT_ASSERT ( wdgt.getX() == 3 );
  CPPUNIT_ASSERT ( wdgt.getY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(3, 3) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 3 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(3, 3) );

  root_wdgt.setTopPadding(1);
  root_wdgt.setRightPadding(2);
  root_wdgt.setBottomPadding(3);
  root_wdgt.setLeftPadding(4);
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(74, 20) );
  CPPUNIT_ASSERT ( root_wdgt.getClientWidth() == 74 );
  CPPUNIT_ASSERT ( root_wdgt.getClientHeight() == 20 );
  CPPUNIT_ASSERT ( root_wdgt.getTopPadding() == 1 );
  CPPUNIT_ASSERT ( root_wdgt.getRightPadding() == 2 );
  CPPUNIT_ASSERT ( root_wdgt.getBottomPadding() == 3 );
  CPPUNIT_ASSERT ( root_wdgt.getLeftPadding() == 4 );
  CPPUNIT_ASSERT ( wdgt.getX() == 3 );
  CPPUNIT_ASSERT ( wdgt.getY() == 3 );
  CPPUNIT_ASSERT ( wdgt.getPos() == finalcut::FPoint(3, 3) );
  CPPUNIT_ASSERT ( wdgt.getTermX() == 7 );
  CPPUNIT_ASSERT ( wdgt.getTermY() == 4 );
  CPPUNIT_ASSERT ( wdgt.getTermPos() == finalcut::FPoint(7, 4) );

  // Set terminal size
  root_wdgt.setTerminalSize( finalcut::FSize(132, 43) );
  CPPUNIT_ASSERT ( root_wdgt.getWidth() == 80 );
  CPPUNIT_ASSERT ( root_wdgt.getHeight() == 24 );
  CPPUNIT_ASSERT ( root_wdgt.getSize() == finalcut::FSize(80, 24) );
  finalcut::FTermcap tcap;
  tcap.setPutCharFunction (nullptr);  // Avoid resizing the real terminal
  tcap.setPutStringFunction (nullptr);
  auto& fdata = finalcut::FTermData::getInstance();
  fdata.setTermType(finalcut::FTermType::xterm);
  auto fsys_ptr = static_cast<FSystemTest*>(finalcut::FSystem::getInstance().get());
  fsys_ptr->setScreenSize(finalcut::FSize(132, 43));
  root_wdgt.setTerminalSize( finalcut::FSize(132, 43) );
  CPPUNIT_ASSERT ( root_wdgt.getWidth() == 132 );
  CPPUNIT_ASSERT ( root_wdgt.getHeight() == 43 );
  CPPUNIT_ASSERT ( root_wdgt.getSize() == finalcut::FSize(132, 43) );
}

//----------------------------------------------------------------------
void FWidgetTest::focusableChildrenTest()
{
  // root_wdgt -> main_wdgt
  finalcut::FWidget root_wdgt{};  // Root widget
  finalcut::FWidget main_wdgt{&root_wdgt};  // Child / main widget

  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 0 );

  // root_wdgt -> main_wdgt -> wdgt1
  //                        -> wdgt2
  //                        -> wdgt3
  //                        -> wdgt4
  finalcut::FWidget wdgt1{&main_wdgt};  // 1st Subchild
  finalcut::FWidget wdgt2{&main_wdgt};  // 2nd Subchild
  finalcut::FWidget wdgt3{&main_wdgt};  // 3rd Subchild
  finalcut::FWidget wdgt4{&main_wdgt};  // 4th Subchild

  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );

  // root_wdgt -> main_wdgt -> wdgt1 -> wdgt1_1
  //                                 -> wdgt1_2
  finalcut::FWidget wdgt1_1{&wdgt1};  // 1st Sub-subchild
  finalcut::FWidget wdgt1_2{&wdgt1};  // 2nd Sub-subchild

  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  main_wdgt.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 1 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( ! main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( !main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  main_wdgt.setFlags().type.window_widget = true;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt1.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 1 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt1_1.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 1 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 1 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt1_2.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 1 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt2.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt3.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt4.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 4 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt2.setFlags().focus.focusable = false;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt1.setFlags().focus.focusable = false;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( ! wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt4.setFlags().focus.focusable = false;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 1 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( ! wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt3.setFlags().focus.focusable = false;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( ! wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt1_1.setFlags().focus.focusable = false;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 1 );
  CPPUNIT_ASSERT ( ! wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( wdgt1_2.getFlags().focus.focusable );

  wdgt1_2.setFlags().focus.focusable = false;
  CPPUNIT_ASSERT ( root_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( root_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( main_wdgt.acceptFocus() );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().type.window_widget );
  CPPUNIT_ASSERT ( main_wdgt.isWindowWidget() );
  CPPUNIT_ASSERT ( wdgt1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt3.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt1_1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( wdgt1_2.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! wdgt1_2.getFlags().focus.focusable );

  // Test focusing the next widget
  FWidget_protected parent{&root_wdgt};
  FWidget_protected child1{&parent};
  FWidget_protected child2{&parent};
  FWidget_protected child3{&parent};
  FWidget_protected child4{&parent};

  CPPUNIT_ASSERT ( ! root_wdgt.focusNextChild() );  // No parent test
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( child1.getFlags().feature.active );
  CPPUNIT_ASSERT ( child2.getFlags().feature.active );
  CPPUNIT_ASSERT ( child3.getFlags().feature.active );
  CPPUNIT_ASSERT ( child4.getFlags().feature.active );
  CPPUNIT_ASSERT ( child1.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( child2.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( child3.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( child4.getFlags().focus.focusable );
  CPPUNIT_ASSERT ( ! child1.getFlags().visibility.shown );
  CPPUNIT_ASSERT ( ! child2.getFlags().visibility.shown );
  CPPUNIT_ASSERT ( ! child3.getFlags().visibility.shown );
  CPPUNIT_ASSERT ( ! child4.getFlags().visibility.shown );
  child1.setFlags().visibility.shown = true;
  child2.setFlags().visibility.shown = true;
  child3.setFlags().visibility.shown = true;
  child4.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 4 );
  CPPUNIT_ASSERT ( child1.getFlags().visibility.shown );
  CPPUNIT_ASSERT ( child2.getFlags().visibility.shown );
  CPPUNIT_ASSERT ( child3.getFlags().visibility.shown );
  CPPUNIT_ASSERT ( child4.getFlags().visibility.shown );
  CPPUNIT_ASSERT ( ! child1.isWindowWidget() );
  CPPUNIT_ASSERT ( ! child2.isWindowWidget() );
  CPPUNIT_ASSERT ( ! child3.isWindowWidget() );
  CPPUNIT_ASSERT ( ! child4.isWindowWidget() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( ! parent.focusNextChild() );
  child1.setFocus();  // Set the initial focus on the first child
  CPPUNIT_ASSERT ( child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.setFlags().type.dialog_widget = true;
  CPPUNIT_ASSERT ( ! child1.focusNextChild() );
  CPPUNIT_ASSERT ( child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.setFlags().type.dialog_widget = false;
  CPPUNIT_ASSERT ( child1.focusNextChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( child2.focusNextChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( child3.focusNextChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( child4.hasFocus() );
  CPPUNIT_ASSERT ( child4.focusNextChild() );
  CPPUNIT_ASSERT ( child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child2.p_setWidgetProperty(false);
  CPPUNIT_ASSERT ( child1.focusNextChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( child3.focusNextChild() );
  CPPUNIT_ASSERT ( child4.focusNextChild() );

  child2.p_setWidgetProperty(true);
  child2.setFlags().feature.active = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( child1.focusNextChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.setFocus();  // Set the initial focus on the first child
  child2.setFlags().feature.active = true;
  child2.setFlags().focus.focusable = false;
  child3.setFlags().visibility.shown = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( child1.focusNextChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( child4.hasFocus() );

  finalcut::FApplication::start();
  child1.setFocus();  // Set the initial focus on the first child
  child4.setFlags().visibility.shown = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 1 );
  resetLastEvent();
  CPPUNIT_ASSERT ( getLastEvent() == finalcut::Event::None );
  CPPUNIT_ASSERT ( child1.focusNextChild() );
  CPPUNIT_ASSERT ( getLastEvent() == finalcut::Event::FailAtChildFocus );
  CPPUNIT_ASSERT ( child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.unsetFocus();  // Reset the focus of first child
  child1.setFlags().visibility.shown = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! child1.focusNextChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.setFlags().visibility.shown = true;
  child1.setFocus();  // Set the initial focus on the first child
  child2.setFlags().focus.focusable = true;
  child3.setFlags().visibility.shown = true;
  child2.setFlags().type.window_widget = true;
  child4.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( child2.isWindowWidget() );
  CPPUNIT_ASSERT ( child1.focusNextChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  // Test focusing the first child widget
  child3.unsetFocus();  // Reset the focus of first child
  child2.setFlags().type.window_widget = false;
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 4 );
  CPPUNIT_ASSERT ( child1.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! child1.focusFirstChild() );

  child1.setFlags().feature.active = false;
  child2.setFlags().feature.active = false;
  child3.setFlags().feature.active = false;
  child4.setFlags().feature.active = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! parent.focusFirstChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.setFlags().feature.active = true;
  child1.setFlags().focus.focusable = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! parent.focusFirstChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child2.setFlags().feature.active = true;
  child2.setFlags().type.menu_widget = true;
  CPPUNIT_ASSERT ( child2.isMenuWidget() );
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 1 );
  // A menu widget cannot receive focus as first element !
  CPPUNIT_ASSERT ( ! parent.focusFirstChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child4.setFlags().feature.active = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( parent.focusFirstChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( child4.hasFocus() );

  child3.setFlags().feature.active = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( parent.focusFirstChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child2.setFlags().type.menu_widget = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( parent.focusFirstChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.setFlags().focus.focusable = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 4 );
  CPPUNIT_ASSERT ( parent.focusFirstChild() );
  CPPUNIT_ASSERT ( child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.p_setWidgetProperty(false);
  CPPUNIT_ASSERT ( parent.focusFirstChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  child1.p_setWidgetProperty(true);

  // Test focusing the previous widget
  child2.unsetFocus();
  CPPUNIT_ASSERT ( ! root_wdgt.focusPrevChild() );  // No parent test
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 4 );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( ! parent.focusPrevChild() );
  child4.setFocus();  // Set the initial focus on the last child
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( child4.hasFocus() );

  child4.setFlags().type.dialog_widget = true;
  CPPUNIT_ASSERT ( ! child4.focusPrevChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( child4.hasFocus() );

  child4.setFlags().type.dialog_widget = false;
  CPPUNIT_ASSERT ( child4.focusPrevChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( child3.focusPrevChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( child2.focusPrevChild() );
  CPPUNIT_ASSERT ( child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( child1.focusPrevChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( child4.hasFocus() );

  child3.p_setWidgetProperty(false);
  CPPUNIT_ASSERT ( child4.focusPrevChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child3.p_setWidgetProperty(true);
  child3.setFlags().feature.active = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( child4.focusPrevChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child4.setFocus();  // Set the initial focus on the last child
  child3.setFlags().feature.active = true;
  child3.setFlags().focus.focusable = false;
  child2.setFlags().visibility.shown = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( child4.focusPrevChild() );
  CPPUNIT_ASSERT ( child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child4.setFocus();  // Set the initial focus on the last child
  child1.setFlags().visibility.shown = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 1 );
  resetLastEvent();
  CPPUNIT_ASSERT ( getLastEvent() == finalcut::Event::None );
  CPPUNIT_ASSERT ( child4.focusPrevChild() );
  CPPUNIT_ASSERT ( getLastEvent() == finalcut::Event::FailAtChildFocus );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( child4.hasFocus() );

  child4.unsetFocus();  // Reset the focus of first child
  child4.setFlags().visibility.shown = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! child4.focusPrevChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child4.setFlags().visibility.shown = true;
  child4.setFocus();  // Set the initial focus on the first child
  child3.setFlags().focus.focusable = true;
  child2.setFlags().visibility.shown = true;
  child3.setFlags().type.window_widget = true;
  child1.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( child3.isWindowWidget() );
  CPPUNIT_ASSERT ( child4.focusPrevChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  // Test focusing the last child widget
  child2.unsetFocus();  // Reset the focus of first child
  child3.setFlags().type.window_widget = false;
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 4 );
  CPPUNIT_ASSERT ( child4.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! child4.focusLastChild() );

  child1.setFlags().feature.active = false;
  child2.setFlags().feature.active = false;
  child3.setFlags().feature.active = false;
  child4.setFlags().feature.active = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! parent.focusLastChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child4.setFlags().feature.active = true;
  child4.setFlags().focus.focusable = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 0 );
  CPPUNIT_ASSERT ( ! parent.focusLastChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child3.setFlags().feature.active = true;
  child3.setFlags().type.menu_widget = true;
  CPPUNIT_ASSERT ( child3.isMenuWidget() );
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 1 );
  // A menu widget cannot receive focus as first element !
  CPPUNIT_ASSERT ( ! parent.focusLastChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child1.setFlags().feature.active = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 2 );
  CPPUNIT_ASSERT ( parent.focusLastChild() );
  CPPUNIT_ASSERT ( child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child2.setFlags().feature.active = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( parent.focusLastChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child3.setFlags().type.menu_widget = false;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 3 );
  CPPUNIT_ASSERT ( parent.focusLastChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );

  child4.setFlags().focus.focusable = true;
  CPPUNIT_ASSERT ( parent.numOfFocusableChildren() == 4 );
  CPPUNIT_ASSERT ( parent.focusLastChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( ! child3.hasFocus() );
  CPPUNIT_ASSERT ( child4.hasFocus() );

  child4.p_setWidgetProperty(false);
  CPPUNIT_ASSERT ( parent.focusLastChild() );
  CPPUNIT_ASSERT ( ! child1.hasFocus() );
  CPPUNIT_ASSERT ( ! child2.hasFocus() );
  CPPUNIT_ASSERT ( child3.hasFocus() );
  CPPUNIT_ASSERT ( ! child4.hasFocus() );
  child4.p_setWidgetProperty(true);
}

//----------------------------------------------------------------------
void FWidgetTest::closeWidgetTest()
{
  finalcut::FWidget root_wdgt{};  // Root widget
  finalcut::FWidget main_wdgt{&root_wdgt};  // Child / main widget
  finalcut::FWidget::setMainWidget(&main_wdgt);
  main_wdgt.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( ! main_wdgt.getFlags().visibility.hidden );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().visibility.shown );

  class TestWidget : public finalcut::FWidget
  {
    public:
      explicit TestWidget (finalcut::FWidget* parent = nullptr)
        : finalcut::FWidget{parent}
      { }

      TestWidget (const TestWidget&) = delete;

      TestWidget (TestWidget&&) noexcept = delete;

      ~TestWidget() override
      { }

      void onClose (finalcut::FCloseEvent* ev) override
      {
        if ( confirmed )
          ev->accept();
        else
          ev->ignore();
      }

      void setConfirmed (bool state = true)
      {
        confirmed = state;
      }

      auto p_getWidgetCloseList() -> finalcut::FWidget::FWidgetList*&
      {
        return finalcut::FWidget::getWidgetCloseList();
      }

    private:
      bool confirmed{false};
  };

  TestWidget wdgt{&main_wdgt};  // Subchild
  wdgt.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( ! wdgt.close() );  // Close without confirmation
  CPPUNIT_ASSERT ( ! wdgt.getFlags().visibility.hidden );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.shown );

  wdgt.setConfirmed();
  finalcut::FApplication::start();
  CPPUNIT_ASSERT ( wdgt.p_getWidgetCloseList()->size() == 0 );
  CPPUNIT_ASSERT ( wdgt.close() );
  CPPUNIT_ASSERT ( wdgt.p_getWidgetCloseList()->size() == 1 );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.hidden );
  CPPUNIT_ASSERT ( ! wdgt.getFlags().visibility.shown );

  // wdgt is already in the widget close list
  wdgt.setFlags().visibility.hidden = false;
  wdgt.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( wdgt.p_getWidgetCloseList()->size() == 1 );
  CPPUNIT_ASSERT ( wdgt.close() );
  CPPUNIT_ASSERT ( wdgt.p_getWidgetCloseList()->size() == 1 );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.hidden );
  CPPUNIT_ASSERT ( ! wdgt.getFlags().visibility.shown );
  wdgt.p_getWidgetCloseList()->clear();

  // wdgt is modal
  wdgt.setFlags().visibility.modal = true;
  wdgt.setFlags().visibility.hidden = false;
  wdgt.setFlags().visibility.shown = true;
  CPPUNIT_ASSERT ( wdgt.p_getWidgetCloseList()->size() == 0 );
  CPPUNIT_ASSERT ( wdgt.close() );
  CPPUNIT_ASSERT ( wdgt.p_getWidgetCloseList()->size() == 0 );
  CPPUNIT_ASSERT ( wdgt.getFlags().visibility.hidden );
  CPPUNIT_ASSERT ( ! wdgt.getFlags().visibility.shown );

  // Close the main widget
  CPPUNIT_ASSERT ( wdgt.p_getWidgetCloseList()->size() == 0 );
  CPPUNIT_ASSERT ( main_wdgt.close() );
  CPPUNIT_ASSERT ( wdgt.p_getWidgetCloseList()->size() == 0 );
  CPPUNIT_ASSERT ( ! main_wdgt.getFlags().visibility.hidden );
  CPPUNIT_ASSERT ( main_wdgt.getFlags().visibility.shown );
}

//----------------------------------------------------------------------
void FWidgetTest::adjustSizeTest()
{
  std::unique_ptr<finalcut::FSystem> fsys = std::make_unique<FSystemTest>();
  finalcut::FTerm::setFSystem(fsys);

  class TestWidget : public finalcut::FWidget
  {
    public:
      explicit TestWidget (finalcut::FWidget* parent = nullptr)
        : finalcut::FWidget{parent}
      { }

      TestWidget (const TestWidget&) = delete;

      TestWidget (TestWidget&&) noexcept = delete;

      ~TestWidget() override
      { }

      void p_adjustSize()
      {
        return finalcut::FWidget::adjustSize();
      }
  };

  TestWidget root_wdgt{};  // Root widget
  TestWidget child_wdgt{&root_wdgt};  // Child widget
  root_wdgt.setGeometry({3, 3}, {10, 5}, false);
  child_wdgt.setGeometry({-2, -2}, {20, 50}, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(20, 50)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(20, 50) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(10, 5) );

  child_wdgt.move({-3, -4});
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(-2, -3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(10, 5) );

  root_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(10, 5) );

  child_wdgt.move({3, 4});
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(4, 5), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(10, 5) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(10, 5) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(10, 5) );

  child_wdgt.setTopPadding(1, false);
  child_wdgt.setRightPadding(1, false);
  child_wdgt.setBottomPadding(1, false);
  child_wdgt.setLeftPadding(1, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(10, 5) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(8, 3) );

  child_wdgt.setTopPadding(2, false);
  child_wdgt.setRightPadding(2, false);
  child_wdgt.setBottomPadding(2, false);
  child_wdgt.setLeftPadding(2, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(8, 3) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(6, 1) );

  child_wdgt.setTopPadding(2, false);
  child_wdgt.setRightPadding(4, false);
  child_wdgt.setBottomPadding(2, false);
  child_wdgt.setLeftPadding(4, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(6, 1) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(2, 1) );

  child_wdgt.setTopPadding(2, false);
  child_wdgt.setRightPadding(5, false);
  child_wdgt.setBottomPadding(2, false);
  child_wdgt.setLeftPadding(4, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(2, 1) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(1, 1) );

  child_wdgt.setTopPadding(3, false);
  child_wdgt.setRightPadding(5, false);
  child_wdgt.setBottomPadding(2, false);
  child_wdgt.setLeftPadding(4, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(1, 1) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(1, 0) );

  child_wdgt.setTopPadding(3, false);
  child_wdgt.setRightPadding(5, false);
  child_wdgt.setBottomPadding(2, false);
  child_wdgt.setLeftPadding(5, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(1, 0) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(0, 0) );

  child_wdgt.setTopPadding(7, false);
  child_wdgt.setRightPadding(9, false);
  child_wdgt.setBottomPadding(2, false);
  child_wdgt.setLeftPadding(5, false);
  child_wdgt.move({5, 5});
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(6, 6), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(0, 0) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(0, 0) );

  child_wdgt.setPos({6, 6}, false);  // The y value is large
  child_wdgt.setSize({0, 0}, false);  // A widget width or a height can never be < 1 character
  child_wdgt.setTopPadding(0, false);
  child_wdgt.setRightPadding(0, false);
  child_wdgt.setBottomPadding(0, false);
  child_wdgt.setLeftPadding(0, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(6, 6), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(0, 0) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(6, 5), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(1, 1) );

  // setMinimumSize before setSize
  child_wdgt.setMinimumWidth(2);
  child_wdgt.setMinimumHeight(2);
  child_wdgt.setSize({0, 0}, false);  // Widget width or height can not be smaller than the minimum
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(6, 6), finalcut::FSize(2, 2)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(1, 1) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(6, 4), finalcut::FSize(2, 2)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(2, 2) );

  // Reset minimum size
  child_wdgt.setMinimumSize({0, 0} );
  // setSize before setMinimumSize
  child_wdgt.setSize({0, 0}, false);  // Widget width or height can not be smaller than the minimum
  child_wdgt.setMinimumSize({3, 3} );
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(6, 6), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(2, 2) );

  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(3, 3), finalcut::FSize(10, 5)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(6, 5), finalcut::FSize(3, 3)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(3, 3) );

  // Set the minimum width and height to 0
  child_wdgt.setMinimumSize({0, 0} );
  child_wdgt.setMaximumSize({INT_MAX, INT_MAX} );
  child_wdgt.setBottomPadding(1, false);
  child_wdgt.setRightPadding(1, false);
  root_wdgt.setPos({1, 1}, false);
  root_wdgt.setSize({1, 1}, false);
  child_wdgt.setPos({-2, -2}, false);
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(3, 3)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(10, 5) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(3, 3) );

  root_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( child_wdgt.getGeometry() == finalcut::FRect(finalcut::FPoint(1, 1), finalcut::FSize(1, 1)) );
  CPPUNIT_ASSERT ( root_wdgt.getClientSize() == finalcut::FSize(1, 1) );
  CPPUNIT_ASSERT ( child_wdgt.getClientSize() == finalcut::FSize(0, 0) );

  root_wdgt.setPos({9, 9}, false);
  root_wdgt.setSize({20, 20}, false);
  child_wdgt.setPos({2, 2}, false);
  child_wdgt.setSize({10, 10}, false);
  CPPUNIT_ASSERT ( root_wdgt.getTermGeometry() == finalcut::FRect(finalcut::FPoint(9, 9), finalcut::FSize(20, 20)) );
  CPPUNIT_ASSERT ( child_wdgt.getTermGeometry() == finalcut::FRect(finalcut::FPoint(2, 2), finalcut::FSize(10, 10)) );
  child_wdgt.setFlags().type.window_widget = true;
  child_wdgt.ignorePadding(true);
  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getTermGeometry() == finalcut::FRect(finalcut::FPoint(9, 9), finalcut::FSize(20, 20)) );
  CPPUNIT_ASSERT ( child_wdgt.getTermGeometry() == finalcut::FRect(finalcut::FPoint(2, 2), finalcut::FSize(10, 10)) );

  child_wdgt.setFlags().type.window_widget = false;
  child_wdgt.p_adjustSize();
  CPPUNIT_ASSERT ( root_wdgt.getTermGeometry() == finalcut::FRect(finalcut::FPoint(9, 9), finalcut::FSize(20, 20)) );
  CPPUNIT_ASSERT ( child_wdgt.getTermGeometry() == finalcut::FRect(finalcut::FPoint(10, 10), finalcut::FSize(10, 10)) );
}

//----------------------------------------------------------------------
void FWidgetTest::callbackTest()
{
  int value{0};
  finalcut::FWidget wdgt{};
  wdgt.addCallback( "signal", [&value] () { value++; } );
  CPPUNIT_ASSERT ( value == 0 );
  wdgt.emitCallback("nonsense");
  CPPUNIT_ASSERT ( value == 0 );
  wdgt.emitCallback("signal");
  CPPUNIT_ASSERT ( value == 1 );
  value *= 300;
  wdgt.emitCallback("signal");
  CPPUNIT_ASSERT ( value == 301 );
  wdgt.delCallback("nonsense");
  wdgt.emitCallback("signal");
  CPPUNIT_ASSERT ( value == 302 );
  wdgt.delCallback("signal");
  wdgt.emitCallback("signal");
  CPPUNIT_ASSERT ( value == 302 );
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FWidgetTest);

// The general unit test main part
#include <main-test.inc>
