/***********************************************************************
* fvterm_check.h - FVTerm test and debugging functions                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022 Markus Gans                                           *
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

#ifndef FVTERM_CHECK_H
#define FVTERM_CHECK_H

#include <final/final.h>

namespace test
{

// Data types
//----------------------------------------------------------------------
template <typename T, typename N = std::size_t>
struct FRepeat
{
  N repeat{0U};
  T data{};
};

using RepeatFChar = FRepeat<finalcut::FChar>;
using RepeatFCharVector = std::vector<RepeatFChar>;
using RepeatFCharLine = FRepeat<RepeatFCharVector>;
using RepeatFCharLineVector = std::vector<RepeatFCharLine>;

template <typename T>
using enable_if_FChar_t =
    std::enable_if_t< ! std::is_same<T, RepeatFCharLineVector>::value
                   && ! std::is_same<T, RepeatFCharLine>::value
                   && ! std::is_same<T, RepeatFCharVector>::value
                   && ! std::is_same<T, RepeatFChar>::value
                   && std::is_same<T, finalcut::FChar>::value
                   && std::is_class<T>::value
                   , std::nullptr_t >;


// Function prototypes
//----------------------------------------------------------------------
void showFCharData (const finalcut::FChar&);
auto getAreaSize (finalcut::FVTerm::FTermArea*) -> std::size_t;
auto isAreaEqual (finalcut::FVTerm::FTermArea*, finalcut::FVTerm::FTermArea*) -> bool;
auto isFCharEqual (const finalcut::FChar&, const finalcut::FChar&) -> bool;
template < typename CharT
         , enable_if_FChar_t<CharT> = nullptr >
void printOnArea (finalcut::FVTerm::FTermArea*, const CharT&);
void printOnArea (finalcut::FVTerm::FTermArea*, const RepeatFChar&);
void printOnArea (finalcut::FVTerm::FTermArea*, const RepeatFCharVector&);
void printOnArea (finalcut::FVTerm::FTermArea*, const RepeatFCharLine&);
void printOnArea (finalcut::FVTerm::FTermArea*, const RepeatFCharLineVector&);
void printArea (finalcut::FVTerm::FTermArea*);
void moveArea (finalcut::FVTerm::FTermArea*, const finalcut::FPoint&);


// Functions
//----------------------------------------------------------------------
void showFCharData (const finalcut::FChar& fchar)
{
  std::wcout << L"FChar data\n" << std::boolalpha;
  std::wcout << L"                         ch: '" <<  fchar.ch.data();
  std::wcout << L"' {" << uInt32(fchar.ch[0]) << L", " <<
                          uInt32(fchar.ch[1]) << L", " <<
                          uInt32(fchar.ch[2]) << L", " <<
                          uInt32(fchar.ch[3]) << L", " <<
                          uInt32(fchar.ch[4]) << L"}\n";
  std::wcout << L"               encoded_char: '" <<  fchar.encoded_char.data();
  std::wcout << L"' {" << uInt32(fchar.encoded_char[0]) << L", " <<
                          uInt32(fchar.encoded_char[1]) << L", " <<
                          uInt32(fchar.encoded_char[2]) << L", " <<
                          uInt32(fchar.encoded_char[3]) << L", " <<
                          uInt32(fchar.encoded_char[4]) << L"}\n";
  std::wcout << L"                   fg_color: " << int(fchar.fg_color) << L'\n';
  std::wcout << L"                   bg_color: " << int(fchar.bg_color) << L'\n';
  std::wcout << L"                    attr[0]: " << int(fchar.attr.byte[0]) << L'\n';
  std::wcout << L"              attr.bit.bold: " << bool(fchar.attr.bit.bold) << L'\n';
  std::wcout << L"               attr.bit.dim: " << bool(fchar.attr.bit.dim) << L'\n';
  std::wcout << L"            attr.bit.italic: " << bool(fchar.attr.bit.italic) << L'\n';
  std::wcout << L"         attr.bit.underline: " << bool(fchar.attr.bit.underline) << L'\n';
  std::wcout << L"             attr.bit.blink: " << bool(fchar.attr.bit.blink) << L'\n';
  std::wcout << L"           attr.bit.reverse: " << bool(fchar.attr.bit.reverse) << L'\n';
  std::wcout << L"          attr.bit.standout: " << bool(fchar.attr.bit.standout) << L'\n';
  std::wcout << L"         attr.bit.invisible: " << bool(fchar.attr.bit.invisible) << L'\n';
  std::wcout << L"                    attr[1]: " << int(fchar.attr.byte[1]) << L'\n';
  std::wcout << L"           attr.bit.protect: " << bool(fchar.attr.bit.protect) << L'\n';
  std::wcout << L"       attr.bit.crossed_out: " << bool(fchar.attr.bit.crossed_out) << L'\n';
  std::wcout << L"     attr.bit.dbl_underline: " << bool(fchar.attr.bit.dbl_underline) << L'\n';
  std::wcout << L"       attr.bit.alt_charset: " << bool(fchar.attr.bit.alt_charset) << L'\n';
  std::wcout << L"        attr.bit.pc_charset: " << bool(fchar.attr.bit.pc_charset) << L'\n';
  std::wcout << L"       attr.bit.transparent: " << bool(fchar.attr.bit.transparent) << L'\n';
  std::wcout << L"     attr.bit.color_overlay: " << bool(fchar.attr.bit.color_overlay) << L'\n';
  std::wcout << L"attr.bit.inherit_background: " << bool(fchar.attr.bit.inherit_background) << L'\n';
  std::wcout << L"                    attr[2]: " << int(fchar.attr.byte[2]) << L'\n';
  std::wcout << L"        attr.bit.no_changes: " << bool(fchar.attr.bit.no_changes) << L'\n';
  std::wcout << L"           attr.bit.printed: " << bool(fchar.attr.bit.printed) << L'\n';
  std::wcout << L" attr.bit.fullwidth_padding: " << bool(fchar.attr.bit.fullwidth_padding) << L'\n';
  std::wcout << L"        attr.bit.char_width: " << int(fchar.attr.bit.char_width) << L'\n';
  std::wcout << L"                    attr[3]: " << int(fchar.attr.byte[3]) << L'\n'
             << std::noboolalpha;
}

//----------------------------------------------------------------------
auto getAreaSize (finalcut::FVTerm::FTermArea* area) -> std::size_t
{
  const auto full_width = std::size_t(area->width) + std::size_t(area->right_shadow);
  const auto full_height = std::size_t(area->height) + std::size_t(area->bottom_shadow);
  return full_width * full_height;
}

//----------------------------------------------------------------------
auto isAreaEqual ( finalcut::FVTerm::FTermArea* area1
                 , finalcut::FVTerm::FTermArea* area2 ) -> bool
{
  auto size1 = getAreaSize(area1);
  auto size2 = getAreaSize(area2);

  if ( size1 != size2
    || area1->width != area2->width
    || area1->right_shadow != area2->right_shadow
    || area1->height != area2->height
    || area1->bottom_shadow != area2->bottom_shadow )
    return false;

  for (std::size_t i{0U}; i < size1; i++)
  {
    if ( ! isFCharEqual (area1->data[i], area2->data[i]) )
    {
      std::wcout << L"differ: char " << i << L" '"
                 << area1->data[i].ch[0] << L"' != '"
                 << area2->data[i].ch[0] << L"'\n";
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------
auto isFCharEqual ( const finalcut::FChar& lhs
                  , const finalcut::FChar& rhs ) -> bool
{
  return finalcut::isFUnicodeEqual(lhs.ch, rhs.ch)
      && finalcut::isFUnicodeEqual(lhs.encoded_char, rhs.encoded_char)
      && lhs.fg_color     == rhs.fg_color
      && lhs.bg_color     == rhs.bg_color
      && lhs.attr.byte[0] == rhs.attr.byte[0]
      && lhs.attr.byte[1] == rhs.attr.byte[1]
      && lhs.attr.byte[2] == rhs.attr.byte[2]
      && lhs.attr.byte[3] == rhs.attr.byte[3];
}

//----------------------------------------------------------------------
template < typename CharT
         , enable_if_FChar_t<CharT> >
void printOnArea ( finalcut::FVTerm::FTermArea* area
                 , const CharT& fchar )
{
  if ( area->cursor_x < 1 )
    area->cursor_x = 1;

  if ( area->cursor_y < 1 )
    area->cursor_y = 1;

  int ax = area->cursor_x - 1;
  int ay = area->cursor_y - 1;
  const int line_length = area->width + area->right_shadow;
  const int line_height = area->height + area->bottom_shadow;
  const int size = line_length * line_height;

  if ( ay * line_length + ax > size )
  {
    auto tmp = ay * line_length + ax % size;
    ax = tmp % line_length;
    ay = tmp / line_height;
    area->cursor_x = ax + 1;
    area->cursor_y = ay + 1;
  }

  auto& ac = area->data[ay * line_length + ax];  // area character
  std::memcpy (&ac, &fchar, sizeof(ac));  // copy character to area
  area->cursor_x = ((ax + 1) % line_length) + 1;
  area->cursor_y = ((ax + 1) / line_length) + area->cursor_y;

  if ( area->cursor_y > line_height )
    area->cursor_y = ((area->cursor_y - 1) % line_height) + 1;
}

//----------------------------------------------------------------------
void printOnArea ( finalcut::FVTerm::FTermArea* area
                 , const RepeatFChar& rep_fchar )
{
  for (std::size_t i{0U}; i < rep_fchar.repeat; i++)
  {
    printOnArea (area, rep_fchar.data);
  }
}

//----------------------------------------------------------------------
void printOnArea ( finalcut::FVTerm::FTermArea* area
                 , const RepeatFCharVector& rep_fchar_vec )
{
  for (const auto& rep_fchar : rep_fchar_vec)
  {
    printOnArea (area, rep_fchar);
  }
}

//----------------------------------------------------------------------
void printOnArea ( finalcut::FVTerm::FTermArea* area
                 , const RepeatFCharLine& rep_fchar_line )
{
  for (std::size_t i{0U}; i < rep_fchar_line.repeat; i++)
  {
    printOnArea (area, rep_fchar_line.data);
  }
}

//----------------------------------------------------------------------
void printOnArea ( finalcut::FVTerm::FTermArea* area
                 , const RepeatFCharLineVector& rep_fchar_line_vec )
{
  for (const auto& rep_fchar_line : rep_fchar_line_vec)
  {
    printOnArea (area, rep_fchar_line);
  }
}

//----------------------------------------------------------------------
void printArea ( finalcut::FVTerm::FTermArea* area )
{
  auto width = area->width + area->right_shadow;
  auto height = area->height + area->bottom_shadow;
  auto size = getAreaSize(area);
  std::wcout << L'┌' << std::wstring(width, L'─') << L"┐\n";

  for (std::size_t i{0U}; i < size; i++)
  {
    if ( area->data[i].attr.bit.fullwidth_padding )
      continue;

    auto col = (i + 1) % width ;
    auto line = (i + 1) / width;

    if ( col == 1 && line < std::size_t(height) )
      std::wcout << L"│";

    auto ch = area->data[i].ch;

    if ( ch[0] == L'\0' )
      ch[0] = L' ';

    std::wcout << ch.data();

    if ( col == 0  )
      std::wcout << L"│" << line << L"\n";
  }

  std::wcout << L'└' << std::wstring(width, L'─') << L"┘\n ";

  if ( width > 9 )
  {
    for (int i{1}; i <= width; i++)
      std::wcout << i / 10;

    std::wcout << L"\n ";
  }

  for (int i{1}; i <= width; i++)
    std::wcout << i % 10;

  std::wcout << L"\n (" << size << L" character) \n";
}

//----------------------------------------------------------------------
void moveArea ( finalcut::FVTerm::FTermArea* area
              , const finalcut::FPoint& pos )
{
  if ( ! area )
    return;

  area->offset_left = pos.getX();
  area->offset_top = pos.getY();
}

}  // namespace test

#endif  // FVTERM_CHECK_H
