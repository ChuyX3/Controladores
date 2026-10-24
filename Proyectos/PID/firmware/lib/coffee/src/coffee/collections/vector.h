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
    /* Vector container with max fixed size and optional default size of 16 */
   template<typename T, uint32_t SIZE = 16>
    struct Vector
    {
        Vector()
        {
            coffee::constexpr_if<std::is_trivially_constructible<T>::value && std::is_trivially_destructible<T>::value>(
                [&](auto...)
            {
                memset(m_data, 0, sizeof(m_data));
            });
        }
        
        template<typename...Args>
        Vector(Args&&...args)
            : m_data{std::forward<Args>(args)...}
        {
        }

        ~Vector() {
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
        T* m_first;
        size_t m_count;
        T m_data[SIZE];
    }; 
}