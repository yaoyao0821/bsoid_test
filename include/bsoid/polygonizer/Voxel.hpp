#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_VOXEL_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_VOXEL_HPP

#pragma once

#include <atlas/math/Math.hpp>

#include <cinttypes>
#include <array>

#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif

namespace bsoid
{
    namespace polygonizer
    {
        struct FieldPoint
        {
            FieldPoint() :
                value(0.0f, 0.0f, 0.0f, -1.0f),
                g(0.0f)
            { }

            FieldPoint(atlas::math::Point const& p, float v) :
                value(p, v),
                g(0.0f)
            { }

            FieldPoint(atlas::math::Point const& p, float v,
                atlas::math::Normal const& grad) :
                value(p, v),
                g(grad),
                svHash(0)
            { }

            FieldPoint(atlas::math::Point const& p, float v, 
                atlas::math::Normal const& grad, std::uint64_t id) :
                value(p, v),
                g(grad),
                svHash(id)
            { }

            bool operator==(FieldPoint const& rhs) const
            {
                return rhs.value == value;
            }

            bool operator!=(FieldPoint const& rhs) const
            {
                return !(*this == rhs);
            }

            atlas::math::Point4 value;
            atlas::math::Normal g;
            std::uint64_t svHash;
        };

        constexpr auto invalidUint()
        {
            return std::numeric_limits<std::uint64_t>::max();
        }


        using PointId = glm::u64vec3;
        using VoxelId = PointId;

        struct Voxel
        {
            Voxel() :
                id(invalidUint())
            { }

            Voxel(glm::u64vec3 const& p) :
                id(p)
            { }

            bool isValid() const
            {
                glm::u64vec3 invalid(invalidUint());
                return (
                    id.x != invalid.x &&
                    id.y != invalid.y &&
                    id.z != invalid.z);
            }

            std::array<FieldPoint, 8> points;
            VoxelId id;
        };
    }
}

#endif