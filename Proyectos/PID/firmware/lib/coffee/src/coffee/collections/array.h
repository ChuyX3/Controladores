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
#include "coffee/collections/arrayview.h"
#include "coffee/collections/storage.h"
#include "coffee/collections/iterators.h"
#include <initializer_list>

COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(collections)

template<typename T, typename A = coffee::collections::static_storage<T, 16>> struct array;

COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename A>
struct array_traits : contiguous_container_traits<T, A> {
    using data_type = typename A::data_type;
};
COFFEE_NAMESPACE_END(details)

template<typename T, typename A>
struct enumerator<T, array<T, A>> {
    using self_type = enumerator<T, array<T, A>>;
    using traits = collections::details::array_traits<T, A>;
    using final_type = array<T, A>;

    using data_type = typename traits::data_type;
    using iterator_t = collections::iterator<self_type, data_type, true>;
    using const_iterator_t = collections::iterator<self_type, const data_type, true>;
    using reverse_iterator_t = collections::iterator<self_type, data_type, false>;
    using const_reverse_iterator_t = collections::iterator<self_type, const data_type, false>;

    friend base_reference<self_type>;

    T* get(base_reference<self_type> const* ref)const {
        if(!ref->valid() || (ref->m_first + ref->m_second) >= this->internal_size() || ref->m_extra != (size_t)internal_data())
            return nullptr;
        return this->internal_data() + (ref->m_first +ref->m_second); 
    }

    void increment(base_reference<self_type>* ref, int32_t offset)const {
        if(!ref->valid())
            return;
        size_t pos = ref->m_first + ref->m_second + offset;
        if(pos >= this->internal_size() || ref->m_extra != (size_t)internal_data()) {
            ref->m_extra  = invalid_index;
            ref->m_second = invalid_index;
            ref->m_extra = 0;
            return;
        }
        ref->m_second = pos - ref->m_first;
    }

    void decrement(base_reference<self_type>* ref, int32_t offset)const {
        if(!ref->valid())
            return;
        size_t pos = ref->m_first + ref->m_second - offset;
        if(pos >= this->internal_size() || ref->m_extra != (size_t)internal_data()) {
            ref->m_extra  = invalid_index;
            ref->m_second = invalid_index;
            ref->m_extra = 0;
            return;
        }
        ref->m_second = pos - ref->m_first;
    }
    
    iterator_t begin() {
        return iterator_t(this, 0, 0, (size_t)internal_data());
    }
    iterator_t end() {
        return iterator_t(this, invalid_index, invalid_index);
    }
    const_iterator_t begin() const {
        return const_iterator_t(this, 0, 0, (size_t)internal_data());
    }
    const_iterator_t end() const {
        return const_iterator_t(this, invalid_index, invalid_index);
    }
    reverse_iterator_t rbegin() {
        return reverse_iterator_t(this, 0, this->internal_size() - 1, (size_t)internal_data());
    }
    reverse_iterator_t rend() {
        return reverse_iterator_t(this, invalid_index, invalid_index);
    }
    const_reverse_iterator_t rbegin() const {
        return const_reverse_iterator_t(this, 0, this->internal_size() - 1, (size_t)internal_data());
    }
    const_reverse_iterator_t rend() const {
        return const_reverse_iterator_t(this, invalid_index, invalid_index);
    }

private:
    size_t internal_size() const {
        return static_cast<final_type const*>(this)->internal_size();
    }
    data_type* internal_data() const {
        return static_cast<final_type const*>(this)->internal_data();
    }
};

template<typename T, typename A>
struct array : protected enumerator<T, array<T, A>> {
    using self_type = array<T, A>;
    using storage_type = A;
    using data_type = typename A::data_type;
    using traits = collections::details::array_traits<T, A>;
    using enum_type = enumerator<T, array<T, A>>;

    using iterator_t = typename enum_type::iterator_t;
    using const_iterator_t = typename enum_type::const_iterator_t;
    using reverse_iterator_t = typename enum_type::reverse_iterator_t;
    using const_reverse_iterator_t = typename enum_type::const_reverse_iterator_t;

    friend enumerator<T, self_type>;

    template<typename U = T, coffee::enable_if<coffee::is_default_constructible<U>::value, bool> = true>
    array() {
        traits::default_construct(this->internal_data(), m_storage.size(), traits::is_default_constructible());   
    }

