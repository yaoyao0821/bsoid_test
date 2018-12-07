#ifndef BSOID_INCLUDE_BSOID_FIELDS_TORUS_HPP
#define BSOID_INCLUDE_BSOID_FIELDS_TORUS_HPP

#pragma once

#include "ImplicitField.hpp"

#include <atlas/core/Float.hpp>

namespace bsoid
{
    namespace fields
    {
        class Torus : public ImplicitField
        {
        public:
            Torus() :
                mC(2.0f),
                mA(1.0f),
                mCentre(0.0f)
            { }

            Torus(float inner, float outer, atlas::math::Point const& centre) :
                mC(inner),
                mA(outer),
                mCentre(centre)
            { }

            ~Torus() = default;

            std::vector<atlas::math::Point> getSeeds() const override
            {
                auto pt = mCentre;
                pt.x += (mC - mA);

                return { pt };
            }

        private:
            float sdf(atlas::math::Point const& p) const override
            {
                using atlas::math::Point;
                using atlas::math::Point2;

                float root = glm::length(p.xy() - mCentre.xy());
                float z2 = (p.z - mCentre.z) * (p.z - mCentre.z);
                float left = (mC - root) * (mC - root);
                return left + z2 - (mA * mA);
            }

            atlas::math::Normal sdg(atlas::math::Point const& p) const override
            {
                using atlas::math::Normal;

                float sqrt = glm::length(p.xy() - mCentre.xy());
                float dx = -2.0f * (mC - sqrt) * (p.x - mCentre.x) / sqrt;
                float dy = -2.0f * (mC - sqrt) * (p.y - mCentre.y) / sqrt;
                float dz = 2.0f * (p.z - mCentre.z);

                return Normal(dx, dy, dz);
            }

            atlas::utils::BBox box() const override
            {
                using atlas::math::Point;

                Point p = {  mC + mA,  mC + mA, -mA };
                Point q = { -mC - mA, -mC - mA,  mA };
                return atlas::utils::BBox(p, q);
            }

            float mC, mA;
            atlas::math::Point mCentre;
        };
    }
}


#endif