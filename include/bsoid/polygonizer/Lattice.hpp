#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_LATTICE_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_LATTICE_HPP
#pragma once

#include "Polygonizer.hpp"
#include "Voxel.hpp"

#include <atlas/math/Math.hpp>
#include <vector>

namespace bsoid
{
    namespace polygonizer
    {
        struct Lattice
        {
            Lattice() = default;

            void makeLattice(std::vector<Voxel> const& voxels);
            void clearBuffers();

            std::vector<atlas::math::Point> vertices;
            std::vector<std::uint32_t> indices;
        };
    }
}

#endif