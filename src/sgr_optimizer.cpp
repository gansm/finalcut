/***********************************************************************
* sgr_optimizer.cpp - Combines SGR attributes                          *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2019-2020 Markus Gans                                      *
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

#include <cstring>
#include <vector>

#include "final/fc.h"
#include "final/sgr_optimizer.h"

namespace finalcut
{

//----------------------------------------------------------------------
// class SGRoptimizer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
SGRoptimizer::SGRoptimizer (attributebuffer& sequence)
  : seq{sequence}
{ }

//----------------------------------------------------------------------
SGRoptimizer::~SGRoptimizer()  // destructor
{ }


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

  const std::size_t len = std::strlen(seq);
  csi_parameter.clear();

  if ( len < 6 )
    return;

  std::size_t start{NOT_SET};
  bool esc{false};
  bool csi{false};

  // Find SGR parameter
  for (std::size_t i = 0; i < len; i++)
  {
    if ( csi )
    {
      if ( start == NOT_SET )
        start = i;

      if ( (seq[i] >= '0' && seq[i] <= '9') || seq[i] == ';' )
        continue;
      else if ( seq[i] == 'm')
      {
        csi_parameter.push_back({start, i});
      }

      esc = csi = false;
      start = NOT_SET;
    }

    // Other content
    if ( ! csi_parameter.empty() && i > csi_parameter.back().end + 2 )
      break;

    if ( esc && seq[i] == '[' )  // Esc [
      csi = true;

    if ( seq[i] == ESC[0] )  // Esc
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
  std::size_t read_pos{};
  std::size_t write_pos = first.end;

  if ( first.start == first.end )  // Esc [ m
  {
    seq[write_pos] = '0';
    write_pos++;
  }

  seq[write_pos] = ';';
  write_pos++;
  const auto& begin = csi_parameter.cbegin() + 1;
  const auto& end = csi_parameter.cend();

  for (auto&& p : std::vector<parameter>(begin, end))
  {
    count++;

    for (read_pos = p.start; read_pos <= p.end; read_pos++)
    {
      if ( seq[read_pos] == 'm' )
      {
        if ( p.start == p.end )  // Esc [ m
        {
          seq[write_pos] = '0';
          write_pos++;
        }

        if ( count == csi_parameter.size() )
          seq[write_pos] = 'm';
        else
          seq[write_pos] = ';';
      }
      else
        seq[write_pos] = seq[read_pos];

      write_pos++;
    }
  }

  while ( seq[write_pos] != '\0' )
  {
    seq[write_pos] = seq[read_pos];
    read_pos++;
    write_pos++;
  }
}

}  // namespace finalcut

