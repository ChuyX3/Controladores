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

#include "coffee.h"

namespace Coffee
{
    template<size_t N>
    struct FlagSet
    {
        static constexpr size_t size = N;
        static constexpr size_t numBytes = (N + 7) / 8;
        static constexpr size_t tailBits = N & 7;
        static constexpr uint8_t tailMask = tailBits ? ((uint8_t(1u) << tailBits) - 1u) : 0xFFu;

    private:
        static constexpr size_t byteIndex(size_t index) {
            return index >> 3;
        }
        static constexpr uint8_t bitMask(size_t index) {
            return uint8_t(1u) << (index & 7);
        }

        static_assert(N > 0, "FlagSet must have at least one flag.");

    public:
        FlagSet() = default;
        FlagSet(FlagSet const&) = default;
        FlagSet& operator=(FlagSet const&) = default;
        FlagSet(FlagSet&&) = default;
        FlagSet& operator=(FlagSet&&) = default;
        
    public:
        void clear() {
            for(size_t i = 0; i < numBytes; ++i) {
                m_data[i] = 0;
            }
        }
        void fill() {
            for(size_t i = 0; i < numBytes; ++i)
                m_data[i] = 0xFF;
            m_data[numBytes - 1] &= tailMask;
        }
        bool test(size_t index) const {
            if(index >= N) return false;
            return (m_data[byteIndex(index)] & bitMask(index)) != 0;
        }
        void set(size_t index) {
            if(index >= N) return;
            m_data[byteIndex(index)] |= bitMask(index);
        }
        void reset(size_t index) {
            if(index >= N) return;
            m_data[byteIndex(index)] &= uint8_t(~bitMask(index));
        }
        void toggle(size_t index) {
            if(index >= N) return;
            m_data[byteIndex(index)] ^= bitMask(index);
        }
        void assign(size_t index, bool value) {
            value ? set(index) : reset(index);
        }
        bool any() const {
            for(size_t i = 0; i < numBytes; ++i) {
                if(m_data[i]) return true;
            }
            return false;
        }
        bool all() const {
            for(size_t i = 0; i < numBytes - 1; ++i) {
                if(m_data[i] != 0xFF)
                    return false;
            }
            return m_data[numBytes - 1] == tailMask;
        }
        bool none() const {
            return !any();
        }

        template<size_t I>
        bool test() const {
            static_assert(I < N, "Flag index out of bounds.");
            return (m_data[byteIndex(I)] & bitMask(I)) != 0;
        }
        template<size_t I>
        void set() {
            static_assert(I < N, "Flag index out of bounds.");
            m_data[byteIndex(I)] |= bitMask(I);   
        }
        template<size_t I>
        void reset() {
            static_assert(I < N, "Flag index out of bounds.");
            m_data[byteIndex(I)] &= uint8_t(~bitMask(I));
        }
        template<size_t I>
        void toggle() {
            static_assert(I < N, "Flag index out of bounds.");
            m_data[byteIndex(I)] ^= bitMask(I);
        }
        template<size_t I>
        void assign(bool value) {
            static_assert(I < N, "Flag index out of bounds.");
            value ? set<I>() : reset<I>();
        }
        
        constexpr size_t count()const {
            return N;
        }

        constexpr size_t bytes()const {
            return numBytes;
        }


    public:
        FlagSet<N> operator~() const {
            FlagSet<N> result;
            for(size_t i = 0; i < numBytes; ++i)
                result.m_data[i] = ~m_data[i];
            result.m_data[numBytes - 1] &= tailMask;
            return result;
        }
        FlagSet<N>& operator&=(FlagSet<N> const& other) {
            for(size_t i = 0; i < numBytes; ++i)
                m_data[i] &= other.m_data[i];
            return *this;
        }
        FlagSet<N>& operator|=(FlagSet<N> const& other) {
            for(size_t i = 0; i < numBytes; ++i)
                m_data[i] |= other.m_data[i];
            m_data[numBytes - 1] &= tailMask;
            return *this;
        }
        FlagSet<N>& operator^=(FlagSet<N> const& other) {
            for(size_t i = 0; i < numBytes; ++i)
                m_data[i] ^= other.m_data[i];
            m_data[numBytes - 1] &= tailMask;
            return *this;
        }
        FlagSet<N> operator&(FlagSet<N> const& other) const {
            FlagSet<N> result = *this;
            result &= other;
            return result;
        }
        FlagSet<N> operator|(FlagSet<N> const& other) const {
            FlagSet<N> result = *this;
            result |= other;
            return result;
        }
        FlagSet<N> operator^(FlagSet<N> const& other) const {
            FlagSet<N> result = *this;
            result ^= other;
            return result;
        }

        explicit operator uint8_t const*() const {
            return m_data;
        }
        explicit operator uint8_t*() {
            return m_data;
        }

        /** Access the read-only raw byte at the given index. */
        uint8_t operator[](size_t index) const {
            return m_data[index];
        }

        operator bool() const {
            return any();
        }
        bool operator!() const {
            return none();
        }

        bool operator==(FlagSet<N> const& other) const {
            for(size_t i = 0; i < numBytes; ++i) {
                if(m_data[i] != other.m_data[i]) return false;
            }
            return true;
        }
        bool operator!=(FlagSet<N> const& other) const {
            return !(*this == other);
        }

    private:
        uint8_t m_data[numBytes] = {0};
    };


    template<size_t N>
    size_t findFirstSet(FlagSet<N> const& flags, size_t start = 0) {
        uint8_t value;
        static constexpr size_t lastBits  = FlagSet<N>::tailBits? FlagSet<N>::tailBits : 8;
        size_t i = coffee::min(start, N) >> 3;
        size_t beg = start & 7;

        while(i < FlagSet<N>::numBytes) {
            value = flags[i];
            if(value != 0) {
                size_t end = (i == FlagSet<N>::numBytes - 1)? lastBits : 8;
                for(size_t j = beg; j < end; ++j)
                    if(value & (1u << j))
                        return (i << 3) + j;
            }
            ++i;
            beg = 0;
        }
        return coffee::invalid_index; // Return size_t(-1) if no set bit is found
    }
    
    template<size_t N>
    size_t findFirstUnset(FlagSet<N> const& flags, size_t start = 0) {
        uint8_t value;
        static constexpr size_t lastBits  = FlagSet<N>::tailBits? FlagSet<N>::tailBits : 8;
        size_t i = coffee::min(start, N) >> 3;
        size_t beg = start & 7;

        while(i < FlagSet<N>::numBytes) {
            value = flags[i];
            if(value != 0xff) {
                size_t end = (i == FlagSet<N>::numBytes - 1)? lastBits : 8;
                for(size_t j = beg; j < end; ++j)
                    if(!(value & (1u << j)))
                        return (i << 3) + j;
            }
            ++i;
            beg = 0;
        }
        return coffee::invalid_index; // Return size_t(-1) if no unset bit is found
    }
}
