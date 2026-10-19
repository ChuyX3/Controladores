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
template<typename T, bool = is_complete<T>::value>
struct coffee_details_size_of
	: integral_constant<size_t, sizeof(T)> {
};

template<typename T>
struct coffee_details_size_of<T, false>
	: integral_constant<size_t, 0> {
};

template<typename T, bool = is_complete<T>::value>
struct coffee_details_align_of
	: integral_constant<size_t, alignof(T)> {
};

template<typename T>
struct coffee_details_align_of<T, false>
	: integral_constant<size_t, 0> {
};
COFFEE_NAMESPACE_END(details)
template<typename T> constexpr size_t size_of() { return coffee::details::coffee_details_size_of<remove_const_reference<T>>::value; }
template<typename T> constexpr size_t align_of() { return coffee::details::coffee_details_align_of<remove_const_reference<T>>::value; }
template<> constexpr size_t align_of<void>() { return coffee::details::coffee_details_align_of<size_t>::value;}

template<typename T> constexpr size_t size_of(T const&) { return coffee::details::coffee_details_size_of<T>::value; }
template<typename T> constexpr size_t align_of(T const&) { return coffee::details::coffee_details_align_of<T>::value; }

template<typename T, size_t N> constexpr size_t size_of(T(&)[N]) { return coffee::details::coffee_details_size_of<T>::value * N; }
template<typename T, size_t N> constexpr size_t size_of(const T(&)[N]) { return coffee::details::coffee_details_size_of<T>::value * N; }
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(runtime)
COFFEE_NAMESPACE_BEGIN(details)
template<typename A, typename B, typename = is_convertible<A, B>>
struct coffee_runtime_details_super_cast {
	static inline remove_reference<A>& cast(B&& b) { return static_cast<remove_reference<A>&>(b); }
	static inline remove_pointer<A>* cast(B* b) { return static_cast<remove_pointer<A>*>(b); }
};

template<typename A, typename B>
struct coffee_runtime_details_super_cast<A,B, false_type> {
	static inline remove_reference<A>& cast(B&& b) { return static_cast<remove_reference<A>&>(b); }
	static inline remove_pointer<A>* cast(B* b) { return static_cast<remove_pointer<A>*>(b); }
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(runtime)
template<typename A, typename B>
inline remove_reference<A>& super_cast(B&& b) {
	return runtime::details::coffee_runtime_details_super_cast<A,B>::cast(coffee::forward<B>(b));
}
template<typename A, typename B>
inline remove_pointer<A>* super_cast(B* b) {
	return runtime::details::coffee_runtime_details_super_cast<remove_pointer<A>, B>::cast(b);
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, bool = is_function<T>::value>
struct coffee_details_adressof {
	static constexpr T* value(T& val) {
		return (reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<const volatile char&>(val))));
	}
};

template<typename T>
struct coffee_details_adressof<T, true> {
	static constexpr T* value(T& val) {
		return std::addressof(val);
	}
};
COFFEE_NAMESPACE_END(details)
template<typename T>
inline constexpr T* address_of(T& val) noexcept {
	return coffee::details::coffee_details_adressof<T>::value(val);
}
template<typename T>
inline constexpr T const* address_of(T const& val) noexcept {
	return coffee::details::coffee_details_adressof<T>::value(const_cast<T&>(val));
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, bool = is_pointer<T>::value>
struct coffee_details_to_pointer {
	static constexpr T value(T ptr) { return ptr; }
};

template<typename T>
struct coffee_details_to_pointer<T, false> {
	static T* value(T& val) { return coffee::address_of(val); }
};
COFFEE_NAMESPACE_END(details)
template<typename T>
remove_pointer<T>* to_pointer(T& val) { 
	return coffee::details::coffee_details_to_pointer<T>::value(val); 
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
template<typename T, typename U>
constexpr size_t offset_of() {
	static_assert(coffee::is_superclass<U, T>::value, "U is not a base of T");
	return reinterpret_cast<size_t>(coffee::super_cast<U*>(reinterpret_cast<T*>(0X10000))) - 0X10000;
}

template<typename T, typename U>
constexpr size_t offset_of(U T::* member) {
	return size_t((byte*)&((T*)nullptr->*member) - (byte*)nullptr);
}

template<typename T, typename U, typename V>
T* field_to_parent(U T::* member, V* field) {
	static_assert(coffee::is_convertible<U, V>::value, "can't convert type V to U");
	return (T*)(size_t(field) - coffee::offset_of(member));
}

template<typename T, typename U, typename V>
T const* field_to_parent(U T::* member, V const* field) {
	static_assert(coffee::is_convertible<U, V>::value, "can't convert type V to U");
	return (T const*)(size_t(field) - coffee::offset_of(member));
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
template<size_t AT, typename T, size_t SZ>
constexpr T get_at(const T(&arr)[SZ]) noexcept { return AT < SZ ? arr[AT] : (T)0; }
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename = is_lvalue_reference<T>>
struct coffee_details_put_args
{
	using type = longest<T>;
	static constexpr type cast(remove_reference<T>&& args) {
		return static_cast<type>(args);
	}
};

template<typename T>
struct coffee_details_put_args<T, true_type>
{
	using type = remove_reference<T>*;
	static constexpr type cast(T args) {
		return coffee::address_of(args);
	}
};

template<typename T, typename = is_lvalue_reference<T>>
struct coffee_details_get_args
{
	using return_type = T;
	using argument_type = longest<remove_reference<T>>;
	static constexpr return_type cast(argument_type args) {
		return static_cast<return_type>(args);
	}
};

template<typename T>
struct coffee_details_get_args<T, true_type>
{
	using return_type = remove_reference<T>&;
	using argument_type = remove_reference<T>*;
	static constexpr return_type cast(argument_type args) {
		return *args;
	}
};

COFFEE_NAMESPACE_END(details)
template <typename T>
constexpr typename details::coffee_details_put_args<T>::type putargs(T&& args) noexcept {
	return details::coffee_details_put_args<T>::cast(coffee::forward<T>(args));
}

template <typename T>
constexpr typename details::coffee_details_put_args<T>::type putargs(T& args) noexcept {
	return details::coffee_details_put_args<T>::cast(coffee::forward<T>(args));
}

template <typename T>
constexpr typename details::coffee_details_get_args<T>::return_type getargs(typename details::coffee_details_get_args<T>::argument_type&& args) noexcept {
	return details::coffee_details_get_args<T>::cast(coffee::forward<typename details::coffee_details_get_args<T>::argument_type>(args));
}

template <typename T>
constexpr typename details::coffee_details_get_args<T>::return_type getargs(typename details::coffee_details_get_args<T>::argument_type& args) noexcept {
	return details::coffee_details_get_args<T>::cast(args);
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
#endif/*COFFEE_STDCPP*/
