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

#include "pid.h"
#include "coffee/async/mutex.h"
#include "coffee/async/dispatcher.h"

namespace Coffee
{
	template<typename Subclass>
	struct AsyncWorker {
		using self = Subclass;
		using method_t = std::function<void()>;

		AsyncWorker() = default;
		~AsyncWorker() = default;

	private:
		enum {
            ASYNCWORKER_CORE = 0,
            ASYNCWORKER_PRIORITY = 1,
            ASYNCWORKER_STACK = 30720,
        };

		SystemState m_taskState = SystemState::Error;
        TaskHandle_t m_asyncWorker = nullptr;
		Async::Dispatcher m_asyncDispatcher;

	public:
		void setup() {
			m_taskState = SystemState::Error;
			m_asyncDispatcher.init();

			if(m_asyncWorker == nullptr) {
				xTaskCreatePinnedToCore(
					[](void *arg) {
						reinterpret_cast<AsyncWorker *>(arg)->onAsyncWorker();
					},"AsyncWorker::onAsyncWorker"
					, ASYNCWORKER_STACK
					, this
					, ASYNCWORKER_PRIORITY
					, &m_asyncWorker
					, ASYNCWORKER_CORE);
			}
            delay(500);
            yield();
		}

		void reloadTaskAsync(){
			postTask([this](){
				if(this->m_taskState != SystemState::Loading) {
                    setTaskState(SystemState::Loading);
                }
			});
		}

		void stopTaskAsync(){
			postTask([this](){
				if(this->m_taskState != SystemState::Error) {
                    setTaskState(SystemState::Error);
                }
			});
		}

		void postTask(Async::Dispatcher::method_t method) {
			m_asyncDispatcher.postTask(std::move(method));
		}
	
		SystemState getTaskState() const {
			return m_taskState;
		}

	protected:
		void setTaskState(SystemState state) {
			m_taskState = state;
		}

	private: //Concurrency
		void onAsyncWorker() {
			static_cast<self*>(this)->println("Async worker started...");
			delay(1000);
			yield();
			
			while(true)
			{
				m_asyncDispatcher.update();
				switch (m_taskState)
				{
				case SystemState::Loading:
					static_cast<self*>(this)->asyncTaskLoad();
					m_taskState = SystemState::Running;
					break;
				case SystemState::Running:
					static_cast<self*>(this)->asyncTaskRun();
					delay(1);
					yield();
					break;

				default:
					static_cast<self*>(this)->asyncTaskError();
					m_taskState = SystemState::Running;
					//static_cast<self*>(this)->println("Async worker encountered an unexpected state.");
					delay(500);
					yield();
					break;
				}
			}
        }
	};
}