
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

int Motor::setup(uint8_t pinEnable, uint8_t pinLeftPWM, uint8_t pinRightPWM, double minValue, double maxValue)
{
    m_value = 0.0;
    m_isRunning = false;
    m_pinEnable = pinEnable;
    m_minValue = std::min(minValue, maxValue);
    m_maxValue = std::max(maxValue, minValue);
    m_maxAbsValue = std::max(std::abs(m_minValue), std::abs(m_maxValue));
    m_maxAbsValue = m_maxAbsValue? m_maxAbsValue : 1.0; // Ensure max absolute value is not zero

    // Check if there are available LEDC channels for the motor
    if(s_channel >= 16 && (m_channelLeftPWM == -1 || m_channelRightPWM == -1)) // ESP32 has 16 LEDC channels
        return -1; 

    m_channelLeftPWM = m_channelLeftPWM == -1 ? (s_channel++) : m_channelLeftPWM;
    m_channelRightPWM = m_channelRightPWM == -1 ? (s_channel++) : m_channelRightPWM;

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


    Serial.println("Motor setup complete.");
    Serial.println("Motor channels: Left PWM = " + String(m_channelLeftPWM) + ", Right PWM = " + String(m_channelRightPWM));
    Serial.println("Motor enable pin = " + String(m_pinEnable));
    Serial.println("Motor min value = " + String(m_minValue) + ", max value = " + String(m_maxValue));
    Serial.println("Motor max absolute value = " + String(m_maxAbsValue));
    Serial.println("Motor frequency = " + String(frequency) + ", resolution = " + String(resolution));
    Serial.println("Motor max duty = " + String(maxDuty));

   return 0;
}

void Motor::start(double value)
{
    m_isRunning = true;
    digitalWrite(m_pinEnable, HIGH);
    set(value);
    Serial.println("Motor started with value = " + String(value));
}

void Motor::set(double value)
{
    uint32_t duty = 0;
    if(m_isRunning) {
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
    m_value = 0.0;
    m_isRunning = false;
    ledcWrite(m_channelLeftPWM, 0);
    ledcWrite(m_channelRightPWM, 0);
    digitalWrite(m_pinEnable, LOW);
    Serial.println("Motor stopped and disabled.");
}