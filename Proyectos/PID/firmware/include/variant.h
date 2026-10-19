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

#include "utils.h"

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)

template<typename TP = type_pack<>>
struct coffee_details_variant_raw_data;

template<size_t AT, typename UD>
struct coffee_details_variant_data_get;

template<typename FN, typename VD, typename IP = pack_range<size_t, 0, size_of<typename remove_const_reference<VD>::types>() - 1>>
struct coffee_details_variant_visitor;

template<>
struct coffee_details_variant_raw_data<type_pack<>> {
    using self = coffee_details_variant_raw_data<type_pack<>>;
    using types = type_pack<>;
    using type = void;
    static constexpr std::true_type is_trivially_destructible = {};
    size_t m_current;
};

template<typename T, typename...Ts>
struct coffee_details_variant_raw_data<type_pack<T, Ts...>> {
    using self = coffee_details_variant_raw_data<type_pack<T, Ts...>>;
    using types = type_pack<T, Ts...>;
    using type = remove_const_reference<T>;
    using rest_type = coffee_details_variant_raw_data<type_pack<Ts...>>;
    static constexpr constexpr_and<std::is_trivially_destructible<T>::value, std::is_trivially_destructible<Ts>::value...> is_trivially_destructible = {};

    union {
         struct {
            size_t m_current;
            uint8_t m_data[sizeof(type)];
        };
        rest_type m_rest;
    };
    type* data()const { return (type*)(uint8_t*)const_cast<self*>(this)->m_data; }
};

template<typename... Ts>
struct coffee_details_variant_data_get<0, coffee_details_variant_raw_data<type_pack<Ts...>>> {
    using data_type = coffee_details_variant_raw_data<type_pack<Ts...>>;
    using return_type = typename data_type::type;
    static return_type& get(data_type* ptr) { return *ptr->data(); }
    static return_type const& get(data_type const* ptr) { return *ptr->data(); }
};

template<size_t AT, typename... Ts>
struct coffee_details_variant_data_get<AT, coffee_details_variant_raw_data<type_pack<Ts...>>> {
    using data_type = coffee_details_variant_raw_data<type_pack<Ts...>>;
    using next_data_type = typename data_type::rest_type;
    using return_type = typename coffee_details_variant_data_get<AT - 1, next_data_type>::return_type;
    static return_type& get(data_type* ptr) { return coffee_details_variant_data_get<AT - 1, next_data_type>::get(&ptr->m_rest); }
    static return_type const& get(data_type const* ptr) { return coffee_details_variant_data_get<AT - 1, next_data_type>::get(&ptr->m_rest); }
};

template<typename FN, typename VD, size_t... IDXS>
struct coffee_details_variant_visitor<FN, VD, value_pack<size_t, IDXS...>> {
    using variant_data = VD;
    using return_type = decltype(std::declval<FN>()(std::integral_constant<size_t, 0>{}, std::declval<variant_data>().get(std::integral_constant<size_t, 0>{})));
    using function_type = return_type(*)(variant_data&&, FN&&);

    inline static return_type visit(size_t idx, variant_data&& data, FN&& func) {
        return s_visitors[idx](std::forward<variant_data>(data), std::forward<FN>(func));
    }
private:
    template<size_t I> inline static return_type visitor(variant_data&& data, FN&& func) {
        return static_cast<FN&&>(func)(std::integral_constant<size_t, I>{}, static_cast<variant_data&&>(data).get(std::integral_constant<size_t, I>{}));
    }
    static constexpr function_type s_visitors[sizeof...(IDXS)] = { &visitor<IDXS>... };
};

template<typename FN, typename variant_data, size_t... IDXS>
constexpr typename coffee_details_variant_visitor<FN, variant_data, value_pack<size_t, IDXS...>>::function_type
coffee_details_variant_visitor<FN, variant_data, value_pack<size_t, IDXS...>>::s_visitors[sizeof...(IDXS)];

template<typename TP>
struct coffee_details_variant_data : coffee_details_variant_raw_data<TP> {
    using base_type = coffee_details_variant_raw_data<TP>;

