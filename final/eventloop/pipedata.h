/***********************************************************************
* pipedata.h - Provides a pipe array to hold the file descriptors      *
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
    // Constructor
    PipeData() = default;

    explicit PipeData (int read_fd, int write_fd)
      : pipe_fd{{read_fd, write_fd}}
    { }

    // Accessors
    auto getClassName() const -> FString;
    auto getArrayData() const -> const int*;
    auto getArrayData() -> int*;
    auto getReadFd() const -> int;
    auto getWriteFd() const -> int;

  private:
    // Enumeration
    enum class Array
    {
      Read  = 0,  // Read end of pipe
      Write = 1,  // Write end of pipe
      Size  = 2   // Size of pipe array
    };

    // Using-declaration
    using ArrayT = std::underlying_type_t<Array>;

    // Data member
    std::array<int, static_cast<ArrayT>(Array::Size)> pipe_fd{};
};

//----------------------------------------------------------------------
inline auto PipeData::getArrayData() const -> const int*
{
  return pipe_fd.data();
}

//----------------------------------------------------------------------
inline auto PipeData::getArrayData() -> int*
{
  return pipe_fd.data();
}

//----------------------------------------------------------------------
inline auto PipeData::getReadFd() const -> int
{
  return pipe_fd[static_cast<ArrayT>(Array::Read)];
}

//----------------------------------------------------------------------
inline auto PipeData::getWriteFd() const -> int
{
  return pipe_fd[static_cast<ArrayT>(Array::Write)];
}

}  // namespace finalcut

#endif  // PIPEDATA_H
