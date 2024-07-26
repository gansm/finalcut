/***********************************************************************
* ftimer-test.cpp - FTimer unit tests                                  *
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

#include <chrono>
#include <thread>

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <final/final.h>

namespace test
{

//----------------------------------------------------------------------
// class FTimer_protected
//----------------------------------------------------------------------

class FTimer_protected : public finalcut::FObjectTimer
                       , public finalcut::FTimer<finalcut::FObject>
{
  public:
    // Using-declarations
    using finalcut::FObjectTimer::addTimer;
    using finalcut::FObjectTimer::delAllTimers;
    using finalcut::FObjectTimer::delOwnTimers;
    using finalcut::FObjectTimer::delTimer;

    // Constructor
    FTimer_protected() = default;

    auto getTimerList() const -> finalcut::FTimer<finalcut::FObject>::FTimerList*
    {
      return finalcut::FObjectTimer::getTimerList();
    }

    auto processEvent() -> uInt
    {
      return finalcut::FObjectTimer::processTimerEvent();
    }

    void performTimerAction (finalcut::FObject*, finalcut::FEvent*) override
    {
      std::cout << ".";
      fflush(stdout);
      count++;
    }

    // Data member
    uInt count{0};
};

//----------------------------------------------------------------------

class FObject_timer : public finalcut::FObject
{
  public:
    FObject_timer() = default;

    auto getValue() const -> int
    {
      return value;
    }

  protected:
    void onTimer (finalcut::FTimerEvent* ev) override
    {
      if ( ev->getTimerId() == 1 )
        value++;
    }

  private:
    // Data member
    int value{0};
};

}  // namespace test


//----------------------------------------------------------------------
// class FTimerTest
//----------------------------------------------------------------------

class FTimerTest : public CPPUNIT_NS::TestFixture
{
  public:
    FTimerTest() = default;

  protected:
    void classNameTest();
    void timeTest();
    void timerTest();
    void performTimerActionTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (FTimerTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (timeTest);
    CPPUNIT_TEST (timerTest);
    CPPUNIT_TEST (performTimerActionTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};

//----------------------------------------------------------------------
void FTimerTest::classNameTest()
{
  finalcut::FTimer<finalcut::FObject> t;
  const finalcut::FString& classname = t.getClassName();
  CPPUNIT_ASSERT ( classname == "FTimer" );
}

//----------------------------------------------------------------------
void FTimerTest::timeTest()
{
  TimeValue time1{};
  uInt64 timeout = 750000;  // 750 ms
  time1 = finalcut::FObjectTimer::getCurrentTime();
  CPPUNIT_ASSERT ( ! finalcut::FObjectTimer::isTimeout (time1, timeout) );
  sleep(1);
  CPPUNIT_ASSERT ( finalcut::FObjectTimer::isTimeout (time1, timeout) );
  time1 = TimeValue{}
        + std::chrono::seconds(300)
        + std::chrono::microseconds(2000000);
  CPPUNIT_ASSERT ( finalcut::FObjectTimer::isTimeout (time1, timeout) );
}

//----------------------------------------------------------------------
void FTimerTest::timerTest()
{
  using finalcut::operator +;
  using finalcut::operator -;
  using finalcut::operator +=;
  using finalcut::operator <;

  test::FTimer_protected t1;
  test::FTimer_protected t2;
  int id1, id2;
  CPPUNIT_ASSERT ( t1.getTimerList()->empty() );
  id1 = t1.addTimer(300);
  CPPUNIT_ASSERT ( ! t1.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 1 );
  id2 = t1.addTimer(900);
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 2 );
  CPPUNIT_ASSERT ( &t1 != &t2 );
  CPPUNIT_ASSERT ( id1 != id2 );
  t1.delTimer (id1);
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 1 );
  t1.delTimer (id2);
  CPPUNIT_ASSERT ( t1.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 0 );

  id1 = t1.addTimer(45);
  id2 = t1.addTimer(95);
  t1.delTimer (id2);
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 1 );
  t1.delTimer (id1);
  CPPUNIT_ASSERT ( t1.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 0 );

  CPPUNIT_ASSERT ( ! t1.delTimer (id1) );  // id double delete
  CPPUNIT_ASSERT ( ! t1.delAllTimers() );

  t1.addTimer(250);
  t1.addTimer(500);
  t2.addTimer(750);
  t2.addTimer(1000);
  CPPUNIT_ASSERT_EQUAL ( t1.getTimerList(), t2.getTimerList() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 4 );
  CPPUNIT_ASSERT ( t2.getTimerList()->size() == 4 );

  t1.delOwnTimers();
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 2 );
  CPPUNIT_ASSERT ( t2.getTimerList()->size() == 2 );

  t1.addTimer(250);
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 3 );
  CPPUNIT_ASSERT ( t2.getTimerList()->size() == 3 );

  t2.delAllTimers();
  CPPUNIT_ASSERT ( t1.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t2.getTimerList()->empty() );
  CPPUNIT_ASSERT ( t1.getTimerList()->size() == 0 );
  CPPUNIT_ASSERT ( t2.getTimerList()->size() == 0 );

  auto tv1 = TimeValue{} + std::chrono::seconds(1321006271);
  auto tv2 = TimeValue{} + std::chrono::seconds(27166271);
  auto tv2_duration = std::chrono::duration_cast<std::chrono::seconds>(tv2.time_since_epoch());
  auto tv_sum = tv1 + tv2_duration;
  auto sec_sum = std::chrono::duration_cast<std::chrono::seconds>(tv_sum.time_since_epoch()).count();
  CPPUNIT_ASSERT ( sec_sum == 1348172542 );

  auto tv_difference = tv1 - tv2_duration;
  auto sec_difference = std::chrono::duration_cast<std::chrono::seconds>(tv_difference.time_since_epoch()).count();
  CPPUNIT_ASSERT ( sec_difference == 1293840000 );

  tv_sum += tv2_duration;
  sec_sum = std::chrono::duration_cast<std::chrono::seconds>(tv_sum.time_since_epoch()).count();
  CPPUNIT_ASSERT ( sec_sum == 1375338813 );

  CPPUNIT_ASSERT ( tv2 < tv1 );
  CPPUNIT_ASSERT ( ! (tv1 < tv2) );
  CPPUNIT_ASSERT ( tv1 < tv_sum );
  CPPUNIT_ASSERT ( ! (tv_sum < tv1) );
  CPPUNIT_ASSERT ( tv2 < tv_sum );
  CPPUNIT_ASSERT ( ! (tv_sum < tv2) );
  CPPUNIT_ASSERT ( tv_difference < tv_sum );
  CPPUNIT_ASSERT ( ! (tv_sum < tv_difference) );

  tv1 += std::chrono::microseconds(600000);
  tv2 += std::chrono::microseconds(600000);
  tv2_duration = std::chrono::duration_cast<std::chrono::seconds>(tv2.time_since_epoch());
  tv_sum = tv1 + tv2_duration;
  auto s_sum = std::chrono::duration_cast<std::chrono::seconds>(tv_sum.time_since_epoch()).count();
  CPPUNIT_ASSERT ( s_sum == 1348172542 );
  auto us_sum = ( std::chrono::duration_cast<std::chrono::microseconds>(tv_sum.time_since_epoch())
                - std::chrono::seconds(1348172542) ).count();
  CPPUNIT_ASSERT ( us_sum == 600000 );

  auto tv1_sec = std::chrono::duration_cast<std::chrono::seconds>(tv1.time_since_epoch());
  auto tv2_sec = std::chrono::duration_cast<std::chrono::seconds>(tv2.time_since_epoch());
  tv1 = TimeValue{} + tv1_sec + std::chrono::microseconds(654321);
  tv2 = TimeValue{} + tv2_sec + std::chrono::microseconds(123456);
  auto tv2_duration_ms = std::chrono::duration_cast<std::chrono::microseconds>(tv2.time_since_epoch());
  tv_difference = tv1 - tv2_duration_ms;
  sec_difference = std::chrono::duration_cast<std::chrono::seconds>(tv_difference.time_since_epoch()).count();
  CPPUNIT_ASSERT ( sec_difference == 1293840000 );
  auto usec_difference = ( std::chrono::duration_cast<std::chrono::microseconds>(tv_difference.time_since_epoch())
                         - std::chrono::seconds(1293840000) ).count();
  CPPUNIT_ASSERT ( usec_difference == 530865 );

  tv2_sec = std::chrono::duration_cast<std::chrono::seconds>(tv2.time_since_epoch());
  tv2 = TimeValue{} + tv2_sec + std::chrono::microseconds(999888);
  auto tv2_duration2 = std::chrono::duration_cast<std::chrono::microseconds>(tv2.time_since_epoch());
  tv_sum += tv2_duration2;
  s_sum = std::chrono::duration_cast<std::chrono::seconds>(tv_sum.time_since_epoch()).count();
  CPPUNIT_ASSERT ( s_sum == 1375338814 );
  us_sum = ( std::chrono::duration_cast<std::chrono::microseconds>(tv_sum.time_since_epoch())
           - std::chrono::seconds(1375338814) ).count();
  CPPUNIT_ASSERT ( us_sum == 599888 );

  CPPUNIT_ASSERT ( tv2 < tv1 );
  CPPUNIT_ASSERT ( ! (tv1 < tv2) );
  CPPUNIT_ASSERT ( tv_difference < tv_sum );
  CPPUNIT_ASSERT ( ! (tv_sum < tv_difference) );

  CPPUNIT_ASSERT ( ! t1.delTimer(0) );
  CPPUNIT_ASSERT ( ! t1.delTimer(-1) );
}

//----------------------------------------------------------------------
void FTimerTest::performTimerActionTest()
{
  test::FTimer_protected t1;
  uInt num_events = 0;
  uInt loop = 0;
  t1.addTimer(100);

  while ( loop < 10 )
  {
    num_events += t1.processEvent();
    // Wait 100 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    loop++;
  }

  CPPUNIT_ASSERT ( loop == 10 );
  CPPUNIT_ASSERT ( num_events == 9 );
  CPPUNIT_ASSERT ( t1.count == 9 );

  test::FObject_timer t2;
  CPPUNIT_ASSERT ( t2.getValue() == 0 );
  finalcut::FTimerEvent timer_ev (finalcut::Event::Timer, 1);

  for (auto x = 0; x < 10; x++)
    finalcut::FApplication::sendEvent (&t2, &timer_ev);

  CPPUNIT_ASSERT ( t2.getValue() == 10 );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (FTimerTest);

// The general unit test main part
#include <main-test.inc>
