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
namespace Coffee
{
	namespace Runtime
	{
		struct KindBase
		{
			enum EnumType : uint8_t
			{
				Value = 0,
				Enum,
				Union,
				Class,
				Function,
			};
		};

		using KindE = typename KindBase::EnumType;

		struct CategoryBase /*category of value types*/
		{
			enum EnumType : uint8_t
			{
				Bool = 0,
				Char,
				Enum,
				Unsigned,
				Signed,
				Float,
				Pointer,
				None = 0XFF /*none value types*/
			};
		};
		
		using CategoryE = typename CategoryBase::EnumType;
	}
}

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_remove_const
	: std::remove_const<T> {
};

template<typename T>
struct coffee_details_remove_reference
	: std::remove_reference<T> {
};

template<typename T>
struct coffee_details_remove_pointer
	: std::remove_pointer<T> {
};

template<typename T>
struct coffee_details_remove_const_reference
	: coffee_details_remove_const<typename coffee_details_remove_reference<T>::type> {
};

template<typename T, typename U>
struct coffee_details_add_modifier {
	using type = T;
};

template<typename T, typename U>
struct coffee_details_add_modifier<T, U const> {
	using type = const T;
};

template<typename T, typename U>
struct coffee_details_add_modifier<T, U*> {
	using type = T*;
};

template<typename T, typename U>
struct coffee_details_add_modifier<T, const U*> {
	using type = const T*;
};

template<typename T, typename U>
struct coffee_details_add_modifier<T, U&> {
	using type = T&;
};

template<typename T, typename U>
struct coffee_details_add_modifier<T, U&&> {
	using type = T&&;
};

template<typename T, typename U>
struct coffee_details_add_modifier<T, const U&> {
	using type = const T&;
};
COFFEE_NAMESPACE_END(details)

template<typename T>
using remove_const = typename coffee::details::coffee_details_remove_const<T>::type;

template<typename T>
using remove_reference = typename coffee::details::coffee_details_remove_reference<T>::type;

template<typename T>
using remove_pointer = typename coffee::details::coffee_details_remove_pointer<T>::type;

template<typename T>
using remove_const_reference = typename coffee::details::coffee_details_remove_const_reference<T>::type;

template<typename T, typename U>
using add_modifier = typename coffee::details::coffee_details_add_modifier<T, U>::type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_add_reference {
	using lvalue_type = typename std::add_lvalue_reference<remove_reference<T>>::type;
	using rvalue_type = typename std::add_rvalue_reference<remove_reference<T>>::type;
};
#ifdef _MANAGED
template<typename T>
struct coffee_details_add_reference<T^> {
	using lvalue_type = T^;
	using rvalue_type = T^;
};
#endif
COFFEE_NAMESPACE_END(details)
template<typename T>
using add_reference = coffee::details::coffee_details_add_reference<T>;

template<typename T>
using add_lvalue = typename coffee::details::coffee_details_add_reference<T>::lvalue_type;

template<typename T>
using add_rvalue = typename coffee::details::coffee_details_add_reference<T>::rvalue_type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_void
	: false_type {
};

template<>
struct coffee_details_is_void<void>
	: true_type {
};

