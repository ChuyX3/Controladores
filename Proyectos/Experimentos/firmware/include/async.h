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
#include <pthread.h>

namespace Coffee
{
    namespace Async
    {
        template<typename MutexT> struct ScopeLocker;

        struct Mutex
		{
		private: //Non copiable
			Mutex(const Mutex&) = delete;
			Mutex& operator = (const Mutex&) = delete;
            
        protected:
            mutable pthread_mutex_t m_handle;

		public:
			Mutex(bool lock = false);
			~Mutex();

		public: //Properties
			void Lock()const;
			bool TryLock()const;
			void Unlock()const;

			Async::ScopeLocker<Mutex> ScopeLocker()const;
			template<typename FNType> auto Sync(FNType&& func)const -> decltype(static_cast<FNType&&>(func)());
			friend struct Event;
		};

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
			void Wait(Mutex const& mutex)const;
			bool Wait(Mutex const& mutex, unsigned long ms)const;
			void Signal()const;

			template<typename FNType>
			void WaitFor(Mutex const& mutex, FNType cond)const {
				while (!cond()) { Wait(mutex); }
			}

			template<typename FNType>
			bool WaitFor(Mutex const& mutex, unsigned long ms, FNType cond)const {
				if (!cond()) { Wait(mutex, ms); }
				return cond();
			}
		};

		template<> struct ScopeLocker<Mutex> final
		{
		private:
			Mutex const* m_mutex;

		public:
			inline ScopeLocker(Mutex const& m) : m_mutex(&m) {
				m_mutex->Lock();
			}
			inline ScopeLocker(ScopeLocker&& m) : m_mutex(nullptr) {
				m_mutex = m.m_mutex;
				m.m_mutex = nullptr;
			}
			inline ~ScopeLocker() {
				if (m_mutex)m_mutex->Unlock();
				m_mutex = nullptr;
			}

			template<typename FNType>
			static auto lock(Mutex& m, FNType func) -> decltype(func())
			{
				ScopeLocker _lock = m;
				return func();
			}
		};

		inline ScopeLocker<Mutex> Mutex::ScopeLocker()const {
			return Async::ScopeLocker<Mutex>(*this);
		}

		template<typename FNType>
		inline auto Mutex::Sync(FNType&& func)const -> decltype(static_cast<FNType&&>(func)()) {
			auto scope = this->ScopeLocker();
			return static_cast<FNType&&>(func)();
		}
    }
}