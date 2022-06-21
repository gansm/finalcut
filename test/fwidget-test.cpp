/***********************************************************************
* fwidget-test.cpp - FWidget unit tests                                *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022 Markus Gans                                           *
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
    void adjustSizeTest();

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
    CPPUNIT_TEST (adjustSizeTest);

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
    uChar inPortByte (uShort) override
    {
      return 0;
    }

    void outPortByte (uChar, uShort) override
    { }

    int isTTY (int) const override
    {
      return 1;
    }

    int ioctl (int fd, uLong request, ...) override
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
          win_size->ws_col = 80;
          win_size->ws_row = 24;
          ret_val = 0;
          break;
      }

      va_end (args);
      return ret_val;
    }

    int open (const char*, int, ...) override
    {
      return 0;
    }

    int close (int) override
    {
      return 0;
    }

    FILE* fopen (const char*, const char*) override
    {
      return nullptr;
    }

    int fputs (const char* str, FILE* stream) override
    {
      return std::fputs(str, stream);
    }

    int fclose (FILE*) override
    {
      return 0;
    }

    int putchar (int c) override
    {
#if defined(__sun) && defined(__SVR4)
      return std::putchar(char(c));
#else
      return std::putchar(c);
#endif
    }

    uid_t getuid() override
    {
      return 0;
    }

    uid_t geteuid() override
    {
      return 0;
    }

    int getpwuid_r ( uid_t, struct passwd*, char*
                   , size_t, struct passwd** ) override
    {
      return 0;
    }

    char* realpath (const char*, char*) override
    {
      return const_cast<char*>("");
    }
};

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
  CPPUNIT_ASSERT ( root_wdgt.getMoveSizeWidget() == nullptr );
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
  CPPUNIT_ASSERT ( wdgt.getMoveSizeWidget() == nullptr );
  wdgt.setMoveSizeWidget(&wdgt);
  CPPUNIT_ASSERT ( wdgt.getMoveSizeWidget() == &wdgt );

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
  auto& default_side = wdgt.doubleFlatLine_ref(static_cast<finalcut::Side>(99));  // Side::Top fallback
  CPPUNIT_ASSERT ( top.size() == 1 );
  CPPUNIT_ASSERT ( right.size() == 1 );
  CPPUNIT_ASSERT ( bottom.size() == 1 );
  CPPUNIT_ASSERT ( left.size() == 1 );
  CPPUNIT_ASSERT ( default_side.size() == 1 );
  CPPUNIT_ASSERT ( top[0] == false );
  CPPUNIT_ASSERT ( right[0] == false );
  CPPUNIT_ASSERT ( bottom[0] == false );
  CPPUNIT_ASSERT ( left[0] == false );
  CPPUNIT_ASSERT ( default_side[0] == false );
  wdgt.setDoubleFlatLine(finalcut::Side::Top);
  CPPUNIT_ASSERT ( top[0] == true );
  wdgt.setDoubleFlatLine(finalcut::Side::Right);
  CPPUNIT_ASSERT ( right[0] == true );
  wdgt.setDoubleFlatLine(finalcut::Side::Bottom);
  CPPUNIT_ASSERT ( bottom[0] == true );
  wdgt.setDoubleFlatLine(finalcut::Side::Left);
  CPPUNIT_ASSERT ( left[0] == true );
  CPPUNIT_ASSERT ( default_side[0] == true );

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

  // Widget flags
  CPPUNIT_ASSERT ( wdgt.getFlags().shadow == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().trans_shadow == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().active == true );
  CPPUNIT_ASSERT ( wdgt.getFlags().visible == true );
  CPPUNIT_ASSERT ( wdgt.getFlags().shown == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().hidden == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().focus == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().focusable == true );
  CPPUNIT_ASSERT ( wdgt.getFlags().scrollable == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().resizeable == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().minimizable == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().modal == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().visible_cursor == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().window_widget == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().dialog_widget == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().menu_widget == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().always_on_top == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().flat == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().no_border == false );
  CPPUNIT_ASSERT ( wdgt.getFlags().no_underline == false );

  wdgt.setVisible(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().visible == false );
  wdgt.setVisible(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().visible == true );
  wdgt.unsetVisible();
  CPPUNIT_ASSERT ( wdgt.getFlags().visible == false );
  wdgt.setVisible();
  CPPUNIT_ASSERT ( wdgt.getFlags().visible == true );

  wdgt.setEnable(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().active == false );
  wdgt.setEnable(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().active == true );
  wdgt.unsetEnable();
  CPPUNIT_ASSERT ( wdgt.getFlags().active == false );
  wdgt.setEnable();
  CPPUNIT_ASSERT ( wdgt.getFlags().active == true );
  wdgt.setDisable();
  CPPUNIT_ASSERT ( wdgt.getFlags().active == false );

  wdgt.setVisibleCursor(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().visible_cursor == false );
  wdgt.setVisibleCursor(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().visible_cursor == true );
  wdgt.unsetVisibleCursor();
  CPPUNIT_ASSERT ( wdgt.getFlags().visible_cursor == false );
  wdgt.setVisibleCursor();
  CPPUNIT_ASSERT ( wdgt.getFlags().visible_cursor == true );

  wdgt.setFocus(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().focus == false );
  wdgt.setFocus(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().focus == false );
  wdgt.setEnable();
  wdgt.setFocus(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().focus == true );
  wdgt.unsetFocus();
  CPPUNIT_ASSERT ( wdgt.getFlags().focus == false );
  wdgt.setFocus();
  CPPUNIT_ASSERT ( wdgt.getFlags().focus == true );

  wdgt.setFocusable(false);
  CPPUNIT_ASSERT ( wdgt.getFlags().focusable == false );
  wdgt.setFocusable(true);
  CPPUNIT_ASSERT ( wdgt.getFlags().focusable == true );
  wdgt.unsetFocusable();
  CPPUNIT_ASSERT ( wdgt.getFlags().focusable == false );
  wdgt.setFocusable();
  CPPUNIT_ASSERT ( wdgt.getFlags().focusable == true );

  wdgt.ignorePadding(false);
  CPPUNIT_ASSERT ( ! wdgt.isPaddingIgnored() );
  wdgt.ignorePadding(true);
  CPPUNIT_ASSERT ( wdgt.isPaddingIgnored() );
  wdgt.acceptPadding();
  CPPUNIT_ASSERT ( ! wdgt.isPaddingIgnored() );
  wdgt.ignorePadding();
  CPPUNIT_ASSERT ( wdgt.isPaddingIgnored() );
}

//----------------------------------------------------------------------
void FWidgetTest::colorThemeTest()
{
  {
    finalcut::FWidget root_wdgt{};  // Root Widget

    // Get color theme
    const auto& color_theme = root_wdgt.getColorTheme();
    CPPUNIT_ASSERT ( finalcut::FVTerm::getFOutput()->getMaxColor() == 1 );
    // Less than 16 colors -> default8ColorTheme
    CPPUNIT_ASSERT ( color_theme->term_fg == finalcut::FColor::Black );
    CPPUNIT_ASSERT ( color_theme->term_bg == finalcut::FColor::Blue );
  }

  {
    finalcut::FTermcap::max_color = 8;
    finalcut::FWidget root_wdgt{};  // Root Widget

    // Get color theme
    const auto& color_theme = root_wdgt.getColorTheme();
    CPPUNIT_ASSERT ( finalcut::FVTerm::getFOutput()->getMaxColor() == 8 );
    // Less than 16 colors -> default8ColorTheme
    CPPUNIT_ASSERT ( color_theme->term_fg == finalcut::FColor::Black );
    CPPUNIT_ASSERT ( color_theme->term_bg == finalcut::FColor::Blue );
  }

  {
    finalcut::FTermcap::max_color = 16;
    finalcut::FWidget root_wdgt{};  // Root Widget

    // Get color theme
    const auto& color_theme = root_wdgt.getColorTheme();
    CPPUNIT_ASSERT ( finalcut::FVTerm::getFOutput()->getMaxColor() == 16 );
    // At least 16 colors or more -> default16ColorTheme
    CPPUNIT_ASSERT ( color_theme->term_fg == finalcut::FColor::Black );
    CPPUNIT_ASSERT ( color_theme->term_bg == finalcut::FColor::LightBlue );
  }
}

//----------------------------------------------------------------------
void FWidgetTest::resetColorsTest()
{
  finalcut::FWidget root_wdgt{};  // Root Widget

  class TestWidget : public finalcut::FWidget
  {
    public:
      TestWidget (finalcut::FWidget* parent = nullptr)
        : finalcut::FWidget{parent}
      { }

      TestWidget (const TestWidget&) = delete;

      TestWidget (TestWidget&&) noexcept = delete;

      ~TestWidget() override
      { }

      void resetColors()
      {
        const auto& wc = getColorTheme();
        setForegroundColor (wc->dialog_fg);
        setBackgroundColor (wc->dialog_bg);
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
  finalcut::FWidget root_wdgt{};  // Root Widget
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
  wdgt.setAcceleratorList() = new_accelerator_list;
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 1 );
  wdgt.delAccelerator();
  CPPUNIT_ASSERT ( wdgt.getAcceleratorList().size() == 1 );
  wdgt.setFlags().window_widget = true;
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

  finalcut::FWidget root_wdgt{};  // Root Widget
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

  wdgt.setFlags().window_widget = true;
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

  wdgt.setFlags().window_widget = false;
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

  wdgt.setFlags().window_widget = true;
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
  wdgt.setFlags().hidden = false;
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(10, 3)) == false );
  wdgt.setFlags().focus = true;
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(11, 3)) == false );
  wdgt.setFlags().window_widget = false;
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(12, 3)) == false );
  root_wdgt.setFlags().window_widget = true;
  CPPUNIT_ASSERT ( wdgt.setCursorPos(finalcut::FPoint(13, 3)) == true );
  CPPUNIT_ASSERT ( wdgt.getCursorPos() == finalcut::FPoint(13, 3) );
}

//----------------------------------------------------------------------
void FWidgetTest::adjustSizeTest()
{

}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FWidgetTest);

// The general unit test main part
#include <main-test.inc>
