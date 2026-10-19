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
#include <pthread.h>

namespace Coffee
{
    template<typename MutexT> struct ScopeLocker;

    namespace Async
    {
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
			void lock()const;
			bool tryLock()const;
			void unlock()const;

			ScopeLocker<Mutex> scopeLocker()const;
			template<typename FNType> auto sync(FNType&& func)const -> decltype(static_cast<FNType&&>(func)());
			friend struct Event;
		};
    }

        
    template<> struct ScopeLocker<Async::Mutex> final
    {
    private:
        Async::Mutex const* m_mutex;

    public:
        inline ScopeLocker(Async::Mutex const& m) : m_mutex(&m) {
            m_mutex->lock();
        }
        inline ScopeLocker(ScopeLocker&& m) : m_mutex(nullptr) {
            m_mutex = m.m_mutex;
            m.m_mutex = nullptr;
        }
        inline ~ScopeLocker() {
            if (m_mutex)m_mutex->unlock();
            m_mutex = nullptr;
        }

        template<typename FNType>
        static auto lock(Async::Mutex& m, FNType func) -> decltype(func()) {
            ScopeLocker<Async::Mutex> _lock = m;
            return func();
        }
    };

    inline ScopeLocker<Async::Mutex> Async::Mutex::scopeLocker()const {
        return ScopeLocker<Async::Mutex>(*this);
    }

    template<typename FNType>
    inline auto Async::Mutex::sync(FNType&& func)const -> decltype(static_cast<FNType&&>(func)()) {
        auto scope = this->scopeLocker();
        return static_cast<FNType&&>(func)();
    }
}