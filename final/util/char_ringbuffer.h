/***********************************************************************
* char_ringbuffer.h - Ring buffer for char elements                    *
*                                                                      *
* This file is part of the FINAL CUT widget toolkit                    *
*                                                                      *
* Copyright 2022-2024 Markus Gans                                      *
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

/*  Inheritance diagram
 *  ═══════════════════
 *
 *  ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 *  ▕ FRingBuffer ▏
 *  ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 *         ▲
 *         │
 * ▕▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▏
 * ▕ CharRingBuffer ▏
 * ▕▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▏
 */

#ifndef CHARRINGBUFFER_H
#define CHARRINGBUFFER_H

#if !defined (USE_FINAL_H) && !defined (COMPILE_FINAL_CUT)
  #error "Only <final/final.h> can be included directly."
#endif

#include <algorithm>
#include <array>
#include <cstring>
#include <string>
#include <utility>

#include <final/util/fstring.h>

namespace finalcut
{

// class forward declaration
template <std::size_t>
class CharRingBuffer;

//----------------------------------------------------------------------
// class FRingBuffer
//----------------------------------------------------------------------

template <typename T, std::size_t Capacity>
class FRingBuffer
{
  public:
    //------------------------------------------------------------------
    // class ring_iterator
    //------------------------------------------------------------------

    template <typename Type, std::size_t N>
    class ring_iterator
    {
      public:
        // Using-declarations
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Type;
        using difference_type   = std::ptrdiff_t;
        using pointer           = Type*;
        using reference         = Type&;

        explicit ring_iterator (pointer p, std::size_t start, std::size_t pos)
          : ptr{p}
          , offset{start}
          , index{pos}
        { }

        inline auto operator ++ () noexcept -> ring_iterator&  // prefix
        {
          index++;
          return *this;
        }

        inline auto operator ++ (int) noexcept -> ring_iterator  // postfix
        {
          ring_iterator i = *this;
          index++;
          return i;
        }

        inline auto operator * () const noexcept -> reference
        {
          return ptr[(offset + index) % N];
        }

        inline auto operator -> () const noexcept -> pointer
        {
          return &**this;
        }

        friend inline auto operator == (const ring_iterator& lhs, const ring_iterator& rhs) noexcept -> bool
        {
          return lhs.index  == rhs.index
              && lhs.ptr    == rhs.ptr
              && lhs.offset == rhs.offset;
        }

        friend inline auto operator != (const ring_iterator& lhs, const ring_iterator& rhs) noexcept -> bool
        {
          return ! (lhs == rhs);
        }

      private:
        // Data members
        pointer           ptr{nullptr};
        const std::size_t offset{0U};
        std::size_t       index{0U};

        // Friend Non-member operator functions
        inline friend auto operator + ( const ring_iterator& iter
                                      , std::ptrdiff_t size ) noexcept -> ring_iterator
        {
          auto tmp = iter;
          tmp.index += std::size_t(size);
          return tmp;
        }
    };

    // Using-declarations
    using iterator        = ring_iterator<T, Capacity>;
    using const_iterator  = ring_iterator<const T, Capacity>;
    using difference_type = std::ptrdiff_t;
    using pointer         = T*;
    using reference       = T&;
    using const_reference = const T&;
    using value_type      = T;

    // Constructors
    FRingBuffer()
      : buffer()
    { }

    virtual ~FRingBuffer() = default;

    // Overloaded operators
    inline auto operator [] (std::size_t index) noexcept -> reference
    {
      static_assert ( Capacity > 0, "Ring buffer has no memory" );
      return buffer[(head + index) % Capacity];
    }

    inline auto operator [] (std::size_t index) const noexcept -> const_reference
    {
      static_assert ( Capacity > 0, "Ring buffer has no memory" );
      return buffer[(head + index) % Capacity];
    }

    // Accessors
    virtual inline auto  getClassName() const -> FString
    {
      return "FRingBuffer";
    }

    inline auto getSize() const noexcept -> std::size_t
    {
      return elements;
    }

    inline auto getCapacity() const noexcept -> std::size_t
    {
      return Capacity;
    }

    inline auto begin() noexcept -> iterator
    {
      return iterator(buffer.data(), head, 0);
    }

    inline auto begin() const noexcept -> const_iterator
    {
      return const_iterator(buffer.data(), head, 0);
    }

