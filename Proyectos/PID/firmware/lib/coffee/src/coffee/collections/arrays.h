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
            : m_first(min(first, last))
            , m_last(max(first, last))
        {
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
            , m_last(std::move(other.m_first))
        {
        }
        T* data()const { return m_first; }
        size_t size()const { return (size_t(m_last) - size_t(m_first)) / sizeof(T); }
        T* begin()const { return m_first; }
        T* end()const { return m_last; }

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
}

namespace Coffee
{
    template<typename T, size_t SIZE>
    struct FixedArray {
        using self_t = FixedArray<T, SIZE>;
        FixedArray() {
        }
        
        FixedArray(std::initializer_list<T>const& args)
            : m_data{}
        {
            size_t i = 0;
            for(auto const& arg : args){
                m_data[i++] = arg;
                if(i == SIZE)
                    break;
            }
                
        }
        FixedArray(T(&other)[SIZE]) {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = other[i];
        }
        FixedArray(FixedArray const& other) {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = other[i];
        }
        FixedArray(FixedArray && other) {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = other[i];
        }

        T* data()const { return (T*)m_data; }
        size_t size()const { return SIZE; }
        T* begin()const { return (T*)m_data; }
        T* end()const { return (T*)m_data + SIZE; }

        FixedArray& operator = (FixedArray const& other) {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = other[i];
            return*this;
        }
        FixedArray& operator = (FixedArray && other) {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = other[i];
            return*this;
        }
        operator ArrayView<T> ()const {
            return ArrayView<T>(const_cast<self_t*>(this)->m_data);
        }
        T& operator [](int32_t index) {
            return m_data[index];
        }
        T const& operator [](int32_t index)const {
            return m_data[index];
        }
    protected:
        T m_data[SIZE];
    };
}

namespace Coffee
{
   template<typename T, uint32_t SIZE>
    struct RingArray : ArrayView<T>
    {
        RingArray()
            : ArrayView<T>(m_data, m_data + SIZE)
        {
            coffee::constexpr_if<std::is_trivially_constructible<T>::value && std::is_trivially_destructible<T>::value>(
                [&](auto...)
            {
                memset(m_data, 0, sizeof(m_data));
            });
        }
        
        template<typename...Args>
        RingArray(Args&&...args)
            : ArrayView<T>(m_data, m_data + SIZE)
            , m_data{std::forward<Args>(args)...}
        {
        }

        ~RingArray() {
            coffee::constexpr_if<!std::is_trivially_destructible<T>::value>(
                [&](auto...)
            {
                for(T& value : m_data)
                {
                    value.~T();//destroy
                }   
            });
        }

        void reset() {
            if(coffee::constexpr_if<!std::is_trivially_destructible<T>::value || !std::is_trivially_constructible<T>::value>(
                [&](auto...)
            {
                for(T& value : m_data)
                {
                    value.~T();//destroy
                    new (std::addressof(value)) T(); //construct
                }
            }));
            else 
            {
                memset(m_data, 0, sizeof(m_data));
            };

        }
        int32_t getIndex(int32_t from, int32_t to)const{
            int32_t sign = (to >> 31) | 1; 
            return (SIZE + ((sign * to) % SIZE) * sign + from) % SIZE;
        }
        T& get(int32_t at){
            return m_data[getIndex(0, at)];
        }
        T const& get(int32_t at)const{
            return m_data[getIndex(0, at)];
        }
        void set(int32_t at, T const& value) {
            m_data[getIndex(0, at)] = value;
        }

        T& operator [](int32_t index){
            return get(index);
        }
        T const& operator [](int32_t index)const {
            return get(index);
        }
    protected:
        T m_data[SIZE];
    }; 
}