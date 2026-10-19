
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

#include "coffee/steptimer.h"

using namespace Coffee;

StepTimer::StepTimer()
    : m_targetDeltaTime(0)
    , m_lastTime(0)
    , m_deltaTime(0)
    , m_totalTime(0)
    , m_leftOverTime(0)
    , m_tickCount(0)
{
}

uint64_t StepTimer::deltaTimeUs() const
{
    return m_deltaTime;
}

uint64_t StepTimer::totalTimeUs() const
{
    return m_totalTime;
}

double StepTimer::deltaTime() const
{
    return static_cast<double>(m_deltaTime) / 1000000.0;
}

double StepTimer::totalTime() const
{
    return static_cast<double>(m_totalTime) / 1000000.0;
}

uint32_t StepTimer::tickCount() const
{
    return m_tickCount;
}

double StepTimer::tickRate() const
{
    return (m_totalTime > 0) ? (static_cast<double>(m_tickCount) / static_cast<double>(m_totalTime)) * 1000000.0 : 0.0;
}

void StepTimer::fixedStepTime(uint64_t us)
{
    m_targetDeltaTime = us;
}

void StepTimer::begin(uint64_t stepTimeUs)
{
    m_targetDeltaTime = stepTimeUs;
    m_lastTime = micros();
    m_deltaTime = 0;
    m_totalTime = 0;
    m_leftOverTime = 0;
    m_tickCount = 0;
}

void StepTimer::reset()
{
    //m_targetDeltaTime = 0;
    m_lastTime = micros();
    m_deltaTime = 0;
    m_totalTime = 0;
    m_leftOverTime = 0;
    m_tickCount = 0;
}

void StepTimer::update()
{
    uint64_t currentTime = micros();
    m_deltaTime = currentTime - m_lastTime;
    m_lastTime = currentTime;
    m_totalTime += m_deltaTime;
    m_leftOverTime = 0;
    m_tickCount++;
}
