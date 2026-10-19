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

template<>
struct type_pack<> {
	using empty = type_pack<>;
};

template<typename T, typename...Ts>
struct type_pack<T, Ts...> {
	using first = T;
	using rest = type_pack<Ts...>;
	using empty = type_pack<>;
};

template<typename T, T...NS>
struct value_pack {
	using type = T;
	using empty = value_pack<T>;
	template<type N> using integral = integral_constant<type, N>;
	static constexpr T pack[] = { NS... };
};

template<typename T>
struct value_pack<T> {
	using type = T;
	using empty = value_pack<T>;
	template<type N> using integral = integral_constant<type, N>;
};
template<char... chars>
using char_pack = value_pack<char, chars...>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename>
struct coffee_details_pack_max;

template<typename>
struct coffee_details_pack_min;

template<typename>
struct coffee_details_pack_pop_front;

template<typename P, typename = typename P::empty>
struct coffee_details_pack_pop_back;

template<typename P>
struct coffee_details_pack_get_front;

template<typename P>
struct coffee_details_pack_get_back;

template<typename P, size_t AT>
struct coffee_details_pack_get_at;

template<typename P, typename T, template<typename, typename> typename compare>
struct coffee_details_pack_contains : false_type {};

template<typename P, typename T, template<typename, typename> typename compare, typename = integral_constant<index_t, 0>>
struct coffee_details_pack_contains_at : invalid_index_t {};

template<typename P, typename T, size_t I = 0>
struct coffee_details_pack_index_of;
COFFEE_NAMESPACE_END(details)
template<typename P, size_t AT>
using pack_get = typename coffee::details::coffee_details_pack_get_at<P, AT>::type;

template<typename P>
using pack_get_front = typename coffee::details::coffee_details_pack_get_front<P>::type;

template<typename P>
using pack_get_back = typename coffee::details::coffee_details_pack_get_back<P>::type;

template<typename P, typename T, template<typename, typename> class compare = is_same>
using pack_contains = bool_constant<coffee::details::coffee_details_pack_contains<P, T, compare>::value>;

template<typename P, typename T, template<typename, typename> class compare = is_same>
using pack_contains_at = integral_constant<index_t, coffee::details::coffee_details_pack_contains_at<P, T, compare>::value>;

template<typename P, typename T>
using pack_contains_s = pack_contains<P, T, is_same>;

template<typename P, typename T>
using pack_contains_at_s = pack_contains_at<P, T, is_same>;

template<typename P>
using pack_pop_back = typename coffee::details::coffee_details_pack_pop_back<P>::type;

template<typename P>
using pack_pop_front = typename coffee::details::coffee_details_pack_pop_front<P>::type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename...Ps>
struct coffee_details_pack_append;

template<typename P1, typename P2, template<typename P, typename T> class contains>
struct coffee_details_pack_norepeat;
COFFEE_NAMESPACE_END(details)
template<typename...Ps>
using pack_append = typename coffee::details::coffee_details_pack_append<Ps...>::type;

template<typename P, template<typename, typename> class contains = pack_contains_s>
using pack_norepeat = typename coffee::details::coffee_details_pack_norepeat<P, typename P::empty, contains>::type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_size_of<value_pack<T>, true>
	: integral_constant<size_t, 0> {
};

template<typename T, T...NS>
struct coffee_details_size_of<value_pack<T, NS...>, true>
	: integral_constant<size_t, sizeof...(NS)> {
};

template<typename T, T N>
struct coffee_details_pack_max<value_pack<T, N>> {
	static constexpr T value = N;
};
template<typename T, T N, T...NS>
struct coffee_details_pack_max<value_pack<T, N, NS...>> {
	static constexpr T value = coffee::max(N, coffee_details_pack_max<value_pack<T, NS...>>::value);
};

