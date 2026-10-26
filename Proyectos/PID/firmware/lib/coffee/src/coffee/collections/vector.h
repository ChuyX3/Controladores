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
#include "coffee/collections/storage.h"
#include "coffee/collections/iterators.h"
#include <initializer_list>

COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(collections)

template<typename T, typename A = coffee::collections::static_storage<T, 16>> struct vector;

COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename A>
struct vector_traits : contiguous_container_traits<T, A> {
    using data_type = typename A::data_type;
};
COFFEE_NAMESPACE_END(details)

template<typename T, typename A>
struct enumerator<T, vector<T, A>> {
    using self_type = enumerator<T, vector<T, A>>;
    using traits = collections::details::vector_traits<T, A>;
    using final_type = vector<T, A>;

    using data_type = typename traits::data_type;
    using iterator_t = collections::iterator<self_type, data_type, true>;
    using const_iterator_t = collections::iterator<self_type, const data_type, true>;
    using reverse_iterator_t = collections::iterator<self_type, data_type, false>;
    using const_reverse_iterator_t = collections::iterator<self_type, const data_type, false>;

    friend base_reference<self_type>;

    T* get(base_reference<self_type> const* ref)const {
        if(!ref->valid() || ref->m_extra != (size_t)internal_data()
         || ref->m_second >= this->internal_size() || ref->m_first != this->internal_first())
            return nullptr;
        return this->internal_data() + this->internal_index(ref->m_first, ref->m_second); 
    }

    void increment(base_reference<self_type>* ref, int32_t offset)const {
        if(!ref->valid())
            return;
        size_t pos = ref->m_second + offset;
        if(pos >= this->internal_size() || ref->m_extra != (size_t)internal_data()) {
            ref->m_first  = invalid_index;
            ref->m_second = invalid_index;
            ref->m_extra = 0;
            return;
        }
        ref->m_second = pos;
    }

    void decrement(base_reference<self_type>* ref, int32_t offset)const {
        if(!ref->valid())
            return;
        size_t pos = ref->m_second - offset;
        if(pos >= this->internal_size() || ref->m_extra != (size_t)internal_data()) {
            ref->m_first  = invalid_index;
            ref->m_second = invalid_index;
            ref->m_extra = 0;
            return;
        }
        ref->m_second = pos;
    }
    
    iterator_t begin() {
        return iterator_t(this, internal_first(), 0, internal_data());
    }
    iterator_t end() {
        return iterator_t(this, invalid_index, invalid_index);
    }
    const_iterator_t begin() const {
        return const_iterator_t(this, internal_first(), 0, internal_data());
    }
    const_iterator_t end() const {
        return const_iterator_t(this, invalid_index, invalid_index);
    }
    reverse_iterator_t rbegin() {
        return reverse_iterator_t(this, internal_first(), internal_size() - 1, internal_data());
    }
    reverse_iterator_t rend() {
        return reverse_iterator_t(this, invalid_index, invalid_index);
    }
    const_reverse_iterator_t rbegin() const {
        return const_reverse_iterator_t(this, internal_first(), internal_size() - 1, internal_data());
    }
    const_reverse_iterator_t rend() const {
        return const_reverse_iterator_t(this, invalid_index, invalid_index);
    }

private:
    size_t internal_index(int32_t index, int32_t offset)const {
        uint32_t max_size = internal_capacity();
        int32_t sign = (offset >> 31) | 1;
        offset = (uint32_t)(static_cast<int64_t>(offset) * sign) % max_size;
        return (max_size + offset * sign + index) % max_size;
    }
    size_t internal_capacity() const {
        return static_cast<final_type const*>(this)->capacity();
    }
    size_t internal_size() const {
        return static_cast<final_type const*>(this)->size();
    }
    data_type* internal_data() const {
        return static_cast<final_type const*>(this)->internal_data();
    }
    size_t internal_first() const {
        return static_cast<final_type const*>(this)->first_index();
    }
};

template<typename T, typename A>
struct vector : protected enumerator<T, vector<T,A>>
{
    using self_t = vector<T, A>;
    using storage_type = A;
    using data_type = typename A::data_type;
    using traits = collections::details::vector_traits<T, A>;
    using enum_type = enumerator<T, vector<T,A>>;

    using iterator_t = typename enum_type::iterator_t;
    using const_iterator_t = typename enum_type::const_iterator_t;
    using reverse_iterator_t = typename enum_type::reverse_iterator_t;
    using const_reverse_iterator_t = typename enum_type::const_reverse_iterator_t;


public:
    bool empty() const { return m_count == 0; }
    size_t size() const { return m_count; }
    size_t capacity() const { return m_storage.size(); }

private:
    size_t first_index()const { return m_first; }
    size_t internal_index(int32_t index)const {
        uint32_t max_size = capacity();
        int32_t sign = (index >> 31) | 1;
        index = (uint32_t)(static_cast<int64_t>(index) * sign) % max_size;
        return (max_size + index * sign + first_index()) % max_size;
    }
    

private:
    storage_type m_storage = {};
    size_t m_first;
    size_t m_count;
};

COFFEE_NAMESPACE_END(collections)
COFFEE_NAMESPACE_END(implement)

COFFEE_NAMESPACE_END(Coffee)
{




    template<typename T, typename C>
    struct Iterator {
        
    private:

    
    };




    /* Vector container with max fixed size and optional default size of 32 */
   template<typename T, uint32_t SIZE = 32>
    struct Vector
    {


        static constexpr uint32_t max_size = SIZE;

        Vector()
            : m_first(dataBeg())
            , m_count(0)
        {
        }

    public:
        size_t size()const { return m_count; }
        static constexpr size_t capacity() { return SIZE; }

    private:
        T* dataBeg() { return reinterpret_cast<T*>(m_data); }
        T const* dataBeg()const { return reinterpret_cast<T const*>(m_data); }
        T* dataEnd() { return dataBeg() + SIZE; }
        T const* dataEnd()const { return dataBeg() + SIZE; }
        
        int32_t absIndex(T const* ptr) const{
            return static_cast<int32_t>(ptr - dataBeg());
        }
        int32_t relIndex(T const* ptr) const{
            return absIndex(ptr) - absIndex(m_first);
        }

        size_t getIndex(int32_t from, int32_t to) const {
            //int32_t sign = 1 - 2 * ((uint32_t)to >> 31);
            int32_t sign = (to >> 31) | 1;
            uint32_t abs = static_cast<int64_t>(to) * sign;
            return (max_size + (abs % max_size) * sign + from) % max_size;
        }

    protected:
        uint8_t alignas(coffee::align_of<T>()) m_data[SIZE * coffee::size_of<T>()];
        T* m_first;
        size_t m_count;
    }; 
}