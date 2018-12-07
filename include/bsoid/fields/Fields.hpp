#ifndef BSOID_INCLUDE_BSOID_FIELDS_FIELDS_HPP
#define BSOID_INCLUDE_BSOID_FIELDS_FIELDS_HPP

#pragma once

#include "bsoid/Bsoid.hpp"

#include <functional>
#include <memory>

namespace bsoid
{
    namespace fields
    {
        using FilterFn = std::function<float(float)>;

        class ImplicitField;
        class Sphere;
        class Torus;

        using ImplicitFieldPtr = std::shared_ptr<ImplicitField>;
    }
}

#endif