#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_HASH_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_HASH_HPP

#pragma once

#include <atlas/core/Macros.hpp>

#include <cinttypes>
#include <limits>
#include "uint128_t.hpp"


namespace bsoid
{
    namespace polygonizer
    {
        template <typename T>
        struct BsoidHash
        {
            static constexpr T bits = std::numeric_limits<T>::digits / 3;
            static constexpr T mask = ~(static_cast<T>(0) << bits);
            static constexpr T hash(T x, T y, T z)
            {
                return (((x & mask) << bits | (y & mask)) << bits | (z & mask));
            }
        };

        template <typename T>
        struct BsoidEdgeHash
        {
            template <typename G>
            static constexpr T hash(G h1, G h2)
            {
                return { h1, h2 };
            }
        };

        using BsoidHash64 = BsoidHash<std::uint64_t>;
        using BsoidHash128 = BsoidEdgeHash<std::uint128_t>;
    }
}

#endif