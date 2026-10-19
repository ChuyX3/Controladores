
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

	struct StepTimer
	{
		StepTimer();
        
	public: /*properties*/
		uint64_t deltaTimeUs() const;
		uint64_t totalTimeUs() const;
		double deltaTime() const;
		double totalTime() const;
		uint32_t tickCount() const;
		double tickRate() const;
		void fixedStepTime(uint64_t us = 0);

	public: /*methods*/
		void begin(uint64_t stepTimeUs = 0);
		void reset();
		void update();

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