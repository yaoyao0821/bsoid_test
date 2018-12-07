#ifndef BSOID_INCLUDE_BSOID_OPERATORS_OPERATORS_HPP
#define BSOID_INCLUDE_BSOID_OPERATORS_OPERATORS_HPP

#pragma once

#include "bsoid/Bsoid.hpp"

#include <memory>

namespace bsoid
{
    namespace operators
    {
        class ImplicitOperator;

        class Blend;
        class Union;
        class Intersection;

        using ImplicitOperatorPtr = std::shared_ptr<ImplicitOperator>;
    }
}

#endif