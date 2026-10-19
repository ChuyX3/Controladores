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

#include "coffee/control/analog.h"

namespace Coffee
{
    struct Encoder
    {
        Encoder();

        void begin(uint32_t pinA, uint32_t pinB);
        void reset(int64_t value = 0);
        Analog<double> position()const;
        Analog<double> speed()const;

        void setOffset(double offset);
        double getOffset()const;
        void setScale(double scale);
        double getScale()const;

    private:
        inline uint8_t update() {
            m_state = m_state >> 2;
            m_state |= gpio_get_level((gpio_num_t)m_pinA)? B0100 : B0000;
            m_state |= gpio_get_level((gpio_num_t)m_pinB)? B1000 : B0000;
            return m_state;
        }
        mutable portMUX_TYPE  m_mux;
        volatile int64_t m_position;
        volatile int64_t m_prevPosition;
        volatile int64_t m_time;
        volatile int64_t m_prevTime;
        volatile uint8_t m_state;
        double m_offset;
        double m_scale;
        uint8_t m_pinA;
        uint8_t m_pinB;

    private:
        static void onPinChanged(void*);
    };
}