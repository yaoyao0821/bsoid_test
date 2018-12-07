#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_UINT128_T_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_UINT128_T_HPP

#pragma once

#include <emmintrin.h>
#include <algorithm>

namespace std
{
    struct uint128_t
    {
        constexpr uint128_t() :
            _low(0),
            _high(0)
        { }

        constexpr uint128_t(uint128_t const& x) :
            _low(x._low),
            _high(x._high)
        { }

        constexpr uint128_t(int x) :
            _low(x),
            _high(0x0)
        { }

        constexpr uint128_t(std::uint64_t const& x) :
            _low(x),
            _high(0x0)
        { }

        constexpr uint128_t(std::uint32_t const& x) :
            _low(x),
            _high(0x0)
        { }

        constexpr uint128_t(std::uint64_t const& x, std::uint64_t const& y) :
            _low(x),
            _high(y)
        { }

//        constexpr uint128_t operator=(uint128_t const& x)
//        {
//            uint128_t tmp(x);
//            std::swap(_low, tmp._low);
//            std::swap(_high, tmp._high);
//            return *this;
//        }

        std::uint64_t _low, _high;
    };

    inline bool operator<(uint128_t const& lhs, uint128_t const& rhs)
    {
        if (lhs._high < rhs._high)
        {
            return true;
        }

        if (lhs._high == rhs._high && lhs._low < rhs._low)
        {
            return true;
        }

        return false;
    }

    inline bool operator>(uint128_t const& lhs, uint128_t const& rhs)
    {
        if (lhs._high > rhs._high)
        {
            return true;
        }

        if (lhs._high == rhs._high && lhs._low > rhs._low)
        {
            return true;
        }

        return false;
    }

    inline bool operator==(uint128_t const& lhs, uint128_t const& rhs)
    {
        return (lhs._low == rhs._low) && (lhs._high == rhs._high);
    }

    inline bool operator!=(uint128_t const& lhs, uint128_t const& rhs)
    {
        return !(lhs == rhs);
    }
}

#endif
