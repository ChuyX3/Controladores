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

	struct StepTimer
	{
		inline StepTimer()
			: m_targetDeltaTime(0)
			, m_lastTime(0)
			, m_deltaTime(0)
			, m_totalTime(0)
			, m_leftOverTime(0)
			, m_tickCount(0)
		{
		}

	public: /*properties*/
		inline uint64_t deltaTimeUs() const{
			return m_deltaTime;
		}
		inline uint64_t totalTimeUs() const{
			return m_totalTime;
		}
		inline double deltaTime() const{
			return static_cast<double>(m_deltaTime) / 1000000.0;
		}
		inline double totalTime() const{
			return static_cast<double>(m_totalTime) / 1000000.0;
		}
		inline uint32_t tickCount() const{
			return m_tickCount;
		}
		inline double tickRate() const{
			return (m_totalTime > 0) ? (static_cast<double>(m_tickCount) / static_cast<double>(m_totalTime)) * 1000000.0 : 0.0;
		}
		inline void fixedStepTime(uint64_t us = 0){
			m_targetDeltaTime = us;
		}
		
	public: /*methods*/
		inline void begin(uint64_t stepTimeUs = 0){
			m_targetDeltaTime = stepTimeUs;
			m_lastTime = micros();
			m_deltaTime = 0;
			m_totalTime = 0;
			m_leftOverTime = 0;
			m_tickCount = 0;
		}
		inline void reset(){
			//m_targetDeltaTime = 0;
			m_lastTime = micros();
			m_deltaTime = 0;
			m_totalTime = 0;
			m_leftOverTime = 0;
			m_tickCount = 0;
		}
		inline void update(){
			uint64_t currentTime = micros();
			m_deltaTime = currentTime - m_lastTime;
			m_lastTime = currentTime;
			m_totalTime += m_deltaTime;
			m_leftOverTime = 0;
			m_tickCount++;
		}

		template<typename T, typename...Ts>
		inline void tick(const T& func, Ts&&...args) {
			uint64_t currentTime = micros();
			uint64_t timeDelta = currentTime - m_lastTime;
			m_lastTime = currentTime;

			if (m_targetDeltaTime > 0) {
				m_leftOverTime += timeDelta;
				while (m_leftOverTime >= m_targetDeltaTime) {
					m_deltaTime = m_targetDeltaTime;
					m_totalTime += m_targetDeltaTime;
					m_leftOverTime -= m_targetDeltaTime;
					m_tickCount++;
					func(std::forward<Ts>(args)...);
				}
			}
			else {
				m_deltaTime = timeDelta;
				m_totalTime += timeDelta;
				m_leftOverTime = 0;
				m_tickCount++;
				func(std::forward<Ts>(args)...);
			}
		}

	private:
		uint64_t m_targetDeltaTime;
		uint64_t m_lastTime;
		uint64_t m_deltaTime;
		uint64_t m_totalTime;
		uint64_t m_leftOverTime;
		uint32_t m_tickCount;
	};
}