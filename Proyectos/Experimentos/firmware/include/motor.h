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

#include <Arduino.h>
#include <stdint.h>

namespace Coffee
{
    struct Motor
    {
        Motor();

        int setup(uint8_t pinEnable, uint8_t pinLeftPWM, uint8_t pinRightPWM, double minValue = -12.0, double maxValue = 12.0);
        
        void start(double value = 0.0);
        void set(double value);
        double get()const;
        void stop();

    private:
        enum : uint32_t {
            frequency = 5000,
            resolution = 10,
            maxDuty = (1ul << resolution) - 1
        };
        static int s_channel;
        double m_value;
        double m_minValue;
        double m_maxValue;
        double m_maxAbsValue;
        uint8_t m_pinEnable;
        int m_channelLeftPWM;
        int m_channelRightPWM;

        bool m_isRunning;
    };
}