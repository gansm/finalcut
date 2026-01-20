/***********************************************************************
* pipedata.h - Provides a pipe array to hold the file descriptors      *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2023-2026 Markus Gans                                      *
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

/*  Standalone class
 *  ════════════════
 *
 * ▕▔▔▔▔▔▔▔▔▔▔▏
 * ▕ PipeData ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef PIPEDATA_H
#define PIPEDATA_H

#include <array>
#include <type_traits>
#include <stdexcept>

namespace finalcut
{

// class forward declaration
class FString;

//----------------------------------------------------------------------
// class PipeData
//----------------------------------------------------------------------

class PipeData final
{
  public:
    // Constant
    static constexpr int NO_FILE_DESCRIPTOR{-1};

    // Constructor
    PipeData() noexcept
      : pipe_fd{{NO_FILE_DESCRIPTOR, NO_FILE_DESCRIPTOR}}
    { }

    explicit PipeData (int read_fd, int write_fd) noexcept
      : pipe_fd{{read_fd, write_fd}}
    { }

    // Copy constructor
    PipeData (const PipeData&) = default;

    // Move constructor
    PipeData (PipeData&&) noexcept = default;

    // Destructor
    ~PipeData() = default;

    // Copy assignment operator
    auto operator = (const PipeData&) -> PipeData& = default;

    // Move assignment operator
    auto operator = (PipeData&&) noexcept -> PipeData& = default;

    // Accessors
    auto getClassName() const -> FString;
    auto getArrayData() const noexcept -> const int*;
    auto getArrayData() noexcept -> int*;
    auto getReadFd() const noexcept -> int;
    auto getWriteFd() const noexcept -> int;

    // Mutators
    void setReadFd (int) noexcept;
    void setWriteFd (int) noexcept;
    void setPipe (int, int) noexcept;
    void reset() noexcept;
    void swap (PipeData&) noexcept;

  private:
    // Enumeration
    enum class Array : unsigned char
    {
      Read  = 0,  // Read end of pipe
      Write = 1,  // Write end of pipe
      Size  = 2   // Size of pipe array
    };

    // Using-declaration
    using ArrayT = std::underlying_type_t<Array>;
    static constexpr auto PIPE_FD_COUNT = static_cast<ArrayT>(Array::Size);
    using PipeDataType = std::array<int, PIPE_FD_COUNT>;

    // Data member
    PipeDataType pipe_fd{};
};

//----------------------------------------------------------------------
inline auto PipeData::getArrayData() const noexcept -> const int*
{
  return pipe_fd.data();
}

//----------------------------------------------------------------------
inline auto PipeData::getArrayData() noexcept -> int*
{
  return pipe_fd.data();
}

//----------------------------------------------------------------------
inline auto PipeData::getReadFd() const noexcept -> int
{
  return pipe_fd[static_cast<ArrayT>(Array::Read)];
}

//----------------------------------------------------------------------
inline auto PipeData::getWriteFd() const noexcept -> int
{
  return pipe_fd[static_cast<ArrayT>(Array::Write)];
}
//----------------------------------------------------------------------
inline void PipeData::setReadFd(int fd) noexcept
{
    pipe_fd[static_cast<ArrayT>(Array::Read)] = fd;
}

//----------------------------------------------------------------------
inline void PipeData::setWriteFd(int fd) noexcept
{
    pipe_fd[static_cast<ArrayT>(Array::Write)] = fd;
}

//----------------------------------------------------------------------
inline void PipeData::setPipe(int read_fd, int write_fd) noexcept
{
    pipe_fd[static_cast<ArrayT>(Array::Read)] = read_fd;
    pipe_fd[static_cast<ArrayT>(Array::Write)] = write_fd;
}

//----------------------------------------------------------------------
inline void PipeData::reset() noexcept
{
  pipe_fd[static_cast<ArrayT>(Array::Read)] = NO_FILE_DESCRIPTOR;
  pipe_fd[static_cast<ArrayT>(Array::Write)] = NO_FILE_DESCRIPTOR;
}

//----------------------------------------------------------------------
inline void PipeData::swap (PipeData& other) noexcept
{ pipe_fd.swap(other.pipe_fd); }

}  // namespace finalcut

#endif  // PIPEDATA_H
