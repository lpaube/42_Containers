/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_vector.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laube <louis-philippe.aube@hotmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/12 17:11:36 by laube             #+#    #+#             */
/*   Updated: 2022/04/26 15:33:26 by laube            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <limits>
#include <memory>
#include <iterator>

#include "iterator.hpp"
#include "utils.hpp"
//#include "Timer.hpp"

namespace ft {

  template <typename T, typename Allocator = std::allocator<T> >
    class vector {
      public:
        // Member Types
        typedef T                                      value_type;
        typedef Allocator                              allocator_type;
        typedef typename allocator_type::difference_type  diff_type;
        typedef std::size_t                            size_type;
        typedef std::ptrdiff_t                         difference_type;
        typedef value_type&                            reference;
        typedef const value_type&                      const_reference;
        typedef T*                                     pointer;
        typedef const T*                               const_pointer;
        typedef normal_iterator<pointer, vector>       iterator;
        typedef normal_iterator<const_pointer, vector> const_iterator;
        typedef ft::reverse_iterator<iterator>         reverse_iterator;
        typedef ft::reverse_iterator<const_iterator>   const_reverse_iterator;

      private:
        pointer m_start;
        pointer m_finish;
        pointer m_end_of_storage;
        allocator_type m_alloc;

        void m_create_storage(size_type count) {
          // ADD ERROR IF COUNT IS NEGATIVE
          if (count <= 0)
            return;
          m_start = m_alloc.allocate(count);
          m_finish = m_start;
          m_end_of_storage = m_start + count;
        }

        /*
           template <typename InputIterator>
           void create_storage_dispatch(InputIterator first, InputIterator last, std::input_iterator_tag)
           {
           for (int i = 0; 
           }
           */

        iterator m_construct_storage(const T& value, size_type count) {
          size_type i = 0;
          for (; i < count; ++i) {
            m_alloc.construct(m_start + i, value);
          }
          m_finish = m_start + i;
          return m_finish;
        }

        template <class InputIt>
          pointer m_construct_storage(pointer dst, InputIt it, InputIt ite)
          {
            int i = 0;
            for (; it + i != ite; ++i)
            {
              m_alloc.construct(dst + i, *(it + i));
            }
            return dst + i;
          }

        void destroy_storage(iterator ite1, iterator ite2)
        {
          for (; ite1 != ite2; ++ite1)
          {
            m_alloc.destroy(ite1.base());
          }
        }

        /*
         * Allocates twice the current storage until total storage is greater
         * than new_cap. new_cap represents the total storage elements needed
         */
        void grow_capacity(size_type new_cap)
        {
          if (new_cap > max_size())
          {
            throw std::length_error("Can't reserve vector size: bigger than max_size()");
          }
          if (new_cap == 0)
            new_cap = 1;
          while (m_end_of_storage - m_start < static_cast<long>(new_cap))
          {
            reserve((m_end_of_storage - m_start) * 2);
          }
        }

      public:
        // MEMBER FUNCTIONS
        // Constructors
        vector() : m_start(), m_finish(m_start), m_end_of_storage(m_start), m_alloc() {
        }

        explicit vector(const Allocator& alloc)
          : m_start(NULL), m_finish(m_start), m_end_of_storage(m_start), m_alloc(alloc) {}

        explicit vector(size_type count, const T& value = T(),
            const Allocator& alloc = Allocator())
          : m_start(NULL)
            , m_finish(m_start)
            , m_end_of_storage(m_start)
            , m_alloc(alloc) {
              //m_create_storage(count);
              grow_capacity(count);
              m_construct_storage(value, count);
            }

        template <typename InputIt>
          vector(typename enable_if<!(is_integral<InputIt>::value),
              InputIt>::type first,
              InputIt last, const Allocator& alloc = Allocator())
          : m_start(NULL)
            , m_finish(m_start)
            , m_end_of_storage(m_start)
            , m_alloc(alloc) {
              //typedef typename iterator_traits<InputIt>::iterator_category Iter_category;
              insert(begin(), first, last);
            }

        vector(const vector& other)
          : m_start(NULL)
            , m_finish(m_start)
            , m_end_of_storage(m_start)
            , m_alloc(Allocator())
      {
        m_create_storage(other.capacity());
        insert(m_start, other.begin(), other.end());
      }
        // Destructors
        ~vector() {
          clear_complete();
          m_start = NULL;
          m_finish = NULL;
          m_end_of_storage = NULL;
        }

        // Other member functions
        vector& operator=(const vector& other) {
          clear_complete();
          m_create_storage(other.capacity());
          insert(m_start, other.begin(), other.end());
          return *this;
        }

        void assign(size_type count, const T& value) {
          clear();
          if (count <= 0)
            return;
          grow_capacity(count);
          m_construct_storage(value, count);
        }

        template <class InputIt>
          void assign(InputIt first, typename enable_if<!is_integral<InputIt>::value,
              InputIt>::type last) {

            vector<value_type> tmp(first, last);
            clear_complete();

            insert(m_start, tmp.begin(), tmp.end());
            /*
            int i = 0;
            InputIt tmp = first;
            while (tmp != last)
            {
              i++;
              tmp++;
            }
            */
            //typedef typename iterator_traits<InputIt>::iterator_category Iter_category;
            //assign_dispatch(m_start, first, last, Iter_category());

            //m_create_storage(i);
          }

        /*
        template <typename InputIt>
          void assign_dispatch(iterator pos, InputIt first, InputIt last, std::input_iterator_tag)
          {
            clear_complete();
            insert(m_start, first, last);
          }

        template <typename InputIt>
          void assign_dispatch(iterator pos, InputIt first, InputIt last, std::forward_iterator_tag)
          {
            size_type i = 0;
            InputIt tmp = first;
            while (tmp != last)
            {
              i++;
              tmp++;
            }
            insert(m_start, first, last);
          }
          */

        allocator_type get_allocator() const { return (this->m_alloc); }

        // Element accesses
        reference at(size_type pos) {
          if (pos >= size()) {
            throw std::out_of_range("at(): position is greater or equal to size");
          }
          return *(m_start + pos);
        }

        const_reference at(size_type pos) const {
          if (pos >= size()) {
            throw std::out_of_range("at(): position is greater or equal to size");
          }
          return *(m_start + pos);
        }

        reference front() {
          return *begin();
        }

        const_reference front() const {
          return *begin();
        }

        reference back() {
          return *(end() - 1);
        }

        const_reference back() const {
          return *(end() - 1);
        }

        T* data() {
          if (empty())
            return NULL;
          return m_start;
        }

        const T* data() const {
          if (empty())
            return NULL;
          return m_start;
        }

        reference operator[](size_type pos) {
          return *(this->m_start + pos);
        }

        const_reference operator[](size_type pos) const {
          return *(this->m_start + pos);
        }

        iterator begin() { return iterator(this->m_start); }

        const_iterator begin() const { return const_iterator(this->m_start); }

        iterator end() { return iterator(this->m_finish); }

        const_iterator end() const { return const_iterator(this->m_finish); }

        reverse_iterator rbegin() {
          return reverse_iterator(this->m_finish);
        }

        const_reverse_iterator rbegin() const
        {
          if (empty())
            return reverse_iterator(this->m_finish);
          return const_reverse_iterator(this->m_finish);
        }

        reverse_iterator rend() {
          return reverse_iterator(this->m_start);
        }

        const_reverse_iterator rend() const
        {
          return const_reverse_iterator(this->m_start);
        }

        bool empty() const {
          return begin() == end();
        }

        size_type size() const {
          return std::distance(begin(), end());
          //return static_cast<size_type>(m_finish - m_start);
        }

        size_type max_size() const {
          return std::min(m_alloc.max_size()
              , static_cast<size_type>(std::numeric_limits<diff_type>::max()));
        }
        size_type capacity() const {
          return m_end_of_storage - m_start;
        }

        void reserve(size_type new_cap) {

          pointer new_m_start;
          pointer new_m_finish;

          if (new_cap > max_size())
          {
            throw std::length_error("Can't reserve vector size: bigger than max_size()");
          }
          if (new_cap == 0)
            new_cap = 1;
          if (new_cap <= capacity())
            return;

          new_m_start = m_alloc.allocate(new_cap);
          new_m_finish = m_construct_storage(new_m_start, m_start, m_finish);
          clear_complete();
          m_start = new_m_start;
          m_finish = new_m_finish;
          m_end_of_storage = m_start + new_cap;
        }

        // Dectucts all nodes without deallocating any
        void clear()
        {
          if (!empty())
            destroy_storage(m_start, m_finish);
          m_finish = m_start;
        }

        // Destructs and deallocate all nodes
        void clear_complete()
        {
          clear();
          if (capacity() != 0)
          {
            m_alloc.deallocate(m_start, m_end_of_storage - m_start);
            m_start = pointer();
            m_finish = m_start;
            m_end_of_storage = m_start;
          }
        }

        template <typename InputIt>
          void insert_dispatch(iterator pos, InputIt first, InputIt last, std::input_iterator_tag)
          {
            difference_type offset = pos - begin();
            vector<value_type, allocator_type> tmp(begin(), end());
            iterator it = tmp.begin();

            clear();
            for (difference_type i = 0; i < offset; ++i, ++it)
            {
              push_back(*it);
            }
            for (; first != last; ++first)
            {
              push_back(*first);
            }
            for (; it != tmp.end(); ++it)
            {
              push_back(*it);
            }
          }

        template<typename InputIt>
          void insert_dispatch(iterator pos, InputIt first, InputIt last, std::forward_iterator_tag)
          {
            size_type offset = pos - begin();
            size_type count = 0;

            count = std::distance(first, last);

            if (size() + count > max_size())
              throw std::length_error("Can't reserve vector size: bigger than max_size()");

            size_type end_len = size() + count;

            grow_capacity(size() + count);
            for (size_type i = 0; i < count; ++i)
            {
              m_alloc.construct(m_finish + i, value_type());
            }
            pos = begin() + offset;

            pointer first_copy = pos.base();
            pointer last_copy = end().base();
            pointer d_last_copy = (begin() + end_len).base();
            //pointer d_first_copy = (pos + count).base();

            /*
            iterator first_copy = pos;
            iterator last_copy = end();
            iterator d_first_copy = pos + count;
            */

            std::copy_backward(first_copy, last_copy, d_last_copy);

            /*
            for (; first_copy != last_copy; ++first_copy, ++d_first_copy)
            {
              std::cerr << "Here1 first_copy: " << *first_copy
                << "d_first_copy: " << *d_first_copy << std::endl;
              *d_first_copy = *first_copy;
            }
            */

            for (; first != last; ++first, ++pos)
            {
              *pos = *first;
            }
            m_finish += count;
          }

        void insert(iterator pos, size_type count, const T& value)
        {
          if (size() + count > max_size())
          {
            throw std::length_error("Can't reserve vector size: bigger than max_size()");
          }

          //difference_type offset = pos - begin();
          //iterator pos_new = begin() + offset;

          for (size_type i = 0; i < count; ++i)
          {
            //pos_new = begin() + offset + i;
            pos = insert(pos, value);
          }
        }

        iterator insert(iterator pos, const T& value)
        {

          //std::cout << "===STARTING INSERT CALL ===" << std::endl;
          difference_type offset = pos - begin();

          grow_capacity(size() + 1);


          m_alloc.construct(end().base(), T());

          pointer last = m_finish;
          pointer first = m_start + offset;
          pointer d_last = last + 1;

          std::copy_backward(first, last, d_last);
          /*
          while (first != last) {
            *(--d_last) = *(--last);
          }
          */
          *(m_start + offset) = value;
          m_finish++;
          return begin() + offset;
        }

        template <typename InputIt>
          void insert(iterator pos
              , typename enable_if<!(is_integral<InputIt>::value), InputIt>::type first
              , InputIt last)
          {
            typedef typename iterator_traits<InputIt>::iterator_category Iter_category;
            insert_dispatch(pos, first, last, Iter_category());
          }

        iterator erase(iterator pos)
        {
          if (pos == end())
            return pos;

          pointer first_copy = (pos + 1).base();
          pointer last_copy = m_finish;
          pointer d_first_copy = pos.base();

          std::copy(first_copy, last_copy, d_first_copy);

          m_alloc.destroy(m_finish - 1);
          --m_finish;
          return pos;
        }

        iterator erase(iterator first, iterator last)
        {
          size_type count = std::distance(first, last);
          size_type new_size = size() - count;
          size_type ret_num = last - begin() - count;

          pointer first_copy = last.base();
          pointer last_copy = m_finish;
          pointer d_first_copy = first.base();

          std::copy(first_copy, last_copy, d_first_copy);

          for (pointer destroy_pointer = m_start + new_size; destroy_pointer != m_finish; ++destroy_pointer)
          {
            m_alloc.destroy(destroy_pointer);
          }

          m_finish -= count;

          return (begin() + ret_num);
          /*
          iterator ret_it = last;

          for (iterator it = first; it != last; ++it)
          {
            ret_it = erase(first);
          }
          return ret_it;
          */
        }

        void push_back(const T& value)
        {
          grow_capacity(size() + 1);
          if (m_start == NULL)
          {
            m_alloc.construct(m_start, value);
            m_finish = m_start + 1;
          } else {
          m_alloc.construct(m_finish, value);
          m_finish++;
          }
        }

        void pop_back()
        {
          erase(end() - 1);
        }

        void resize(size_type count, T value = T())
        {
          if (count > size())
          {
            grow_capacity(count);
            for (iterator ite = end(); static_cast<size_type>(ite - begin()) < count; ++ite)
            {
              // *ite = value;
              m_alloc.construct(ite.base(), value);
            }
            m_finish = begin().base() + count;
          }
          else if (count < size())
          {
            destroy_storage(begin() + count, end());
            m_finish = begin().base() + count;
          }
        }

        void swap(vector& other)
        {
          std::swap(m_start, other.m_start);
          std::swap(m_finish, other.m_finish);
          std::swap(m_end_of_storage, other.m_end_of_storage);
        }
    };

  template <typename T, typename Alloc>
    bool operator==(const ft::vector<T, Alloc>& lhs, const ft::vector<T, Alloc>& rhs)
    {
      return lhs.size() == rhs.size() && ft::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

  template <typename T, typename Alloc>
    bool operator!=(const ft::vector<T, Alloc>& lhs, const ft::vector<T, Alloc>& rhs)
    {
      return !(lhs == rhs);
    }

  template <typename T, typename Alloc>
    bool operator<(const ft::vector<T, Alloc>& lhs, const ft::vector<T, Alloc>& rhs)
    {
      return ft::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

  template <typename T, typename Alloc>
    bool operator<=(const ft::vector<T, Alloc>& lhs, const ft::vector<T, Alloc>& rhs)
    {
      return (lhs < rhs || lhs == rhs);
    }

  template <typename T, typename Alloc>
    bool operator>(const ft::vector<T, Alloc>& lhs, const ft::vector<T, Alloc>& rhs)
    {
      return (!(lhs < rhs) && !(lhs == rhs));
    }

  template <typename T, typename Alloc>
    bool operator>=(const ft::vector<T, Alloc>& lhs, const ft::vector<T, Alloc>& rhs)
    {
      return (lhs > rhs || lhs == rhs);
    }

  template <typename T, typename Alloc>
    void swap(ft::vector<T, Alloc>& lhs, ft::vector<T, Alloc>& rhs)
    {
      lhs.swap(rhs);
    }

}
// SOURCE CODE:
// https://code.woboq.org/gcc/libstdc++-v3/include/bits/stl_vector.h.html
// DOCUMENTATION:
// https://en.cppreference.com/w/cpp/container/vector
//  LIST
//  *
//  * MEMBER FUNCTIONS
//  *   CONSTRUCTORS
//  *     - [x] 1.  vector();
//  *     - [x] 2.  explicit vector( const Allocator& alloc );
//  *     - [x] 3.  explicit vector( size_type count, const T& value = T(),
//  const Allocator& alloc = Allocator());
//  *     - [x] 5.  template< class InputIt >
//  *         vector( InputIt first, InputIt last, const Allocator& alloc =
//  Allocator() );
//  *     - [x] 6.  vector( const vector& other );
//  *
//  *   DESTRUCTORS
//  *     - [x] 1.  ~vector();
//  *
//  *   OTHER MEMBER FUNCTIONS
//  *     - [x] 1.  vector& operator=( const vector& other );
//  *     - [x] 1.  void assign( size_type count, const T& value );
//  *     - [x] 2.  template< class InputIt > void assign( InputIt first,
//  InputIt last );
//  *     - [x] 1.  allocator_type get_allocator() const;
//  *
//  *   ELEMENT ACCESS
//  *     - [x] 1.  reference at( size_type pos );
//  *     - [x] 2.  const_reference at( size_type pos ) const;
//  *     - [x] 1.  reference front();
//  *     - [x] 2.  const_reference front() const;
//  *     - [x] 1.  reference back();
//  *     - [x] 2.  const_reference back() const;
//  *     - [x] 1.  T* data();
//  *     - [x] 2.  const T* data() const;
//  *
//  *   ITERATORS
//  *     - [x] 1.  iterator begin();
//  *     - [x] 2.  const_iterator begin() const;
//  *     - [x] 1.  iterator end();
//  *     - [x] 2.  const_iterator end() const;
//  *     - [x] 1.  reverse_iterator rbegin();
//  *     - [x] 2.  const_reverse_iterator rbegin() const;
//  *     - [x] 1.  reverse_iterator rend();
//  *     - [x] 2.  const_reverse_iterator rend() const;
//  *
//  *   CAPACITY
//  *     - [x] 1.  bool empty() const;
//  *     - [x] 1.  size_type size() const;
//  *     - [x] 1.  size_type max_size() const;
//  *     - [x] 1.  void reserve( size_type new_cap );
//  *     - [x] 1.  size_type capacity() const;
//  *
//  *   MODIFIERS
//  *     - [x] 1.  void clear();
//  *     - [x] 1.  iterator insert( iterator pos, const T& value );
//  *     - [x] 3.  void insert( iterator pos, size_type count, const T& value);
//  *     - [x] 4.  template< class InputIt >
//  *                   void insert( iterator pos, InputIt first, InputIt last );
//  *     - [x] 1.  iterator erase( iterator pos );
//  *     - [x] 2.  iterator erase( iterator first, iterator last );
//  *     - [x] 1.  void push_back( const T& value );
//  *     - [x] 1.  void pop_back();
//  *     - [x] 2.  void resize( size_type count, T value = T() );
//  *     - [x] 1.  void swap( vector& other );
//  *
//  * NON-MEMBER FUNCTIONS
//  *   - [x] 1.  template< class T, class Alloc >
//  *       bool operator==( const std::vector<T,Alloc>& lhs, const
//  std::vector<T,Alloc>& rhs );
//  *   - [x] 2.  template< class T, class Alloc >
//  *       bool operator!=( const std::vector<T,Alloc>& lhs, const
//  std::vector<T,Alloc>& rhs );
//  *   - [x] 3.  template< class T, class Alloc >
//  *       bool operator<( const std::vector<T,Alloc>& lhs, const
//  std::vector<T,Alloc>& rhs );
//  *   - [x] 4.  template< class T, class Alloc >
//  *       bool operator<=( const std::vector<T,Alloc>& lhs, const
//  std::vector<T,Alloc>& rhs );
//  *   - [x] 5.  template< class T, class Alloc >
//  *       bool operator>( const std::vector<T,Alloc>& lhs, const
//  std::vector<T,Alloc>& rhs );
//  *   - [x] 6.  template< class T, class Alloc >
//  *       bool operator>=( const std::vector<T,Alloc>& lhs, const
//  std::vector<T,Alloc>& rhs );
//  *   - [x] 1.  template< class T, class Alloc >
//  *       void swap( std::vector<T,Alloc>& lhs, std::vector<T,Alloc>& rhs );
