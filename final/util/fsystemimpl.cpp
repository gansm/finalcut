/***********************************************************************
* fsystemimpl.cpp - FSystem implementation                             *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2025 Markus Gans                                      *
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

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__OpenBSD__)
  #define USE_KQUEUE_TIMER
#endif

#if defined(USE_KQUEUE_TIMER)
  #include <unistd.h>
  #include <sys/event.h>
  #include <sys/types.h>
  #include <sys/time.h>
#endif

#if defined(__CYGWIN__)
  #include "final/fconfig.h"  // need for getpwuid_r and realpath
#endif

#include <csignal>

#include "final/util/fsystemimpl.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class FSystemImpl
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FSystemImpl::~FSystemImpl() noexcept = default;  // destructor


// public methods of FSystemImpl
//----------------------------------------------------------------------

auto FSystemImpl::sigaction ( int signum
                            , const struct sigaction* act
                            , struct sigaction* oldact ) noexcept -> int
{
  return ::sigaction (signum, act, oldact);
}

//----------------------------------------------------------------------
#if defined(USE_POSIX_TIMER)

auto FSystemImpl::timer_create ( clockid_t clockid
                               , struct sigevent* sevp
                               , timer_t* timerid ) noexcept -> int
{
  return ::timer_create (clockid, sevp, timerid);
}

#else

auto FSystemImpl::timer_create ( clockid_t
                               , struct sigevent*
                               , timer_t* ) noexcept -> int
{
  return 0;
}

#endif

//----------------------------------------------------------------------
#if defined(USE_POSIX_TIMER)

auto FSystemImpl::timer_settime ( timer_t timer_id, int flags
                                , const struct itimerspec* new_value
                                , struct itimerspec* old_value ) noexcept -> int
{
  return ::timer_settime(timer_id, flags, new_value, old_value);
}

#else

auto FSystemImpl::timer_settime ( timer_t, int
                                , const struct itimerspec*
                                , struct itimerspec* ) noexcept -> int
{
  return 0;
}

#endif

//----------------------------------------------------------------------
#if defined(USE_POSIX_TIMER)

auto FSystemImpl::timer_delete (timer_t timer_id) noexcept -> int
{
  return ::timer_delete (timer_id);
}

#else

auto FSystemImpl::timer_delete (timer_t) noexcept -> int
{
  return 0;
}

#endif

//----------------------------------------------------------------------

#if defined(USE_KQUEUE_TIMER)

auto FSystemImpl::kqueue() noexcept -> int
{
  return ::kqueue();
}

#else

auto FSystemImpl::kqueue() noexcept -> int
{
  return 0;
}

#endif

//----------------------------------------------------------------------
#if defined(USE_KQUEUE_TIMER)

auto FSystemImpl::kevent ( int kq, const struct kevent* changelist
                         , int nchanges, struct kevent* eventlist
                         , int nevents, const struct timespec* timeout) noexcept -> int
{
  return ::kevent (kq, changelist, nchanges, eventlist, nevents, timeout);
}

#else

auto FSystemImpl::kevent ( int, const struct kevent*
                         , int, struct kevent*
                         , int, const struct timespec* ) noexcept -> int
{
  return 0;
}

#endif

//----------------------------------------------------------------------
auto FSystemImpl::getpwuid_r ( uid_t uid, struct passwd* pwd
                             , char* buf, size_t buflen
                             , struct passwd** result ) noexcept -> int
{
  return ::getpwuid_r (uid, pwd, buf, buflen, result);
}

//----------------------------------------------------------------------
auto FSystemImpl::realpath (const char* path, char* resolved_path) noexcept -> char*
{
  return ::realpath(path, resolved_path);
}

}  // namespace finalcut

