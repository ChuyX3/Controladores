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

COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(collections)

template<typename T, typename C>
struct enumerator;

template<typename E>
struct base_reference {
    friend E;
    base_reference() = default;
    base_reference(base_reference &&) = default;
    base_reference(base_reference const&) = default;
    base_reference(E const* collection, size_t index = invalid_index, size_t offset = invalid_index) 
        : m_enum(collection), m_first(index), m_second(offset)
    {
    }
    
    base_reference& operator=(base_reference &&) = default;
    base_reference& operator=(base_reference const&) = default;
    
    bool valid() const {
        return m_enum != nullptr && m_first != invalid_index && m_second != invalid_index;
    }
    
    operator bool() const {
        return valid();
    }
    bool operator!() const {
        return !valid();
    }
    bool operator==(base_reference const& other) const {
        return m_enum == other.m_enum && m_first == other.m_first && m_second == other.m_second;
    }
    bool operator!=(base_reference const& other) const {
        return !(*this == other);
    }

protected:
    auto get()const {
        return this->m_enum->get(this);
    }
    void increment(int32_t n, true_type) {
        this->m_enum->increment(this, n);
    }
    void decrement(int32_t n, true_type) {
        this->m_enum->decrement(this, n);
    }
    void increment(int32_t n, false_type) {
         this->m_enum->decrement(this, n);
    }
    void decrement(int32_t n, false_type) {
        this->m_enum->increment(this, n);
    }
    
protected:
    E const* m_enum = nullptr;
    size_t m_first = invalid_index;
    size_t m_second = invalid_index;
    size_t m_extra = 0;
};

template<typename E, typename T>
struct reference : base_reference<E> {
    using base_type = base_reference<E>;
    reference() = default;
    reference(E const* collection, size_t index = invalid_index, size_t offset = invalid_index)
        : base_type(collection, index, offset)
    {
    }
    T& operator*() {
        return *this->get();
    }
    T* operator->() {
        return this->get();
    }
    const T& operator*() const {
        return *this->get();
    }
    const T* operator->() const {
        return this->get();
    }
    explicit operator T*() {
        return this->get();
    }
    explicit operator const T*() const {
        return this->get();
    }
};

template<typename E, typename T>
struct reference<E, const T> : base_reference<E> {
    using base_type = base_reference<E>;
    reference() = default;
    reference(E const* collection, size_t index = invalid_index, size_t offset = invalid_index) 
    : base_type(collection, index, offset)
    {
    }
    const T& operator*() const {
        return *this->get();
    }
    const T* operator->() const {
        return this->get();
    }
    explicit operator const T*() const {
        return this->get();
    }
};

template<typename E, typename T, bool forward = true>
struct iterator : reference<E, T> {
    using base_type = reference<E, T>;
    using self_type = iterator<E, T, forward>;
    static constexpr bool_constant<forward> is_forward = {};

    iterator() = default;
    iterator(E const* collection, size_t index = invalid_index, size_t offset = invalid_index) {
        base_type::m_enum = collection;
        base_type::m_first = index;
        base_type::m_second = offset;
    }

    self_type& operator++() {
        this->increment(1);
        return *this;
    }

    self_type operator++(int) {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    self_type& operator--() {
        this->decrement(1);
        return *this;
    }

    self_type operator--(int) {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    self_type operator+(int32_t n) const {
        auto tmp = *this;
        tmp.increment(n);
        return tmp;
    }

    self_type operator-(int32_t n) const {
        auto tmp = *this;
        tmp.decrement(n);
        return tmp;
    }
    self_type& operator+=(int32_t n) {
        this->increment(n);
        return *this;
    }

    self_type& operator-=(int32_t n) {
        this->decrement(n);
        return *this;
    }

protected:
    void increment(int32_t n) {
        base_type::increment(n, is_forward);
    }
    void decrement(int32_t n) {
        base_type::decrement(n, is_forward);
    }
};

COFFEE_NAMESPACE_END(collections)
COFFEE_NAMESPACE_END(Coffee, implement)

namespace Coffee
{
    template<typename T, typename C>
    using Enumerator = coffee::collections::enumerator<T, C>;

    template<typename E, typename T>
    using Reference = coffee::collections::reference<E, T>;
}
