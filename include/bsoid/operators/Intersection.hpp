#ifndef BSOID_INCLUDE_BSOID_OPERATORS_INTERSECTION_HPP
#define BSOID_INCLUDE_BSOID_OPERATORS_INTERSECTION_HPP

#pragma once

#include "Operators.hpp"
#include "ImplicitOperator.hpp"

#include <atlas/core/Constants.hpp>

namespace bsoid
{
    namespace operators
    {
        class Intersection : public ImplicitOperator
        {
        public:
            Intersection()
            { }

            ~Intersection() = default;

            std::vector<atlas::math::Point> getSeeds() const override
            {
                std::vector<atlas::math::Point> result;
                for (auto& f : mFields)
                {
                    auto seeds = f->getSeeds();
                    result.insert(result.end(), seeds.begin(), seeds.end());
                }

                return result;
            }

        private:
            float sdf(atlas::math::Point const& p) const override
            {
                float field = atlas::core::infinity();
                for (auto& f : mFields)
                {
                    field = glm::min(field, f->eval(p));
                }

                return field;
            }

            atlas::math::Normal sdg(atlas::math::Point const& p) const override
            {
                atlas::math::Normal gradient(atlas::core::infinity());
                for (auto& f : mFields)
                {
                    gradient = glm::min(gradient, f->grad(p));
                }

                return gradient;
            }

            atlas::utils::BBox box() const override
            {
                using atlas::utils::BBox;

                atlas::utils::BBox box;
                for (auto& f : mFields)
                {
                    box = join(box, f->getBBox());
                }

                return box;
            }

            Intersection* cloneEmpty() const override
            {
                return new Intersection;
            }
        };
    }
}



#endif
