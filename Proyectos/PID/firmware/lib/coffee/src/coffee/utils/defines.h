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

#define COFFEE_EXPAND(x) x
#define COFFEE_EXPAND_ARGS(...) COFFEE_EXPAND(__VA_ARGS__)
#define COFFEE_TO_STRING__(x) #x
#define COFFEE_TO_STRING(x) COFFEE_EXPAND(COFFEE_TO_STRING__(x))

#define COFFEE_NUM_ARGS_EXPAND__(...) unused, __VA_ARGS__
#define COFFEE_NUM_ARGS__(...)	COFFEE_EXPAND(COFFEE_PPARG_N(__VA_ARGS__))
#define COFFEE_NUM_ARGS_(...)	COFFEE_EXPAND(COFFEE_NUM_ARGS__( __VA_ARGS__, COFFEE_PP_RSEQ_N()))
#define COFFEE_NUM_ARGS(...)	COFFEE_EXPAND(COFFEE_NUM_ARGS_(COFFEE_NUM_ARGS_EXPAND__(__VA_ARGS__)))

#define COFFEE_PPARG_N(\
        _1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,\
		_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,_33,_34,_35,_36,_37,_38,_39,_40,\
		_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,\
		_61,_62,_63,_64,_65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,\
		_81,_82,_83,_84,_85,_86,_87,_88,_89,_90,_91,_92,_93,_94,_95,_96,_97,_98,_99,_100,\
		N,...)	N

#define COFFEE_PP_RSEQ_N() \
		99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,\
		80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61, \
		60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,\
		40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,\
		20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

#include "coffee/utils/inline/utils_join.inl"

#define COFFEE_JOIN_AB_(__A__, __B__) __A__##__B__
#define COFFEE_JOIN_AB(__A__, __B__) COFFEE_JOIN_AB_(__A__, __B__)
#define COFFEE_JOIN_N(__JOIN_MACRO__,...) COFFEE_EXPAND(__JOIN_MACRO__(__VA_ARGS__))
#define COFFEE_JOIN(...) COFFEE_EXPAND(COFFEE_JOIN_N(COFFEE_JOIN_AB(COFFEE_JOIN, COFFEE_EXPAND(COFFEE_NUM_ARGS(__VA_ARGS__))), __VA_ARGS__))

/********************************************************************************************************************************************/

#include "coffee/utils/inline/utils_apply.inl"

#define COFFEE_APPLY_FUNCX_(FUNCX, SEPARATOR, M, ...) COFFEE_EXPAND(M(FUNCX,SEPARATOR,__VA_ARGS__))
#define COFFEE_APPLY_FUNCX_N(FUNCX, SEPARATOR, ...) COFFEE_EXPAND(COFFEE_APPLY_FUNCX_(FUNCX,SEPARATOR,COFFEE_JOIN_AB(COFFEE_APPLY_FUNCX,COFFEE_NUM_ARGS(__VA_ARGS__)),__VA_ARGS__))

#define TOKEN_NONE()
#define TOKEN_COMA(),
#define TOKEN_OR()||
#define TOKEN_AND()&&
#define TOKEN_UNDERSCORE()_
#define TOKEN_SCOPE()::

#define FUNCX_NONE(A0) A0
#define FUNCX_TYPENAME(A0) typename A0

#define FUNCX_NAMESPACE_BEGIN(A0) namespace A0 {
#define FUNCX_NAMESPACE_END(A0) }

/********************************************************************************************************************************************/

#include "coffee/utils/inline/utils_last.inl"

#define COFFEE_LAST(...) COFFEE_EXPAND(COFFEE_JOIN_AB(COFFEE_LAST, COFFEE_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))
#define COFFEE_NO_LAST(...) COFFEE_EXPAND(COFFEE_JOIN_AB(COFFEE_NO_LAST, COFFEE_NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))

/********************************************************************************************************************************************/

#define COFFEE_NOT_false true
#define COFFEE_NOT_true  false
#define COFFEE_NOT(__A__) COFFEE_EXPAND(COFFEE_JOIN(COFFEE_NOT_, __A__))

