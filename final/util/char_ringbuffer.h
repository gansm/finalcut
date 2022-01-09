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

        ring_iterator (const ring_iterator& it)  // copy constructor
          : ptr{it.ptr}
          , offset{it.offset}
          , index{it.index}
        { }

        ring_iterator& operator ++ ()  // prefix
        {
          index++;
          return *this;
        }

        ring_iterator operator ++ (int)  // postfix
        {
          ring_iterator i = *this;
          index++;
          return i;
        }

        reference operator * () const
        {
          return ptr[(offset + index) % N];
        }

        pointer operator -> () const
        {
          return &**this;
        }

        bool operator == (const ring_iterator& rhs) const
        {
          return index  == rhs.index
              && ptr    == rhs.ptr
              && offset == rhs.offset;
        }

        bool operator != (const ring_iterator& rhs) const
        {
          return ! (*this == rhs);
        }

      private:
        // Data members
        pointer           ptr{nullptr};
        const std::size_t offset{0U};
        std::size_t       index{0U};

        // Friend Non-member operator functions
        friend ring_iterator operator + (const ring_iterator& iter, std::ptrdiff_t size)
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
    reference operator [] (std::size_t index) noexcept
    {
      return buffer[(head + index) % Capacity];
    }

    const_reference operator [] (std::size_t index) const noexcept
    {
      return buffer[(head + index) % Capacity];
    }

    // Accessors
    virtual FString  getClassName() const
    {
      return "FRingBuffer";
    }

    size_t getSize() const noexcept
    {
      return elements;
    }

    size_t getCapacity() const noexcept
    {
      return Capacity;
    }

    iterator begin() noexcept
    {
      return iterator(buffer.data(), head, 0);
    }

    const_iterator begin() const noexcept
    {
      return const_iterator(buffer.data(), head, 0);
    }

    iterator end() noexcept
    {
      return iterator(buffer.data(), head, getSize());
    }

    const_iterator end() const noexcept
    {
      return const_iterator(buffer.data(), head, getSize());
    }

    reference front() noexcept
    {
      if ( isEmpty() )
        return empty_char;

      return buffer[head];
    }

    const_reference front() const noexcept
    {
      if ( isEmpty() )
        return empty_char;

      return buffer[head];
    }

    reference back() noexcept
    {
      if ( isEmpty() )
        return empty_char;

      std::size_t index = (tail == 0) ? Capacity - 1 : tail - 1;
      return buffer[index];
    }

    const_reference back() const noexcept
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
    bool isEmpty() const noexcept
    {
      return elements == 0;
    }

    bool hasData() const noexcept
    {
      return ! isEmpty();
    }

    bool isFull() const noexcept
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

  protected:
    // Data members
    std::array<value_type, Capacity> buffer{};
    value_type  empty_char{};
    std::size_t head{0U};
    std::size_t tail{0U};
    std::size_t elements{0U};
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
    FString getClassName() const override
    {
      return "CharRingBuffer";
    }

    // Method
    bool strncmp_front (const char* string, std::size_t length)
    {
      if ( length > getSize() )
        return false;

      if ( tail > head )
      {
        length = std::min(length, getCapacity());
        return ( std::strncmp(string, &front(), length) == 0 );
      }
      else
      {
        auto l1 = std::min(length, getCapacity() - head);
        auto l2 = length - l1;
        auto found1 = bool( std::strncmp(string, &front(), l1) == 0 );
        auto found2 = bool( l2 == 0
                         || std::strncmp(string + l1, &buffer[0], l2) == 0 );
        return found1 && found2;
      }
    }
};

}  // namespace finalcut

#endif  // CHARRINGBUFFER_H
