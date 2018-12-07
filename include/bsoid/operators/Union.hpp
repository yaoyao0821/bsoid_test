#ifndef BSOID_INCLUDE_BSOID_OPERATORS_UNION_HPP
#define BSOID_INCLUDE_BSOID_OPERATORS_UNION_HPP

#pragma once

#include "Operators.hpp"
#include "ImplicitOperator.hpp"

namespace bsoid
{
    namespace operators
    {
        class Union : public ImplicitOperator
        {
        public:
            Union()
            { }

            ~Union() = default;


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
                float field = -std::numeric_limits<float>::infinity();
                for (auto& f : mFields)
                {
                    field = glm::max(field, f->eval(p));
                }

                return field;
            }

            atlas::math::Normal sdg(atlas::math::Point const& p) const override
            {
                atlas::math::Normal gradient(-std::numeric_limits<float>::infinity());
                for (auto& f : mFields)
                {
                    gradient = glm::max(gradient, f->grad(p));
                }

                return gradient;
            }

            atlas::utils::BBox box() const override
            {
                atlas::utils::BBox box;
                for (auto& f : mFields)
                {
                    box = join(box, f->getBBox());
                }

                return box;
            }

            Union* cloneEmpty() const override
            {
                return new Union;
            }
        };
    }
}

#endif
