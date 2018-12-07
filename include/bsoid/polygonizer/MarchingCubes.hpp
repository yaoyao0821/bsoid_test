#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_MARCHING_CUBES_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_MARCHING_CUBES_HPP

#pragma once

#include "Polygonizer.hpp"
#include "bsoid/tree/BlobTree.hpp"

#include <atlas/utils/Mesh.hpp>

#include <sstream>
#include <string>
#include <vector>
#include <cinttypes>
#include <mutex>

namespace bsoid
{
    namespace polygonizer
    {
        class MarchingCubes
        {
        public:
            MarchingCubes();
            MarchingCubes(tree::BlobTree const& model, std::string const& name,
                float isoValue = 0.5f);
            MarchingCubes(MarchingCubes&& mc);

            ~MarchingCubes() = default;

            void setModel(tree::BlobTree const& tree);
            void setIsoValue(float isoValue);
            void setResolution(std::uint32_t res);

            void polygonize();

            atlas::utils::Mesh& getMesh();

            void setName(std::string const& name);
            std::string getName() const;

            std::string getLog() const;
            void clearLog();

            void saveMesh();
            std::size_t size() const;

        private:
            struct VoxelPoint
            {
                VoxelPoint() = default;
                
                atlas::math::Vector4 data;
            };

            void constructGrid();
            void createTriangles();

            glm::u32vec3 mResolution;
            atlas::utils::Mesh mMesh;
            std::vector<std::vector<std::vector<VoxelPoint>>> mGrid;
            std::vector<atlas::math::Point> mVertices;
            std::vector<atlas::math::Normal> mNormals;
            std::vector<std::uint32_t> mIndices;
            std::mutex mDataMutex;
            tree::TreePointer mTree;
            float mMagic;

            std::stringstream mLog;
            std::string mName;

        };
    }
}

#endif
