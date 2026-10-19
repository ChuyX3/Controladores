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
COFFEE_NAMESPACE_BEGIN(algorithms)
COFFEE_NAMESPACE_BEGIN(details)

template<uint64_t SEED, typename P>
struct coffee_algorithms_details_hash_from_pack_base;

template<size_t N>
uint64_t append_bytes(uint64_t value, uint8_t const* ptr);

template<>
inline uint64_t append_bytes<1>(uint64_t value, uint8_t const* ptr) {
	value ^= static_cast<size_t>(*ptr);
	value *= 16777619U;
	return value;
}

template<size_t N>
inline uint64_t append_bytes(uint64_t value, uint8_t const* ptr) {
	value ^= static_cast<uint64_t>(*ptr);
	value *= 16777619U;
	return append_bytes<N - 1>(value, ++ptr);
}

template<uint64_t SEED, char c, char...chars>
struct coffee_algorithms_details_hash_from_pack_base<SEED, char_pack<c, chars...>> {
	static constexpr uint64_t base_value = (SEED ^ static_cast<uint64_t>(c)) * 16777619U;
	static constexpr uint64_t value = coffee_algorithms_details_hash_from_pack_base<base_value, char_pack<chars...>>::value;
};

template<uint64_t SEED>
struct coffee_algorithms_details_hash_from_pack_base<SEED, char_pack<>> {
	static constexpr uint64_t value = SEED;
};
COFFEE_NAMESPACE_END(details)

template<typename P>
using hash_from_pack = integral_constant<uint64_t, coffee::algorithms::details::coffee_algorithms_details_hash_from_pack_base<2166136261U, P>::value>;

template<typename T, size_t N>
inline constexpr size_t array_size(T(&)[N]) { return N; }

COFFEE_NAMESPACE_END(algorithms)
using algorithms::array_size;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
#endif/*COFFEE_STDCPP*/
