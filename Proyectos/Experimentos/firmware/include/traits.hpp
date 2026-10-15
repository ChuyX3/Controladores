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

#include <type_traits>

namespace Coffee { namespace implement {} }
namespace coffee = Coffee::implement;

#define BEGIN_COFFEE namespace Coffee { namespace implement {
#define END_COFFEE   } }

#define BEGIN_DETAILS namespace details {
#define END_DETAILS   }

typedef size_t index_t;

/****************************************************************************************/
BEGIN_COFFEE
template<typename...Ts> struct type_pack {};
template<typename T, T...Vs> struct value_pack { using type = T; };

template<typename T> using remove_const = typename std::remove_const<T>::type;
template<typename T> using remove_reference = typename std::remove_reference<T>::type;
template<typename T> using remove_const_reference = typename std::remove_const_volatile_reference<T>::type;
END_COFFEE

/****************************************************************************************/
BEGIN_COFFEE
BEGIN_DETAILS
template<typename...Ts>
struct coffee_details_voider {
    using type = void;
};
END_DETAILS
template <typename... Ts>
using voider = typename details::coffee_details_voider<Ts...>::type;
END_COFFEE

/****************************************************************************************/
BEGIN_COFFEE
using std::false_type;
using std::true_type;
template<bool VALUE> using bool_constant = std::integral_constant<bool, VALUE>;
template<bool VALUE> using constexpr_yes = bool_constant<VALUE>;
template<bool VALUE> using constexpr_not = bool_constant<!VALUE>;
END_COFFEE

/****************************************************************************************/
BEGIN_COFFEE
BEGIN_DETAILS
template<bool...VALUE>
struct coffee_details_constexpr_and;
template<bool VALUE1, bool VALUE2>
struct coffee_details_constexpr_and_v_v
    : false_type {
};
template<>
struct coffee_details_constexpr_and_v_v<true, true>
    : true_type {
};
template<>
struct coffee_details_constexpr_and<>
    : false_type {
};
template<bool VALUE>
struct coffee_details_constexpr_and<VALUE>
    : constexpr_yes<VALUE> {
};
template<bool VALUE1, bool VALUE2>
struct coffee_details_constexpr_and<VALUE1, VALUE2>
    : coffee_details_constexpr_and_v_v<VALUE1, VALUE2> {
};
template<bool VALUE1, bool...VALUES>
struct coffee_details_constexpr_and<VALUE1, VALUES...>
    : coffee_details_constexpr_and_v_v<VALUE1, coffee_details_constexpr_and<VALUES...>::value> {
};
END_DETAILS
template<bool...VALUE>
using constexpr_and = constexpr_yes<details::coffee_details_constexpr_and<VALUE...>::value>;
END_COFFEE

/****************************************************************************************/
BEGIN_COFFEE
BEGIN_DETAILS
template<bool...VALUE> struct coffee_details_constexpr_or;
template<bool VALUE1, bool VALUE2> struct coffee_details_constexpr_or_v_v : true_type { };
template<> struct coffee_details_constexpr_or_v_v<false, false> : false_type { };
template<> struct coffee_details_constexpr_or<> : false_type { };
template<bool VALUE> struct coffee_details_constexpr_or<VALUE> : constexpr_yes<VALUE> { };
template<bool VALUE1, bool VALUE2> struct coffee_details_constexpr_or<VALUE1, VALUE2> : coffee_details_constexpr_or_v_v<VALUE1, VALUE2> { };
template<bool VALUE1, bool...VALUES>  struct coffee_details_constexpr_or<VALUE1, VALUES...> : coffee_details_constexpr_or_v_v<VALUE1, coffee_details_constexpr_or<VALUES...>::value> { };
END_DETAILS
template<bool...VALUE>
using constexpr_or = constexpr_yes<details::coffee_details_constexpr_or<VALUE...>::value>;
END_COFFEE