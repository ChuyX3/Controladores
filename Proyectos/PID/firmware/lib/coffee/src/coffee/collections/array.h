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
    template<typename T, size_t SIZE>
    struct Array {
        using self_t = Array<T, SIZE>;
        Array()
        {
        }
        
        Array(std::initializer_list<T>const& args)
            : m_data{}
        {
            size_t i = 0;
            for(auto const& arg : args){
                m_data[i++] = arg;
                if(i == SIZE)
                    break;
            }
                
        }
        Array(T(&other)[SIZE])
        {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = other[i];
        }
        Array(Array const& other)
        {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = other[i];
        }
        Array(Array && other) noexcept
        {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = coffee::move(other[i]);
        }

        Array& operator = (Array const& other) {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = other[i];
            return*this;
        }
        Array& operator = (Array && other) noexcept {
            for(size_t i = 0; i < SIZE; i++)
                m_data[i] = coffee::move(other[i]);
            return*this;
        }

        constexpr size_t size()const { return SIZE; }
        T* data() { return m_data; }
        T const* data()const { return m_data; }
        T* begin() { return m_data; }
        T* end() { return m_data + SIZE; }
        T const* begin()const { return m_data; }
        T const* end()const { return m_data + SIZE; }

        operator ArrayView<T>() { return ArrayView<T>(m_data, m_data + SIZE); }
        operator ArrayView<T const>() const { return ArrayView<T const>(m_data, m_data + SIZE); }

        T& operator [](int32_t index){
            return m_data[index];
        }
        T const& operator [](int32_t index)const {
            return m_data[index];
        }
    protected:
        T m_data[SIZE];
    };
}
