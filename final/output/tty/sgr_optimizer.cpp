/***********************************************************************
* sgr_optimizer.cpp - Combines SGR attributes                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2023 Markus Gans                                      *
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

#include <cctype>
#include <cstring>
#include <iostream>
#include <vector>

#include "final/fc.h"
#include "final/output/tty/sgr_optimizer.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class SGRoptimizer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
SGRoptimizer::SGRoptimizer (std::string& sequence)
  : seq{sequence}
{
  seq.reserve(ATTR_BUF_SIZE);
}


// public methods of SGRoptimizer
//----------------------------------------------------------------------
void SGRoptimizer::optimize()
{
  findParameter();
  combineParameter();
}


// private methods of SGRoptimizer
//----------------------------------------------------------------------
void SGRoptimizer::findParameter()
{
  // Find ANSI X3.64 terminal SGR (Select Graphic Rendition) strings

  csi_parameter.clear();

  if ( seq.length() < 6 )
    return;

  std::size_t start{NOT_SET};
  bool esc{false};
  bool csi{false};

  // Find SGR parameter
  for (std::size_t index{0}; index < seq.length(); ++index)
  {
    char ch = seq[index];

    if ( csi )
    {
      if ( start == NOT_SET )
        start = index;

      if ( std::isdigit(ch) || ch == ';' )
        continue;

      if ( ch == 'm')
        csi_parameter.push_back({start, index});

      esc = csi = false;
      start = NOT_SET;
    }

    // Other content
    if ( ! csi_parameter.empty() && index > csi_parameter.back().end + 2 )
      break;

    if ( esc && ch == '[' )  // Esc [
      csi = true;

    if ( ch == ESC[0] )  // Esc
      esc = true;
  }
}

//----------------------------------------------------------------------
void SGRoptimizer::combineParameter()
{
  // Combine SGR (Select Graphic Rendition) attributes

  if ( csi_parameter.size() < 2 )
    return;

  const auto& first = csi_parameter.front();
  std::size_t count = 1;
  std::size_t read_pos = 0;
  std::size_t write_pos = first.end;

  if ( first.start == first.end )  // Esc [ m
  {
    seq[write_pos] = '0';
    write_pos++;
  }

  seq[write_pos] = ';';
  write_pos++;
  auto iter = csi_parameter.cbegin() + 1;
  const auto end = csi_parameter.cend();

  while ( iter != end )
  {
    count++;

    // Copy SGR parameter values
    std::size_t param_size = iter->end - iter->start;
    std::memcpy(&seq[write_pos], &seq[iter->start], param_size);
    write_pos += param_size;
    read_pos = iter->end + 1;

    // Handle terminating SGR parameter
    if ( seq[iter->end] == 'm' )
      handleSGRterminating(iter, write_pos, count, csi_parameter.size());

    ++iter;
  }

  // Copy remaining characters in the sequence
  std::memcpy(&seq[write_pos], &seq[read_pos], seq.size() - read_pos);
  write_pos += seq.size() - read_pos;

  seq.resize(write_pos);
}

//----------------------------------------------------------------------
inline void SGRoptimizer::handleSGRterminating ( const std::vector<parameter>::const_iterator iter
                                               , std::size_t& write_pos
                                               , std::size_t count
                                               , std::size_t size )
{
  if ( iter->start == iter->end )  // Esc [ m
  {
    seq[write_pos] = '0';
    write_pos++;
  }

  seq[write_pos] = count != size ? ';' : 'm';
  write_pos++;
}

}  // namespace finalcut

