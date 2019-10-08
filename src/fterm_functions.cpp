/***********************************************************************
* fterm_functions.cpp - FTerm helper functions                         *
*                                                                      *
* This file is part of the Final Cut widget toolkit                    *
*                                                                      *
* Copyright 2019 Markus Gans                                           *
*                                                                      *
* The Final Cut is free software; you can redistribute it and/or       *
* modify it under the terms of the GNU Lesser General Public License   *
* as published by the Free Software Foundation; either version 3 of    *
* the License, or (at your option) any later version.                  *
*                                                                      *
* The Final Cut is distributed in the hope that it will be useful,     *
* but WITHOUT ANY WARRANTY; without even the implied warranty of       *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
* GNU Lesser General Public License for more details.                  *
*                                                                      *
* You should have received a copy of the GNU Lesser General Public     *
* License along with this program.  If not, see                        *
* <http://www.gnu.org/licenses/>.                                      *
***********************************************************************/

#include "final/fcharmap.h"
#include "final/fterm.h"
#include "final/ftermbuffer.h"


namespace finalcut
{

// FTerm non-member functions
//----------------------------------------------------------------------
uInt env2uint (const char* env)
{
  FString str(getenv(env));

  if ( str.isEmpty() )
    return 0;

  try
  {
    return str.toUInt();
  }
  catch (const std::exception&)
  {
    return 0;
  }
}

//----------------------------------------------------------------------
wchar_t cp437_to_unicode (uChar c)
{
  constexpr std::size_t CP437 = 0;
  constexpr std::size_t UNICODE = 1;
  wchar_t ucs = c;

  for (std::size_t i{0}; i <= fc::lastCP437Item; i++)
  {
    if ( fc::cp437_ucs[i][CP437] == c )  // found
    {
      ucs = fc::cp437_ucs[i][UNICODE];
      break;
    }
  }

  return ucs;
}

//----------------------------------------------------------------------
uChar unicode_to_cp437 (wchar_t ucs)
{
  constexpr std::size_t CP437 = 0;
  constexpr std::size_t UNICODE = 1;
  uChar c{'?'};

  for (std::size_t i{0}; i <= fc::lastCP437Item; i++)
  {
    if ( fc::cp437_ucs[i][UNICODE] == ucs )  // found
    {
      c = uChar(fc::cp437_ucs[i][CP437]);
      break;
    }
  }

  return c;
}

//----------------------------------------------------------------------
FString getFullWidth (const FString& str)
{
  // Converts half-width to full-width characters

  FString s(str);
  constexpr std::size_t HALF = 0;
  constexpr std::size_t FULL = 1;

  for (auto&& c : s)
  {
    if ( c > L'\x20' && c < L'\x7f' )  // half-width ASCII
    {
      c += 0xfee0;
    }
    else
    {
      for (std::size_t i{0}; i <= fc::lastHalfWidthItem; i++)
      {
        if ( fc::halfWidth_fullWidth[i][HALF] == c )  // found
          c = fc::halfWidth_fullWidth[i][FULL];
      }
    }
  }

  return s;
}

//----------------------------------------------------------------------
FString getHalfWidth (const FString& str)
{
  // Converts full-width to half-width characters

  FString s(str);
  constexpr std::size_t HALF = 0;
  constexpr std::size_t FULL = 1;

  for (auto&& c : s)
  {
    if ( c > L'\xff00' && c < L'\xff5f' )  // full-width ASCII
    {
      c -= 0xfee0;
    }
    else
    {
      for (std::size_t i{0}; i <= fc::lastHalfWidthItem; i++)
      {
        if ( fc::halfWidth_fullWidth[i][FULL] == c )  // found
          c = fc::halfWidth_fullWidth[i][HALF];
      }
    }
  }

  return s;
}

//----------------------------------------------------------------------
FString getColumnSubString ( const FString& str
                           , std::size_t col_pos, std::size_t col_len )
{
  FString s(str);
  std::size_t col_first{1}, col_num{0}, first{1}, num{0};

  if ( col_len == 0 || s.isEmpty() )
    return FString(L"");

  if ( col_pos == 0 )
    col_pos = 1;

  for (auto&& ch : s)
  {
    std::size_t width = getColumnWidth(ch);

    if ( col_first < col_pos )
    {
      if ( col_first + width <= col_pos )
      {
        col_first += width;
        first++;
      }
      else
      {
        ch = fc::SingleLeftAngleQuotationMark;  // ‹
        num = col_num = 1;
        col_pos = col_first;
      }
    }
    else
    {
      if ( col_num + width <= col_len )
      {
        col_num += width;
        num++;
      }
      else if ( col_num < col_len )
      {
        ch = fc::SingleRightAngleQuotationMark;  // ›
        num++;
        break;
      }
    }
  }

  if ( col_first < col_pos )  // String length < col_pos
    return FString(L"");

  return s.mid(first, num);
}

//----------------------------------------------------------------------
std::size_t getLengthFromColumnWidth ( const FString& str
                                     , std::size_t col_len )
{
  std::size_t column_width{0}, length{0};

  for (auto&& ch : str)
  {
    if ( column_width < col_len )
    {
      column_width += getColumnWidth(ch);
      length++;
    }
  }

  return length;
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const FString& s, std::size_t pos)
{
  if ( s.isEmpty() )
    return 0;

  std::size_t column_width{0};
  auto length = s.getLength();

  if ( pos > length )
    pos = length;

  for (std::size_t i{0}; i < pos; i++)
    column_width += getColumnWidth(s[i]);

  return column_width;
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const FString& s)
{
  if ( s.isEmpty() )
    return 0;

  const wchar_t* str = s.wc_str();
  size_t len = std::wcslen(str);
  int column_width = wcswidth (str, len);
  return ( column_width == -1 ) ? 0 : std::size_t(column_width);
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const wchar_t wchar)
{
  int column_width{};

  if ( wchar >= fc::NF_rev_left_arrow2 && wchar <= fc::NF_check_mark )
    column_width = 1;
  else
    column_width = wcwidth(wchar);

  return ( column_width == -1 ) ? 0 : std::size_t(column_width);
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (FChar& term_char)
{
  std::size_t char_width = getColumnWidth(term_char.ch);

  if ( char_width == 2 && FTerm::getEncoding() != fc::UTF8 )
  {
    term_char.ch = '.';
    term_char.attr.bit.char_width = 1;
  }
  else
    term_char.attr.bit.char_width = char_width & 0x03;

  return char_width;
}

//----------------------------------------------------------------------
std::size_t getColumnWidth (const FTermBuffer& termbuffer)
{
  std::size_t column_width{0};

  for (auto&& tc : termbuffer)
    column_width += tc.attr.bit.char_width;

  return column_width;
}

}  // namespace finalcut
