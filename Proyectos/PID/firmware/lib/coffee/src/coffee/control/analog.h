
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
    struct Analog {
        inline Analog(T value = 0, T scale = 1, T offset = 0) 
            : m_value(value)
            , m_scale(scale)
            , m_offset(offset)
        {
        }

        inline Analog(Analog const& input) = default;
        inline Analog(Analog && input) = default;
        inline Analog& operator = (Analog const& input) = default;
        inline Analog& operator = (Analog && input) = default;

        inline Analog& operator = (T const& value) {
            setRawValue(value);
            return *this;
        }

        inline operator T ()const{
            return getValue();
        }

    public: // Public interface for accessing and modifying analog values
        inline T getValue()const { return m_value * m_scale + m_offset; }
        inline T getRawValue()const { return m_value; }
        inline T getScale()const { return m_scale; }
        inline T getOffset()const { return m_offset; }

        inline void setRawValue(T value) { m_value = value; }
        inline void setScale(T value) { m_scale = value; }
        inline void setOffset(T value) { m_offset = value; }

    private:
        T m_value;
        T m_scale;
        T m_offset;
    };

}