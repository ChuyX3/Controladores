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
#include "coffee/async/mutex.h"

namespace Coffee
{
    namespace Async
    {
        struct Event
		{
		private: //Non copiable
			Event(const Event&) = delete;
			Event& operator = (const Event&) = delete;

		protected:
            mutable pthread_cond_t m_handle;

		public:
			Event();
			~Event();

		public: //Properties
			void wait(Mutex const& mutex)const;
			bool wait(Mutex const& mutex, unsigned long ms)const;
			void signal()const;

			template<typename FNType>
			void waitFor(Mutex const& mutex, FNType cond)const {
				while (!cond()) { wait(mutex); }
			}

			template<typename FNType>
			bool waitFor(Mutex const& mutex, unsigned long ms, FNType cond)const {
				if (!cond()) { wait(mutex, ms); }
				return cond();
			}
		};

    }
}