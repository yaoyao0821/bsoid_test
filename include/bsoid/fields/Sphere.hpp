#ifndef BSOID_INCLUDE_BSOID_FIELDS_SPHERE_HPP
#define BSOID_INCLUDE_BSOID_FIELDS_SPHERE_HPP

#pragma once

#include "ImplicitField.hpp"

namespace bsoid
{
    namespace fields
    {
        class Sphere : public ImplicitField
        {
        public:
            Sphere() :
                mRadius(1.0f),
                mCentre(0.0f)
            { }

            Sphere(float radius, atlas::math::Point const& centre) :
                mRadius(radius),
                mCentre(centre)
            { }

            ~Sphere() = default;

            std::vector<atlas::math::Point> getSeeds() const override
            {
                auto seed = mCentre;
                seed.x += mRadius;
                return { seed };
            }

        private:
            float sdf(atlas::math::Point const& p) const override
            {
                return glm::length(p - mCentre) - mRadius;
            }

            atlas::math::Normal sdg(atlas::math::Point const& p) const override
            {
                return 2.0f * (p - mCentre);
            }

            atlas::utils::BBox box() const override
            {
                using atlas::utils::BBox;
                using atlas::math::Point;

                Point min = mCentre - mRadius;
                Point max = mCentre + mRadius;

                return BBox(min, max);
            }

            float mRadius;
            atlas::math::Point mCentre;
        };
    }
}

#endif
