/***********************************************************************
* eventloop-monitor-test.cpp - Event loop monitor unit tests           *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023-2025 Markus Gans                                      *
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
#include <queue>
#include <string>

#include <final/final.h>
#define USE_FINAL_H
#include <final/eventloop/eventloop_functions.h>
#undef USE_FINAL_H

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::high_resolution_clock;

void getException()
{ throw finalcut::monitor_error{"Monitor error"}; }

void getNoException()
{ }


//----------------------------------------------------------------------
// class Monitor_protected
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

    template <typename T>
    void p_setUserContext (T&&);
    void p_setInitialized();

    // Inquiry
    auto p_isInitialized() const -> bool;

    // Methods
    void p_trigger (short);
};

//----------------------------------------------------------------------
inline Monitor_protected::~Monitor_protected() = default;  // destructor

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
template <typename T>
inline void Monitor_protected::p_setUserContext (T&& uc)
{
  finalcut::Monitor::setUserContext(std::forward<T>(uc));
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
// class PosixTimer_protected
//----------------------------------------------------------------------

class PosixTimer_protected : public finalcut::PosixTimer
{
  public:
    // Using-declaration
    using PosixTimer::PosixTimer;

    // Destructor
    ~PosixTimer_protected() override;

    // Mutators
    void p_setFileDescriptor (int);

    // Methods
    void p_trigger (short);
};

//----------------------------------------------------------------------
inline PosixTimer_protected::~PosixTimer_protected() = default;  // destructor

//----------------------------------------------------------------------
inline void PosixTimer_protected::p_setFileDescriptor (int file_descriptor)
{
  finalcut::Monitor::setFileDescriptor(file_descriptor);
}

//----------------------------------------------------------------------
inline void PosixTimer_protected::p_trigger (short return_events)
{
  finalcut::PosixTimer::trigger(return_events);
}


//----------------------------------------------------------------------
// class KqueueTimer_protected
//----------------------------------------------------------------------

class KqueueTimer_protected : public finalcut::KqueueTimer
{
  public:
    // Using-declaration
    using KqueueTimer::KqueueTimer;

    // Destructor
    ~KqueueTimer_protected() override;

    // Methods
    void p_trigger (short);
};

//----------------------------------------------------------------------
inline KqueueTimer_protected::~KqueueTimer_protected() = default;  // destructor

//----------------------------------------------------------------------
inline void KqueueTimer_protected::p_trigger (short return_events)
{
  finalcut::KqueueTimer::trigger(return_events);
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

#if !defined(USE_KQUEUE_TIMER)
  using u_short = unsigned short;
  using u_int = unsigned int;

  struct kevent
  {
    uintptr_t ident;   // Identifier for this event
    short     filter;  // Filter for event
    u_short   flags;   // Action flags for kqueue
    u_int     fflags;  // Filter flag value
    int64_t   data;    // Filter data value
    void*     udata;   // Opaque user data identifier
  };

  // Event filter
  #define EVFILT_READ    (-1)  // File descriptor read
  #define EVFILT_WRITE   (-2)  // File descriptor write
  #define EVFILT_AIO     (-3)  // Attached to aio requests
  #define EVFILT_VNODE   (-4)  // Attached to vnodes
  #define EVFILT_PROC    (-5)  // Attached to struct process
  #define EVFILT_SIGNAL  (-6)  // Attached to struct process
  #define EVFILT_TIMER   (-7)  // Timers
  #define EVFILT_DEVICE  (-8)  // Devices
  #define EVFILT_EXCEPT  (-9)  // Exceptional conditions
#endif


namespace test
{

//----------------------------------------------------------------------
// class FSystemTest
//----------------------------------------------------------------------

class FSystemTest : public finalcut::FSystem
{
  public:
    // Constructor
    FSystemTest() = default;

    // Methods
    auto inPortByte (uShort) noexcept -> uChar override;
    void outPortByte (uChar, uShort) noexcept override;
    auto isTTY (int) const noexcept -> int override;
    auto ioctl (int, uLong, ...) noexcept -> int override;
    auto pipe (finalcut::PipeData&) noexcept -> int override;
    auto open (const char*, int, ...) noexcept -> int override;
    auto close (int) noexcept -> int override;
    auto fopen (const char*, const char*) noexcept -> FILE* override;
    auto fputs (const char*, FILE*) noexcept -> int override;
    auto fclose (FILE*) noexcept -> int override;
    auto putchar (int) noexcept -> int override;
    auto putstring (const char*, std::size_t) noexcept -> int override;
    auto sigaction ( int, const struct sigaction*
                   , struct sigaction*) noexcept -> int override;
    auto timer_create ( clockid_t, struct sigevent*
                      , timer_t* ) noexcept -> int override;
    auto timer_settime ( timer_t, int
                       , const struct itimerspec*
                       , struct itimerspec* ) noexcept -> int override;
    auto timer_delete (timer_t) noexcept -> int override;
    auto kqueue() noexcept -> int override;
    auto kevent ( int, const struct kevent*
                , int, struct kevent*
                , int, const struct timespec* ) noexcept -> int override;
    auto getuid() noexcept -> uid_t override;
    auto geteuid() noexcept -> uid_t override;
    auto getpwuid_r ( uid_t, struct passwd*, char*
                    , size_t, struct passwd** ) noexcept -> int override;
    auto realpath (const char*, char*) noexcept -> char* override;
    void setPipeReturnValue (int) noexcept;
    void setSigactionReturnValue (int) noexcept;
    void setTimerCreateReturnValue (int) noexcept;
    void setTimerSettimeReturnValue (int) noexcept;
    void setTimerDeleteReturnValue (int) noexcept;
    void setKqueueReturnValue (int) noexcept;
    void setKeventReturnValue (int) noexcept;

  private:
    int pipe_ret_value{0};
    int sigaction_ret_value{0};
    int timer_create_ret_value{0};
    int timer_settime_ret_value{0};
    int timer_delete_ret_value{0};
    int kqueue_ret_value{0};
    int kevent_ret_value{0};
};


// public methods of FSystemTest
//----------------------------------------------------------------------
inline auto FSystemTest::inPortByte (uShort) noexcept -> uChar
{
  return 0;
}

//----------------------------------------------------------------------
inline void FSystemTest::outPortByte (uChar, uShort) noexcept
{ }

//----------------------------------------------------------------------
inline auto FSystemTest::isTTY (int file_descriptor) const noexcept -> int
{
  std::cerr << "Call: isatty (file_descriptor=" << file_descriptor << ")\n";
  return 1;
}

//----------------------------------------------------------------------
inline auto FSystemTest::ioctl (int file_descriptor, uLong request, ...) noexcept -> int
{
  va_list args{};
  void* argp{};
  int ret_val{0};

  va_start (args, request);
  argp = va_arg (args, void*);
  va_end (args);

  std::cerr << "Call: ioctl (file_descriptor=" << file_descriptor
            << ", request=0x" << std::hex << request
            << ", argp=" << argp << std::dec << ")\n";
  return ret_val;
}

//----------------------------------------------------------------------
inline auto FSystemTest::pipe (finalcut::PipeData& pipe) noexcept -> int
{
  std::cerr << "Call: pipe (pipefd={"
            << pipe.getReadFd() << ", "
            << pipe.getWriteFd() << "})\n";
  pipe.setReadFd(5);
  pipe.setWriteFd(6);
  return pipe_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::open (const char* pathname, int flags, ...) noexcept -> int
{
  va_list args{};
  va_start (args, flags);
  auto mode = static_cast<mode_t>(va_arg (args, int));
  va_end (args);

  std::cerr << "Call: open (pathname=\"" << pathname
            << "\", flags=" << flags
            << ", mode=" << mode << ")\n";

  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::close (int file_descriptor) noexcept -> int
{
  std::cerr << "Call: close (file_descriptor=" << file_descriptor << ")\n";
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::fopen (const char* path, const char* mode) noexcept -> FILE*
{
  std::cerr << "Call: fopen (path=" << path
            << ", mode=" << mode << ")\n";
  return nullptr;
}

//----------------------------------------------------------------------
inline auto FSystemTest::fclose (FILE* file_ptr) noexcept -> int
{
  std::cerr << "Call: fclose (file_ptr=" << file_ptr << ")\n";
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::fputs (const char* str, FILE* stream) noexcept -> int
{
  return std::fputs(str, stream);
}

//----------------------------------------------------------------------
inline auto FSystemTest::putchar (int c) noexcept -> int
{
#if defined(__sun) && defined(__SVR4)
  return std::putchar(char(c));
#else
  return std::putchar(c);
#endif
}

//----------------------------------------------------------------------
inline auto FSystemTest::putstring (const char* str, std::size_t len) noexcept -> int
{
  for (std::size_t i{0}; i < len; ++i)
    if ( FSystemTest::putchar(int(str[i])) == EOF )
      return EOF;

  return 1;
}

//----------------------------------------------------------------------
inline auto FSystemTest::sigaction ( int signum
                                   , const struct sigaction* act
                                   , struct sigaction* oldact ) noexcept -> int
{
  std::cerr << "Call: sigaction (signum=" << signum
            << ", act=" << act
            << ", oldact=" << oldact << ")\n";
  return sigaction_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::timer_create ( clockid_t clockid
                                      , struct sigevent* sevp
                                      , timer_t* timerid ) noexcept -> int
{
  std::cerr << "Call: timer_create (clockid=" << clockid
            << ", sevp=" << sevp
            << ", timerid=" << timerid << ")\n";
  return timer_create_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::timer_settime ( timer_t timer_id
                                       , int flags
                                       , const struct itimerspec* new_value
                                       , struct itimerspec* old_value ) noexcept -> int
{
  std::cerr << "Call: timer_settime (timer_id=" << timer_id
            << ", flags=" << flags
            << ", new_value=" << new_value
            << ", old_value=" << old_value << ")\n";
  return timer_settime_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::timer_delete (timer_t timer_id) noexcept -> int
{
  std::cerr << "Call: timer_delete (timer_id=" << timer_id << ")\n";
  return timer_delete_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::kqueue() noexcept -> int
{
  std::cerr << "Call: kqueue()\n";
  return kqueue_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::kevent ( int kq, const struct ::kevent* changelist
                                , int nchanges, struct ::kevent* eventlist
                                , int nevents, const struct timespec* timeout) noexcept -> int
{
  std::cerr << "Call: kevent (kq=" << kq
            << ", changelist=" << changelist
            << ", nchanges=" << nchanges
            << ", eventlist=" << eventlist
            << ", nevents=" << nevents
            << ", timeout=" << timeout << ")\n";

  if ( ! changelist )
  {
    if ( eventlist && nevents )
    {
      eventlist[0].ident  = 1;
      eventlist[0].filter = EVFILT_TIMER;
    }
    return 1;
  }

  return kevent_ret_value;
}

//----------------------------------------------------------------------
inline auto FSystemTest::getuid() noexcept -> uid_t
{
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::geteuid() noexcept -> uid_t
{
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::getpwuid_r ( uid_t, struct passwd*, char*
                                    , size_t, struct passwd** ) noexcept -> int
{
  return 0;
}

//----------------------------------------------------------------------
inline auto FSystemTest::realpath (const char*, char*) noexcept -> char*
{
  return const_cast<char*>("");
}

//----------------------------------------------------------------------
inline void FSystemTest::setPipeReturnValue (int ret_val) noexcept
{
  pipe_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setSigactionReturnValue (int ret_val) noexcept
{
  sigaction_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setTimerCreateReturnValue (int ret_val) noexcept
{
  timer_create_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setTimerSettimeReturnValue (int ret_val) noexcept
{
  timer_settime_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setTimerDeleteReturnValue (int ret_val) noexcept
{
  timer_delete_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setKqueueReturnValue (int ret_val) noexcept
{
  kqueue_ret_value = ret_val;
}

//----------------------------------------------------------------------
inline void FSystemTest::setKeventReturnValue (int ret_val) noexcept
{
  kevent_ret_value = ret_val;
}


//----------------------------------------------------------------------
// class StringParser
//----------------------------------------------------------------------

class StringParser
{
  public:
    explicit StringParser (finalcut::EventLoop* eloop)
      : queue_monitor(eloop)
    {
      auto callback = [this, eloop] (const finalcut::Monitor*, short)
                      {
                        // After a backend event is triggered, this event
                        // handling code is called by the dispatcher
                        // of the event loop. Immediately before this call,
                        // the triggering event has already been destroyed.
                        std::cout << "BackendMonitor callback handle";
                        processQueuedInput();
                        eloop->leave();
                      };
      queue_monitor.init (callback, nullptr);
      queue_monitor.resume();
    }

    // Disable default constructor
    StringParser() = delete;

    // Disable copy constructor
    StringParser(const StringParser&) = delete;

    // Disable move constructor
    StringParser(const StringParser&&) = delete;

    // Accessors
    auto getQueue() const -> const std::deque<int>&
    {
      return number_queue;
    }

    // Mutator
    void setString (const finalcut::FString& s)
    {
      data_string = s;
    }

    // Methods
    void parseString();
    void processQueuedInput();

  private:
    std::deque<int> number_queue{};
    finalcut::FString data_string{};
    finalcut::BackendMonitor queue_monitor;
};

//----------------------------------------------------------------------
void StringParser::parseString()
{
  auto is_empty_before = number_queue.empty();
  auto number_parts = data_string.split(",");

  for (auto&& number_string : number_parts)
    number_queue.push_back(number_string.toInt());

  if ( is_empty_before && ! number_queue.empty() )
    queue_monitor.setEvent();  // This line creates the event
}

//----------------------------------------------------------------------
void StringParser::processQueuedInput()
{
  std::cout << "\nData in queue: ";

  while ( ! number_queue.empty() )
  {
    std::cout << "<" << number_queue.front() << "> ";
    number_queue.pop_front();
  }

  std::cout << "\n";
}

}  // namespace test


//----------------------------------------------------------------------
// class EventloopMonitorTest
//----------------------------------------------------------------------

class EventloopMonitorTest : public CPPUNIT_NS::TestFixture
{
  public:
    EventloopMonitorTest();

  protected:
    void classNameTest();
    void noArgumentTest();
    void PipeDataTest();
    void eventLoopTest();
    void setMonitorTest();
    void IoMonitorTest();
    void SignalMonitorTest();
    void TimerMonitorTest();
    void BackendMonitorTest();
    void exceptionTest();

  private:
    void keyboardInput (std::string&&);
    void drainStdin();
    void enableFakingInput();

    // Adds code needed to register the test suite
    CPPUNIT_TEST_SUITE (EventloopMonitorTest);

    // Add a methods to the test suite
    CPPUNIT_TEST (classNameTest);
    CPPUNIT_TEST (noArgumentTest);
    CPPUNIT_TEST (PipeDataTest);
    CPPUNIT_TEST (eventLoopTest);
    CPPUNIT_TEST (setMonitorTest);
    CPPUNIT_TEST (IoMonitorTest);
    CPPUNIT_TEST (SignalMonitorTest);
    CPPUNIT_TEST (TimerMonitorTest);
    CPPUNIT_TEST (BackendMonitorTest);
    CPPUNIT_TEST (exceptionTest);

    // End of test suite definition
    CPPUNIT_TEST_SUITE_END();
};


// constructors and destructor
//----------------------------------------------------------------------
EventloopMonitorTest::EventloopMonitorTest()
{
  enableFakingInput();
}

//----------------------------------------------------------------------
void EventloopMonitorTest::classNameTest()
{
  finalcut::EventLoop eloop{};
  const finalcut::Monitor monitor(&eloop);
  const finalcut::FString& eloop_classname = eloop.getClassName();
  const finalcut::FString& monitor_classname = monitor.getClassName();
  CPPUNIT_ASSERT ( eloop_classname == "EventLoop" );
  CPPUNIT_ASSERT ( monitor_classname == "Monitor" );
  const finalcut::PipeData pipedata;
  const finalcut::FString& pipedata_classname = pipedata.getClassName();
  CPPUNIT_ASSERT ( pipedata_classname == "PipeData" );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::noArgumentTest()
{
  finalcut::EventLoop eloop{};
  const finalcut::Monitor m(&eloop);
  CPPUNIT_ASSERT ( m.getEvents() == 0 );
  CPPUNIT_ASSERT ( m.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( m.getUserContext<void*>() == nullptr );
  CPPUNIT_ASSERT ( m.getUserContext<int>() == 0 );
  CPPUNIT_ASSERT ( m.getUserContext<int*>() == nullptr );
  CPPUNIT_ASSERT ( m.getUserContext<long>() == 0 );
  CPPUNIT_ASSERT ( m.getUserContext<long*>() == nullptr );
  CPPUNIT_ASSERT ( m.getUserContext<double>() == 0.0 );
  CPPUNIT_ASSERT ( m.getUserContext<double*>() == nullptr );
  CPPUNIT_ASSERT ( m.getUserContext<finalcut::FPoint>() == finalcut::FPoint() );
  CPPUNIT_ASSERT ( m.getUserContext<finalcut::FPoint>() == finalcut::FPoint(0, 0) );
  CPPUNIT_ASSERT ( m.getUserContext<finalcut::FPoint>() != finalcut::FPoint(1, 2) );
  CPPUNIT_ASSERT ( m.getUserContext<std::size_t>() == 0 );
  CPPUNIT_ASSERT ( m.getUserContext<std::size_t*>() == nullptr );
  CPPUNIT_ASSERT ( ! m.isActive() );

  const finalcut::PipeData pipedata;
  CPPUNIT_ASSERT ( pipedata.getReadFd() == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata.getWriteFd() == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata.getArrayData()[0] == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata.getArrayData()[1] == finalcut::PipeData::NO_FILE_DESCRIPTOR );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::PipeDataTest()
{
  finalcut::PipeData pipedata1;
  CPPUNIT_ASSERT ( pipedata1.getReadFd() == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata1.getWriteFd() == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[0] == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[1] == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  pipedata1.getArrayData()[0] = 2;
  pipedata1.getArrayData()[1] = 5;
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[0] == 2 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[1] == 5 );

  finalcut::PipeData pipedata2{10, 11};
  CPPUNIT_ASSERT ( pipedata2.getReadFd() == 10 );
  CPPUNIT_ASSERT ( pipedata2.getWriteFd() == 11 );
  CPPUNIT_ASSERT ( pipedata2.getArrayData()[0] == 10 );
  CPPUNIT_ASSERT ( pipedata2.getArrayData()[1] == 11 );

  pipedata2.swap(pipedata1);
  CPPUNIT_ASSERT ( pipedata2.getReadFd() == 2 );
  CPPUNIT_ASSERT ( pipedata2.getWriteFd() == 5 );
  CPPUNIT_ASSERT ( pipedata2.getArrayData()[0] == 2 );
  CPPUNIT_ASSERT ( pipedata2.getArrayData()[1] == 5 );
  CPPUNIT_ASSERT ( pipedata1.getReadFd() == 10 );
  CPPUNIT_ASSERT ( pipedata1.getWriteFd() == 11 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[0] == 10 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[1] == 11 );

  pipedata1.setReadFd(4);
  CPPUNIT_ASSERT ( pipedata1.getReadFd() == 4 );
  CPPUNIT_ASSERT ( pipedata1.getWriteFd() == 11 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[0] == 4 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[1] == 11 );

  pipedata1.setWriteFd(7);
  CPPUNIT_ASSERT ( pipedata1.getReadFd() == 4 );
  CPPUNIT_ASSERT ( pipedata1.getWriteFd() == 7 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[0] == 4 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[1] == 7 );

  pipedata1.setPipe(8, 9);
  CPPUNIT_ASSERT ( pipedata1.getReadFd() == 8 );
  CPPUNIT_ASSERT ( pipedata1.getWriteFd() == 9 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[0] == 8 );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[1] == 9 );

  pipedata1.reset();
  CPPUNIT_ASSERT ( pipedata1.getReadFd() == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata1.getWriteFd() == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[0] == finalcut::PipeData::NO_FILE_DESCRIPTOR );
  CPPUNIT_ASSERT ( pipedata1.getArrayData()[1] == finalcut::PipeData::NO_FILE_DESCRIPTOR );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::eventLoopTest()
{
  // Test without monitor
  finalcut::EventLoop eloop{};
  auto eloop_ptr = &eloop;
  signal_handler = [eloop_ptr] (int)
  {
    eloop_ptr->leave();
  };
  signal(SIGALRM, sigHandler);  // Register signal handler
  std::cout << "\n";
  alarm(1);  // Schedule a alarm after 1 seconds
  CPPUNIT_ASSERT ( eloop.run() == 0 );  // Run event loop

  // Test with one monitor
  Monitor_protected mon(&eloop);
  CPPUNIT_ASSERT ( mon.getEvents() == 0 );
  CPPUNIT_ASSERT ( mon.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( mon.getUserContext<void*>() == nullptr );
  CPPUNIT_ASSERT ( ! mon.isActive() );
  mon.p_setEvents (POLLIN);
  std::array<int, 2> pipe_fd{{-1, -1}};
  auto callback_handler = [&pipe_fd, eloop_ptr] (const finalcut::Monitor*, short)
  {
    std::cout << "Callback handle";
    uint64_t buf{0};
    CPPUNIT_ASSERT ( ::read(pipe_fd[0], &buf, sizeof(buf)) == sizeof(buf) );
    CPPUNIT_ASSERT ( buf == std::numeric_limits<uint64_t>::max() );
    eloop_ptr->leave();
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
  CPPUNIT_ASSERT ( eloop.run() == 0 );  // Run event loop
  CPPUNIT_ASSERT ( mon.getEvents() == POLLIN );
  CPPUNIT_ASSERT ( mon.getFileDescriptor() == pipe_fd[0] );
  CPPUNIT_ASSERT ( mon.getUserContext<void*>() == nullptr );
  CPPUNIT_ASSERT ( mon.isActive() );
  signal(SIGALRM, SIG_DFL);
  signal_handler = [] (int) { };  // Do nothing
}

//----------------------------------------------------------------------
void EventloopMonitorTest::setMonitorTest()
{
  finalcut::EventLoop eloop{};
  Monitor_protected m(&eloop);
  CPPUNIT_ASSERT ( m.getEvents() == 0 );
  CPPUNIT_ASSERT ( m.getFileDescriptor() == -1 );  // No File Descriptor
  CPPUNIT_ASSERT ( m.getUserContext<void*>() == nullptr );
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
  m.deactivate();
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
  m.p_setUserContext(f);
  CPPUNIT_ASSERT ( value == 8 );
  m.getUserContext<Function>()();
  CPPUNIT_ASSERT ( value == 80 );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::IoMonitorTest()
{
  finalcut::FTermios::init();
  finalcut::FTermios::storeTTYsettings();
  drainStdin();
  auto stdin_no = finalcut::FTermios::getStdIn();
  auto stdin_status_flags = fcntl(stdin_no, F_GETFL);
  finalcut::EventLoop eloop{};
  auto eloop_ptr = &eloop;
  finalcut::IoMonitor io_monitor{&eloop};
  const finalcut::FString& io_monitor_classname = io_monitor.getClassName();
  CPPUNIT_ASSERT ( io_monitor_classname == "IoMonitor" );
  auto callback_handler = [&stdin_status_flags, &stdin_no, eloop_ptr] (const finalcut::Monitor* mon, short)
  {
    char read_character{'\0'};
    CPPUNIT_ASSERT ( read_character == '\0' );
    stdin_status_flags |= O_NONBLOCK;
    CPPUNIT_ASSERT ( fcntl(stdin_no, F_SETFL, stdin_status_flags) != -1 );
    const auto bytes = ::read(mon->getFileDescriptor(), &read_character, 1);
    CPPUNIT_ASSERT ( bytes == 1 );
    CPPUNIT_ASSERT ( read_character == 'A' );
    stdin_status_flags &= ~O_NONBLOCK;
    CPPUNIT_ASSERT ( fcntl(stdin_no, F_SETFL, stdin_status_flags) != -1 );
    eloop_ptr->leave();
    std::cout << "\nIoMonitor callback handle" << std::flush;
  };
  io_monitor.init (stdin_no, POLLIN, callback_handler, nullptr);
  std::cout << "\n" << std::flush;
  io_monitor.resume();
  // Enter 'A'
  keyboardInput("A");

  // Keyboard interval timeout 75 ms
  std::this_thread::sleep_for(std::chrono::milliseconds(75));
  CPPUNIT_ASSERT ( eloop.run() == 0 );  // Run event loop
  finalcut::FTermios::restoreTTYsettings();
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
  finalcut::SignalMonitor signal_monitor{&eloop};
  const finalcut::FString& signal_monitor_classname = signal_monitor.getClassName();
  CPPUNIT_ASSERT ( signal_monitor_classname == "SignalMonitor" );
  auto callback_handler = [&eloop] (const finalcut::Monitor*, short)
  {
    std::cout << "SignalMonitor callback handle";
    eloop.leave();
  };
  signal_monitor.init(SIGABRT, callback_handler, nullptr);
  std::cout << "\n";
  alarm(1);  // Schedule a alarm after 1 seconds
  signal_monitor.resume();
  CPPUNIT_ASSERT ( eloop.run() == 0 );  // Run event loop
  signal(SIGALRM, SIG_DFL);
  signal_handler = [] (int) { };  // Do nothing
}

//----------------------------------------------------------------------
void EventloopMonitorTest::TimerMonitorTest()
{
  finalcut::EventLoop eloop{};
  auto eloop_ptr = &eloop;
  finalcut::TimerMonitor timer_monitor{&eloop};
  const finalcut::FString& timer_monitor_classname = timer_monitor.getClassName();
  CPPUNIT_ASSERT ( timer_monitor_classname == "TimerMonitor" );
  int num{0};
  auto callback_handler = [eloop_ptr, &num] (const finalcut::Monitor*, short)
  {
    num++;
    std::cout << "TimerMonitor callback handle (" << num << ")\n";

    if ( num == 3 )
      eloop_ptr->leave();
  };
  timer_monitor.init (callback_handler, nullptr);
  timer_monitor.setInterval ( std::chrono::nanoseconds{ 500'000'000 }
                            , std::chrono::nanoseconds{ 1'000'000'000 } );
  std::cout << "\n";
  timer_monitor.resume();
  auto start = high_resolution_clock::now();
  CPPUNIT_ASSERT ( eloop.run() == 0 );  // Run event loop
  auto end = high_resolution_clock::now();
  auto duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( num == 3 );
  CPPUNIT_ASSERT ( duration_ms >= 2500 );
  CPPUNIT_ASSERT ( duration_ms < 2510 );

  timer_monitor.setInterval ( std::chrono::nanoseconds{ 100'000'000 }
                            , std::chrono::nanoseconds{ 100'000'000 } );
  num = 0;
  start = high_resolution_clock::now();
  CPPUNIT_ASSERT ( eloop.run() == 0 );  // Run event loop
  end = high_resolution_clock::now();
  duration_ms = int(duration_cast<milliseconds>(end - start).count());
  CPPUNIT_ASSERT ( num == 3 );
  CPPUNIT_ASSERT ( duration_ms >= 300 );
  CPPUNIT_ASSERT ( duration_ms < 310 );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::BackendMonitorTest()
{
  finalcut::EventLoop eloop{};
  finalcut::BackendMonitor backend_monitor{&eloop};
  const finalcut::FString& backend_monitor_classname = backend_monitor.getClassName();
  CPPUNIT_ASSERT ( backend_monitor_classname == "BackendMonitor" );

  test::StringParser string_parser{&eloop};
  string_parser.setString("59,7,99,34,71,38,24,56,4");
  std::cout << "\n";
  CPPUNIT_ASSERT ( string_parser.getQueue().empty() );
  // The queue receives data and sends a backend event
  string_parser.parseString();  // Create a backend event in the self-pipe
  CPPUNIT_ASSERT ( ! string_parser.getQueue().empty() );
  CPPUNIT_ASSERT ( string_parser.getQueue().size() == 9 );
  CPPUNIT_ASSERT ( eloop.run() == 0 );  // Run event loop
  CPPUNIT_ASSERT ( string_parser.getQueue().empty() );
}

//----------------------------------------------------------------------
void EventloopMonitorTest::exceptionTest()
{
  CPPUNIT_ASSERT_THROW ( getException(), finalcut::monitor_error );
  CPPUNIT_ASSERT_NO_THROW ( getNoException() );

  // Bad file descriptor
  auto max_fd = int(sysconf(_SC_OPEN_MAX));
  CPPUNIT_ASSERT_THROW ( finalcut::drainPipe(max_fd), std::system_error );
  CPPUNIT_ASSERT_THROW ( finalcut::drainPipe(-1), std::system_error);

  // Monitor
  //---------------

  finalcut::EventLoop eloop{};
  CPPUNIT_ASSERT_THROW ( finalcut::Monitor(nullptr), finalcut::monitor_error );
  CPPUNIT_ASSERT_NO_THROW ( finalcut::Monitor monitor(&eloop) );
  finalcut::Monitor monitor1(&eloop);
  // Call the constructor from monitor1 again.
  CPPUNIT_ASSERT_THROW ( new (&monitor1) finalcut::Monitor(&eloop)
                       , std::exception );

  // Signal monitor
  //---------------

  finalcut::SignalMonitor signal_monitor1{&eloop};
  auto callback_handler = [] (finalcut::Monitor*, short) { };

  // SIGALRM used
  CPPUNIT_ASSERT_THROW ( signal_monitor1.init(SIGALRM, callback_handler, nullptr)
                       , std::invalid_argument );

  // No pipe could be established
  std::unique_ptr<finalcut::FSystem> fsys = std::make_unique<test::FSystemTest>();
  finalcut::FSystem::getInstance().swap(fsys);
  auto fsys_ptr = static_cast<test::FSystemTest*>(finalcut::FSystem::getInstance().get());
  fsys_ptr->setPipeReturnValue(-1);
  std::cout << "\n";
  CPPUNIT_ASSERT_THROW ( signal_monitor1.init(SIGTERM, callback_handler, nullptr)
                       , finalcut::monitor_error );
  fsys_ptr->setPipeReturnValue(0);

  // Double monitor instance for one signal
  CPPUNIT_ASSERT_NO_THROW ( signal_monitor1.init(SIGTERM, callback_handler, nullptr) );
  finalcut::SignalMonitor signal_monitor2{&eloop};
  CPPUNIT_ASSERT_THROW ( signal_monitor2.init(SIGTERM, callback_handler, nullptr)
                       , std::invalid_argument );
  CPPUNIT_ASSERT_NO_THROW ( signal_monitor2.init(SIGABRT, callback_handler, nullptr) );

  // Already initialised
  CPPUNIT_ASSERT_THROW ( signal_monitor1.init(SIGINT, callback_handler, nullptr)
                       , finalcut::monitor_error );

  // Sigaction error
#if defined(NSIG) && (NSIG > 0)
    static constexpr int NUMBER_OF_SIGNALS{NSIG};
#elif defined(_NSIG) && (_NSIG > 0)
    static constexpr int NUMBER_OF_SIGNALS{_NSIG};
#else
    static constexpr int NUMBER_OF_SIGNALS{65};  // Fallback value
#endif
  fsys_ptr->setSigactionReturnValue(-1);
  finalcut::SignalMonitor signal_monitor3{&eloop};
  CPPUNIT_ASSERT_THROW ( signal_monitor3.init(SIGHUP, callback_handler, nullptr)
                       , std::system_error );
  fsys_ptr->setSigactionReturnValue(0);
  CPPUNIT_ASSERT_THROW ( signal_monitor3.init(0, callback_handler, nullptr)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_THROW ( signal_monitor3.init(NUMBER_OF_SIGNALS, callback_handler, nullptr)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_THROW ( signal_monitor3.init(NUMBER_OF_SIGNALS, nullptr, nullptr)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_NO_THROW ( signal_monitor3.init(SIGHUP, callback_handler, nullptr) );

  // Posix timer monitor
  //--------------------

  PosixTimer_protected posix_timer_monitor{&eloop};

  // This (sigaction error) test only works on the initial call
  // before the sig_alrm_handler_installer is set.
  // This is simulated by calling ReinstallSigAlrmHandler()
  fsys_ptr->setSigactionReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.ReinstallSigAlrmHandler()
                       , std::system_error );
  fsys_ptr->setSigactionReturnValue(0);

  // No pipe could be established
  fsys_ptr->setPipeReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );
  fsys_ptr->setPipeReturnValue(0);

  // Posix timer cannot be created
  fsys_ptr->setTimerCreateReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );
  fsys_ptr->setTimerCreateReturnValue(0);

  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.init(nullptr, nullptr)
                       , finalcut::monitor_error );

  CPPUNIT_ASSERT_NO_THROW ( posix_timer_monitor.init(callback_handler, nullptr) );

  // Already initialised
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );

  // Timer interval cannot be set
  fsys_ptr->setTimerSettimeReturnValue(-1);
  auto t1 = std::chrono::nanoseconds{ 500'000'000 };
  auto t2 = std::chrono::nanoseconds{ 1'000'000'000 };
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.setInterval(t1, t2)
                       , std::system_error );
  fsys_ptr->setTimerSettimeReturnValue(0);
  CPPUNIT_ASSERT_NO_THROW ( posix_timer_monitor.setInterval(t1, t2) );

  auto t3 = std::chrono::nanoseconds{ 0 };
  auto t4 = std::chrono::nanoseconds{ -100 };
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.setInterval(t3, t3)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.setInterval(t4, t3)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.setInterval(t3, t4)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_NO_THROW ( posix_timer_monitor.setInterval(t1, t3) );
  CPPUNIT_ASSERT_NO_THROW ( posix_timer_monitor.setInterval(t3, t1) );

  posix_timer_monitor.resume();
  CPPUNIT_ASSERT_THROW ( posix_timer_monitor.p_trigger(555)
                       , std::system_error );

  // Kqueue timer monitor
  //---------------------

  fsys_ptr->setKqueueReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( finalcut::KqueueTimer{&eloop}
                       , std::system_error );
  fsys_ptr->setKqueueReturnValue(0);
  CPPUNIT_ASSERT_NO_THROW ( finalcut::KqueueTimer{&eloop} );

  KqueueTimer_protected kqueue_timer_monitor{&eloop};
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.init(nullptr, nullptr)
                       , finalcut::monitor_error );

  finalcut::useAlternativeKqueueTimerID() = true;
  finalcut::alternativeKqueueTimerID() = -999;
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.init(callback_handler, nullptr)
                       , std::invalid_argument );

  finalcut::useAlternativeKqueueTimerID() = false;
  CPPUNIT_ASSERT_NO_THROW ( kqueue_timer_monitor.init(callback_handler, nullptr) );

  // Already initialised
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );

  // Event cannot be registered
  fsys_ptr->setKeventReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.setInterval(t1, t2)
                       , finalcut::monitor_error );
  fsys_ptr->setKeventReturnValue(0);
  CPPUNIT_ASSERT_NO_THROW ( kqueue_timer_monitor.setInterval(t1, t2) );
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.setInterval(t3, t3)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.setInterval(t4, t3)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.setInterval(t3, t4)
                       , finalcut::monitor_error );
  CPPUNIT_ASSERT_NO_THROW ( kqueue_timer_monitor.setInterval(t1, t3) );
  CPPUNIT_ASSERT_NO_THROW ( kqueue_timer_monitor.setInterval(t3, t1) );

  // First timer event - cannot register event in kqueue
  kqueue_timer_monitor.resume();
  fsys_ptr->setKeventReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( kqueue_timer_monitor.p_trigger(5)
                       , finalcut::monitor_error );

  fsys_ptr->setKeventReturnValue(0);
  CPPUNIT_ASSERT_NO_THROW ( kqueue_timer_monitor.p_trigger(5) );

  // Backend monitor
  //----------------

  finalcut::BackendMonitor backend_monitor{&eloop};

  // No pipe could be established
  fsys_ptr->setPipeReturnValue(-1);
  CPPUNIT_ASSERT_THROW ( backend_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );
  fsys_ptr->setPipeReturnValue(0);

  CPPUNIT_ASSERT_NO_THROW ( backend_monitor.init(callback_handler, nullptr) );
  CPPUNIT_ASSERT_THROW ( backend_monitor.init(nullptr, nullptr)
                       , finalcut::monitor_error );

  // Already initialised
  CPPUNIT_ASSERT_THROW ( backend_monitor.init(callback_handler, nullptr)
                       , finalcut::monitor_error );

  // Switch back to the default FSystem implementation
  finalcut::FSystem::getInstance().swap(fsys);
  posix_timer_monitor.ReinstallSigAlrmHandler();
}

//----------------------------------------------------------------------
void EventloopMonitorTest::keyboardInput (std::string&& s)
{
  // Simulates keystrokes

  const auto input = std::move(s);
  const char EOT = 0x04;  // End of Transmission
  const auto stdin_no = finalcut::FTermios::getStdIn();

  fflush(stdout);
  std::string::const_iterator iter = input.cbegin();

  while ( iter != input.cend() )
  {
    char c = *iter;

    if ( ::ioctl (stdin_no, TIOCSTI, &c) < 0 )
      break;

    ++iter;
  }

  if ( ::ioctl (stdin_no, TIOCSTI, &EOT) < 0 )
    return;

  fflush(stdin);
}

//----------------------------------------------------------------------
void EventloopMonitorTest::drainStdin()
{
  auto stdin_no = finalcut::FTermios::getStdIn();
  auto stdin_no2 = dup(stdin_no);

  if ( stdin_no2 < 0 )
    return;

  if ( tcdrain(stdin_no2) < 0 )
  {
    ::close(stdin_no2);
    return;
  }

  if ( tcflush(stdin_no2, TCIFLUSH) < 0 )
  {
    ::close(stdin_no2);
    return;
  }

  if ( ::close(stdin_no2) < 0 )
    return;
}

//----------------------------------------------------------------------
void EventloopMonitorTest::enableFakingInput()
{
  //--------------------------------------------------------------------
  // Note: The dev.tty.legacy_tiocsti sysctl variable must be set
  //       to true to perform the TIOCSTI (faking input) operation
  //       in Linux 6.2.0 or later.
  //--------------------------------------------------------------------

  static const auto& fsystem = finalcut::FSystem::getInstance();

  // Check for root privileges
  if ( fsystem->getuid() != 0 )
    return;

  // Open the sysctl variable "dev.tty.legacy_tiocsti"
  int fd = ::open("/proc/sys/dev/tty/legacy_tiocsti", O_WRONLY);

  if ( fd < 0 )  // Cannot open file descriptor
    return;

  // Set dev.tty.legacy_tiocsti to true
  if ( dprintf(fd, "%d", 1) < 1 )
    std::cerr << "-> Unable to modify dev.tty.legacy_tiocsti\n";

  if ( ::close(fd) < 0 )
    std::cerr << "-> Cannot close file descriptor\n";
}

// Put the test suite in the registry
CPPUNIT_TEST_SUITE_REGISTRATION (EventloopMonitorTest);

// The general unit test main part
#include <main-test.inc>