template<>
struct coffee_details_is_void<const void>
	: true_type {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_void = bool_constant<coffee::details::coffee_details_is_void<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T1, typename T2>
struct coffee_details_is_same
	: false_type {
};

template<typename T>
struct coffee_details_is_same<T, T>
	: true_type {
};

template<typename T1, typename T2>
struct coffee_details_is_base_of
	: std::is_base_of<T1,T2> {
};
COFFEE_NAMESPACE_END(details)
template<typename T1, typename T2>
using is_same = bool_constant<coffee::details::coffee_details_is_same<remove_const_reference<T1>, remove_const_reference<T2>>::value>;

template<typename T1, typename T2>
using is_superclass = bool_constant<coffee::details::coffee_details_is_base_of<remove_const_reference<T1>, remove_const_reference<T2>>::value>;

template<typename T1, typename T2>
using is_subclass = bool_constant<coffee::details::coffee_details_is_base_of<remove_const_reference<T2>, remove_const_reference<T1>>::value>;

COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_pointer
	: std::is_pointer<T> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_pointer = bool_constant<coffee::details::coffee_details_is_pointer<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_enum
	: std::is_enum<T> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_enum = bool_constant<coffee::details::coffee_details_is_enum<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_union
	: std::is_union<T> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_union = bool_constant<coffee::details::coffee_details_is_union<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_class
	: std::is_class<T> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_class = bool_constant<coffee::details::coffee_details_is_class<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_final
	: std::is_final<T> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_final = bool_constant<coffee::details::coffee_details_is_final<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_abstract
	: std::is_abstract<T> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_abstract = bool_constant<coffee::details::coffee_details_is_abstract<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename = void>
struct coffee_details_is_complete
	: false_type {
};

template<typename T>
struct coffee_details_is_complete<T, voidT<bool_constant<sizeof(T) == sizeof(T)>>>
	: true_type {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_complete = bool_constant<coffee::details::coffee_details_is_complete<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename TP, typename = void>
struct coffee_details_is_functor
	: false_type {
};

template<typename T, typename... Ts>
struct coffee_details_is_functor<T, type_pack<Ts...>, voidT<decltype(&T::template operator() < Ts... > )>>
	: true_type {
};

template<typename T>
struct coffee_details_is_functor<T, type_pack<>, voidT<decltype(&T::operator())>>
	: true_type {
};
COFFEE_NAMESPACE_END(details)
template<typename T, typename... Ts>
using is_functor = bool_constant<coffee::details::coffee_details_is_functor<T, type_pack<Ts...>>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename TP, typename = void>
struct coffee_details_is_calleable
	: false_type {
};

template<typename T, typename... Ts>
struct coffee_details_is_calleable<T, type_pack<Ts...>, voidT<decltype(coffee::declval<T>()(coffee::declval<Ts>()...))>>
	: true_type {
	using return_type = decltype(coffee::declval<T>()(coffee::declval<Ts>()...));
};
COFFEE_NAMESPACE_END(details)
template<typename T, typename... Ts>
using is_calleable = bool_constant<coffee::details::coffee_details_is_calleable<T, type_pack<Ts...>>::value>;

template<typename T, typename... Ts>
using return_type = typename coffee::details::coffee_details_is_calleable<T, type_pack<Ts...>>::return_type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_function
	: false_type {
};

template<typename T, typename... Ts>
struct coffee_details_is_function<T(Ts...)>
	: true_type {
};

template<typename T, typename... Ts>
struct coffee_details_is_function<T(*)(Ts...)>
	: true_type {
};

template<typename T, typename O, typename... Ts>
struct coffee_details_is_function<T(O::*)(Ts...)>
	: true_type {
};

COFFEE_NAMESPACE_END(details)
template<typename T>
using is_function = bool_constant<coffee::details::coffee_details_is_function<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T,
	bool = is_enum<T>::value,
	bool = is_union<T>::value,
	bool = is_class<T>::value,
	bool = is_function<T>::value>
struct coffee_details_kind_of_base
	: integral_constant<Coffee::Runtime::KindE, Coffee::Runtime::KindE::Value> {
};

template<typename T>
struct coffee_details_kind_of_base<T, true, false, false, false>
	: integral_constant<Coffee::Runtime::KindE, Coffee::Runtime::KindE::Enum> {
};

template<typename T>
struct coffee_details_kind_of_base<T, false, true, false, false>
	: integral_constant<Coffee::Runtime::KindE, Coffee::Runtime::KindE::Union> {
};

template<typename T>
struct coffee_details_kind_of_base<T, false, false, true, false>
	: integral_constant<Coffee::Runtime::KindE, Coffee::Runtime::KindE::Class> {
};

template<typename T>
struct coffee_details_kind_of_base<T, false, false, false, true>
	: integral_constant<Coffee::Runtime::KindE, Coffee::Runtime::KindE::Function> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using kind_of = integral_constant<Coffee::Runtime::KindE, coffee::details::coffee_details_kind_of_base<remove_const_reference<T>>::value>;

using value_kind = integral_constant<Coffee::Runtime::KindE, Coffee::Runtime::KindE::Value>;

COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
template<typename T>
using is_value = constexpr_or<kind_of<T>::value == Coffee::Runtime::KindE::Value, kind_of<T>::value == Coffee::Runtime::KindE::Enum>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_signed
	: constexpr_and<std::is_integral<T>::value, std::is_signed<T>::value> {
};

template<typename T>
struct coffee_details_is_unsigned
	: std::is_unsigned<T> {
};

template<typename T>
struct coffee_details_is_integral
	: constexpr_or<coffee_details_is_signed<T>::value, coffee_details_is_unsigned<T>::value> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_signed = bool_constant<coffee::details::coffee_details_is_signed<remove_const_reference<T>>::value>;

template<typename T>
using is_unsigned = bool_constant<coffee::details::coffee_details_is_unsigned<remove_const_reference<T>>::value>;

template<typename T>
using is_integral = bool_constant<coffee::details::coffee_details_is_integral<remove_const_reference<T>>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_float
	: false_type {
};

template<>
struct coffee_details_is_float<float>
	: true_type {
};

template<>
struct coffee_details_is_float<double>
	: true_type {
};

template<>
struct coffee_details_is_float<long double>
	: true_type {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_float = bool_constant<coffee::details::coffee_details_is_float<remove_const_reference<T>>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_char
	: false_type {
};

template<>
struct coffee_details_is_char<char>
	: true_type {
};

template<>
struct coffee_details_is_char<wchar_t>
	: true_type {
};

//template<>
//struct coffee_details_is_char<char8_t>
//	: true_type {
//};

template<>
struct coffee_details_is_char<char16_t>
	: true_type {
};

template<>
struct coffee_details_is_char<char32_t>
	: true_type {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_char = bool_constant<coffee::details::coffee_details_is_char<remove_const_reference<T>>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_bool
	: false_type {
};

template<>
struct coffee_details_is_bool<bool>
	: true_type {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_bool = bool_constant<coffee::details::coffee_details_is_bool<remove_const_reference<T>>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_is_arithmetic
	: std::is_arithmetic<T> {
};

template<typename...Ts>
struct coffee_details_is_arithmetic<type_pack<Ts...>>
	: constexpr_and<coffee_details_is_arithmetic<Ts>::value...> {
};

COFFEE_NAMESPACE_END(details)
template<typename T>
using is_arithmetic = bool_constant<coffee::details::coffee_details_is_arithmetic<remove_const_reference<T>>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, bool = is_enum<T>::value>
struct coffee_details_underlying_type {
	using type = typename std::underlying_type<remove_const_reference<T>>::type;
};

template<typename T>
struct coffee_details_underlying_type<T, false> {
	using type = remove_const_reference<T>;
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using underlying = typename coffee::details::coffee_details_underlying_type<T>::type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)

template<typename T, size_t = sizeof(T)>
struct coffee_details_integral_type;

template<typename T>
struct coffee_details_integral_type<T, 1u> {
	using signed_type = int8_t;
	using unsigned_type = uint8_t;
	using integral_type = conditional<is_signed<T>::value, signed_type, unsigned_type>;
};

template<typename T>
struct coffee_details_integral_type<T, 2u> {
	using signed_type = int16_t;
	using unsigned_type = uint16_t;
	using integral_type = conditional<is_signed<T>::value, signed_type, unsigned_type>;
};

template<typename T>
struct coffee_details_integral_type<T, 4u> {
	using signed_type = int32_t;
	using unsigned_type = uint32_t;
	using integral_type = conditional<is_signed<T>::value, signed_type, unsigned_type>;
};

template<typename T>
struct coffee_details_integral_type<T, 8u> {
	using signed_type = int64_t;
	using unsigned_type = uint64_t;
	using integral_type = conditional<is_signed<T>::value, signed_type, unsigned_type>;
};

COFFEE_NAMESPACE_END(details)
template<typename T>
using signed_type = typename coffee::details::coffee_details_integral_type<remove_const_reference<T>>::signed_type;
template<typename T>
using unsigned_type = typename coffee::details::coffee_details_integral_type<remove_const_reference<T>>::unsigned_type;
template<typename T>
using integral_type = typename coffee::details::coffee_details_integral_type<remove_const_reference<T>>::integral_type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T
	, bool = is_bool<T>::value
	, bool = is_char<T>::value
	, bool = is_enum<T>::value
	, bool = is_unsigned<T>::value
	, bool = is_signed<T>::value
	, bool = is_float<T>::value
	, bool = is_pointer<T>::value
	>
struct coffee_details_category {
	using type = T;
	static constexpr Runtime::CategoryE value = Runtime::CategoryE::None;
};

template<typename T, bool N1, bool N2, bool N3, bool N4, bool N5, bool N6>
struct coffee_details_category<T, true, N1, N2, N3, N4, N5, N6> {
	using type = T;
	static constexpr Runtime::CategoryE value = Runtime::CategoryE::Bool;
};

template<typename T, bool N1, bool N2, bool N3, bool N4, bool N5>
struct coffee_details_category<T, false, true, N1, N2, N3, N4, N5> {
	using type = T;
	static constexpr Runtime::CategoryE value = Runtime::CategoryE::Char;
};

template<typename T, bool N1, bool N2, bool N3, bool N4>
struct coffee_details_category<T, false, false, true, N1, N2, N3, N4> {
	using type = T;
	static constexpr Runtime::CategoryE value = Runtime::CategoryE::Enum;
};

template<typename T, bool N1, bool N2, bool N3>
struct coffee_details_category<T, false, false, false, true, N1, N2, N3> {
	using type = T;
	static constexpr Runtime::CategoryE value = Runtime::CategoryE::Unsigned;
};

template<typename T, bool N1, bool N2>
struct coffee_details_category<T, false, false, false, false, true, N1, N2> {
	using type = T;
	static constexpr Runtime::CategoryE value = Runtime::CategoryE::Signed;
};

template<typename T, bool N1>
struct coffee_details_category<T, false, false, false, false, false, true, N1> {
	using type = T;
	static constexpr Runtime::CategoryE value = Runtime::CategoryE::Float;
};

template<typename T>
struct coffee_details_category<T, false, false, false, false, false, false, true> {
	using type = T;
	static constexpr Runtime::CategoryE value = Runtime::CategoryE::Pointer;
};

COFFEE_NAMESPACE_END(details)
template<typename T>
using category = integral_constant<Runtime::CategoryE, coffee::details::coffee_details_category<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T
	, bool = is_float<T>::value
	, bool = is_unsigned<T>::value
	, bool = is_signed<T>::value>
struct coffee_details_longest_type {
	using type = T;
};

template<typename T, bool N1, bool N2>
struct coffee_details_longest_type<T, true, N1, N2> {
	using type = double;
};

template<typename T, bool N1>
struct coffee_details_longest_type<T, false, true, N1> {
	using type = uint64_t;
};

template<typename T>
struct coffee_details_longest_type<T, false, false, true> {
	using type = int64_t;
};

template<typename...Ts>
struct coffee_details_longest_type<type_pack<Ts...>, false, false, false> {
	using type = type_pack<typename coffee_details_longest_type<Ts>::type...>;
};

COFFEE_NAMESPACE_END(details)
template<typename T>
using longest = typename coffee::details::coffee_details_longest_type<T>::type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T1, typename T2>
struct coffee_details_is_same_category
	: std::false_type {
};



COFFEE_NAMESPACE_END(details)
template<typename T>
using is_arithmetic = bool_constant<coffee::details::coffee_details_is_arithmetic<remove_const_reference<T>>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename TP, bool = is_abstract<T>::value, typename = void>
struct coffee_details_is_constructible
	: false_type {
};

template<typename T, typename...Ts>
struct coffee_details_is_constructible<T, type_pack<Ts...>, false, voidT<decltype(new T(coffee::forward<Ts>(coffee::declval<Ts&&>())...))>>
	: true_type {
};

template<typename T, bool = is_abstract<T>::value>
struct coffee_details_is_default_constructible
	: false_type {
};

template<typename T>
struct coffee_details_is_default_constructible<T, false>
	: std::is_default_constructible<T> {
};

template<typename T>
struct coffee_details_is_trivially_constructible
	: std::is_trivially_constructible<T> {
};

template<typename...Ts>
struct coffee_details_is_trivially_constructible<type_pack<Ts...>>
	: constexpr_and<coffee_details_is_trivially_constructible<Ts>::value...> {
};

template<typename T>
struct coffee_details_is_trivially_destructible
	: std::is_trivially_destructible<T> {
};

template<typename...Ts>
struct coffee_details_is_trivially_destructible<type_pack<Ts...>>
	: constexpr_and<coffee_details_is_trivially_destructible<Ts>::value...> {
};

COFFEE_NAMESPACE_END(details)

template<typename T, typename U>
using is_constructible = bool_constant<coffee::details::coffee_details_is_constructible<T, packing<U>>::value>;

template<typename T, typename U>
using is_constructible_at_inverse = bool_constant<coffee::details::coffee_details_is_constructible<U, packing<T>>::value>;

template<typename T, typename...Ts>
using is_constructible_args = bool_constant<coffee::details::coffee_details_is_constructible<T, type_pack<Ts...>>::value>;

template<typename T>
using is_default_constructible = bool_constant<coffee::details::coffee_details_is_constructible<T, type_pack<>>::value>;

template<typename T>
using is_move_constructible = bool_constant<coffee::details::coffee_details_is_constructible<T, type_pack<T&&>>::value>;

template<typename T>
using is_copy_constructible = bool_constant<coffee::details::coffee_details_is_constructible<T, type_pack<const T&>>::value>;

template<typename T>
using is_trivially_constructible = bool_constant<coffee::details::coffee_details_is_trivially_constructible<T>::value>;

template<typename T>
using is_trivially_destructible = bool_constant<coffee::details::coffee_details_is_trivially_destructible<T>::value>;

COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename = is_abstract<T>>
struct coffee_details_is_move_assignable
	: false_type {
};

template<typename T>
struct coffee_details_is_move_assignable<T, false_type>
	: std::is_move_assignable<T> {
};

template<typename T, typename = is_abstract<T>>
struct coffee_details_is_copy_assignable
	: false_type {
};

template<typename T>
struct coffee_details_is_copy_assignable<T, false_type>
	: std::is_copy_assignable<T> {
};

template<typename T,typename U, typename = is_abstract<T>>
struct coffee_details_is_assignable
	: false_type {
};

template<typename T, typename U>
struct coffee_details_is_assignable<T, U, false_type>
	: std::is_assignable<T, U> {
};
COFFEE_NAMESPACE_END(details)
template<typename T>
using is_move_assignable = bool_constant<coffee::details::coffee_details_is_move_assignable<T>::value>;

template<typename T>
using is_copy_assignable = bool_constant<coffee::details::coffee_details_is_copy_assignable<T>::value>;

template<typename T, typename U>
using is_assignable = bool_constant<coffee::details::coffee_details_is_assignable<T, U>::value>;

template<typename T, typename U>
using is_assignable_at_inverse = bool_constant<coffee::details::coffee_details_is_assignable<U, T>::value>;

COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
COFFEE_NAMESPACE_END(details)

template<typename T1, typename T2>
using is_convertible = constexpr_or<
	is_same<T1,T2>::value,
	is_superclass<T1, T2>::value,
	is_subclass<T1, T2>::value,
	is_constructible<T1, T2>::value
>;

COFFEE_NAMESPACE_END(Coffee, implement)
/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)

template<typename T>
struct coffee_details_is_nullable
	: is_pointer<T> {
};

COFFEE_NAMESPACE_END(details)
template<typename T>
using is_nullable = bool_constant<coffee::details::coffee_details_is_nullable<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
#endif/*COFFEE_STDCPP*/
