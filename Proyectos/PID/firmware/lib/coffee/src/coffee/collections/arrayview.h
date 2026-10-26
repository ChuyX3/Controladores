/* 
 * Copyright (c) 2023 Jesus Angel Rocha Morales.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "coffee.h"
#include "coffee/collections/iterators.h"

COFFEE_NAMESPACE_BEGIN(Coffee, implement)

COFFEE_NAMESPACE_BEGIN(collections)
template<typename T>
struct array_view {
    using forward_iterator = coffee::collections::iterator<array_view<T>, T, true>;
    using reverse_iterator = coffee::collections::iterator<array_view<T>, T, false>;
    using const_forward_iterator = coffee::collections::iterator<array_view<T>, const T, true>;
    using const_reverse_iterator = coffee::collections::iterator<array_view<T>, const T, false>;

    friend base_reference<array_view<T>>;

    array_view()
        : m_first(nullptr)
        , m_last(nullptr)
    {
    }
    array_view(T* first, T* last)
        : m_first(first)
        , m_last(last)
    {
        assert(first <= last);
    }
    template<uint32_t N>
    array_view(T(&ar)[N])
        : m_first((T*)ar)
        , m_last((T*)ar + N)
    {
    }
    array_view(array_view const& other)
        : m_first(other.m_first)
        , m_last(other.m_last)
    {
    }
    array_view(array_view && other)
        : m_first(std::move(other.m_first))
        , m_last(std::move(other.m_last))
    {
    }
    size_t size()const { return m_last - m_first; }
    T* data() { return m_first; }
    T const* data()const { return m_first; }
    
    forward_iterator begin() { return forward_iterator(this, 0, 0); }
    forward_iterator end() { return forward_iterator(this, invalid_index, invalid_index); }
    reverse_iterator rbegin() { return reverse_iterator(this, size() - 1, size() - 1); }
    reverse_iterator rend() { return reverse_iterator(this, invalid_index, invalid_index); }
    const_forward_iterator begin()const { return const_forward_iterator(const_cast<array_view<T>*>(this), 0, 0); }
    const_forward_iterator end()const { return const_forward_iterator(const_cast<array_view<T>*>(this), invalid_index, invalid_index); }
    const_reverse_iterator rbegin()const { return const_reverse_iterator(const_cast<array_view<T>*>(this), size() - 1, size() - 1); }
    const_reverse_iterator rend()const { return const_reverse_iterator(const_cast<array_view<T>*>(this), invalid_index, invalid_index); }

    T& operator [](int32_t index){
        return m_first[index];
    }
    T const& operator [](int32_t index)const {
        return m_first[index];
    }

protected:
    T* get(base_reference<array_view<T>> const* ref) {
        if(!ref->valid())
            return nullptr;

        if(ref->m_index >= size())
            return nullptr;

        return m_first + ref->m_index; 
    }

    void increment(base_reference<array_view<T>>* ref, int32_t offset) {
        
        if(!ref->valid())
            return;

        size_t pos = ref->m_index + offset;

        if(offset < 0)
        {
            if(static_cast<size_t>(-offset) > ref->m_index)
            {
                ref->m_index  = invalid_index;
                ref->m_offset = invalid_index;
                return;
            }
        }

        if(pos >= size())
        {
            ref->m_index  = invalid_index;
            ref->m_offset = invalid_index;
            return;
        }

        ref->m_index  = pos;
        ref->m_offset += offset;

    }

    void decrement(base_reference<array_view<T>>* ref, int32_t offset) {
        increment(ref, -offset);
    }

protected:
    T* m_first;
    T* m_last;
};

template<typename T>
struct array_view<T const> {
    using forward_iterator = coffee::collections::iterator<array_view<T>, const T, true>;
    using reverse_iterator = coffee::collections::iterator<array_view<T>, const T, false>;

    friend base_reference<array_view<const T>>;

    array_view()
        : m_first(nullptr)
        , m_last(nullptr)
    {
    }
    array_view(T const* first, T const* last)
        : m_first(first)
        , m_last(last)
    {
        assert(first <= last);
    }
    template<size_t N>
    array_view(const T(&ar)[N])
        : m_first((T const*)ar)
        , m_last((T const*)ar + N)
    {
    }
    array_view(array_view const& other)
        : m_first(other.m_first)
        , m_last(other.m_last)
    {
    }
    array_view(array_view && other)
        : m_first(std::move(other.m_first))
        , m_last(std::move(other.m_last))
    {
    }

    size_t size()const { return m_last - m_first; }
    T const* data()const { return m_first; }
    forward_iterator begin()const { return forward_iterator(this, 0, 0); }
    forward_iterator end()const { return forward_iterator(this, invalid_index, invalid_index); }
    reverse_iterator rbegin()const { return reverse_iterator(this, size() - 1, size() - 1); }
    reverse_iterator rend()const { return reverse_iterator(this, invalid_index, invalid_index); }


    T const& operator [](int32_t index)const {
        return m_first[index];
    }

protected:
    T const* get(base_reference<array_view<const T>> const* ref) {
        if(!ref->valid())
            return nullptr;

        if(ref->m_index >= size())
            return nullptr;

        return m_first + ref->m_index; 
    }

    void increment(base_reference<array_view<const T>>* ref, int32_t offset) {
        
        if(!ref->valid())
            return;

        size_t pos = ref->m_index + offset;

        if(offset < 0)
        {
            if(static_cast<size_t>(-offset) > ref->m_index)
            {
                ref->m_index  = invalid_index;
                ref->m_offset = invalid_index;
                return;
            }
        }

        if(pos >= size())
        {
            ref->m_index  = invalid_index;
            ref->m_offset = invalid_index;
            return;
        }

        ref->m_index  = pos;
        ref->m_offset += offset;

    }

    void decrement(base_reference<array_view<const T>>* ref, int32_t offset) {
        increment(ref, -offset);
    }

protected:
    T const* m_first;
    T const* m_last;
};

COFFEE_NAMESPACE_END(collections)

COFFEE_NAMESPACE_BEGIN(algorithms)
template<typename T>
inline size_t array_size(collections::array_view<T> const& view) { return view.size(); }
COFFEE_NAMESPACE_END(algorithms)

COFFEE_NAMESPACE_END(implement, Coffee)

namespace Coffee
{
    template<typename T>
    using ArrayView = coffee::collections::array_view<T>;
}
