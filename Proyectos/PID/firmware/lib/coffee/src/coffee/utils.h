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

#ifdef __cplusplus
#define COFFEE_STDCPP __cplusplus
#else
#endif

#include <Arduino.h>
#include <stdint.h>
#include <type_traits>
#include "coffee/utils/defines.h"

typedef size_t index_t;

/********************************************************************************************************************************************/
#ifdef COFFEE_STDCPP

namespace Coffee { namespace implement {} }
namespace coffee = Coffee::implement;

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
using std::integral_constant;
using std::bool_constant;
using std::true_type;
using std::false_type;
using nullptr_t = decltype(nullptr);
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
template<typename T>
using is_const = bool_constant<std::is_const<T>::value>;
template<typename T>
using is_reference = bool_constant<std::is_reference<T>::value>;
template<typename T>
using is_lvalue_reference = bool_constant<std::is_lvalue_reference<T>::value>;
template<typename T>
using is_rvalue_reference = bool_constant<std::is_rvalue_reference<T>::value>;
COFFEE_NAMESPACE_END(Coffee, implement)


/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
using std::initializer_list;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
using std::move;
using std::forward;
COFFEE_NAMESPACE_END(Coffee, implement)


/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
COFFEE_NAMESPACE_BEGIN(details)
template<typename...>
struct coffee_details_voider { 
	using type = void;
};

template<typename T>
struct coffee_details_packing;

COFFEE_NAMESPACE_END(details)
template <typename... Ts>
using voidT = typename coffee::details::coffee_details_voider<Ts...>::type;

template<typename...Ts>
struct type_pack;

template<typename T>
using packing = typename coffee::details::coffee_details_packing<T>::type;

template<typename T> typename std::add_lvalue_reference<T>::type declval() noexcept;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
template<size_t N>
using indexT = integral_constant<size_t, N>;
COFFEE_NAMESPACE_END(Coffee, implement)

/********************************************************************************************************************************************/
COFFEE_NAMESPACE_BEGIN(Coffee, implement)
using invalid_index_t = integral_constant<index_t, (index_t)~0ull>;
static constexpr invalid_index_t invalid_index = {};

using invalid_size_t = integral_constant<size_t, (size_t)~0ull>;
static constexpr invalid_size_t invalid_size = {};
COFFEE_NAMESPACE_END(Coffee, implement)

namespace Coffee
{
	using coffee::invalid_size;
	using coffee::invalid_index;
}
/********************************************************************************************************************************************/
#endif/*COFFEE_STDCPP*/

/********************************************************************************************************************************************/
#include "utils/constexpr.h"
#include "utils/traits.h"
#include "utils/memory.h"
#include "utils/packs.h"
#include "utils/operators.h"
#include "utils/algorithms.h"

/********************************************************************************************************************************************/
