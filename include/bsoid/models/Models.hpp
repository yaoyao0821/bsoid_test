#ifndef ATHENA_INCLUDE_ATHENA_MODELS_MODELS_HPP
#define ATHENA_INCLUDE_ATHENA_MODELS_MODELS_HPP

#pragma once

#include "bsoid/polygonizer/Bsoid.hpp"
#include "bsoid/polygonizer/MarchingCubes.hpp"

#include <functional>
#include <tuple>

#define MAKE_FUNCTION(name) \
bsoid::polygonizer::Bsoid make##name(Resolution const& res); \
bsoid::polygonizer::MarchingCubes makeMC##name(Resolution const& res)


namespace bsoid
{
    namespace models
    {

        using Resolution = std::tuple<std::size_t, std::size_t>;
        constexpr Resolution lowResolution = { 32, 8 };
        constexpr Resolution midResolution = { 256, 64 };
        constexpr Resolution highResolution = { 1024, 256 };

        constexpr Resolution currentResolution = midResolution;

        using ModelFn = 
            std::function<bsoid::polygonizer::Bsoid()>;
        using MCModelFn = 
            std::function<bsoid::polygonizer::MarchingCubes()>;

        MAKE_FUNCTION(Sphere);
        MAKE_FUNCTION(Torus);

        MAKE_FUNCTION(Blend);
        MAKE_FUNCTION(Intersection);
        MAKE_FUNCTION(Union);
        MAKE_FUNCTION(Transform);

        MAKE_FUNCTION(Butterfly);

        MAKE_FUNCTION(Particles);

        MAKE_FUNCTION(Chain);

    }
}

#endif
