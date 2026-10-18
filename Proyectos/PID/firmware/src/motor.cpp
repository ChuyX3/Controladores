
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

#include <Arduino.h>
#include "motor.h"

using namespace Coffee;

int Motor::s_channel = 0;

Motor::Motor()
    : m_value(0.0)
    , m_minValue(-1.0)
    , m_maxValue(1.0)
    , m_pinEnable(0)
    , m_channelLeftPWM(-1)
    , m_channelRightPWM(-1)
    , m_isRunning(0)
{

}

void Motor::setup(uint8_t pinEnable, uint8_t pinLeftPWM, uint8_t pinRightPWM, double minValue, double maxValue)
{
    m_value = 0.0;
    m_isRunning = false;
    m_pinEnable = pinEnable;
    m_minValue = std::min(minValue, maxValue);
    m_maxValue = std::max(maxValue, minValue);
    m_maxAbsValue = std::max(std::abs(m_minValue), std::abs(m_maxValue));

    m_channelLeftPWM = m_channelLeftPWM == -1 ? (s_channel++) : m_channelLeftPWM;
    m_channelRightPWM = m_channelRightPWM == -1 ? (s_channel++) : m_channelRightPWM;

    ledcSetup(m_channelLeftPWM, frequency, resolution);
    ledcAttachPin(pinLeftPWM, m_channelLeftPWM);
    ledcSetup(m_channelRightPWM, frequency, resolution);
    ledcAttachPin(pinRightPWM, m_channelRightPWM);

    ledcWrite(m_channelLeftPWM, 0);
    ledcWrite(m_channelRightPWM, 0);

    pinMode(m_pinEnable, OUTPUT);
    pinMode(pinLeftPWM, OUTPUT);
    pinMode(pinRightPWM, OUTPUT);
}

void Motor::start()
{
    m_isRunning = true;
    digitalWrite(m_pinEnable, HIGH);
    set(m_value);
}

void Motor::set(double value)
{
    m_value = std::min(std::max(value, m_minValue), m_maxValue);
    uint32_t duty = 0;
    uint32_t maxDuty = (1 << resolution) - 1;

    if(m_isRunning){
        if(m_value > 0.0){
            duty = static_cast<uint32_t>(std::round((m_value / m_maxAbsValue) * maxDuty));
            ledcWrite(m_channelLeftPWM, duty);
            ledcWrite(m_channelRightPWM, 0);
        }
        else if(m_value < 0.0){
            m_value = m_value * -1.0;
            duty = static_cast<uint32_t>(std::round((m_value / m_maxAbsValue) * maxDuty));
            ledcWrite(m_channelLeftPWM, 0);
            ledcWrite(m_channelRightPWM, duty); 
        }
        else{
            ledcWrite(m_channelLeftPWM, 0);
            ledcWrite(m_channelRightPWM, 0);
        }
    }
}

double Motor::get()const
{
    return m_value;
}

void Motor::stop()
{
    m_isRunning = false;
    ledcWrite(m_channelLeftPWM, 0);
    ledcWrite(m_channelRightPWM, 0);
    digitalWrite(m_pinEnable, LOW);
}