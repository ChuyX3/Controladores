
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

#include "coffee/control/motor.h"

using namespace Coffee;


FlagSet<16> Motor::s_channels = {};

Motor::Motor()
    : m_state(Motor::State::Invalid)
    , m_value(0.0)
    , m_minValue(-1.0)
    , m_maxValue(1.0)
    , m_pinEnable(0)
    , m_channelLeftPWM(coffee::invalid_index)
    , m_channelRightPWM(coffee::invalid_index)
{

}

int Motor::setup(uint8_t pinEnable, uint8_t pinLeftPWM, uint8_t pinRightPWM, double minValue, double maxValue)
{
    if(isNot<Invalid>())
        return -1; // Motor is not in an invalid state, cannot setup again

    m_value = 0.0;
    m_pinEnable = pinEnable;
    m_minValue = std::min(minValue, maxValue);
    m_maxValue = std::max(maxValue, minValue);
    m_maxAbsValue = std::max(std::abs(m_minValue), std::abs(m_maxValue));
    m_maxAbsValue = m_maxAbsValue? m_maxAbsValue : 1.0; // Ensure max absolute value is not zero

    m_channelLeftPWM = findFirstUnset(s_channels);
    if(m_channelLeftPWM == coffee::invalid_index)
        return -1;
    m_channelRightPWM = findFirstUnset(s_channels, m_channelLeftPWM + 1);
    if(m_channelRightPWM == coffee::invalid_index) {
        m_channelLeftPWM = coffee::invalid_index;
        return -1;
    }

    pinMode(m_pinEnable, OUTPUT);
    pinMode(pinLeftPWM, OUTPUT);
    pinMode(pinRightPWM, OUTPUT);

    digitalWrite(m_pinEnable, LOW);
    digitalWrite(pinLeftPWM, LOW);
    digitalWrite(pinRightPWM, LOW);

    ledcSetup(m_channelLeftPWM, frequency, resolution);
    ledcAttachPin(pinLeftPWM, m_channelLeftPWM);
    ledcSetup(m_channelRightPWM, frequency, resolution);
    ledcAttachPin(pinRightPWM, m_channelRightPWM);

    ledcWrite(m_channelLeftPWM, 0);
    ledcWrite(m_channelRightPWM, 0);

    s_channels.set(m_channelRightPWM);
    s_channels.set(m_channelLeftPWM);

    m_state = State::Stopped;

   return 0;
}

int Motor::teardown()
{
    if(is<Invalid>())
        return -1;
    stop();
    s_channels.reset(m_channelLeftPWM);
    s_channels.reset(m_channelRightPWM);
    m_channelLeftPWM = coffee::invalid_index;
    m_channelRightPWM = coffee::invalid_index;
    m_state = State::Invalid;
    return 0;
}

void Motor::start(double value)
{
    if(is<Invalid>())
        return;
    m_state = State::Running;
    digitalWrite(m_pinEnable, HIGH);
    set(value);
}

void Motor::set(double value)
{
    uint32_t duty = 0;
    if(is<Running>()) {
        /*Clamp the value within the min and max range*/
        value = std::min(std::max(value, m_minValue), m_maxValue);
        m_value = value;
        value = std::abs(value);
        duty = static_cast<uint32_t>(std::round((value / m_maxAbsValue) * maxDuty));
        Serial.println("Motor set with value = " + String(m_value) + ", duty = " + String(duty));
        if(m_value > 0.0){
            ledcWrite(m_channelLeftPWM, duty);
            ledcWrite(m_channelRightPWM, 0);
            Serial.println("Motor running forward with duty = " + String(duty));
        }
        else if(m_value < 0.0){
            ledcWrite(m_channelLeftPWM, 0);
            ledcWrite(m_channelRightPWM, duty); 
            Serial.println("Motor running backward with duty = " + String(duty));
        }
        else{
            ledcWrite(m_channelLeftPWM, 0);
            ledcWrite(m_channelRightPWM, 0);
            Serial.println("Motor stopped.");
        }
    }
}

double Motor::get()const
{
    return m_value;
}

void Motor::stop()
{
    if(is<Invalid>())
        return;
    m_value = 0.0;
    m_state = State::Stopped;
    ledcWrite(m_channelLeftPWM, 0);
    ledcWrite(m_channelRightPWM, 0);
    digitalWrite(m_pinEnable, LOW);
}
