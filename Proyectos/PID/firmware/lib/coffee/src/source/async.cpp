
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

#include "coffee/async/mutex.h"
#include "coffee/async/events.h"
#include "coffee/async/dispatcher.h"

using namespace Coffee;
using namespace Coffee::Async;

static struct pthread_mutexattr_type
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_type(){
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    }
}s_attr;

Mutex::Mutex(bool lock)
{
    pthread_mutex_init(&m_handle, &s_attr.attr);
    if(lock) this->lock();
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_handle);
}

void Mutex::lock()const
{
    pthread_mutex_lock(&m_handle);
}

bool Mutex::tryLock()const
{
    return pthread_mutex_trylock(&m_handle) == 0;
}

void Mutex::unlock()const
{
    pthread_mutex_unlock(&m_handle);
}

//////////////////////////////////////////////////////////////////////////////////

Event::Event()
{
    pthread_cond_init(&m_handle, nullptr);
}

Event::~Event()
{
    pthread_cond_destroy(&m_handle);
}

void Event::wait(Mutex const& mutex)const
{
    pthread_cond_wait(&m_handle, &mutex.m_handle);
}

bool Event::wait(Mutex const& mutex, unsigned long ms)const
{
    timespec time;
    time.tv_sec = ms/1000;
    time.tv_nsec = (ms%1000) * 1000;
    return pthread_cond_timedwait(&m_handle, &mutex.m_handle, &time) == 0;
}

void Event::signal()const
{
    pthread_cond_broadcast(&m_handle); 
}


//////////////////////////////////////////////////////////////////////////////////

Dispatcher::~Dispatcher()
{
    auto locker = m_mutex.scopeLocker();
    while(m_queue.size() > 0)
        m_queue.pop();
}

void Dispatcher::init()
{
    // Clear the task queue before starting the async worker
    auto locker = m_mutex.scopeLocker();
    while(m_queue.size() > 0)
        m_queue.pop();
}

void Dispatcher::update()
{
    method_t method;
    auto locker = m_mutex.scopeLocker();
    while (m_queue.size() > 0) {
        method = std::move(m_queue.front());
        m_queue.pop();
        m_mutex.unlock();
        try {
            method();
        }
        catch (...) {
        }
        m_mutex.lock();
    }
}

void Dispatcher::postTask(method_t method)
{
    m_mutex.sync([&]() {
        m_queue.push(std::move(method));
    });
}