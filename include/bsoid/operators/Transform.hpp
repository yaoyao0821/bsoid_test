#ifndef BSOID_INCLUDE_BSOID_OPERATORS_TRANSFORM_HPP
#define BSOID_INCLUDE_BSOID_OPERATORS_TRANSFORM_HPP

#pragma once

#include "Operators.hpp"
#include "ImplicitOperator.hpp"

namespace bsoid
{
    namespace operators
    {
        class Transform : public ImplicitOperator
        {
        public:
            Transform()
            { }

            Transform(atlas::math::Matrix4 t) :
                mTransform(t),
                mInverse(glm::inverse(t)),
                mInverseT(glm::transpose(mInverse))
            { }

            ~Transform() = default;

            std::vector<atlas::math::Point> getSeeds() const override
            {
                using atlas::math::Point;
                using atlas::math::Point4;

                std::vector<atlas::math::Point> result;
                auto seeds = mFields[0]->getSeeds();
                for (auto& seed : seeds)
                {
                    result.push_back(Point(mTransform * Point4(seed, 1.0f)));
                }

                return result;
            }

        private:
            float sdf(atlas::math::Point const& p) const override
            {
                using atlas::math::Point;
                using atlas::math::Point4;

                Point q = Point(mInverse * Point4(p, 1.0f));
                return mFields.front()->eval(q);
            }

            atlas::math::Normal sdg(atlas::math::Point const& p) const override
            {
                using atlas::math::Point;
                using atlas::math::Point4;

                Point q = Point(mInverse * Point4(p, 1.0f));
                auto grad = mFields.front()->grad(q);
                return Point(Point4(grad, 1.0f) * mInverseT);
            }
            
            atlas::utils::BBox box() const override
            {
                using atlas::math::Point;
                using atlas::math::Point4;
                using atlas::utils::BBox;
//                using atlas::utils::join;

                auto b = mFields.front()->getBBox();
                BBox ret(
                    Point(mTransform * Point4(b.pMin.x, b.pMin.y, b.pMin.z, 1.0f)));
                ret = join(ret, 
                    Point(mTransform * Point4(b.pMax.x, b.pMin.y, b.pMin.z, 1.0f)));
                ret = join(ret, 
                    Point(mTransform * Point4(b.pMin.x, b.pMax.y, b.pMin.z, 1.0f)));
                ret = join(ret, 
                    Point(mTransform * Point4(b.pMin.x, b.pMin.y, b.pMax.z, 1.0f)));
                ret = join(ret, 
                    Point(mTransform * Point4(b.pMin.x, b.pMax.y, b.pMax.z, 1.0f)));
                ret = join(ret, 
                    Point(mTransform * Point4(b.pMax.x, b.pMax.y, b.pMin.z, 1.0f)));
                ret = join(ret, 
                    Point(mTransform * Point4(b.pMax.x, b.pMin.y, b.pMax.z, 1.0f)));
                ret = join(ret, 
                    Point(mTransform * Point4(b.pMax.x, b.pMax.y, b.pMax.z, 1.0f)));

                return ret;
            }

            Transform* cloneEmpty() const override
            {
                return new Transform;
            }

            atlas::math::Matrix4 mTransform, mInverse, mInverseT;
        };
    }
}

#endif
