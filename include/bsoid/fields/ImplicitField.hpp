#ifndef BSOID_INCLUDE_BSOID_FIELDS_IMPLICIT_FIELD_HPP
#define BSOID_INCLUDE_BSOID_FIELDS_IMPLICIT_FIELD_HPP

#pragma once

#include "Fields.hpp"
#include "Filters.hpp"

#include <atlas/math/Math.hpp>
#include <atlas/utils/BBox.hpp>

#include <vector>
#include <atomic>

namespace bsoid
{
    namespace fields
    {
        class ImplicitField
        {
        public:
            ImplicitField() :
                mCounter(0)
            { }

            virtual ~ImplicitField() = default;

            virtual atlas::utils::BBox getBBox() const
            {
                auto b = box();
                b.expand(1.0f);
                return b;
            }

            virtual float eval(atlas::math::Point const& p) const
            {
                ++mCounter;
                return compactField(sdf(p));
            }

            virtual atlas::math::Normal grad(atlas::math::Point const& p) const
            {
                return compactGradient(sdf(p)) * sdg(p);
            }

            virtual std::vector<atlas::math::Point> getSeeds() const = 0;

            std::uint64_t getCount() const
            {
                return mCounter.load();
            }

        protected:
            virtual float sdf(atlas::math::Point const& p) const = 0;
            virtual atlas::math::Normal sdg(atlas::math::Point const& p) const = 0;
            virtual atlas::utils::BBox box() const = 0;

        private:
            mutable std::atomic<std::uint64_t> mCounter;
        };
    }
}


#endif