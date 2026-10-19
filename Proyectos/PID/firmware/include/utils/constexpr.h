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
template<bool VALUE>
using constexpr_yes = bool_constant<VALUE>;

template<bool VALUE>
using constexpr_not = bool_constant<!VALUE>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<bool...VALUES> 
struct coffee_details_constexpr_and;

template<bool VALUE>
struct coffee_details_constexpr_and<VALUE>
	: bool_constant<VALUE> {
};

template<bool VALUE, bool...VALUES>
struct coffee_details_constexpr_and<VALUE, VALUES...>
	: bool_constant<VALUE && coffee_details_constexpr_and<VALUES...>::value> {
};
COFFEE_NAMESPACE_END(details)
template<bool...VALUES>
using constexpr_and = bool_constant<coffee::details::coffee_details_constexpr_and<VALUES...>::value>;

template<bool...VALUES>
using constexpr_nand = constexpr_not<coffee::details::coffee_details_constexpr_and<VALUES...>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<bool...VALUES>
struct coffee_details_constexpr_or;

template<bool VALUE>
struct coffee_details_constexpr_or<VALUE>
	: bool_constant<VALUE> {
};

template<bool VALUE, bool...VALUES>
struct coffee_details_constexpr_or<VALUE, VALUES...>
	: bool_constant<VALUE || coffee_details_constexpr_or<VALUES...>::value> {
};
COFFEE_NAMESPACE_END(details)
template<bool...VALUES>
using constexpr_or = bool_constant<coffee::details::coffee_details_constexpr_or<VALUES...>::value>;

template<bool...VALUES>
using constexpr_nor = constexpr_not<coffee::details::coffee_details_constexpr_or<VALUES...>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<bool V1, bool V2>
struct coffee_details_constexpr_xor
	: false_type {
};

template<>
struct coffee_details_constexpr_xor<true,false>
	: true_type {
};

template<>
struct coffee_details_constexpr_xor<false,true>
	: true_type {
};
COFFEE_NAMESPACE_END(details)
template<bool V1, bool V2>
using constexpr_xor = bool_constant<coffee::details::coffee_details_constexpr_xor<V1, V2>::value>;

template<bool V1, bool V2>
using constexpr_xnor = constexpr_not<coffee::details::coffee_details_constexpr_xor<V1, V2>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
template<bool TEST, typename T = void>
using enable_if = typename std::enable_if<TEST, T>::type;

template<bool TEST, typename T1, typename T2>
using conditional = typename std::conditional<TEST, T1, T2>::type;

template<bool TEST, typename T, T VALUE1, T VALUE2>
using constexpr_select = conditional<TEST, integral_constant<T, VALUE1>, integral_constant<T, VALUE2>>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, T N1, T N2 = N1>
struct coffee_details_constexpr_equal {
	static constexpr bool value = (N1 == N2);
};

template<typename T, T N1, T N2 = N1>
struct coffee_details_constexpr_not_equal {
	static constexpr bool value = (N1 != N2);
};

template<typename T, T N1, T N2 = N1>
struct coffee_details_constexpr_greater {
	static constexpr bool value = (N1 > N2);
};

template<typename T, T N1, T N2 = N1>
struct coffee_details_constexpr_less {
	static constexpr bool value = (N1 < N2);
};
COFFEE_NAMESPACE_END(details)
template<typename T, T N1, T N2 = N1>
using constexpr_equal = bool_constant<coffee::details::coffee_details_constexpr_equal<T,N1,N2>::value>;

template<typename T, T N1, T N2 = N1>
using constexpr_not_equal = bool_constant<coffee::details::coffee_details_constexpr_not_equal<T, N1, N2>::value>;

template<typename T, T N1, T N2 = N1>
using constexpr_greater = bool_constant<coffee::details::coffee_details_constexpr_greater<T, N1, N2>::value>;

template<typename T, T N1, T N2 = N1>
using constexpr_less = bool_constant<coffee::details::coffee_details_constexpr_less<T, N1, N2>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T1, typename T2 = T1>
struct coffee_details_comparison_type {
	using type = decltype(coffee::declval<T1>() + coffee::declval<T2>());
};

template<typename T>
struct coffee_details_comparison_type<T,T> {
	using type = T;
};

template<typename T>
struct coffee_details_comparison_type<T*, T*> {
	using type = T*;
};
COFFEE_NAMESPACE_END(details)
template<typename T1, typename T2 = T1>
using comparison_type = typename coffee::details::coffee_details_comparison_type<T1, T2>::type;

template<typename T1, typename T2>
constexpr inline comparison_type<T1, T2> min(T1 N1, T2 N2) {
	return coffee::forward<comparison_type<T1, T2>>(N1 < N2 ? comparison_type<T1, T2>(N1) : comparison_type<T1, T2>(N2));
}
template<typename T1, typename T2>
constexpr inline comparison_type<T1, T2> max(T1 N1, T2 N2) {
	return coffee::forward<comparison_type<T1, T2>>(N1 > N2 ? comparison_type<T1, T2>(N1) : comparison_type<T1, T2>(N2));
}
template<typename T1, typename T2>
constexpr inline comparison_type<T1, T2> align_up(T1 VALUE, T2 ALIGNMENT) {
	return (VALUE % ALIGNMENT) && VALUE ? VALUE + ALIGNMENT - (VALUE % ALIGNMENT) : VALUE;
}
template<typename T1, typename T2>
constexpr inline comparison_type<T1, T2> align_down(T1 ALIGNMENT, T2 VALUE) {
	return (VALUE % ALIGNMENT) && VALUE ? VALUE - (VALUE % ALIGNMENT) : VALUE;
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<bool TEST, typename FN1, typename FN2>
struct coffee_details_constexpr_if_else {
	using type = decltype(declval<FN1&&>()(0));
	static type execute(FN1&& func, FN2&&) {
		return coffee::forward<type>(static_cast<FN1&&>(func)(0));
	}
};
template<typename FN1, typename FN2>
struct coffee_details_constexpr_if_else<false, FN1, FN2> {
	using type = decltype(declval<FN2&&>()(0));
	static type execute(FN1&&, FN2&& func) {
		return coffee::forward<type>(static_cast<FN2&&>(func)(0));
	}
};
COFFEE_NAMESPACE_END(details)
template<bool TEST, typename FN1, typename FN2>
typename coffee::details::coffee_details_constexpr_if_else<TEST, FN1, FN2>::type constexpr_if(FN1&& func1, FN2&& func2) {
	return coffee::details::coffee_details_constexpr_if_else<TEST, FN1, FN2>::execute(forward<FN1&&>(func1), coffee::forward<FN2&&>(func2));
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<bool TEST, typename FN>
struct coffee_details_constexpr_if {
	static constexpr bool execute(FN&& func) {
		static_cast<FN&&>(func)();
		return true;
	}
};
template<typename FN>
struct coffee_details_constexpr_if<false, FN> {
	static constexpr bool execute(FN&& func) {
		return false;
	}
};
COFFEE_NAMESPACE_END(details)
template<bool TEST, typename FN>
constexpr bool constexpr_if(FN&& func) {
	return coffee::details::coffee_details_constexpr_if<TEST, FN>::execute(coffee::forward<FN&&>(func));
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
#endif/*COFFEE_STDCPP*/
