#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_SUPER_VOXEL_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_SUPER_VOXEL_HPP

#pragma once

#include "bsoid/fields/ImplicitField.hpp"

#include <atlas/math/Math.hpp>
#include <atlas/utils/BBox.hpp>
#include <vector>

namespace bsoid
{
    namespace polygonizer
    {
        struct SuperVoxel
        {
            SuperVoxel()
            { }

            float eval(atlas::math::Point const& p) const
            {
                return field->eval(p);
            }

            atlas::math::Normal grad(atlas::math::Point const& p) const
            {
                return field->grad(p);
            }

            glm::u64vec3 id;
            fields::ImplicitFieldPtr field;
            atlas::utils::BBox cell;
        };
    }
}

#endif