#define COFFEE_AND_false_false false
#define COFFEE_AND_false_true  false
#define COFFEE_AND_true_false  false
#define COFFEE_AND_true_true   true
#define COFFEE_AND(__A__, __B__) COFFEE_EXPAND(COFFEE_JOIN(COFFEE_AND_, __A__, _, __B__))

#define COFFEE_OR_false_false false
#define COFFEE_OR_false_true  true
#define COFFEE_OR_true_false  true
#define COFFEE_OR_true_true   true
#define COFFEE_OR(__A__, __B__) COFFEE_EXPAND(COFFEE_JOIN(COFFEE_OR_, __A__, _, __B__))

#define COFFEE_XOR_false_false false
#define COFFEE_XOR_false_true  true
#define COFFEE_XOR_true_false  true
#define COFFEE_XOR_true_true   false
#define COFFEE_XOR(__A__, __B__) COFFEE_EXPAND(COFFEE_JOIN(COFFEE_XOR_, __A__, _, __B__))

#define COFFEE_NAND(__A__, __B__) COFFEE_NOT(COFFEE_AND(__A__, __B__))
#define COFFEE_NOR(__A__, __B__)  COFFEE_NOT(COFFEE_OR(__A__, __B__))
#define COFFEE_XNOR(__A__, __B__) COFFEE_NOT(COFFEE_XOR(__A__, __B__))

#define COFFEE_ALIGNUP(VALUE, ALIGNMENT) (((VALUE) % (ALIGNMENT)) ? ((VALUE) + (ALIGNMENT) - ((VALUE) % (ALIGNMENT))) : (VALUE))
#define COFFEE_MAX(A,B) (((A)>(B))?(A):(B))
#define COFFEE_MIN(A,B) (((A)<(B))?(A):(B))

#define __COFFEE_FUNCTION_NAME__(__NAMESPACE__, __OWNER__, __NAME__, __EXTRA__ ) COFFEE_JOIN(__NAMESPACE__, _, __OWNER__, _ ,__NAME__, __EXTRA__)
	//COFFEE_EXPAND_ARGS( __NAMESPACE__ )COFFEE_EXPAND_ARGS( ##_##__OWNER__ )COFFEE_EXPAND_ARGS(##_##__NAME__ )COFFEE_EXPAND_ARGS(##__EXTRA__ )

#define COFFEE_FUNCTION_NAME(...) COFFEE_EXPAND( __COFFEE_FUNCTION_NAME__(__VA_ARGS__) )

/********************************************************************************************************************************************/

#define COFFEE_SCOPE_JOIN(...)  COFFEE_EXPAND(COFFEE_APPLY_FUNCX_N(FUNCX_NONE,TOKEN_SCOPE,##__VA_ARGS__))
#define COFFEE_UNDERSCORE_JOIN(...)  COFFEE_EXPAND(COFFEE_APPLY_FUNCX_N(FUNCX_NONE,TOKEN_UNDERSCORE,##__VA_ARGS__))
#define COFFEE_FUNCTION_TYPE(...) COFFEE_EXPAND(COFFEE_UNDERSCORE_JOIN(__VA_ARGS__,FnType))
#define COFFEE_FUNCTION_FULL_NAME(...) COFFEE_EXPAND(COFFEE_UNDERSCORE_JOIN(__VA_ARGS__))
#define COFFEE_FUNCTION_SHORT_NAME(...) COFFEE_EXPAND(COFFEE_LAST(__VA_ARGS__))

#define TEMPLATE(...) template<COFFEE_EXPAND(COFFEE_APPLY_FUNCX_N(FUNCX_TYPENAME, TOKEN_COMA,##__VA_ARGS__))>

/********************************************************************************************************************************************/
#ifdef COFFEE_STDCPP
#define COFFEE_NAMESPACE_BEGIN(...) COFFEE_EXPAND(COFFEE_APPLY_FUNCX_N(FUNCX_NAMESPACE_BEGIN, TOKEN_NONE, __VA_ARGS__))
#define COFFEE_NAMESPACE_END(...) COFFEE_EXPAND(COFFEE_APPLY_FUNCX_N(FUNCX_NAMESPACE_END, TOKEN_NONE, __VA_ARGS__))
#else
#define COFFEE_NAMESPACE_BEGIN(...)
#define COFFEE_NAMESPACE_END(...)
#endif // COFFEE_STDCPP

