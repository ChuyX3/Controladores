
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

#include "coffee/control/encoder.h"

using namespace Coffee;

//                           _______         _______       
//               PinA ______|       |_______|       |______ PinA
// backward <---         _______         _______         __      --> forward
//               PinB __|       |_______|       |_______|   PinB
//	pin2	pin1	pin2	pin1	Result
//	----	----	----	----	------
//	    0	    0	    0	    0	no movement
//	    0	    0	    0	    1	    +1
//	    0	    0	    1	    0	    -1
//	    0   	0	    1	    1	    +2  (assume pin1 edges only)
//	    0	    1	    0	    0	    -1
//	    0	    1	    0	    1	no movement
//	    0	    1	    1	    0	    -2  (assume pin1 edges only)
//	    0	    1	    1	    1   	+1
//	    1	    0	    0	    0	    +1
//	    1	    0	    0	    1	    -2  (assume pin1 edges only)
//	    1	    0	    1   	0	no movement
//	    1	    0	    1	    1	    -1
//  	1	    1	    0	    0	    +2  (assume pin1 edges only)
//	    1	    1	    0	    1	    -1
//	    1	    1	    1	    0	    +1
//	    1	    1	    1	    1	no movement

void IRAM_ATTR Encoder::onPinChanged(void* args) {
    auto encoder = reinterpret_cast<Encoder*>(args);
    uint8_t state = encoder->update();
    portENTER_CRITICAL_ISR(&encoder->m_mux);
    encoder->m_prevPosition = encoder->m_position;
    encoder->m_prevTime = encoder->m_time;
    encoder->m_time = esp_timer_get_time();
    switch (state)
    {
    case B0001: case B0111: case B1000: case B1110:
        encoder->m_position++;
        break;
    case B0010: case B0100: case B1011: case B1101:
        encoder->m_position--;
        break;
    case B0011: case B1100:
        encoder->m_position += 2;
        break;
    case B0110: case B1001:
        encoder->m_position -= 2;
        break;
    }
    portEXIT_CRITICAL_ISR(&encoder->m_mux);
}

Encoder::Encoder()
    : m_mux(portMUX_INITIALIZER_UNLOCKED)
    , m_position(0)
    , m_pinA(0)
    , m_pinB(0)
    , m_state(0)
    , m_offset(0.0)
    , m_scale(1.0)
{
}

void Encoder::begin(uint32_t pinA, uint32_t pinB)
{
    m_pinA = pinA;
    m_pinB = pinB;
    pinMode(m_pinA, INPUT_PULLUP);
    pinMode(m_pinB, INPUT_PULLUP);
    digitalWrite(m_pinA, HIGH);
    digitalWrite(m_pinB, HIGH);
    delay(2);
    m_state = 0;
    m_position = 0;
    update();
    attachInterruptArg(digitalPinToInterrupt(m_pinA), &Encoder::onPinChanged, this, CHANGE);
    attachInterruptArg(digitalPinToInterrupt(m_pinB), &Encoder::onPinChanged, this, CHANGE);
}

void Encoder::reset(int64_t value) { /*degrees*/
    portENTER_CRITICAL(&m_mux);
    m_position = value;
    m_prevPosition = value;
    m_time = esp_timer_get_time();
    m_prevTime = m_time;
    portEXIT_CRITICAL(&m_mux);
}

Analog<double> Encoder::position()const { /*degrees*/
    portENTER_CRITICAL(&m_mux);
    Analog<double> result((double)m_position, m_scale, m_offset);
    portEXIT_CRITICAL(&m_mux);
    return result;
}

Analog<double> Encoder::speed()const { /*degrees*/
    portENTER_CRITICAL(&m_mux);
    double deltaTime = coffee::max(1e6 *(double)(m_time - m_prevTime), 1e-6); // Prevent division by zero
    Analog<double> result((double)(m_position - m_prevPosition) / deltaTime, m_scale, 0);
    portEXIT_CRITICAL(&m_mux);
    return result;
}

void Encoder::setOffset(double offset) {
    m_offset = offset;
}

double Encoder::getOffset()const {
    auto res = m_offset;
    return res;
}

void Encoder::setScale(double scale) {
    m_scale = scale;
}

double Encoder::getScale()const {
    auto res = m_scale;
    return res;
}