    template<size_t I>
    typename coffee_details_variant_data_get<I, base_type>::return_type& get(std::integral_constant<size_t, I> const& = {}) {
        return coffee_details_variant_data_get<I, base_type>::get(this);
    }

    template<size_t I>
    typename coffee_details_variant_data_get<I, base_type>::return_type const& get(std::integral_constant<size_t, I> const& = {})const {
        return coffee_details_variant_data_get<I, base_type>::get(this);
    }
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)

//same type : checks if the type T is the same as any type within the variant TP
// is_same

//category : checks if the type T belongs to a specific category within the variant TP
// is_same<longest<TP>, longest<underlying<T>>>

//change category : checks if the type T can be converted to a specific category within the variant TP
// is_constructible<TP, T>



template<typename T, typename TP, template<class, class> class compare, bool = pack_contains<longest<TP>, longest<underlying<T>>, compare>::value>
struct coffee_details_variant_candidate : std::false_type {
    static constexpr integral_constant<size_t, size_t(~0ull)> index = {};
    using type = void;
};

template<typename T, typename TP, template<class, class> class compare>
struct coffee_details_variant_candidate<T, TP, compare, true> : std::true_type {
    static constexpr constexpr_select<
        pack_contains<TP, T, compare>::value, index_t,
        pack_contains_at<TP, T, compare>::value,
        pack_contains_at<longest<TP>, longest<underlying<T>>, compare>::value
    > index = {};

    using type = pack_get<TP, index>;
};

template<typename T, typename TP>
using coffee_details_variant_best_constructible = conditional<
    coffee_details_variant_candidate<T, TP, is_same>::value,
    coffee_details_variant_candidate<T, TP, is_same>,
    coffee_details_variant_candidate<T, TP, is_constructible>
>;

template<typename T, typename TP>
using coffee_details_variant_best_assignable = conditional<
    coffee_details_variant_candidate<T, TP, is_same>::value,
    coffee_details_variant_candidate<T, TP, is_same>,
    coffee_details_variant_candidate<T, TP, is_assignable>
>;

template<typename T, typename TP>
using coffee_details_variant_best_constructible_at_inverse = conditional<
    coffee_details_variant_candidate<T, TP, is_same>::value,
    coffee_details_variant_candidate<T, TP, is_same>,
    coffee_details_variant_candidate<T, TP, is_constructible_at_inverse>
>;

template<typename T, typename TP>
using coffee_details_variant_best_assignable_at_inverse = conditional<
    coffee_details_variant_candidate<T, TP, is_same>::value,
    coffee_details_variant_candidate<T, TP, is_same>,
    coffee_details_variant_candidate<T, TP, is_assignable_at_inverse>
>;

COFFEE_NAMESPACE_END(details)

template<typename...Ts>
struct variant
{
    using self = variant<Ts...>;
    using types = pack_norepeat<type_pack<Ts...>>;
    using data_type = typename coffee::details::coffee_details_variant_data<types>;
    static constexpr size_t SIZE = coffee::size_of<types>();

    template<typename U> using enable_if_is_constructible =  enable_if<constexpr_less<size_t, coffee::details::coffee_details_variant_best_constructible<U, types>::index, SIZE>::value, bool>;
    template<typename U> using enable_if_is_assignable =  enable_if<constexpr_less<size_t, coffee::details::coffee_details_variant_best_assignable<U, types>::index, SIZE>::value, bool>;
    template<typename U> using enable_if_is_constructible_at_inverse =  enable_if<constexpr_less<size_t, coffee::details::coffee_details_variant_best_constructible_at_inverse<U, types>::index, SIZE>::value, bool>;
    template<typename U> using enable_if_is_assignable_at_inverse =  enable_if<constexpr_less<size_t, coffee::details::coffee_details_variant_best_assignable_at_inverse<U, types>::index, SIZE>::value, bool>;

    template<typename U> using enable_if_is_convertible = enable_if<constexpr_or<
        is_same<remove_const_reference<U>, self>::value,
        constexpr_less<size_t, coffee::details::coffee_details_variant_best_constructible<U, types>::index, SIZE>::value,
        constexpr_less<size_t, coffee::details::coffee_details_variant_best_assignable<U, types>::index, SIZE>::value>::value, bool>;

