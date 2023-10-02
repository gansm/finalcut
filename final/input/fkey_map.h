/***********************************************************************
* fkey_map.h - Key name mapping                                        *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2015-2023 Markus Gans                                      *
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

#ifndef FKEYMAP_H
#define FKEYMAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <array>
#include <string>

#include "final/ftypes.h"
#include "final/util/fstring.h"

namespace finalcut
{

enum class FKey : uInt32;   // forward declaration

class FKeyMap final
{
  public:
    struct KeyCapMap
    {
      FKey num{};
      const char* string{};
      uInt8 length{};
      std::array<char, 4> tname{};
    };

    struct KeyMap
    {
      FKey num{};
      std::array<char, 8> string{};
      uInt8 length{};
    };

    struct KeyName
    {
      FKey num{};
      std::array<char, 26> string{};
    };

    // Using-declaration
    using KeyCapMapType = std::array<KeyCapMap, 190>;
    using KeyMapType = std::array<KeyMap, 234>;
    using KeyNameType = std::array<KeyName, 390>;

    // Constructors
    FKeyMap() = default;

    // Accessors
    auto        getClassName() const -> FString;
    static auto getInstance() -> FKeyMap&;
    static auto getKeyCapMap() -> KeyCapMapType&;
    static auto getKeyMap() -> KeyMapType&;
    static auto getKeyName() -> const KeyNameType&;

  private:
    // Data members
    static KeyCapMapType     fkey_cap_table;
    static KeyMapType        fkey_table;
    static const KeyNameType fkeyname;
};

// FKeyMap inline functions
//----------------------------------------------------------------------
inline auto FKeyMap::getClassName() const -> FString
{ return "FKeyMap"; }

}  // namespace finalcut

#endif  // FKEYMAP_H
