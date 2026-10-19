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

#include <queue>
#include <functional>

namespace Coffee
{
    namespace Async
    {
        struct Dispatcher {
			using method_t = std::function<void()>;

			Dispatcher() = default;
			Dispatcher(const Dispatcher&) = delete;
			Dispatcher& operator=(const Dispatcher&) = delete;

			~Dispatcher();

		public:
			void init();
			void update();
			void postTask(method_t method);
			
		private:
			Mutex m_mutex;
			std::queue<method_t> m_queue;
		};
    }
}