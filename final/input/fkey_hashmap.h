/***********************************************************************
* fkey_hashmap.h - Key sequence hash map access                        *
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

#ifndef FKEYHASHMAP_H
#define FKEYHASHMAP_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>

#include "final/fc.h"
#include "final/input/fkey_map.h"

namespace finalcut
{

namespace fkeyhashmap
{

namespace internal
{

struct Const
{
  static auto getMaxHashSize() -> std::size_t
  {
    static const auto size = std::max( FKeyMap::getKeyMap().size()
                                     , FKeyMap::getKeyCapMap().size() ) * 2;
    return size;
  }
};

//----------------------------------------------------------------------
template <typename BufferT>
struct KeySequence
{
  KeySequence (const char* s, uInt8 l)
    : string(s)
    , length(l)
  { }

  explicit KeySequence (const BufferT& buf)
    : buffer(&buf)
  { }

  const char* string{nullptr};
  const uInt8 length{0};
  const BufferT* buffer{nullptr};
};

//----------------------------------------------------------------------
template <typename IterT>
constexpr auto hash_function (IterT iter, const IterT end) -> std::size_t
{
  std::size_t sum = 0;

  while ( iter != end )
  {
    sum += std::size_t(*iter);
    ++iter;
    sum += ( iter != end ) ? std::size_t(*iter) << 8 : 0;
  }

  return sum % Const::getMaxHashSize();
}

//----------------------------------------------------------------------
template <typename BufferT>
constexpr auto hash_function (const BufferT& buf) -> std::size_t
{
  return hash_function (std::begin(buf), std::end(buf));
}

//----------------------------------------------------------------------
template <typename BufferT>
struct KeySequenceHash
{
  auto operator () (const KeySequence<BufferT>& key) const noexcept -> std::size_t
  {
    if ( key.string && ! key.buffer )
      return hash_function (key.string, key.string + key.length);

    if ( key.buffer )
      return hash_function (*key.buffer);

    return hash_function(std::string("unknown"));
  }
};

//----------------------------------------------------------------------
template <typename BufferT>
struct KeySequenceEqual
{
  auto operator () ( const KeySequence<BufferT>& lhs
                   , const KeySequence<BufferT>& rhs) const noexcept -> bool
  {
    if ( lhs.string && ! lhs.buffer && rhs.string && ! rhs.buffer )
    {
      return lhs.length == rhs.length
          && std::memcmp(lhs.string, rhs.string, rhs.length) == 0;
    }

    if ( ! lhs.string && lhs.buffer && rhs.string && ! rhs.buffer )
    {
      return lhs.buffer->getSize() == rhs.length
          && lhs.buffer->strncmp_front(rhs.string, rhs.length);
    }

    if ( lhs.string && ! lhs.buffer && ! rhs.string && rhs.buffer )
    {
      return lhs.length == rhs.buffer->getSize()
          && rhs.buffer->strncmp_front(lhs.string, lhs.length);
    }

    return false;
  }
};

//----------------------------------------------------------------------
// Using-declaration
template <typename BufferT>
using HashMap = std::unordered_map<KeySequence<BufferT>
                                 , FKey
                                 , KeySequenceHash<BufferT>
                                 , KeySequenceEqual<BufferT>>;

//----------------------------------------------------------------------
template <typename BufferT>
auto createKeyCapMap() -> HashMap<BufferT>
{
  const auto& fkey_cap_table = FKeyMap::getKeyCapMap();
  HashMap<BufferT> fkey_cap_map;
  fkey_cap_map.reserve(fkey_cap_table.size());

  for (const auto& item : fkey_cap_table)
    if ( item.string && item.length != 0 )
      fkey_cap_map[{item.string, item.length}] = item.num;

  return fkey_cap_map;
}

//----------------------------------------------------------------------
template <typename BufferT, typename IterT>
auto createKeyCapMap (IterT begin, IterT end) -> HashMap<BufferT>
{
  HashMap<BufferT> fkey_cap_map;
  fkey_cap_map.reserve(std::size_t(end - begin));
  auto iter = begin;

  while ( iter != end )
  {
    if ( iter->string && iter->length != 0 )
      fkey_cap_map[{iter->string, iter->length}] = iter->num;

    ++iter;
  }

  return fkey_cap_map;
}

//----------------------------------------------------------------------
template <typename BufferT>
auto createKeyMap() -> HashMap<BufferT>
{
  auto& fkey_table = FKeyMap::getKeyMap();
  HashMap<BufferT> fkey_map;
  fkey_map.reserve(fkey_table.size());

  for (auto& item : fkey_table)
    if ( item.length != 0 )  // Note: item.string is an array and always allocated
      fkey_map[{item.string.data(), item.length}] = item.num;

  return fkey_map;
}

}  // namespace internal

//----------------------------------------------------------------------
template <typename BufferT>
auto getKeyCapMap() -> internal::HashMap<BufferT>&
{
  using HashMapType = internal::HashMap<BufferT>;
  static const auto& fkey_cap_map = std::make_unique<HashMapType>(internal::createKeyCapMap<BufferT>());
  return *fkey_cap_map;
}

//----------------------------------------------------------------------
template <typename BufferT, typename IterT>
void setKeyCapMap (IterT begin, IterT end)
{
  getKeyCapMap<BufferT>() = internal::createKeyCapMap<BufferT>(begin, end);
}

//----------------------------------------------------------------------
template <typename BufferT>
auto getKeyMap() -> internal::HashMap<BufferT>&
{
  using HashMapType = internal::HashMap<BufferT>;
  static const auto& fkey_map = std::make_unique<HashMapType>(internal::createKeyMap<BufferT>());
  return *fkey_map;
}

//----------------------------------------------------------------------
template <typename BufferT>
auto getTermcapKey (const BufferT& char_rbuf) -> FKey
{
  auto& hashmap = getKeyCapMap<BufferT>();
  auto iter = hashmap.find(internal::KeySequence<BufferT>(char_rbuf));

  if ( iter != hashmap.end() )  // found
    return iter->second;

  return FKey::None;
}

//----------------------------------------------------------------------
template <typename BufferT>
auto getKnownKey (const BufferT& char_rbuf) -> FKey
{
  auto& hashmap = getKeyMap<BufferT>();
  auto iter = hashmap.find(internal::KeySequence<BufferT>(char_rbuf));

  if ( iter != hashmap.end() )  // found
    return iter->second;

  return FKey::None;
}

}  // namespace fkeyhashmap

}  // namespace finalcut

#endif  // FKEYHASHMAP_H
