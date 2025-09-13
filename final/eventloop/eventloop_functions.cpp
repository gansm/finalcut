/***********************************************************************
* eventloop_functions.cpp - Event loop helper functions                *
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

#include <fcntl.h>
#include <unistd.h>

#include <cerrno>
#include <cstddef>
#include <system_error>
#include <thread>
		
#include "final/eventloop/eventloop_functions.h"
#include "final/ftypes.h"

namespace finalcut
{

// Using-declaration
using PipeBuffer = std::uint64_t;
using ByteCount = std::size_t;

// Constants
namespace
{
  static constexpr int MAX_DRAIN_ATTEMPTS = 5;  // Prevent infinite loops
  static constexpr std::size_t PIPE_BUFFER_SIZE = sizeof(PipeBuffer);
  static constexpr std::chrono::milliseconds RETRY_DELAY{1};
}

// Function forward declarations
auto isValidFileDescriptor (FileDescriptor) noexcept -> bool;
auto readFromPipe (FileDescriptor, PipeBuffer&, ByteCount) -> ByteCount;


// Event loop non-member functions
//----------------------------------------------------------------------
void drainPipe (FileDescriptor fd)
{
  // Draining the pipe to reset the signaling for poll()

  if ( ! isValidFileDescriptor(fd) )
  {
    const std::error_code err_code{EBADF, std::system_category()};
    const std::system_error sys_err{err_code, "Invalid file descriptor"};
    throw sys_err;
  }

  PipeBuffer buffer{0};
  ByteCount bytes_read{0};
  int attempts{0};

  // Ensure that the correct number of bytes are read from the pipe
  while ( bytes_read < PIPE_BUFFER_SIZE
       && attempts < MAX_DRAIN_ATTEMPTS )
  {
     try
     {
       bytes_read += readFromPipe (fd, buffer, PIPE_BUFFER_SIZE - bytes_read);
       attempts = 0;  // Reset retry count on successful read
     }
     catch (const std::system_error& e)
     {
       if ( e.code().value() == EAGAIN
         || e.code().value() == EWOULDBLOCK )
       {
         attempts++;
         std::this_thread::sleep_for(RETRY_DELAY);
         continue;
       }

       throw; // Re-throw other errors
     }
  }
}

//----------------------------------------------------------------------
inline auto isValidFileDescriptor (FileDescriptor fd) noexcept -> bool
{
  return fd >= 0 && ::fcntl(fd, F_GETFD) != -1;
}

//----------------------------------------------------------------------
inline auto readFromPipe ( FileDescriptor fd
                         , PipeBuffer& buffer
                         , ByteCount bytes_to_read) -> ByteCount
{
  const auto current_bytes_read = ::read(fd, &buffer, bytes_to_read);

  if ( current_bytes_read < 0 )  // Underflow safe for all negative numbers
  {
    const int error{errno};
    const std::error_code err_code{error, std::generic_category()};
    const std::system_error sys_err{err_code, strerror(error)};
    throw sys_err;
  }

  return static_cast<ByteCount>(current_bytes_read);
}

}  // namespace finalcut