    template<typename U = T, coffee::enable_if<coffee::is_copy_constructible<U>::value, bool> = true>
    array(std::initializer_list<T> const& init) {
        static_assert(
            traits::is_default_constructible() || traits::is_reallocatable(),
            "array elements must be default constructible when initializer_list contains fewer elements than storage");

        size_t i = 0;
        m_storage.reallocate(init.size());
        for(const auto& elem : init){
            if(i >= internal_size())
                break;
            new (internal_data() + i) T(elem);
            i++;
        }
        /* If the initializer list has fewer elements than the storage size, default-construct the remaining elements */
        coffee::constexpr_if<traits::is_default_constructible()>([&](auto...)
        {
            for(size_t j = i; j < internal_size(); j++)
                new (internal_data() + j) T();
        });
    }

    template<typename U = T, coffee::enable_if<traits::is_copy_constructible(), bool> = true>
    array(array const& other) {
        copy(other);
    }

    explicit array(A&& storage) {
        attach(coffee::move(storage));
    }
    
    template<typename U = T, coffee::enable_if<traits::is_swappable() || traits::is_move_constructible(), bool> = true>
    array(array && other) noexcept {
        move(other);
    }

    ~array() {
        traits::destroy(internal_data(), internal_size());
    }

    template<typename U = T, coffee::enable_if<coffee::is_copy_assignable<U>::value || coffee::is_copy_constructible<U>::value, bool> = true>
    array& operator = (array const& other) {
        if(this == &other)
            return *this;
        copy(other);
        return *this;
    }

    template<typename U = T, coffee::enable_if<traits::is_swappable() || traits::is_move_assignable() || traits::is_move_constructible(), bool> = true>
    array& operator = (array && other) noexcept {
        if(this == &other)
            return *this;
        move_storage(&other.m_storage, traits::is_swappable());
        return *this;
    }

public:
    bool empty() const {
        return internal_size() == 0;
    }
    size_t size() const {
        return internal_size();
    }
    size_t capacity() const {
        return internal_size();
    }
    enum_type & enumerate() {
        return static_cast<enum_type&>(*this);
    }
    enum_type const& enumerate() const {
        return static_cast<enum_type const&>(*this);
    }
    
public: /*utils*/
    void copy(array const& other) {
        if(this == &other)
            return;
        if(coffee::constexpr_if<storage_type::is_reallocatable()>([&](auto...)
        {
            if(internal_size() != other.internal_size()) {
                traits::destroy(internal_data(), internal_size());
                m_storage.reallocate(other.internal_size());
                traits::copy_construct(other.internal_data(), other.internal_size(), internal_data(), internal_size(), traits::is_copy_constructible());
            }
            else {
                traits::copy_assign(other.internal_data(), other.internal_size(), internal_data(), internal_size(), traits::is_copy_assignable());
            }
        }));
        else{ //stack-based storage and external storage
            // If storage is not reallocatable, just copy assign the elements
            traits::copy_assign(other.internal_data(), other.internal_size(), internal_data(), internal_size(), traits::is_copy_assignable());
        }
    }
    void move(array& other) {
        if(this == &other)
            return;
        traits::move(&other.m_storage, &m_storage, traits::is_swappable());
    }
    void attach(A&& storage) {
        if(&this->m_storage == &storage)
            return;
        traits::move(&storage, &this->m_storage, traits::is_swappable());
    }

public: /*iterators*/
    using enum_type::begin;
    using enum_type::end;
    using enum_type::rbegin;
    using enum_type::rend;
    
public: /*operators*/
    T& operator [](int32_t index){
        return internal_data()[index];
    }
    T const& operator [](int32_t index)const {
        return internal_data()[index];
    }

private:
    size_t internal_size()const {
        return m_storage.size();
    }
    data_type* internal_data()const {
        return m_storage.data();
    }
    
protected:
    storage_type m_storage = {};
};

COFFEE_NAMESPACE_END(collections)

COFFEE_NAMESPACE_BEGIN(algorithms)
template<typename T, typename A>
inline size_t array_size(collections::array<T, A> const& arr) { return arr.size(); }
COFFEE_NAMESPACE_END(algorithms)
COFFEE_NAMESPACE_END(implement)

template<typename T, typename A>
using Array = coffee::collections::array<T, A>;

template<typename T, size_t N>
using StaticArray = coffee::collections::array<T, coffee::collections::static_storage<T, N>>;

template<typename T>
using DynamicArray = coffee::collections::array<T, coffee::collections::dynamic_storage<T>>;

template<typename T>
using ArrayWrapper = coffee::collections::array<T, coffee::collections::external_storage<T>>;

COFFEE_NAMESPACE_END(Coffee)
