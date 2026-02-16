/***********************************************************************
* fvtermbuffer.cpp - Buffer for virtual terminal strings               *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2017-2026 Markus Gans                                      *
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

#include <algorithm>
#include <string>
#include <vector>

#include "final/fc.h"
#include "final/ftypes.h"
#include "final/output/foutput.h"
#include "final/output/tty/ftermdata.h"
#include "final/util/fstring.h"
#include "final/vterm/fcolorpair.h"
#include "final/vterm/fstyle.h"
#include "final/vterm/fvtermbuffer.h"
#include "final/vterm/fvterm.h"

namespace finalcut
{
namespace internal
{

template<typename T>
constexpr auto createMask (T setter) noexcept -> uInt32
{
  FCharAttribute mask{};
  setter(mask);
  return FCharAttribute_to_uInt32(mask);
}

constexpr void setAttributeMask (FCharAttribute& attr) noexcept
{
  attr.bold = true;
  attr.dim = true;
  attr.italic = true;
  attr.underline = true;
  attr.blink = true;
  attr.reverse = true;
  attr.standout = true;
  attr.invisible = true;
  attr.protect = true;
  attr.crossed_out = true;
  attr.dbl_underline = true;
  attr.alt_charset = true;
  attr.pc_charset = true;
  attr.transparent = true;
  attr.color_overlay = true;
  attr.inherit_background = true;
}

struct var
{
  static constexpr auto attr_mask = createMask(setAttributeMask);
};

constexpr uInt32 var::attr_mask;

}  // namespace internal


//----------------------------------------------------------------------
// class FVTermBuffer
//----------------------------------------------------------------------

// constructors and destructor
//----------------------------------------------------------------------
FVTermBuffer::FVTermBuffer()  // constructor
{
  data.reserve(2048);  // Set initial data size to 2048 FChars
}

// public methods of FVTermBuffer
//----------------------------------------------------------------------
auto FVTermBuffer::toString() const -> FString
{
  std::wstring wide_string{};
  checkCapacity(wide_string, data.size());
  std::for_each ( data.cbegin()
                , data.cend()
                , [&wide_string] (const auto& fchar)
                  {
                    for (const auto& ch : fchar.ch)
                    {
                      if ( ch == L'\0' )
                        return;

                      wide_string.push_back(ch);
                    }
                  }
                );
  return {std::move(wide_string)};
}

//----------------------------------------------------------------------
auto FVTermBuffer::print (const FString& string) -> int
{
  checkCapacity(data, data.size() + string.getLength());
  UnicodeBoundary ucb{string.cbegin(), string.cend(), string.cbegin(), 0};

  for (const auto& ch : string)
  {
    auto width = getColumnWidth(ch);
    auto ctrl_char = std::iswcntrl(wint_t(ch));

    if ( width == 0 && ! ctrl_char )  // zero-width character
    {
      if ( ucb.iter == ucb.cbegin )
        ++ucb.cbegin;

      ++ucb.iter;
    }
    else if ( ucb.iter != ucb.cbegin )
      add(ucb);

    if ( ucb.iter == ucb.cbegin && (width > 0 || is7bit(ch)) )  // 1st char
      ++ucb.iter;

    if ( width > 0 )
      ucb.char_width += width;

    if ( ctrl_char )
      add(ucb);
  }

  if ( ucb.iter == ucb.cend )
    add(ucb);

  return int(string.getLength());
}

//----------------------------------------------------------------------
auto FVTermBuffer::print (wchar_t ch) -> int
{
  data.emplace_back();
  auto& nc = data.back();  // next character
  setAttribute(nc);
  nc.ch.unicode_data[0] = ch;
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
  nc.ch.unicode_data[1] = L'\0';
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
  const auto column_width = getColumnWidth(nc.ch.unicode_data[0]);
  addColumnWidth(nc, column_width);  // add column width
  return 1;
}

//----------------------------------------------------------------------
void FVTermBuffer::print (const FStyle& style) const
{
  FVTermAttribute::print(style);
}

//----------------------------------------------------------------------
void FVTermBuffer::print (const FColorPair& pair) const noexcept
{
  FVTermAttribute::setColor( pair.getForegroundColor()
                           , pair.getBackgroundColor() );
}


// private methods of FVTermBuffer
//----------------------------------------------------------------------
inline void FVTermBuffer::setAttribute (FChar& fchar) const noexcept
{
  static const auto& next_attribute = FVTermAttribute::getAttribute();
  fchar.color.data = next_attribute.color.data;
  fchar.attr.data = next_attribute.attr.data & internal::var::attr_mask;
}

//----------------------------------------------------------------------
void FVTermBuffer::add (UnicodeBoundary& ucb)
{
  static const auto& fterm_data = FTermData::getInstance();

  if ( ucb.cbegin == ucb.iter )
    return;

  data.emplace_back();
  auto& nc = data.back();  // next character
  setAttribute(nc);

  if ( ucb.char_width == 2
    && fterm_data.getTerminalEncoding() != Encoding::UTF8 )
  {
    nc.ch.unicode_data[0] = L'.';
#if defined(__clang__)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
    nc.ch.unicode_data[1] = L'\0';
#if defined(__clang__)
  #pragma clang diagnostic pop
#endif
    nc.attr.bit.char_width = 1;
  }
  else
  {
    const auto end = std::min(ucb.iter, ucb.cbegin + UNICODE_MAX);
    std::copy(ucb.cbegin, end, nc.ch.begin());
    nc.attr.bit.char_width = uInt8(ucb.char_width) & 0x03;
    const auto idx = std::size_t(end - ucb.cbegin);

    if ( idx < UNICODE_MAX )
      nc.ch[idx] = L'\0';
  }

  ucb.cbegin = ucb.iter;
  ucb.char_width = 0;  // reset char width
}

}  // namespace finalcut
