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

/********************************************************************************************************************************************/
#ifdef COFFEE_STDCPP

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T1, typename T2, typename = void>
struct coffee_details_has_equal_operator
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_has_equal_operator<T1, T2, voidT<decltype(coffee::declval<T1>() == coffee::declval<T2>())>>
	: true_type {
};

template<typename T1, typename T2, typename = void>
struct coffee_details_has_not_equal_operator
	: false_type {
};
template<typename T1, typename T2>
struct coffee_details_has_not_equal_operator<T1, T2, voidT<decltype(coffee::declval<T1>() != coffee::declval<T2>())>>
	: true_type {
};

template<typename T1, typename T2, typename = void>
struct coffee_details_has_less_operator
	: false_type {
};
template<typename T1, typename T2>
struct coffee_details_has_less_operator<T1, T2, voidT<decltype(coffee::declval<T1>() < coffee::declval<T2>())>>
	: true_type {
};

template<typename T1, typename T2, typename = void>
struct coffee_details_has_greater_operator
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_has_greater_operator<T1, T2, voidT<decltype(coffee::declval<T1>() > coffee::declval<T2>())>>
	: true_type {
};

template<typename T1, typename T2, typename = void>
struct coffee_details_has_less_equal_operator
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_has_less_equal_operator<T1, T2, voidT<decltype(coffee::declval<T1>() <= coffee::declval<T2>())>>
	: true_type {
};

template<typename T1, typename T2, typename = void>
struct coffee_details_has_greater_equal_operator
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_has_greater_equal_operator<T1, T2, voidT<decltype(coffee::declval<T1>() >= coffee::declval<T2>())>>
	: true_type {
};

COFFEE_NAMESPACE_END(details)
template<typename T1, typename T2 = T1>
using has_equal_operator = bool_constant<coffee::details::coffee_details_has_equal_operator<T1, T2>::value>;

template<typename T1, typename T2 = T1>
using has_not_equal_operator = bool_constant<coffee::details::coffee_details_has_not_equal_operator<T1, T2>::value>;

template<typename T1, typename T2 = T1>
using has_less_operator = bool_constant<coffee::details::coffee_details_has_less_operator<T1, T2>::value>;

template<typename T1, typename T2 = T1>
using has_greater_operator = bool_constant<coffee::details::coffee_details_has_greater_operator<T1, T2>::value>;

template<typename T1, typename T2 = T1>
using has_less_equal_operator = bool_constant<coffee::details::coffee_details_has_less_equal_operator<T1, T2>::value>;

template<typename T1, typename T2 = T1>
using has_greater_equal_operator = bool_constant<coffee::details::coffee_details_has_greater_equal_operator<T1, T2>::value>;

COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)

template<typename T1, typename T2, typename = void>
struct coffee_details_has_plus_operator
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_has_plus_operator<T1, T2, voidT<decltype(coffee::declval<T1>() + coffee::declval<T2>())>>
	: true_type {
};

template<typename T1, typename T2, typename = void>
struct coffee_details_has_minus_operator
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_has_minus_operator<T1, T2, voidT<decltype(coffee::declval<T1>() - coffee::declval<T2>())>>
	: true_type {
};


template<typename T1, typename T2, typename = void>
struct coffee_details_has_multiply_operator
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_has_multiply_operator<T1, T2, voidT<decltype(coffee::declval<T1>() * coffee::declval<T2>())>>
	: true_type {
};

template<typename T1, typename T2, typename = void>
struct coffee_details_has_divide_operator
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_has_divide_operator<T1, T2, voidT<decltype(coffee::declval<T1>() / coffee::declval<T2>())>>
	: true_type {
};

template<typename T, typename = void>
struct coffee_details_has_unary_minus_operator
	: false_type {
};
template<typename T>
struct coffee_details_has_unary_minus_operator<T, voidT<decltype(-coffee::declval<T>())>>
	: true_type {
};


COFFEE_NAMESPACE_END(details)
template<typename T1, typename T2 = T1>
using has_plus_operator = bool_constant<coffee::details::coffee_details_has_plus_operator<T1, T2>::value>;

template<typename T1, typename T2 = T1>
using has_minus_operator = bool_constant<coffee::details::coffee_details_has_minus_operator<T1, T2>::value>;

template<typename T1, typename T2 = T1>
using has_multiply_operator = bool_constant<coffee::details::coffee_details_has_multiply_operator<T1, T2>::value>;

template<typename T1, typename T2 = T1>
using has_divide_operator = bool_constant<coffee::details::coffee_details_has_divide_operator<T1, T2>::value>;

template<typename T>
using has_unary_minus_operator = bool_constant<coffee::details::coffee_details_has_unary_minus_operator<T>::value>;

COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)

template<typename T1, typename T2, typename = has_equal_operator<T1, T2>, typename = has_less_operator<T1, T2>, typename = has_greater_operator<T1, T2>>
struct coffee_details_is_comparable
	: false_type {
};

template<typename T1, typename T2>
struct coffee_details_is_comparable<T1, T2, true_type, true_type, true_type>
	: true_type {
};

COFFEE_NAMESPACE_END(details)
template<typename T1, typename T2 = T1>
using is_comparable = bool_constant<coffee::details::coffee_details_is_comparable<T1, T2>::value>;

COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
#endif/*COFFEE_STDCPP*/