    inline auto end() noexcept -> iterator
    {
      return iterator(buffer.data(), head, getSize());
    }

    inline auto end() const noexcept -> const_iterator
    {
      return const_iterator(buffer.data(), head, getSize());
    }

    inline auto front() noexcept -> reference
    {
      if ( isEmpty() )
        return empty_element;

      return buffer[head];
    }

    inline auto front() const noexcept -> const_reference
    {
      if ( isEmpty() )
        return empty_element;

      return buffer[head];
    }

    inline auto back() noexcept -> reference
    {
      if ( isEmpty() )
        return empty_element;

      std::size_t index = (tail == 0) ? Capacity - 1 : tail - 1;
      return buffer[index];
    }

    inline auto back() const noexcept -> const_reference
    {
      if ( isEmpty() )
        return empty_element;

      std::size_t index = (tail == 0) ? Capacity - 1 : tail - 1;
      return buffer[index];
    }

    // Mutators
    inline void clear() noexcept
    {
      head = 0U;
      tail = 0U;
      elements = 0U;
    }

    // Inquiries
    inline auto isEmpty() const noexcept -> bool
    {
      return elements == 0;
    }

    inline auto hasData() const noexcept -> bool
    {
      return ! isEmpty();
    }

    inline auto isFull() const noexcept -> bool
    {
      return elements == Capacity;
    }

    // Methods
    inline void push (const T& item) noexcept
    {
      if ( isFull() )
        return;

      static_assert ( Capacity > 0, "Ring buffer has no memory" );
      buffer[tail] = item;
      tail = (tail + 1) % Capacity;
      elements++;
    }

    inline void push_back (const T& item) noexcept
    {
      push (item);
    }

    template <typename... Args>
    inline void emplace (Args&&... args)
    {
      if ( isFull() )
        return;

      static_assert ( Capacity > 0, "Ring buffer has no memory" );
      buffer[tail] = T(std::forward<Args>(args)...);
      tail = (tail + 1) % Capacity;
      elements++;
    }

    template <typename... Args>
    inline void emplace_back (Args&&... args)
    {
      emplace (std::forward<Args>(args)...);
    }

    inline void pop() noexcept
    {
      if ( isEmpty() )
        return;

      static_assert ( Capacity > 0, "Ring buffer has no memory" );
      head = (head + 1) % Capacity;
      elements--;
    }

    inline void pop_front() noexcept
    {
      pop();
    }

    inline void pop (std::size_t s) noexcept
    {
      if ( isEmpty() )
        return;

      static_assert ( Capacity > 0, "Ring buffer has no memory" );
      s = std::min(s, elements);
      head = (head + s) % Capacity;
      elements -= s;
    }

  private:
    // Data members
    std::array<value_type, Capacity> buffer;
    value_type  empty_element{};
    std::size_t head{0U};
    std::size_t tail{0U};
    std::size_t elements{0U};

    // Friend classes
    friend class CharRingBuffer<Capacity>;
};


//----------------------------------------------------------------------
// class CharRingBuffer
//----------------------------------------------------------------------

template <std::size_t Capacity>
class CharRingBuffer final : public FRingBuffer<char, Capacity>
{
  public:
    // Using-declarations
    using FRingBuffer<char, Capacity>::getSize;
    using FRingBuffer<char, Capacity>::getCapacity;
    using FRingBuffer<char, Capacity>::front;
    using FRingBuffer<char, Capacity>::buffer;
    using FRingBuffer<char, Capacity>::tail;
    using FRingBuffer<char, Capacity>::head;

    // Accessor
    inline auto getClassName() const -> FString override
    {
      return "CharRingBuffer";
    }

    // Method
    auto strncmp_front (const char* string, std::size_t length) const noexcept -> bool
    {
      if ( length > getSize() )
        return false;

      if ( tail > head )
      {
        const auto min_length = std::min(length, getCapacity());
        return std::memcmp(string, &front(), min_length) == 0;
      }

      auto l1 = std::min(length, getCapacity() - head);
      auto l2 = length - l1;

      if ( std::memcmp(string, &front(), l1) != 0 )
        return false;

      if ( l2 == 0 )
        return true;

      return std::memcmp(string + l1, &buffer[0], l2) == 0;
    }
};

}  // namespace finalcut

#endif  // CHARRINGBUFFER_H
