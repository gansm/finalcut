/***********************************************************************
* eventloop-monitor-test.cpp - Event loop monitor unit tests           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023 Markus Gans                                           *
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

void getException()
{ throw finalcut::monitor_error{"Monitor error"}; }

void getNoException()
{ }

//----------------------------------------------------------------------
// class FWidget_protected
//----------------------------------------------------------------------

class Monitor_protected : public finalcut::Monitor
{
  public:
    // Using-declaration
    using Monitor::Monitor;

    // Destructor
    ~Monitor_protected() override;

    // Mutators
    void p_setFileDescriptor (int);
    void p_setEvents (short);
    void p_setHandler (finalcut::handler_t&&);
    void p_setUserContext (void*);
    void p_setInitialized();

    // Inquiry
    auto p_isInitialized() const -> bool;

    // Methods
    void p_trigger (short) ;
};

//----------------------------------------------------------------------
inline Monitor_protected::~Monitor_protected() noexcept = default;  // destructor

//----------------------------------------------------------------------
inline void Monitor_protected::p_setFileDescriptor (int file_descriptor)
{
  finalcut::Monitor::setFileDescriptor(file_descriptor);
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_setEvents (short ev)
{
  finalcut::Monitor::setEvents(ev);
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_setHandler (finalcut::handler_t&& hdl)
{
  finalcut::Monitor::setHandler(std::move(hdl));
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_setUserContext (void* uc)
{
  finalcut::Monitor::setUserContext(uc);
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_setInitialized()
{
  finalcut::Monitor::setInitialized();
}

//----------------------------------------------------------------------
inline auto Monitor_protected::p_isInitialized() const -> bool
{
  return finalcut::Monitor::isInitialized();
}

//----------------------------------------------------------------------
inline void Monitor_protected::p_trigger (short return_events)
{
  finalcut::Monitor::trigger(return_events);
}


//----------------------------------------------------------------------
// class EventloopMonitorTest
//----------------------------------------------------------------------

class EventloopMonitorTest : public CPPUNIT_NS::TestFixture
{
  public:
    EventloopMonitorTest() = default;

  protected:
    void classNameTest();
    void noArgumentTest();
    void setMonitorTest();
    void exceptionTest();

  private:
    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (EventloopMonitorTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (setMonitorTest);
    CPPUNIT_TEST (exceptionTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
void EventloopMonitorTest::classNameTest()
{
  finalcut::EventLoop eloop{};
  const finalcut::Monitor m(&eloop);
  const finalcut::FString& classname = m.getClassName();
  CPPUNIT_ASSERT ( classname == "Monitor" );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::noArgumentTest()
{
  finalcut::EventLoop eloop{};
  const finalcut::Monitor m(&eloop);
  CPPUNIT_ASSERT ( m.getEvents() == 0 );
  CPPUNIT_ASSERT ( m.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( m.getUserContext() == nullptr );
  CPPUNIT_ASSERT ( ! m.isActive() );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::setMonitorTest()
{
  finalcut::EventLoop eloop{};
  Monitor_protected m(&eloop);
  CPPUNIT_ASSERT ( m.getEvents() == 0 );
  CPPUNIT_ASSERT ( m.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( m.getUserContext() == nullptr );
  CPPUNIT_ASSERT ( ! m.p_isInitialized() );
  m.p_setInitialized();
  CPPUNIT_ASSERT ( m.p_isInitialized() );
  CPPUNIT_ASSERT ( ! m.isActive() );
  m.resume();
  CPPUNIT_ASSERT ( m.isActive() );
  m.suspend();
  CPPUNIT_ASSERT ( ! m.isActive() );
  m.resume();
  CPPUNIT_ASSERT ( m.isActive() );
  m.p_setFileDescriptor (0);
  CPPUNIT_ASSERT ( m.getFileDescriptor() == 0 );
  m.p_setFileDescriptor (245);
  CPPUNIT_ASSERT ( m.getFileDescriptor() == 245 );
  m.p_setEvents (2);
  CPPUNIT_ASSERT ( m.getEvents() == 2 );
  m.p_setEvents (std::numeric_limits<short>::max());
  CPPUNIT_ASSERT ( m.getEvents() == std::numeric_limits<short>::max() );
  int value = 10;
  m.p_setHandler ([&value] (const finalcut::Monitor*, short n) { value -= n; });
  CPPUNIT_ASSERT ( value == 10 );
  m.p_trigger(2);
  CPPUNIT_ASSERT ( value == 8 );
  using Function = std::function<void()>;
  Function f = [&value] () { value *= 10; };
  m.p_setUserContext (reinterpret_cast<void*>(&f));
  CPPUNIT_ASSERT ( value == 8 );
  (*reinterpret_cast<Function*>(m.getUserContext()))();
  CPPUNIT_ASSERT ( value == 80 );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::exceptionTest()
{
  CPPUNIT_ASSERT_THROW ( getException()
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_NO_THROW ( getNoException() );
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (EventloopMonitorTest);

// The general unit test main part
#include <main-test.inc>
