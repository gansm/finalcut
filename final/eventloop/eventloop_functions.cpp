/***********************************************************************
* eventloop_functions.cpp - Event loop helper functions                *
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

#include <unistd.h>

#include <cerrno>
#include <cstddef>
#include <system_error>
		
#include "final/eventloop/eventloop_functions.h"
#include "final/ftypes.h"

namespace finalcut
{

// Function forward declarations
std::size_t readFromPipe (int, uint64_t&, std::size_t);


// Event loop non-member functions
//----------------------------------------------------------------------
void drainPipe (int fd)
{
  // Draining the pipe to reset the signaling for poll()

  uint64_t buffer{0};
  static constexpr auto buffer_size = sizeof(buffer);
  std::size_t bytes_read{0};

  // Ensure that the correct number of bytes are read from the pipe
  while ( bytes_read < buffer_size )
  {
     bytes_read += readFromPipe (fd, buffer, buffer_size - bytes_read);
  }
}

//----------------------------------------------------------------------
inline std::size_t readFromPipe (int fd, uint64_t& buffer, std::size_t bytes_to_read)
{
  auto current_bytes_read = ::read(fd, &buffer, bytes_to_read);

  if ( current_bytes_read == -1 )
  {
    int error{errno};
    std::error_code err_code{error, std::generic_category()};
    std::system_error sys_err{err_code, strerror(error)};
    throw sys_err;
  }

  return static_cast<std::size_t>(current_bytes_read);
}

}  // namespace finalcut