template<typename T, T N>
struct coffee_details_pack_min<value_pack<T, N>> {
	static constexpr T value = N;
};
template<typename T, T N, T...NS>
struct coffee_details_pack_min<value_pack<T, N, NS...>> {
	static constexpr T value = coffee::min(N, coffee_details_pack_min<value_pack<T, NS...>>::value);
};
COFFEE_NAMESPACE_END(details)
template<typename T, T... NS>
constexpr inline comparison_type<T> min(value_pack<T, NS...> const&) {
	return coffee::details::coffee_details_pack_min<value_pack<T, NS...>>::value;
}
template<typename T, T... NS>
constexpr inline comparison_type<T> max(value_pack<T, NS...> const&) {
	return coffee::details::coffee_details_pack_max<value_pack<T, NS...>>::value;
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, T N, T... NS>
struct coffee_details_pack_pop_front<value_pack<T, N, NS...>> {
	using type = value_pack<T, NS...>;
};

template<typename T, T N, T... N1S, T... N2S>
struct coffee_details_pack_pop_back<value_pack<T, N, N1S...>, value_pack<T, N, N2S...>>
	: coffee_details_pack_pop_back<value_pack<T, N1S...>, value_pack<T, N2S..., N>> {
};

template<typename T, T N, T... NS>
struct coffee_details_pack_pop_back<value_pack<T, N>, value_pack<T, NS...>> {
	using type = value_pack<T, NS...>;
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, T N, T...NS>
struct coffee_details_pack_get_at<value_pack<T,N, NS...>, 0> {
	using type = integral_constant<T, N>;
};

template<typename T, T N, T...NS, size_t AT>
struct coffee_details_pack_get_at<value_pack<T, N, NS...>, AT>
	: coffee_details_pack_get_at<value_pack<T, NS...>, AT - 1> {
};

template<typename T, T N, T...NS>
struct coffee_details_pack_get_front<value_pack<T, N, NS...>> {
	using type = integral_constant<T, N>;
};

template<typename T, T N, T...NS>
struct coffee_details_pack_get_back<value_pack<T, N, NS...>>
	: coffee_details_pack_get_back<value_pack<T, NS...>> {
};

template<typename T, T N>
struct coffee_details_pack_get_back<value_pack<T, N>> {
	using type = integral_constant<T, N>;
};
COFFEE_NAMESPACE_END(details)

template<size_t AT, typename T, T...NS>
constexpr T get_at(const value_pack<T, NS...>&, indexT<AT> const& = {}) noexcept {
	return pack_get<value_pack<T, NS...>, AT>::value;
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, T N, T N1, T...NS, template<typename, typename> typename compare>
struct coffee_details_pack_contains<value_pack<T, N1, NS...>, integral_constant<T, N>, compare>
	: conditional<compare<integral_constant<T,N1>, integral_constant<T, N>>::value
	, true_type
	, coffee_details_pack_contains<value_pack<T, NS...>, integral_constant<T, N>, compare>> {
};

template<typename T, T N, T N1, T...NS, template<typename, typename> typename compare, index_t AT>
struct coffee_details_pack_contains_at<value_pack<T, N1, NS...>, integral_constant<T, N>, compare, integral_constant<index_t, AT>>
	: conditional<compare<integral_constant<T, N1>, integral_constant<T, N>>::value
	, integral_constant<index_t, AT>
	, coffee_details_pack_contains_at<value_pack<T, NS...>, integral_constant<T, N>, compare, integral_constant<index_t, AT + 1>> > {
};

COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, T N, T...NS, size_t I>
struct coffee_details_pack_index_of<value_pack<T, N, NS...>, integral_constant<T, N>, I> {
	using type = integral_constant<size_t, I>;
};

template<typename T, T N, size_t I>
struct coffee_details_pack_index_of<value_pack<T>, integral_constant<T, N>, I> {
	using type = integral_constant<size_t, I>;
};

template<typename T, T N, T...NS, T M, size_t I>
struct coffee_details_pack_index_of<value_pack<T, N, NS...>, integral_constant<T, M>, I>
	: coffee_details_pack_index_of<value_pack<T, NS...>, integral_constant<T, M>, I + 1> {
};
COFFEE_NAMESPACE_END(details)

template<typename T, typename P>
constexpr size_t index_of(P const&, T const&) {
	return coffee::details::coffee_details_pack_index_of<P, T>::type::value;
}
template<typename T, typename P>
constexpr size_t index_of(P const& = {}) {
	return coffee::details::coffee_details_pack_index_of<P, T>::type::value;
}
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, T...Ns>
struct coffee_details_pack_append<value_pack<T, Ns...>> {
	using type = value_pack<T, Ns...>;
};

template<typename T, T...N1S, T...N2S, typename...Ps>
struct coffee_details_pack_append<value_pack<T, N1S...>, value_pack<T, N2S...>, Ps...> {
	using type = typename coffee_details_pack_append<value_pack<T, N1S..., N2S...>, Ps...>::type;
};

template<typename T, T BEG, T END, bool = constexpr_less<T, END, BEG>::value>
struct coffee_details_pack_range {
	using type = typename coffee_details_pack_append<value_pack<T, BEG>, typename coffee_details_pack_range<T, T((underlying<T>)(BEG) + 1), END>::type>::type;
};

template<typename T, T BEG>
struct coffee_details_pack_range<T, BEG, BEG, false> {
	using type = value_pack<T, BEG>;
};

template<typename T, T BEG, T END>
struct coffee_details_pack_range<T, BEG, END, true> {
	using type = typename coffee_details_pack_append<value_pack<T, BEG>, typename coffee_details_pack_range<T, T((underlying<T>)(BEG) - 1), END>::type>::type;
};
COFFEE_NAMESPACE_END(details)
template<typename T, T BEG, T END>
using pack_range = typename coffee::details::coffee_details_pack_range<T, BEG, END>::type;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, T N, T...N1S, T...N2S, template<typename P, typename U> class contains>
struct coffee_details_pack_norepeat<value_pack<T, N, N1S...>, value_pack<T, N2S...>, contains>
	: conditional<contains<value_pack<T, N2S...>, integral_constant<T, N>>::value
	, coffee_details_pack_norepeat<value_pack<T, N1S...>, value_pack<T, N2S...>, contains>
	, coffee_details_pack_norepeat<value_pack<T, N1S...>, value_pack<T, N2S..., N>, contains>>
{
};

template<typename T, T...NS, template<typename P, typename U> class contains>
struct coffee_details_pack_norepeat<value_pack<T>, value_pack<T, NS...>, contains> {
	using type = value_pack<T, NS...>;
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T>
struct coffee_details_packing {
	using type = type_pack<T>;
};

template<typename...Ts>
struct coffee_details_packing<type_pack<Ts...>> {
	using type = type_pack<Ts...>;
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<>
struct coffee_details_size_of<type_pack<>, true>
	: integral_constant<size_t, 0> {
};

template<typename... Ts>
struct coffee_details_size_of<type_pack<Ts...>, true>
	: integral_constant<size_t, sizeof...(Ts)> {
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename...Ts>
struct coffee_details_pack_pop_front<type_pack<T, Ts...>> {
	using type = type_pack<Ts...>;
};

template<typename T, typename...T1s, typename...T2s>
struct coffee_details_pack_pop_back<type_pack<T, T1s...>, type_pack<T2s...>>
	: coffee_details_pack_pop_back<type_pack<T1s...>, type_pack<T2s..., T>> {
};

template<typename T, typename...Ts>
struct coffee_details_pack_pop_back<type_pack<T>, type_pack<Ts...>> {
	using type = type_pack<Ts...>;
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename...Ts>
struct coffee_details_pack_get_at<type_pack<T, Ts...>, 0> {
	using type = T;
};

//template<>
//struct coffee_details_pack_get_at<type_pack<>, 0> {
//	using type = void;
//};

template<typename T, typename...Ts, size_t AT>
struct coffee_details_pack_get_at<type_pack<T, Ts...>, AT>
	: coffee_details_pack_get_at<type_pack<Ts...>, AT - 1> {
};

template<typename T, typename...Ts>
struct coffee_details_pack_get_front<type_pack<T, Ts...>> {
	using type = T;
};

template<typename T, typename...Ts>
struct coffee_details_pack_get_back<type_pack<T, Ts...>>
	: coffee_details_pack_get_back<type_pack<Ts...>> {
};

template<typename T>
struct coffee_details_pack_get_back<type_pack<T>> {
	using type = T;
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename T, typename T1, typename...Ts, template<typename, typename> typename compare>
struct coffee_details_pack_contains<type_pack<T1, Ts...>, T, compare>
	: conditional<compare<T1, T>::value
	, true_type
	, coffee_details_pack_contains<type_pack<Ts...>, T, compare>> {
};

template<typename T, typename T1, typename...Ts, template<typename, typename> typename compare, index_t AT>
struct coffee_details_pack_contains_at<type_pack<T1, Ts...>, T, compare, integral_constant<index_t, AT>>
	: conditional<compare<T1, T>::value
	, integral_constant<index_t, AT>
	, coffee_details_pack_contains_at<type_pack<Ts...>, T, compare, integral_constant<index_t, AT + 1>>> {
};

COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename...Ts>
struct coffee_details_pack_append<type_pack<Ts...>> {
	using type = type_pack<Ts...>;
};

template<typename...T1s, typename...T2s, typename...Ps>
struct coffee_details_pack_append<type_pack<T1s...>, type_pack<T2s...>, Ps...> {
	using type = typename coffee_details_pack_append<type_pack<T1s..., T2s...>, Ps...>::type;
};

template<typename T1, typename...T1s, typename...T2s, template<typename P, typename T> class contains>
struct coffee_details_pack_norepeat<type_pack<T1, T1s...>, type_pack<T2s...>, contains>
	: conditional<contains<type_pack<T2s...>, T1>::value
	, coffee_details_pack_norepeat<type_pack<T1s...>, type_pack<T2s...>, contains>
	, coffee_details_pack_norepeat<type_pack<T1s...>, type_pack<T2s..., T1>, contains>>
{
};

template<typename...Ts, template<typename P, typename T> class contains>
struct coffee_details_pack_norepeat<type_pack<>, type_pack<Ts...>, contains> {
	using type = type_pack<Ts...>;
};
COFFEE_NAMESPACE_END(details)
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
#endif/*COFFEE_STDCPP*/
