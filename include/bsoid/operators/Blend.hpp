#ifndef BSOID_INCLUDE_BSOID_OPERATORS_BLEND_HPP
#define BSOID_INCLUDE_BSOID_OPERATORS_BLEND_HPP

#pragma once

#include "Operators.hpp"
#include "ImplicitOperator.hpp"

namespace bsoid
{
    namespace operators
    {
        class Blend : public ImplicitOperator
        {
        public:
            Blend()
            { }

            ~Blend() = default;

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
                float field = 0.0f;
                for (auto& f : mFields)
                {
                    field += f->eval(p);
                }

                return field;
            }

            atlas::math::Normal sdg(atlas::math::Point const& p) const override
            {
                atlas::math::Normal gradient;
                for (auto& f : mFields)
                {
                    gradient += f->grad(p);
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

            Blend* cloneEmpty() const override
            {
                return new Blend;
            }

        };
    }
}

#endif
