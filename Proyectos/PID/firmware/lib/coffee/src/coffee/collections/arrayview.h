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

namespace Coffee
{
    template<typename T>
    struct ArrayView {
        ArrayView()
            : m_first(nullptr)
            , m_last(nullptr)
        {
        }
        ArrayView(T* first, T* last)
            : m_first(first)
            , m_last(last)
        {
            assert(first <= last);
        }
        template<uint32_t N>
        ArrayView(T(&ar)[N])
            : m_first((T*)ar)
            , m_last((T*)ar + N)
        {
        }
        ArrayView(ArrayView const& other)
            : m_first(other.m_first)
            , m_last(other.m_last)
        {
        }
        ArrayView(ArrayView && other)
            : m_first(std::move(other.m_first))
            , m_last(std::move(other.m_last))
        {
        }
        size_t size()const { return m_last - m_first; }
        T* data() { return m_first; }
        T const* data()const { return m_first; }
        T* begin() { return m_first; }
        T* end() { return m_last; }
        T const* begin()const { return m_first; }
        T const* end()const { return m_last; }

        T& operator [](int32_t index){
            return m_first[index];
        }
        T const& operator [](int32_t index)const {
            return m_first[index];
        }
    protected:
        T* m_first;
        T* m_last;
    };

    template<typename T>
    struct ArrayView<T const> {
        ArrayView()
            : m_first(nullptr)
            , m_last(nullptr)
        {
        }
        ArrayView(T const* first, T const* last)
            : m_first(first)
            , m_last(last)
        {
            assert(first <= last);
        }
        template<size_t N>
        ArrayView(const T(&ar)[N])
            : m_first((T const*)ar)
            , m_last((T const*)ar + N)
        {
        }
        ArrayView(ArrayView const& other)
            : m_first(other.m_first)
            , m_last(other.m_last)
        {
        }
        ArrayView(ArrayView && other)
            : m_first(std::move(other.m_first))
            , m_last(std::move(other.m_last))
        {
        }

        size_t size()const { return m_last - m_first; }
        T const* data()const { return m_first; }
        T const* begin()const { return m_first; }
        T const* end()const { return m_last; }

        T const& operator [](int32_t index)const {
            return m_first[index];
        }

    protected:
        T const* m_first;
        T const* m_last;
    };

}
