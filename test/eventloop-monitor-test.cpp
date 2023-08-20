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

#include <chrono>
#include <string>

#include <final/final.h>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::high_resolution_clock;

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

namespace
{

std::function<void(int)> signal_handler;

//----------------------------------------------------------------------
void sigHandler (int num)
{
  CPPUNIT_ASSERT ( num == SIGALRM );
  std::cout << "Call sigHandler(" << num << ")\n";
  signal_handler(num);
}

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
    void eventLoopTest();
    void setMonitorTest();
    void IoMonitorTest();
    void SignalMonitorTest();
    void TimerMonitorTest();
    void exceptionTest();

  private:
    void keyboard_input (std::string);

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (EventloopMonitorTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (eventLoopTest);
    CPPUNIT_TEST (setMonitorTest);
    CPPUNIT_TEST (IoMonitorTest);
    CPPUNIT_TEST (SignalMonitorTest);
    CPPUNIT_TEST (TimerMonitorTest);
    CPPUNIT_TEST (exceptionTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


//----------------------------------------------------------------------
void EventloopMonitorTest::classNameTest()
{
  finalcut::EventLoop eloop{};
  const finalcut::Monitor monitor(&eloop);
  const finalcut::IoMonitor io_monitor(&eloop);
  const finalcut::SignalMonitor signal_monitor(&eloop);
  const finalcut::TimerMonitor timer_monitor(&eloop);
  const finalcut::FString& eloop_classname = eloop.getClassName();
  const finalcut::FString& monitor_classname = monitor.getClassName();
  const finalcut::FString& io_monitor_classname = io_monitor.getClassName();
  const finalcut::FString& signal_monitor_classname = signal_monitor.getClassName();
  const finalcut::FString& timer_monitor_classname = timer_monitor.getClassName();
  CPPUNIT_ASSERT ( eloop_classname == "EventLoop" );
  CPPUNIT_ASSERT ( monitor_classname == "Monitor" );
  CPPUNIT_ASSERT ( io_monitor_classname == "IoMonitor" );
  CPPUNIT_ASSERT ( signal_monitor_classname == "SignalMonitor" );
  CPPUNIT_ASSERT ( timer_monitor_classname == "TimerMonitor" );
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
void EventloopMonitorTest::eventLoopTest()
{
  // Test without monitor
  finalcut::EventLoop eloop{};
  signal_handler = [&eloop] (int num)
  {
    eloop.leave();
  };
  signal(SIGALRM, sigHandler);  // Register signal handler
  std::cout << "\n";
  alarm(1);  // Schedule a alarm after 1 seconds
  CPPUNIT_ASSERT ( eloop.run() == 0 );

  // Test with one monitor
  Monitor_protected mon(&eloop);
  CPPUNIT_ASSERT ( mon.getEvents() == 0 );
  CPPUNIT_ASSERT ( mon.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( mon.getUserContext() == nullptr );
  CPPUNIT_ASSERT ( ! mon.isActive() );
  mon.p_setEvents (POLLIN);
  std::array<int, 2> pipe_fd{{-1, -1}};
  auto callback_handler = [&pipe_fd, &eloop] (const finalcut::Monitor*, short)
  {
    std::cout << "Callback handle";
    uint64_t buf{0};
    CPPUNIT_ASSERT ( ::read(pipe_fd[0], &buf, sizeof(buf)) == sizeof(buf) );
    CPPUNIT_ASSERT ( buf == std::numeric_limits<uint64_t>::max() );
    eloop.leave();
  };
  mon.p_setHandler(callback_handler);
  CPPUNIT_ASSERT ( ::pipe(pipe_fd.data()) == 0 );
  mon.p_setFileDescriptor(pipe_fd[0]);  // Read end of pipe
  mon.resume();
  signal_handler = [&pipe_fd] (int)
  {
    uint64_t buf{std::numeric_limits<uint64_t>::max()};
    CPPUNIT_ASSERT ( ::write (pipe_fd[1], &buf, sizeof(buf)) > 0 );
  };
  alarm(1);  // Schedule a alarm after 1 seconds
  CPPUNIT_ASSERT ( eloop.run() == 0 );
  CPPUNIT_ASSERT ( mon.getEvents() == POLLIN );
  CPPUNIT_ASSERT ( mon.getFileDescriptor() == pipe_fd[0] );
  CPPUNIT_ASSERT ( mon.getUserContext() == nullptr );
  CPPUNIT_ASSERT ( mon.isActive() );
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
void EventloopMonitorTest::IoMonitorTest()
{
  finalcut::EventLoop eloop{};
  signal_handler = [this] (int)
  {
    keyboard_input("A");
  };
  signal(SIGALRM, sigHandler);  // Register signal handler
  finalcut::IoMonitor stdin_monitor{&eloop};
  auto callback_handler = [&eloop] (const finalcut::Monitor* mon, short)
  {
    std::cout << "\nIoMonitor callback handle";
    uint8_t buf{0};
    const auto bytes = ::read(mon->getFileDescriptor(), &buf, 1);
    CPPUNIT_ASSERT ( bytes == 1 );
    CPPUNIT_ASSERT ( buf == 'A' );
    eloop.leave();
  };
  stdin_monitor.init (STDIN_FILENO, POLLIN, callback_handler, nullptr);
  std::cout << "\n";
  alarm(1);  // Schedule a alarm after 1 seconds
  stdin_monitor.resume();
  CPPUNIT_ASSERT ( eloop.run() == 0 );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::SignalMonitorTest()
{
  finalcut::EventLoop eloop{};
  signal_handler = [] (int)
  {
    std::raise(SIGABRT);  // Send abort signal
  };
  signal(SIGALRM, sigHandler);  // Register signal handler
  finalcut::SignalMonitor sig_abrt_monitor{&eloop};
  auto callback_handler = [&eloop] (const finalcut::Monitor*, short)
  {
    std::cout << "SignalMonitor callback handle";
    eloop.leave();
  };
  sig_abrt_monitor.init(SIGABRT, callback_handler, nullptr);
  std::cout << "\n";
  alarm(1);  // Schedule a alarm after 1 seconds
  sig_abrt_monitor.resume();
  CPPUNIT_ASSERT ( eloop.run() == 0 );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::TimerMonitorTest()
{
  finalcut::EventLoop eloop{};
  finalcut::TimerMonitor timer_monitor{&eloop};
  int num{0};
  auto callback_handler = [&eloop, &num] (const finalcut::Monitor*, short)
  {
    num++;
    std::cout << "TimerMonitor callback handle (" << num << ")\n";

    if ( num == 3 )
      eloop.leave();
  };
  timer_monitor.init (callback_handler, nullptr);
  timer_monitor.setInterval ( std::chrono::nanoseconds{ 500'000'000 }
                            , std::chrono::nanoseconds{ 1'000'000'000 } );
  std::cout << "\n";
  timer_monitor.resume();
  auto start = high_resolution_clock::now();
  CPPUNIT_ASSERT ( eloop.run() == 0 );
  auto end = high_resolution_clock::now();
  auto duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( num == 3 );
  CPPUNIT_ASSERT ( duration_ms >= 2500 );
  CPPUNIT_ASSERT ( duration_ms < 2510 );

  timer_monitor.setInterval ( std::chrono::nanoseconds{ 100'000'000 }
                            , std::chrono::nanoseconds{ 100'000'000 } );
  num = 0;
  start = high_resolution_clock::now();
  CPPUNIT_ASSERT ( eloop.run() == 0 );
  end = high_resolution_clock::now();
  duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( num == 3 );
  CPPUNIT_ASSERT ( duration_ms >= 300 );
  CPPUNIT_ASSERT ( duration_ms < 310 );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::exceptionTest()
{
  CPPUNIT_ASSERT_THROW ( getException()
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_NO_THROW ( getNoException() );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::keyboard_input (std::string s)
{
  // Simulates keystrokes

  const char EOT = 0x04;  // End of Transmission
  auto stdin_no = finalcut::FTermios::getStdIn();
  fflush(stdout);

  std::string::const_iterator iter;
  iter = s.begin();

  while ( iter != s.end() )
  {
    char c = *iter;

    if ( ioctl (stdin_no, TIOCSTI, &c) < 0 )
      break;

    ++iter;
  }

  if ( ioctl (stdin_no, TIOCSTI, &EOT) < 0 )
    return;

  fflush(stdin);
}


// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (EventloopMonitorTest);

// The general unit test main part
#include <main-test.inc>
