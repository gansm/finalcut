/***********************************************************************
* fevent-test.cpp - FEvent unit tests                                  *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2024 Markus Gans                                      *
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

#include <utility>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

namespace finalcut
{

// Friend functions definition of FEvent
//----------------------------------------------------------------------
void setSend (FEvent& event, bool state)
{
  event.send = state;
}

//----------------------------------------------------------------------
void setQueued (FEvent& event, bool state)
{
  event.queued = state;
}

}  // namespace finalcut


//----------------------------------------------------------------------
// class FEventTest
//----------------------------------------------------------------------

class FEventTest : public CPPUNIT_NS::TestFixture
{
  public:
    FEventTest() = default;

  protected:
    void feventTest();
    void fkeyeventTest();
    void fmouseeventTest();
    void fwheeleventTest();
    void ffocuseventTest();
    void facceleventTest();
    void fresizeeventTest();
    void fshoweventTest();
    void fhideeventTest();
    void fcloseeventTest();
    void ftimereventTest();
    void fusereventTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FEventTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (feventTest);
    CPPUNIT_TEST (fkeyeventTest);
    CPPUNIT_TEST (fmouseeventTest);
    CPPUNIT_TEST (fwheeleventTest);
    CPPUNIT_TEST (ffocuseventTest);
    CPPUNIT_TEST (facceleventTest);
    CPPUNIT_TEST (fresizeeventTest);
    CPPUNIT_TEST (fshoweventTest);
    CPPUNIT_TEST (fhideeventTest);
    CPPUNIT_TEST (fcloseeventTest);
    CPPUNIT_TEST (ftimereventTest);
    CPPUNIT_TEST (fusereventTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
void FEventTest::feventTest()
{
  finalcut::FEvent event (finalcut::Event::None);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::None );
  CPPUNIT_ASSERT ( ! event.wasSent() );
  finalcut::setSend(event, true);
  CPPUNIT_ASSERT ( event.wasSent() );
  finalcut::setSend(event, false);
  CPPUNIT_ASSERT ( ! event.wasSent() );
  CPPUNIT_ASSERT ( ! event.isQueued() );
  finalcut::setQueued(event, true);
  CPPUNIT_ASSERT ( event.isQueued() );
  finalcut::setQueued(event, false);
  CPPUNIT_ASSERT ( ! event.isQueued() );

  finalcut::FEvent event1 (finalcut::Event::KeyPress);
  CPPUNIT_ASSERT ( event1.getType() == finalcut::Event::KeyPress );

  finalcut::FEvent event2 (finalcut::Event::KeyUp);
  CPPUNIT_ASSERT ( event2.getType() == finalcut::Event::KeyUp );

  finalcut::FEvent event3 (finalcut::Event::KeyDown);
  CPPUNIT_ASSERT ( event3.getType() == finalcut::Event::KeyDown );

  finalcut::FEvent event4 (finalcut::Event::MouseDown);
  CPPUNIT_ASSERT ( event4.getType() == finalcut::Event::MouseDown );

  finalcut::FEvent event5 (finalcut::Event::MouseUp);
  CPPUNIT_ASSERT ( event5.getType() == finalcut::Event::MouseUp );

  finalcut::FEvent event6 (finalcut::Event::MouseDoubleClick);
  CPPUNIT_ASSERT ( event6.getType() == finalcut::Event::MouseDoubleClick );

  finalcut::FEvent event7 (finalcut::Event::MouseWheel);
  CPPUNIT_ASSERT ( event7.getType() == finalcut::Event::MouseWheel );

  finalcut::FEvent event8 (finalcut::Event::MouseMove);
  CPPUNIT_ASSERT ( event8.getType() == finalcut::Event::MouseMove );

  finalcut::FEvent event9 (finalcut::Event::FocusIn);
  CPPUNIT_ASSERT ( event9 .getType() == finalcut::Event::FocusIn );

  finalcut::FEvent event10 (finalcut::Event::FocusOut);
  CPPUNIT_ASSERT ( event10.getType() == finalcut::Event::FocusOut );

  finalcut::FEvent event11 (finalcut::Event::ChildFocusIn);
  CPPUNIT_ASSERT ( event11.getType() == finalcut::Event::ChildFocusIn );

  finalcut::FEvent event12 (finalcut::Event::ChildFocusOut);
  CPPUNIT_ASSERT ( event12.getType() == finalcut::Event::ChildFocusOut );

  finalcut::FEvent event13 (finalcut::Event::WindowActive);
  CPPUNIT_ASSERT ( event13.getType() == finalcut::Event::WindowActive );

  finalcut::FEvent event14 (finalcut::Event::WindowInactive);
  CPPUNIT_ASSERT ( event14.getType() == finalcut::Event::WindowInactive );

  finalcut::FEvent event15 (finalcut::Event::WindowRaised);
  CPPUNIT_ASSERT ( event15.getType() == finalcut::Event::WindowRaised );

  finalcut::FEvent event16 (finalcut::Event::WindowLowered);
  CPPUNIT_ASSERT ( event16.getType() == finalcut::Event::WindowLowered );

  finalcut::FEvent event17 (finalcut::Event::Accelerator);
  CPPUNIT_ASSERT ( event17.getType() == finalcut::Event::Accelerator );

  finalcut::FEvent event18 (finalcut::Event::Resize);
  CPPUNIT_ASSERT ( event18 .getType() == finalcut::Event::Resize );

  finalcut::FEvent event19 (finalcut::Event::Show);
  CPPUNIT_ASSERT ( event19 .getType() == finalcut::Event::Show );

  finalcut::FEvent event20 (finalcut::Event::Hide);
  CPPUNIT_ASSERT ( event20.getType() == finalcut::Event::Hide );

  finalcut::FEvent event21 (finalcut::Event::Close);
  CPPUNIT_ASSERT ( event21.getType() == finalcut::Event::Close );

  finalcut::FEvent event22 (finalcut::Event::Timer);
  CPPUNIT_ASSERT ( event22.getType() == finalcut::Event::Timer );

  finalcut::FEvent event23 (finalcut::Event::User);
  CPPUNIT_ASSERT ( event23.getType() == finalcut::Event::User );
}

//----------------------------------------------------------------------
void FEventTest::fkeyeventTest()
{
  finalcut::FKeyEvent event (finalcut::Event::KeyPress, finalcut::FKey::Escape);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::KeyPress );
  CPPUNIT_ASSERT ( event.key() == finalcut::FKey::Escape );
  CPPUNIT_ASSERT ( ! event.isAccepted() );  // reject by default
  event.accept();
  CPPUNIT_ASSERT ( event.isAccepted() );
  event.ignore();
  CPPUNIT_ASSERT ( ! event.isAccepted() );

  finalcut::FKeyEvent event1 (finalcut::Event::KeyUp, finalcut::FKey::Ctrl_a);
  CPPUNIT_ASSERT ( event1.getType() == finalcut::Event::KeyUp );
  CPPUNIT_ASSERT ( event1.key() == finalcut::FKey::Ctrl_a );
  CPPUNIT_ASSERT ( ! event1.isAccepted() );

  finalcut::FKeyEvent event2 (finalcut::Event::KeyDown, finalcut::FKey::F1);
  CPPUNIT_ASSERT ( event2.getType() == finalcut::Event::KeyDown );
  CPPUNIT_ASSERT ( event2.key() == finalcut::FKey::F1 );
  CPPUNIT_ASSERT ( ! event2.isAccepted() );

  finalcut::FKeyEvent event3 (finalcut::Event::KeyPress, finalcut::FKey('~'));
  CPPUNIT_ASSERT ( event3.getType() == finalcut::Event::KeyPress );
  CPPUNIT_ASSERT ( event3.key() == finalcut::FKey::Tilde );
  CPPUNIT_ASSERT ( ! event3.isAccepted() );
}

//----------------------------------------------------------------------
void FEventTest::fmouseeventTest()
{
  finalcut::FMouseEvent event (finalcut::Event::MouseDown, {1, 1}, finalcut::MouseButton::None);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::MouseDown );
  CPPUNIT_ASSERT ( event.getButton() == finalcut::MouseButton::None );
  CPPUNIT_ASSERT ( event.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( event.getTermPos() != finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( event.getTermPos() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( event.getX() == 1 );
  CPPUNIT_ASSERT ( event.getY() == 1 );
  CPPUNIT_ASSERT ( event.getTermX() == 0 );
  CPPUNIT_ASSERT ( event.getTermY() == 0 );

  finalcut::FMouseEvent event1 (finalcut::Event::MouseUp, {5, 6}, {33, 12}, finalcut::MouseButton::Left);
  CPPUNIT_ASSERT ( event1.getType() == finalcut::Event::MouseUp );
  CPPUNIT_ASSERT ( event1.getButton() == finalcut::MouseButton::Left );
  CPPUNIT_ASSERT ( event1.getPos() != finalcut::FPoint(6, 5) );
  CPPUNIT_ASSERT ( event1.getPos() == finalcut::FPoint(5, 6) );
  CPPUNIT_ASSERT ( event1.getTermPos() == finalcut::FPoint(33, 12) );
  CPPUNIT_ASSERT ( event1.getX() == 5 );
  CPPUNIT_ASSERT ( event1.getY() == 6 );
  CPPUNIT_ASSERT ( event1.getTermX() == 33 );
  CPPUNIT_ASSERT ( event1.getTermY() == 12 );
  event1.setPos({9, 10});
  CPPUNIT_ASSERT ( event1.getPos() == finalcut::FPoint(9, 10) );
  CPPUNIT_ASSERT ( event1.getX() == 9 );
  CPPUNIT_ASSERT ( event1.getY() == 10 );
  CPPUNIT_ASSERT ( event1.getTermPos() == finalcut::FPoint(33, 12) );
  CPPUNIT_ASSERT ( event1.getTermX() == 33 );
  CPPUNIT_ASSERT ( event1.getTermY() == 12 );
  event1.setTermPos({26, 14});
  CPPUNIT_ASSERT ( event1.getPos() == finalcut::FPoint(9, 10) );
  CPPUNIT_ASSERT ( event1.getX() == 9 );
  CPPUNIT_ASSERT ( event1.getY() == 10 );
  CPPUNIT_ASSERT ( event1.getTermPos() == finalcut::FPoint(26, 14) );
  CPPUNIT_ASSERT ( event1.getTermX() == 26 );
  CPPUNIT_ASSERT ( event1.getTermY() == 14 );

  finalcut::FMouseEvent event2 (finalcut::Event::MouseDoubleClick, {1, 2}, {3, 4}, finalcut::MouseButton::Right);
  CPPUNIT_ASSERT ( event2.getType() == finalcut::Event::MouseDoubleClick );
  CPPUNIT_ASSERT ( event2.getButton() == finalcut::MouseButton::Right );
  CPPUNIT_ASSERT ( event2.getPos() == finalcut::FPoint(1, 2) );
  CPPUNIT_ASSERT ( event2.getTermPos() == finalcut::FPoint(3, 4) );
  CPPUNIT_ASSERT ( event2.getX() == 1 );
  CPPUNIT_ASSERT ( event2.getY() == 2 );
  CPPUNIT_ASSERT ( event2.getTermX() == 3 );
  CPPUNIT_ASSERT ( event2.getTermY() == 4 );

  finalcut::FMouseEvent event3 (finalcut::Event::MouseDown, {7, 8}, {10, 20}, finalcut::MouseButton::Middle);
  CPPUNIT_ASSERT ( event3.getType() == finalcut::Event::MouseDown );
  CPPUNIT_ASSERT ( event3.getButton() == finalcut::MouseButton::Middle );
  CPPUNIT_ASSERT ( event3.getPos() == finalcut::FPoint(7, 8) );
  CPPUNIT_ASSERT ( event3.getTermPos() == finalcut::FPoint(10, 20) );
  CPPUNIT_ASSERT ( event3.getX() == 7 );
  CPPUNIT_ASSERT ( event3.getY() == 8 );
  CPPUNIT_ASSERT ( event3.getTermX() == 10 );
  CPPUNIT_ASSERT ( event3.getTermY() == 20 );

  finalcut::FMouseEvent event4 (finalcut::Event::MouseDown, {2, 2}, {15, 11}, finalcut::MouseButton::Left | finalcut::MouseButton::Shift);
  CPPUNIT_ASSERT ( event4.getType() == finalcut::Event::MouseDown );
  CPPUNIT_ASSERT ( event4.getButton() != finalcut::MouseButton::Left );
  CPPUNIT_ASSERT ( event4.getButton() != finalcut::MouseButton::Shift );
  CPPUNIT_ASSERT ( uInt(event4.getButton()) == uInt(finalcut::MouseButton::Left)
                                             + uInt(finalcut::MouseButton::Shift) );

  auto mb = finalcut::MouseButton::Right;
  mb |= finalcut::MouseButton::Control;
  finalcut::FMouseEvent event5 (finalcut::Event::MouseDoubleClick, {4, 2}, {5, 9}, mb);
  CPPUNIT_ASSERT ( event5.getType() == finalcut::Event::MouseDoubleClick );
  CPPUNIT_ASSERT ( event5.getButton() != finalcut::MouseButton::Right );
  CPPUNIT_ASSERT ( event5.getButton() != finalcut::MouseButton::Control );
  CPPUNIT_ASSERT ( uInt(event5.getButton()) == uInt(finalcut::MouseButton::Right)
                                             + uInt(finalcut::MouseButton::Control) );

  mb = finalcut::MouseButton::Left | finalcut::MouseButton::Right;
  mb |= finalcut::MouseButton::Middle;
  mb |= finalcut::MouseButton::Shift;
  mb |= finalcut::MouseButton::Control;
  mb |= finalcut::MouseButton::Meta;
  finalcut::FMouseEvent event6 (finalcut::Event::MouseMove, {1, 1}, {1, 1}, mb);
  CPPUNIT_ASSERT ( event6.getType() == finalcut::Event::MouseMove );
  CPPUNIT_ASSERT ( event6.getButton() != finalcut::MouseButton::Left );
  CPPUNIT_ASSERT ( event6.getButton() != finalcut::MouseButton::Right );
  CPPUNIT_ASSERT ( event6.getButton() != finalcut::MouseButton::Middle );
  CPPUNIT_ASSERT ( event6.getButton() != finalcut::MouseButton::Shift );
  CPPUNIT_ASSERT ( event6.getButton() != finalcut::MouseButton::Control );
  CPPUNIT_ASSERT ( event6.getButton() != finalcut::MouseButton::Meta );
  CPPUNIT_ASSERT ( uInt(event6.getButton()) == uInt(finalcut::MouseButton::Left)
                                             + uInt(finalcut::MouseButton::Right)
                                             + uInt(finalcut::MouseButton::Middle)
                                             + uInt(finalcut::MouseButton::Shift)
                                             + uInt(finalcut::MouseButton::Meta)
                                             + uInt(finalcut::MouseButton::Control) );
}

//----------------------------------------------------------------------
void FEventTest::fwheeleventTest()
{
  finalcut::FWheelEvent event (finalcut::Event::MouseWheel, {1, 1}, finalcut::MouseWheel::None);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::MouseWheel );
  CPPUNIT_ASSERT ( event.getWheel() == finalcut::MouseWheel::None );
  CPPUNIT_ASSERT ( event.getPos() == finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( event.getTermPos() != finalcut::FPoint(1, 1) );
  CPPUNIT_ASSERT ( event.getTermPos() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( event.getX() == 1 );
  CPPUNIT_ASSERT ( event.getY() == 1 );
  CPPUNIT_ASSERT ( event.getTermX() == 0 );
  CPPUNIT_ASSERT ( event.getTermY() == 0 );

  finalcut::FWheelEvent event1 (finalcut::Event::MouseWheel, {1, 2}, {3, 4}, finalcut::MouseWheel::Up);
  CPPUNIT_ASSERT ( event1.getType() == finalcut::Event::MouseWheel );
  CPPUNIT_ASSERT ( event1.getWheel() == finalcut::MouseWheel::Up );
  CPPUNIT_ASSERT ( event1.getPos() == finalcut::FPoint(1, 2) );
  CPPUNIT_ASSERT ( event1.getTermPos() == finalcut::FPoint(3, 4) );
  CPPUNIT_ASSERT ( event1.getX() == 1 );
  CPPUNIT_ASSERT ( event1.getY() == 2 );
  CPPUNIT_ASSERT ( event1.getTermX() == 3 );
  CPPUNIT_ASSERT ( event1.getTermY() == 4 );

  finalcut::FWheelEvent event2 (finalcut::Event::MouseWheel, {3, 1}, {54, 18}, finalcut::MouseWheel::Down);
  CPPUNIT_ASSERT ( event2.getType() == finalcut::Event::MouseWheel );
  CPPUNIT_ASSERT ( event2.getWheel() == finalcut::MouseWheel::Down );
  CPPUNIT_ASSERT ( event2.getPos() == finalcut::FPoint(3, 1) );
  CPPUNIT_ASSERT ( event2.getTermPos() == finalcut::FPoint(54, 18) );
  CPPUNIT_ASSERT ( event2.getX() == 3 );
  CPPUNIT_ASSERT ( event2.getY() == 1 );
  CPPUNIT_ASSERT ( event2.getTermX() == 54 );
  CPPUNIT_ASSERT ( event2.getTermY() == 18 );
}

//----------------------------------------------------------------------
void FEventTest::ffocuseventTest()
{
  finalcut::FFocusEvent event (finalcut::Event::FocusIn);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::FocusIn );
  CPPUNIT_ASSERT ( event.gotFocus() );
  CPPUNIT_ASSERT ( ! event.lostFocus() );
  CPPUNIT_ASSERT ( event.getFocusType() == finalcut::FocusTypes::DefiniteWidget );
  CPPUNIT_ASSERT ( event.isAccepted() );  // accept by default
  event.ignore();
  CPPUNIT_ASSERT ( ! event.isAccepted() );
  event.accept();
  CPPUNIT_ASSERT ( event.isAccepted() );

  finalcut::FFocusEvent event1 (finalcut::Event::FocusOut);
  CPPUNIT_ASSERT ( event1.getType() == finalcut::Event::FocusOut );
  CPPUNIT_ASSERT ( ! event1.gotFocus() );
  CPPUNIT_ASSERT ( event1.lostFocus() );
  CPPUNIT_ASSERT ( event1.getFocusType() == finalcut::FocusTypes::DefiniteWidget );
  event1.setFocusType(finalcut::FocusTypes::NextWidget);
  CPPUNIT_ASSERT ( event1.getFocusType() != finalcut::FocusTypes::DefiniteWidget );
  CPPUNIT_ASSERT ( event1.getFocusType() == finalcut::FocusTypes::NextWidget );
  event1.setFocusType(finalcut::FocusTypes::PreviousWidget);
  CPPUNIT_ASSERT ( event1.getFocusType() == finalcut::FocusTypes::PreviousWidget );

  finalcut::FFocusEvent event2 (finalcut::Event::ChildFocusIn);
  CPPUNIT_ASSERT ( event2.getType() == finalcut::Event::ChildFocusIn );
  CPPUNIT_ASSERT ( ! event2.gotFocus() );
  CPPUNIT_ASSERT ( ! event2.lostFocus() );
  CPPUNIT_ASSERT ( event2.getFocusType() == finalcut::FocusTypes::DefiniteWidget );

  finalcut::FFocusEvent event3 (finalcut::Event::ChildFocusOut);
  CPPUNIT_ASSERT ( event3.getType() == finalcut::Event::ChildFocusOut );
  CPPUNIT_ASSERT ( ! event3.gotFocus() );
  CPPUNIT_ASSERT ( ! event3.lostFocus() );
  CPPUNIT_ASSERT ( event3.getFocusType() == finalcut::FocusTypes::DefiniteWidget );
}

//----------------------------------------------------------------------
void FEventTest::facceleventTest()
{
  finalcut::FWidget w{};
  finalcut::FAccelEvent event (finalcut::Event::Accelerator, &w);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::Accelerator );
  CPPUNIT_ASSERT ( event.focusedWidget() != nullptr );
  CPPUNIT_ASSERT ( event.focusedWidget() == &w );
  CPPUNIT_ASSERT ( ! event.isAccepted() );
  event.accept();
  CPPUNIT_ASSERT ( event.isAccepted() );
  event.ignore();
  CPPUNIT_ASSERT ( ! event.isAccepted() );
}

//----------------------------------------------------------------------
void FEventTest::fresizeeventTest()
{
  finalcut::FResizeEvent event (finalcut::Event::Resize);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::Resize );
  CPPUNIT_ASSERT ( ! event.isAccepted() );
  event.accept();
  CPPUNIT_ASSERT ( event.isAccepted() );
  event.ignore();
  CPPUNIT_ASSERT ( ! event.isAccepted() );
}

//----------------------------------------------------------------------
void FEventTest::fshoweventTest()
{
  finalcut::FShowEvent event (finalcut::Event::Show);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::Show );
}

//----------------------------------------------------------------------
void FEventTest::fhideeventTest()
{
  finalcut::FHideEvent event (finalcut::Event::Hide);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::Hide );
}

//----------------------------------------------------------------------
void FEventTest::fcloseeventTest()
{
  finalcut::FCloseEvent event (finalcut::Event::Close);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::Close );
  CPPUNIT_ASSERT ( ! event.isAccepted() );
  event.accept();
  CPPUNIT_ASSERT ( event.isAccepted() );
  event.ignore();
  CPPUNIT_ASSERT ( ! event.isAccepted() );
}

//----------------------------------------------------------------------
void FEventTest::ftimereventTest()
{
  finalcut::FTimerEvent event (finalcut::Event::Timer, 0);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::Timer );
  CPPUNIT_ASSERT ( event.getTimerId() == 0 );

  finalcut::FTimerEvent event1 (finalcut::Event::Timer, 1);
  CPPUNIT_ASSERT ( event1.getTimerId() == 1 );

  finalcut::FTimerEvent event2 (finalcut::Event::Timer, 99);
  CPPUNIT_ASSERT ( event2.getTimerId() == 99 );
}

//----------------------------------------------------------------------
void FEventTest::fusereventTest()
{
  finalcut::FUserEvent event (finalcut::Event::User, 0);
  CPPUNIT_ASSERT ( event.getType() == finalcut::Event::User );
  CPPUNIT_ASSERT ( event.getUserId() == 0 );

  finalcut::FUserEvent event1 (finalcut::Event::User, 1);
  CPPUNIT_ASSERT ( event1.getUserId() == 1 );
  char data[3] = { 'G', 'o', '\0' };
  event1.setData (data);
  auto& get_data = event1.getData<char[3]>();
  CPPUNIT_ASSERT ( get_data[0] == 'G' );
  CPPUNIT_ASSERT ( get_data[1] == 'o' );
  CPPUNIT_ASSERT ( get_data[2] == '\0' );
  auto&& fdata_obj = event1.getFDataObject<char*>();
  CPPUNIT_ASSERT ( fdata_obj.get()[0] == 'G' );
  CPPUNIT_ASSERT ( fdata_obj.get()[1] == 'o' );
  CPPUNIT_ASSERT ( fdata_obj.get()[2] == '\0' );

  finalcut::FUserEvent event2 (finalcut::Event::User, 99);
  CPPUNIT_ASSERT ( event2.getUserId() == 99 );
  auto lambda = [] () { return 5; };
  auto&& fdata = finalcut::makeFData(std::move(lambda));
  event2.setFDataObject<decltype(*fdata)>(*fdata);
  auto& lambda_from_event = event2.getData<decltype(lambda)>();
  CPPUNIT_ASSERT ( lambda_from_event() == 5 );
  auto&& fdata_obj2 = event2.getFDataObject<decltype(lambda)>();
  auto& lambda_from_fdata = fdata_obj2.get();
  CPPUNIT_ASSERT ( lambda_from_fdata() == 5 );
}

//----------------------------------------------------------------------

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FEventTest);

// The general unit test main part
#include <main-test.inc>
