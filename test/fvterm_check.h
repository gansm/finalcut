/***********************************************************************
* fvterm_check.h - FVTerm test and debugging functions                 *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2026 Markus Gans                                      *
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
auto getRegionSize (finalcut::FVTerm::FTermRegion*) -> std::size_t;
auto isRegionEqual (finalcut::FVTerm::FTermRegion*, finalcut::FVTerm::FTermRegion*) -> bool;
auto isFCharEqual (const finalcut::FChar&, const finalcut::FChar&) -> bool;
template < typename FCharT
         , enable_if_FChar_t<FCharT> = nullptr >
void printOnRegion (finalcut::FVTerm::FTermRegion*, const FCharT&);
void printOnRegion (finalcut::FVTerm::FTermRegion*, const RepeatFChar&);
void printOnRegion (finalcut::FVTerm::FTermRegion*, const RepeatFCharVector&);
void printOnRegion (finalcut::FVTerm::FTermRegion*, const RepeatFCharLine&);
void printOnRegion (finalcut::FVTerm::FTermRegion*, const RepeatFCharLineVector&);
void printRegion (finalcut::FVTerm::FTermRegion*);
void moveRegion (finalcut::FVTerm::FTermRegion*, const finalcut::FPoint&);


// Functions
//----------------------------------------------------------------------
void showFCharData (const finalcut::FChar& fchar)
{
  std::wcout << L"FChar data\n" << std::boolalpha;
  std::wcout << L"                         ch: '" <<  fchar.ch.data();
  std::wcout << L"' {" << uInt32(fchar.ch.unicode_data[0]) << L", " <<
                          uInt32(fchar.ch.unicode_data[1]) << L", " <<
                          uInt32(fchar.ch.unicode_data[2]) << L", " <<
                          uInt32(fchar.ch.unicode_data[3]) << L", " <<
                          uInt32(fchar.ch.unicode_data[4]) << L"}\n";
  std::wcout << L"               encoded_char: '" <<  fchar.encoded_char.data();
  std::wcout << L"' {" << uInt32(fchar.encoded_char.unicode_data[0]) << L", " <<
                          uInt32(fchar.encoded_char.unicode_data[1]) << L", " <<
                          uInt32(fchar.encoded_char.unicode_data[2]) << L", " <<
                          uInt32(fchar.encoded_char.unicode_data[3]) << L", " <<
                          uInt32(fchar.encoded_char.unicode_data[4]) << L"}\n";
  std::wcout << L"              color.pair.fg: " << int(fchar.color.getFgColor()) << L'\n';
  std::wcout << L"              color.pair.bg: " << int(fchar.color.getBgColor()) << L'\n';
  std::wcout << L"                    attr[0]: " << int(fchar.attr.byte()->at(0)) << L'\n';
  std::wcout << L"              attr.bit.bold: " << bool(fchar.attr.bit()->bold) << L'\n';
  std::wcout << L"               attr.bit.dim: " << bool(fchar.attr.bit()->dim) << L'\n';
  std::wcout << L"            attr.bit.italic: " << bool(fchar.attr.bit()->italic) << L'\n';
  std::wcout << L"         attr.bit.underline: " << bool(fchar.attr.bit()->underline) << L'\n';
  std::wcout << L"             attr.bit.blink: " << bool(fchar.attr.bit()->blink) << L'\n';
  std::wcout << L"           attr.bit.reverse: " << bool(fchar.attr.bit()->reverse) << L'\n';
  std::wcout << L"          attr.bit.standout: " << bool(fchar.attr.bit()->standout) << L'\n';
  std::wcout << L"         attr.bit.invisible: " << bool(fchar.attr.bit()->invisible) << L'\n';
  std::wcout << L"                    attr[1]: " << int(fchar.attr.byte()->at(1)) << L'\n';
  std::wcout << L"           attr.bit.protect: " << bool(fchar.attr.bit()->protect) << L'\n';
  std::wcout << L"       attr.bit.crossed_out: " << bool(fchar.attr.bit()->crossed_out) << L'\n';
  std::wcout << L"     attr.bit.dbl_underline: " << bool(fchar.attr.bit()->dbl_underline) << L'\n';
  std::wcout << L"       attr.bit.alt_charset: " << bool(fchar.attr.bit()->alt_charset) << L'\n';
  std::wcout << L"        attr.bit.pc_charset: " << bool(fchar.attr.bit()->pc_charset) << L'\n';
  std::wcout << L"       attr.bit.transparent: " << bool(fchar.attr.bit()->transparent) << L'\n';
  std::wcout << L"     attr.bit.color_overlay: " << bool(fchar.attr.bit()->color_overlay) << L'\n';
  std::wcout << L"attr.bit.inherit_background: " << bool(fchar.attr.bit()->inherit_background) << L'\n';
  std::wcout << L"                    attr[2]: " << int(fchar.attr.byte()->at(2)) << L'\n';
  std::wcout << L"        attr.bit.no_changes: " << bool(fchar.attr.bit()->no_changes) << L'\n';
  std::wcout << L"           attr.bit.printed: " << bool(fchar.attr.bit()->printed) << L'\n';
  std::wcout << L" attr.bit.fullwidth_padding: " << bool(fchar.attr.bit()->fullwidth_padding) << L'\n';
  std::wcout << L"        attr.bit.char_width: " << int(fchar.attr.bit()->char_width) << L'\n';
  std::wcout << L"                    attr[3]: " << int(fchar.attr.byte()->at(3)) << L'\n'
             << std::noboolalpha;
}

//----------------------------------------------------------------------
auto getRegionSize (finalcut::FVTerm::FTermRegion* region) -> std::size_t
{
  if ( ! region )
    return 0;

  const auto full_width = std::size_t(region->size.width) + std::size_t(region->shadow.width);
  const auto full_height = std::size_t(region->size.height) + std::size_t(region->shadow.height);
  return full_width * full_height;
}

//----------------------------------------------------------------------
auto isRegionEqual ( finalcut::FVTerm::FTermRegion* region1
                   , finalcut::FVTerm::FTermRegion* region2 ) -> bool
{
  if ( ! region1 || ! region2 )
    return false;

  auto size1 = getRegionSize(region1);
  auto size2 = getRegionSize(region2);

  if ( size1 != size2
    || region1->size.width != region2->size.width
    || region1->shadow.width != region2->shadow.width
    || region1->size.height != region2->size.height
    || region1->shadow.height != region2->shadow.height )
    return false;

  for (std::size_t i{0U}; i < size1; i++)
  {
    if ( ! isFCharEqual (region1->data[i], region2->data[i]) )
    {
      std::wcout << L"differ: char " << i << L" '"
                 << region1->data[i].ch[0] << L"' != '"
                 << region2->data[i].ch[0] << L"'\n";
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------
auto isFCharEqual ( const finalcut::FChar& lhs
                  , const finalcut::FChar& rhs ) -> bool
{
  finalcut::FAttribute attr{};
  attr.bit()->no_changes = true;
  attr.bit()->printed = true;

  return lhs.ch == rhs.ch
      && lhs.encoded_char == rhs.encoded_char
      && lhs.color.data   == rhs.color.data
      && lhs.attr.byte()->at(0) == rhs.attr.byte()->at(0)
      && lhs.attr.byte()->at(1) == rhs.attr.byte()->at(1)
      && (~ attr.byte()->at(2) & lhs.attr.byte()->at(2)) == (~ attr.byte()->at(2) & rhs.attr.byte()->at(2))
      && lhs.attr.byte()->at(3) == rhs.attr.byte()->at(3);
}

//----------------------------------------------------------------------
template < typename FCharT
         , enable_if_FChar_t<FCharT> >
void printOnRegion ( finalcut::FVTerm::FTermRegion* region
                   , const FCharT& fchar )
{
  if ( region->cursor.x < 1 )
    region->cursor.x = 1;

  if ( region->cursor.y < 1 )
    region->cursor.y = 1;

  int ax = region->cursor.x - 1;
  int ay = region->cursor.y - 1;
  const int line_length = region->size.width + region->shadow.width;
  const int line_height = region->size.height + region->shadow.height;
  const int size = line_length * line_height;

  if ( ay * line_length + ax > size )
  {
    auto tmp = ay * line_length + ax % size;
    ax = tmp % line_length;
    ay = tmp / line_height;
    region->cursor.x = ax + 1;
    region->cursor.y = ay + 1;
  }

  auto& ac = region->data[ay * line_length + ax];  // region character
  std::memcpy (&ac, &fchar, sizeof(ac));  // copy character to region
  region->cursor.x = ((ax + 1) % line_length) + 1;
  region->cursor.y = ((ax + 1) / line_length) + region->cursor.y;

  if ( region->cursor.y > line_height )
    region->cursor.y = ((region->cursor.y - 1) % line_height) + 1;
}

//----------------------------------------------------------------------
void printOnRegion ( finalcut::FVTerm::FTermRegion* region
                   , const RepeatFChar& rep_fchar )
{
  for (std::size_t i{0U}; i < rep_fchar.repeat; i++)
  {
    printOnRegion (region, rep_fchar.data);
  }
}

//----------------------------------------------------------------------
void printOnRegion ( finalcut::FVTerm::FTermRegion* region
                   , const RepeatFCharVector& rep_fchar_vec )
{
  for (const auto& rep_fchar : rep_fchar_vec)
  {
    printOnRegion (region, rep_fchar);
  }
}

//----------------------------------------------------------------------
void printOnRegion ( finalcut::FVTerm::FTermRegion* region
                   , const RepeatFCharLine& rep_fchar_line )
{
  for (std::size_t i{0U}; i < rep_fchar_line.repeat; i++)
  {
    printOnRegion (region, rep_fchar_line.data);
  }
}

//----------------------------------------------------------------------
void printOnRegion ( finalcut::FVTerm::FTermRegion* region
                   , const RepeatFCharLineVector& rep_fchar_line_vec )
{
  for (const auto& rep_fchar_line : rep_fchar_line_vec)
  {
    printOnRegion (region, rep_fchar_line);
  }
}

//----------------------------------------------------------------------
void printRegion ( finalcut::FVTerm::FTermRegion* region )
{
  auto width = region->size.width + region->shadow.width;
  auto height = region->size.height + region->shadow.height;
  auto size = getRegionSize(region);
  std::wcout << L'┌' << std::wstring(width, L'─') << L"┐\n";

  for (std::size_t i{0U}; i < size; i++)
  {
    if ( region->data[i].attr.bit()->fullwidth_padding )
      continue;

    auto col = (i + 1) % width ;
    auto line = (i + 1) / width;

    if ( col == 1 && line < std::size_t(height) )
      std::wcout << L"│";

    auto ch = region->data[i].ch;

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
void moveRegion ( finalcut::FVTerm::FTermRegion* region
                , const finalcut::FPoint& pos )
{
  if ( ! region )
    return;

  region->position.x = pos.getX();
  region->position.y = pos.getY();
}

}  // namespace test

#endif  // FVTERM_CHECK_H
