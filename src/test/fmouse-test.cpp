/***********************************************************************
* fmouse-test.cpp - FMouse unit tests                                  *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2018 Markus Gans                                           *
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

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>


//----------------------------------------------------------------------
// class FMouse_protected
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouse_protected : public FMouse
{
  public:
    virtual bool hasData()
    { return true; }

    virtual void setRawData (char[], int)
    { }

    virtual void processEvent (struct timeval*)
    { }

    short getMaxWidth()
    {
      return max_width;
    }

    short getMaxHeight()
    {
      return max_height;
    }

    FPoint& getNewMousePosition()
    {
      return new_mouse_position;
    }

    long getDblclickInterval()
    {
      return dblclick_interval;
    }

    bool isDblclickTimeout (timeval* t)
    {
      return FMouse::isDblclickTimeout(t);
    }
};
#pragma pack(pop)


//----------------------------------------------------------------------
// class FMouseTest
//----------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

class FMouseTest : public CPPUNIT_NS::TestFixture
{
  public:
    FMouseTest()
    { }

  protected:
    void classNameTest();
    void noArgumentTest();
    void doubleClickTest();
    void workspaceSizeTest();
#ifdef F_HAVE_LIBGPM
    void gpmMouseTest();
#endif
    void x11MouseTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FMouseTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (doubleClickTest);
    CPPUNIT_TEST (workspaceSizeTest);
#ifdef F_HAVE_LIBGPM
    CPPUNIT_TEST (gpmMouseTest);
#endif
    CPPUNIT_TEST (x11MouseTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};
#pragma pack(pop)

//----------------------------------------------------------------------
void FMouseTest::classNameTest()
{
  FMouse_protected m;
  const char* const classname1 = m.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname1, "FMouse") == 0 );

#ifdef F_HAVE_LIBGPM
  FMouseGPM gpm_mouse;
  const char* const classname2 = gpm_mouse.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname2, "FMouseGPM") == 0 );
#endif

  FMouseX11 x11_mouse;
  const char* const classname3 = x11_mouse.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname3, "FMouseX11") == 0 );

  FMouseSGR sgr_mouse;
  const char* const classname4 = sgr_mouse.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname4, "FMouseSGR") == 0 );

  FMouseUrxvt urxvt_mouse;
  const char* const classname5 = urxvt_mouse.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname5, "FMouseUrxvt") == 0 );

  FMouseControl mouse_control;
  const char* const classname6 = mouse_control.getClassName();
  CPPUNIT_ASSERT ( std::strcmp(classname6, "FMouseControl") == 0 );
}

//----------------------------------------------------------------------
void FMouseTest::noArgumentTest()
{
  FMouse_protected mouse;
  CPPUNIT_ASSERT ( mouse.getPos() == FPoint(0, 0) );
  CPPUNIT_ASSERT ( mouse.getNewMousePosition() == FPoint(0, 0) );
  CPPUNIT_ASSERT ( ! mouse.hasEvent() );
  CPPUNIT_ASSERT ( ! mouse.isLeftButtonPressed() );
  CPPUNIT_ASSERT ( ! mouse.isLeftButtonReleased() );
  CPPUNIT_ASSERT ( ! mouse.isLeftButtonDoubleClick() );
  CPPUNIT_ASSERT ( ! mouse.isRightButtonPressed() );
  CPPUNIT_ASSERT ( ! mouse.isRightButtonReleased() );
  CPPUNIT_ASSERT ( ! mouse.isMiddleButtonPressed() );
  CPPUNIT_ASSERT ( ! mouse.isMiddleButtonReleased() );
  CPPUNIT_ASSERT ( ! mouse.isShiftKeyPressed() );
  CPPUNIT_ASSERT ( ! mouse.isControlKeyPressed() );
  CPPUNIT_ASSERT ( ! mouse.isMetaKeyPressed() );
  CPPUNIT_ASSERT ( ! mouse.isWheelUp() );
  CPPUNIT_ASSERT ( ! mouse.isWheelDown() );
  CPPUNIT_ASSERT ( ! mouse.isMoved() );
  CPPUNIT_ASSERT ( ! mouse.isInputDataPending() );

#ifdef F_HAVE_LIBGPM
  FMouseGPM gpm_mouse;
  CPPUNIT_ASSERT ( ! gpm_mouse.hasData() );
#endif

  FMouseX11 x11_mouse;
  CPPUNIT_ASSERT ( ! x11_mouse.hasData() );

  FMouseSGR sgr_mouse;
  CPPUNIT_ASSERT ( ! sgr_mouse.hasData() );

  FMouseUrxvt urxvt_mouse;
  CPPUNIT_ASSERT ( ! urxvt_mouse.hasData() );

  FMouseControl mouse_control;
  CPPUNIT_ASSERT ( ! mouse_control.hasData() );
}

//----------------------------------------------------------------------
void FMouseTest::doubleClickTest()
{
  FMouse_protected mouse;
  CPPUNIT_ASSERT ( mouse.getDblclickInterval() == 500000 );  // 500 ms
  timeval tv = { 0, 0 };
  CPPUNIT_ASSERT ( mouse.isDblclickTimeout(&tv) );

  FObject::getCurrentTime(&tv);
  CPPUNIT_ASSERT ( ! mouse.isDblclickTimeout(&tv) );

  tv.tv_sec--;  // Minus one second
  CPPUNIT_ASSERT ( mouse.isDblclickTimeout(&tv) );

  mouse.setDblclickInterval(1000000);
  FObject::getCurrentTime(&tv);
  CPPUNIT_ASSERT ( ! mouse.isDblclickTimeout(&tv) );

  timeval tv_delta = { 0, 500000 };
  tv = tv - tv_delta;
  CPPUNIT_ASSERT ( ! mouse.isDblclickTimeout(&tv) );
  tv = tv - tv_delta;
  CPPUNIT_ASSERT ( mouse.isDblclickTimeout(&tv) );
}

//----------------------------------------------------------------------
void FMouseTest::workspaceSizeTest()
{
  FMouse_protected mouse;
  CPPUNIT_ASSERT ( mouse.getMaxWidth() == 80 );
  CPPUNIT_ASSERT ( mouse.getMaxHeight() == 25 );

  mouse.setMaxWidth(92);
  mouse.setMaxHeight(30);
  CPPUNIT_ASSERT ( mouse.getMaxWidth() == 92 );
  CPPUNIT_ASSERT ( mouse.getMaxHeight() == 30 );
}

#ifdef F_HAVE_LIBGPM
//----------------------------------------------------------------------
void FMouseTest::gpmMouseTest()
{
  FMouseGPM gpm_mouse;
  CPPUNIT_ASSERT ( ! gpm_mouse.isGpmMouseEnabled() );

  gpm_mouse.setStdinNo(fileno(stdin));

  if ( gpm_mouse.enableGpmMouse() )
  {
    CPPUNIT_ASSERT ( gpm_mouse.isGpmMouseEnabled() );
    timeval tv;
    FObject::getCurrentTime(&tv);
    gpm_mouse.processEvent(&tv);
    CPPUNIT_ASSERT ( ! gpm_mouse.hasEvent() );
  }
  else
    CPPUNIT_ASSERT ( ! gpm_mouse.isGpmMouseEnabled() );

  gpm_mouse.disableGpmMouse();
  CPPUNIT_ASSERT ( ! gpm_mouse.isGpmMouseEnabled() );
}
#endif

//----------------------------------------------------------------------
void FMouseTest::x11MouseTest()
{
  FMouseX11 x11_mouse;
  CPPUNIT_ASSERT ( ! x11_mouse.hasData() );

  char x11_mouse_data[8] = { 0x1b, '[', 'M', 0x23, 0x50, 0x32, 0x40, 0x40 };
  x11_mouse.setRawData (x11_mouse_data, 8);
  CPPUNIT_ASSERT ( x11_mouse.hasData() );
  CPPUNIT_ASSERT ( x11_mouse.isInputDataPending() );

  timeval tv;
  FObject::getCurrentTime(&tv);
  x11_mouse.processEvent (&tv);

  CPPUNIT_ASSERT ( x11_mouse.getPos() == FPoint(48, 18) );
  CPPUNIT_ASSERT ( x11_mouse.hasEvent() );
  CPPUNIT_ASSERT ( ! x11_mouse.isLeftButtonPressed() );
  CPPUNIT_ASSERT ( ! x11_mouse.isLeftButtonReleased() );
  CPPUNIT_ASSERT ( ! x11_mouse.isLeftButtonDoubleClick() );
  CPPUNIT_ASSERT ( ! x11_mouse.isRightButtonPressed() );
  CPPUNIT_ASSERT ( ! x11_mouse.isRightButtonReleased() );
  CPPUNIT_ASSERT ( ! x11_mouse.isMiddleButtonPressed() );
  CPPUNIT_ASSERT ( ! x11_mouse.isMiddleButtonReleased() );
  CPPUNIT_ASSERT ( ! x11_mouse.isShiftKeyPressed() );
  CPPUNIT_ASSERT ( ! x11_mouse.isControlKeyPressed() );
  CPPUNIT_ASSERT ( ! x11_mouse.isMetaKeyPressed() );
  CPPUNIT_ASSERT ( ! x11_mouse.isWheelUp() );
  CPPUNIT_ASSERT ( ! x11_mouse.isWheelDown() );
  CPPUNIT_ASSERT ( ! x11_mouse.isMoved() );
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FMouseTest);

// The general unit test main part
#include <main-test.inc>
