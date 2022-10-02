/***********************************************************************
* char_ringbuffer.h - Ring buffer for char elements                    *
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

#include <cstring>
#include <array>

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
    class ring_iterator : public std::iterator<std::forward_iterator_tag, Type>
    {
      public:
        // Using-declarations
        using pointer = Type*;
        using reference = Type&;

        explicit ring_iterator (pointer p, std::size_t start, std::size_t pos)
          : ptr{p}
          , offset{start}
          , index{pos}
        { }

        auto operator ++ () -> ring_iterator&  // prefix
        {
          index++;
          return *this;
        }

        auto operator ++ (int) -> ring_iterator  // postfix
        {
          ring_iterator i = *this;
          index++;
          return i;
        }

        auto operator * () const -> reference
        {
          return ptr[(offset + index) % N];
        }

        auto operator -> () const -> pointer
        {
          return &**this;
        }

        auto operator == (const ring_iterator& rhs) const -> bool
        {
          return index  == rhs.index
              && ptr    == rhs.ptr
              && offset == rhs.offset;
        }

        auto operator != (const ring_iterator& rhs) const -> bool
        {
          return ! (*this == rhs);
        }

      private:
        // Data members
        pointer           ptr{nullptr};
        const std::size_t offset{0U};
        std::size_t       index{0U};

        // Friend Non-member operator functions
        friend auto operator + (const ring_iterator& iter, std::ptrdiff_t size) -> ring_iterator
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
    FRingBuffer () = default;

    // Overloaded operators
    auto operator [] (std::size_t index) noexcept -> reference
    {
      return buffer[(head + index) % Capacity];
    }

    auto operator [] (std::size_t index) const noexcept -> const_reference
    {
      return buffer[(head + index) % Capacity];
    }

    // Accessors
    virtual auto  getClassName() const -> FString
    {
      return "FRingBuffer";
    }

    auto getSize() const noexcept -> size_t
    {
      return elements;
    }

    auto getCapacity() const noexcept -> size_t
    {
      return Capacity;
    }

    auto begin() noexcept -> iterator
    {
      return iterator(buffer.data(), head, 0);
    }

    auto begin() const noexcept -> const_iterator
    {
      return const_iterator(buffer.data(), head, 0);
    }

    auto end() noexcept -> iterator
    {
      return iterator(buffer.data(), head, getSize());
    }

    auto end() const noexcept -> const_iterator
    {
      return const_iterator(buffer.data(), head, getSize());
    }

    auto front() noexcept -> reference
    {
      if ( isEmpty() )
        return empty_char;

      return buffer[head];
    }

    auto front() const noexcept -> const_reference
    {
      if ( isEmpty() )
        return empty_char;

      return buffer[head];
    }

    auto back() noexcept -> reference
    {
      if ( isEmpty() )
        return empty_char;

      std::size_t index = (tail == 0) ? Capacity - 1 : tail - 1;
      return buffer[index];
    }

    auto back() const noexcept -> const_reference
    {
      if ( isEmpty() )
        return empty_char;

      std::size_t index = (tail == 0) ? Capacity - 1 : tail - 1;
      return buffer[index];
    }

    // Mutators
    void clear() noexcept
    {
      head = 0U;
      tail = 0U;
      elements = 0U;
    }

    // Inquiries
    auto isEmpty() const noexcept -> bool
    {
      return elements == 0;
    }

    auto hasData() const noexcept -> bool
    {
      return ! isEmpty();
    }

    auto isFull() const noexcept -> bool
    {
      return elements == Capacity;
    }

    // Methods
    void push (const T& item) noexcept
    {
      if ( isFull() )
        return;

      buffer[tail] = item;
      tail = (tail + 1) % Capacity;
      elements++;
    }

    void pop() noexcept
    {
      if ( isEmpty() )
        return;

      head = (head + 1) % Capacity;
      elements--;
    }

    void pop (std::size_t s) noexcept
    {
      if ( isEmpty() )
        return;

      s = std::min(s, elements);
      head = (head + s) % Capacity;
      elements -= s;
    }

  private:
    // Data members
    std::array<value_type, Capacity> buffer{};
    value_type  empty_char{};
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
    auto getClassName() const -> FString override
    {
      return "CharRingBuffer";
    }

    // Method
    auto strncmp_front (const char* string, std::size_t length) -> bool
    {
      if ( length > getSize() )
        return false;

      if ( tail > head )
      {
        length = std::min(length, getCapacity());
        return ( std::strncmp(string, &front(), length) == 0 );
      }

      auto l1 = std::min(length, getCapacity() - head);
      auto l2 = length - l1;
      auto found1 = bool( std::strncmp(string, &front(), l1) == 0 );
      auto found2 = bool( l2 == 0
                       || std::strncmp(string + l1, &buffer[0], l2) == 0 );
      return found1 && found2;
    }
};

}  // namespace finalcut

#endif  // CHARRINGBUFFER_H