    variant()
        : m_current((size_t)~0ull)
    {

    }

    
    ~variant()
    {
        this->reset();
    }

public:
    template<typename T = bool, coffee::enable_if<data_type::is_trivially_destructible, T> = true>
    void reset() { }

    template<typename T = bool, coffee::enable_if<!data_type::is_trivially_destructible, T> = false>
    void reset() {
        if (valid()) {
            this->visit(current(), m_data, [](auto, auto& item) {
                self::destroy(coffee::address_of(item));
            });
        }
    }

    constexpr data_type& data() & noexcept {
        return m_data;
    }
    constexpr const data_type& data() const& noexcept {
        return m_data;
    }
    constexpr data_type&& data() && noexcept {
        return (coffee::move(m_data));
    }
    constexpr const data_type&& data() const&& noexcept {
        return (coffee::move(m_data));
    }

    template<typename U, enable_if_is_convertible<U> = true>
    void set(U const& value) {
        if(constexpr_if<is_same<U, self>::value>([&](auto...)
        {
            if(this->current() == value.current()) {
                if()


                this->assign(value.current(), m_data, value.data().get(value.current()));
            }
            else {
                if(this->valid()){
                    this->reset();
               }
            }

            visit(value.current(), value.data(), [&](auto idx, auto& item) {
                this->assign(idx, m_data, item);
            });
        }));    
        else if(constexpr_if<is_same<U, self>::value>([&](auto...)
        {

        }));
    }

public: /*utilities*/
    bool valid()const {
        return current() < SIZE;
    }
    size_t current()const {
        return m_current;
    }

private: /*utilities*/
    template<typename T, typename U, coffee::enable_if<coffee::is_constructible<T, U&&>::value, bool> = true>
    static constexpr bool construct(T& value, U&& arg) {
        ::new((void*)coffee::address_of(value)) T(coffee::forward<U>(arg));
        return true;
    }
    template<typename T, typename U, coffee::enable_if<!coffee::is_constructible<T, U&&>::value, bool> = false>
    static constexpr bool construct(T& value, U&& arg) {
        return false;
    }
    template<size_t INDEX, typename U, coffee::enable_if<coffee::constexpr_less<size_t, INDEX, SIZE>::value, bool> = true>
    static constexpr bool construct(integral_constant<size_t, INDEX> index, data_type& value, U&& arg) {
        using type = coffee::type_pack_at<types, INDEX>;
        ::new((void*)coffee::address_of(value.get(index))) type(coffee::forward<U>(arg));
        return true;
    }
    template<size_t INDEX, typename U, coffee::enable_if<!coffee::constexpr_less<size_t, INDEX, SIZE>::value, bool> = false>
    static constexpr bool construct(integral_constant<size_t, INDEX>, data_type&, U&&) {
        return false;
    }
    template<size_t INDEX, typename U, coffee::enable_if<coffee::constexpr_less<size_t, INDEX, SIZE>::value, bool> = true>
    static constexpr bool assign(integral_constant<size_t, INDEX> index, data_type& value, U&& arg) {
        value.get(index) = coffee::forward<U>(arg);
        return true;
    }
    template<size_t INDEX, typename U, coffee::enable_if<!coffee::constexpr_less<size_t, INDEX, SIZE>::value, bool> = false>
    static constexpr bool assign(integral_constant<size_t, INDEX>, data_type&, U&&) {
        return false;
    }
    template<typename T>
    static constexpr void destroy(T* value) {
        value->~T();
    }

    template<typename VD, typename FN>
    auto visit(size_t idx, VD&& data, FN&& func) {
        return details::coffee_details_variant_visitor<FN, VD>::visit(idx, static_cast<VD&&>(data), static_cast<FN&&>(func));
    }

private:
	union {
        size_t m_current;
        data_type m_data;
    };
};

COFFEE_NAMESPACE_END(Coffee, implement)



