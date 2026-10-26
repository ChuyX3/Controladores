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

template<typename T>
struct external_storage
{
    using data_type = T;

    static constexpr false_type is_reallocable() { return {}; }
    static constexpr coffee::is_const<T> is_readonly() { return {}; }
    static constexpr true_type is_swappable() { return {}; }
    
    external_storage()
        : m_data(nullptr), m_size(0) {}
    external_storage(T* data, size_t size)
        : m_data(data), m_size(size) {}
    external_storage(const external_storage&) = default;
    external_storage(external_storage&&) = default;

    external_storage& operator=(const external_storage&) = default;
    external_storage& operator=(external_storage&&) = default;
    
    bool empty()const { return m_size == 0; }
    data_type* data()const { return m_data; }
    size_t size()const { return m_size; }
   
    size_t reallocate(size_t new_size) {
        /* external_storage cannot be reallocated, so always return 0 */
        return 0;
    } 

    static void swap(external_storage& a, external_storage& b) {
        std::swap(a.m_data, b.m_data);
        std::swap(a.m_size, b.m_size);
    }
    explicit operator data_type*()const { return m_data; }

private:
    data_type* m_data;
    size_t m_size;
};

template<typename T, size_t SIZE = 16U>
struct static_storage
{
    using data_type = remove_const_reference<T>;
    static constexpr size_t max_size = SIZE;

    static constexpr false_type is_reallocable() { return {}; }
    static constexpr false_type is_readonly() { return {}; }
    static constexpr false_type is_swappable() { return {}; }

    static_storage() = default;
    static_storage(const static_storage&) = delete;
    static_storage(static_storage&&) = delete;

    static_storage& operator=(const static_storage&) = delete;
    static_storage& operator=(static_storage&&) = delete;

    static constexpr bool empty() { return SIZE == 0; }
    data_type* data()const { return (data_type*)m_data; }
    static constexpr size_t size() { return SIZE; }
    size_t reallocate(size_t new_size) { 
        /* static_storage cannot be reallocated, so always return 0 */
        return 0; 
    }

    static void swap(static_storage& a, static_storage& b) {
        /*can't swap stack buffers as they are fixed in memory*/
    }
private:
    alignas(coffee::align_of<data_type>()) uint8_t m_data[SIZE * coffee::size_of<data_type>()];
};

template<typename T>
struct dynamic_storage
{
    using data_type = remove_const_reference<T>;

    static constexpr true_type is_reallocable() { return {}; }
    static constexpr false_type is_readonly() { return {}; }
    static constexpr true_type is_swappable() { return {}; }

    dynamic_storage(size_t size = 0) {
        if (size > 0) {
            m_data = (data_type*)heap_caps_aligned_alloc(coffee::align_of<data_type>(), size * coffee::size_of<data_type>(), MALLOC_CAP_DEFAULT);
            if (m_data) {
                m_size = size;
            }
        }
    }
    dynamic_storage(dynamic_storage&& other){
        swap(*this, other);
    }
    dynamic_storage(const dynamic_storage&) = delete;
    ~dynamic_storage() {
        if (m_data) {
            heap_caps_free(m_data);
            m_data = nullptr;
            m_size = 0;
        }
    }
    
    dynamic_storage& operator=(dynamic_storage&& other) {
        swap(*this, other);
        return *this;
    }
    dynamic_storage& operator=(const dynamic_storage&) = delete;
    
    data_type* data()const { return m_data; }
    size_t size()const { return m_size; }

    size_t reallocate(size_t new_size) { 
        dynamic_storage tmp(new_size);
        if(tmp.size() == new_size) {
            swap(*this, tmp);
        }
        return size();
    }

    static void swap(dynamic_storage& a, dynamic_storage& b) {
        std::swap(a.m_data, b.m_data);
        std::swap(a.m_size, b.m_size);
    }
private:
    data_type* m_data = nullptr;
    size_t m_size = 0;
};

COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename A>
struct contiguous_container_traits {
    using type = T;
    using storage_type = A;
    using internal_type = typename A::data_type;

