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

#include "coffee/collections/flags.h"
#include "coffee/control/analog.h"

namespace Coffee
{
    struct Motor
    {
        enum State : int {
            Invalid = -1,   
            Stopped = 0,
            Running = 1,
        };

        Motor();

        int setup(uint8_t pinEnable, uint8_t pinLeftPWM, uint8_t pinRightPWM, double minValue = -12.0, double maxValue = 12.0);
        int teardown();

        void start(double value = 0.0);
        void set(double value);
        double get()const;
        void stop();

        template<State STATE>
        bool is()const { return m_state == STATE; }

        template<State STATE>
        bool isNot()const { return m_state != STATE; }

        operator bool()const { return isNot<Invalid>(); }
        bool operator!()const { return is<Invalid>(); }

    private:
        static FlagSet<16> s_channels;

        enum : uint32_t {
            frequency = 5000,
            resolution = 10,
            maxDuty = (1ul << resolution) - 1
        };
        State m_state;
        double m_value;
        double m_minValue;
        double m_maxValue;
        double m_maxAbsValue;
        uint8_t m_pinEnable;
        uint32_t m_channelLeftPWM;
        uint32_t m_channelRightPWM;
    };
}