    static constexpr decltype(A::is_reallocable()) is_reallocable(){ return{};}
    static constexpr decltype(A::is_swappable()) is_swappable(){ return{};}
    static constexpr coffee::is_default_constructible<T> is_default_constructible(){ return{};}
    static constexpr coffee::is_copy_constructible<T> is_copy_constructible(){ return{};}
    static constexpr coffee::is_move_constructible<T> is_move_constructible(){ return{};}
    static constexpr coffee::is_copy_assignable<T> is_copy_assignable(){ return{};}
    static constexpr coffee::is_move_assignable<T> is_move_assignable(){ return{};}

    template<typename U1, typename U2>
    static void move(U1* src, U2* dst, true_type const&){
        A::swap(*src, *dst);
    }
    template<typename U1, typename U2>
    static void move(U1* src, U2* dst, false_type const&) {
        if(src->size() != dst->size()){
            destroy(dst->data(), dst->size());
            dst->reallocate(src->size());
            move_construct(src->data(), src->size(), dst->data(), dst->size(), coffee::is_move_constructible<T>{});
        }
        else {
            move_assign(src->data(), src->size(), dst->data(), dst->size(), coffee::is_move_assignable<T>{});
        }
    }
    static void destroy(internal_type* dst, size_t dsts) {
        coffee::constexpr_if<!std::is_trivially_destructible<T>::value>([&](auto...)
        {
            for(size_t i = 0; i < dsts; i++){
                dst[i].~internal_type();
            }
        });
    }
    static size_t default_construct(internal_type* dst, size_t dsts, true_type const&) {
        coffee::constexpr_if<!std::is_trivially_default_constructible<T>::value>([&](auto...)
        {
            for(size_t i = 0; i < dsts; i++){
                new (dst + i) T();
            }
        });
        return dsts;
    }
    static size_t default_construct(internal_type* dst, size_t dsts, false_type const&) {
        static_assert(coffee::is_default_constructible<T>::value, "T must be default constructible");
        return 0;
    }
    template<typename U>
    static size_t copy_construct(U const* src, size_t srcs, internal_type* dst, size_t dsts, true_type const&) {
        size_t i;
        for(i = 0; i < dsts && i < srcs; i++){
            new (dst + i) T(src[i]);
        }
        return i;
    }
    template<typename U>
    static size_t copy_construct(U const* src, size_t srcs, internal_type* dst, size_t dsts, false_type const&) {
        static_assert(coffee::is_copy_constructible<U>::value, "T must be copy constructible");
        return 0;
    }
    template<typename U>
    static size_t copy_assign(U const* src, size_t srcs, internal_type* dst, size_t dsts, true_type const&) {
        size_t i;
        for(i = 0; i < dsts && i < srcs; i++){
            dst[i] = src[i];
        }
        return i;
    }
    template<typename U>
    static size_t copy_assign(U const* src, size_t srcs, internal_type* dst, size_t dsts, false_type const&) {
        destroy(dst, dsts);
        return copy_construct(src, srcs, dst, dsts, coffee::is_copy_constructible<T>{});
    }
    template<typename U>
    static size_t move_construct(U* src, size_t srcs, internal_type* dst, size_t dsts, true_type const&) {
        size_t i;
        for(i = 0; i < dsts && i < srcs; i++){
            new (dst + i) T(coffee::move(src[i]));
        }
        return i;
    }
    template<typename U>
    static size_t move_construct(U* src, size_t srcs, internal_type* dst, size_t dsts, false_type const&) {
        static_assert(coffee::is_move_constructible<U>::value, "T must be move constructible");
        return 0;
    }
    template<typename U>
    static size_t move_assign(U* src, size_t srcs, internal_type* dst, size_t dsts, true_type const&) {
        size_t i;
        for(i = 0; i < dsts && i < srcs; i++){
            dst[i] = coffee::move(src[i]);
        }
        return i;
    }
    template<typename U>
    static size_t move_assign(U* src, size_t srcs, internal_type* dst, size_t dsts, false_type const&) {
        destroy(dst, dsts);
        return move_construct(src, srcs, dst, dsts, coffee::is_move_constructible<T>{});
    }
};

COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(collections)
COFFEE_NAMESPACE_END(implement, Coffee)