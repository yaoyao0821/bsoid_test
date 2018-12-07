#ifndef BSOID_INCLUDE_BSOID_POLYGONIZER_BSOID_HPP
#define BSOID_INCLUDE_BSOID_POLYGONIZER_BSOID_HPP

#pragma once

#include "Polygonizer.hpp"
#include "Lattice.hpp"
#include "SuperVoxel.hpp"
#include "uint128_t.hpp"
#include "bsoid/tree/BlobTree.hpp"

#include <atlas/utils/Mesh.hpp>

#include <sstream>
#include <string>
#include <cinttypes>
#include <unordered_map>
#include <mutex>

namespace bsoid
{
    namespace polygonizer
    {
        class Bsoid
        {
        public:
            Bsoid();
            Bsoid(tree::BlobTree const& model, std::string const& name,
                float isoValue = 0.5f);
            Bsoid(Bsoid&& b);

            ~Bsoid() = default;

            void setModel(tree::BlobTree const& tree);
            void setIsoValue(float isoValue);
            void setResolution(std::uint64_t gridRes, std::uint64_t svRes);

            tree::BlobTree* tree() const;

            void constructLattice();
            void constructMesh();
            void polygonize();

            Lattice const& getLattice() const;
            atlas::utils::Mesh& getMesh();

            void setName(std::string const& name);
            std::string getName() const;

            std::string getLog() const;
            void clearLog();

            void saveMesh();

            std::size_t size() const;

        private:
            struct LinePoint
            {
                LinePoint()
                { }

                LinePoint(FieldPoint const& p) : 
                    point(p)
                { }

                LinePoint(FieldPoint const& p, std::uint128_t const& e) :
                    point(p),
                    edge(e)
                { }

                FieldPoint point;
                std::uint128_t edge;
            };

            void makeVoxels();
            void makeTriangles();

            atlas::math::Point createCellPoint(glm::u64vec3 const& p,
                atlas::math::Point const& delta);
            atlas::math::Point createCellPoint(std::uint64_t x,
                std::uint64_t y, std::uint64_t z, atlas::math::Point const& delta);

            FieldPoint findVoxelPoint(PointId const& id);
            void fillVoxel(Voxel& v);
            bool seenVoxel(VoxelId const& id);

            FieldPoint interpolate(FieldPoint const& p1, FieldPoint const& p2);
            LinePoint generateLinePoint(PointId const& p1, PointId const& p2,
                FieldPoint const& fp1, FieldPoint const& fp2);

            void marchVoxelOnSurface(std::vector<Voxel> const& seeds);
            bool validVoxel(Voxel const& v);

            void validateVoxels();


            atlas::math::Point mGridDelta, mSvDelta, mMin, mMax;
            std::uint64_t mGridSize, mSvSize;
            float mMagic;

            std::vector<Voxel> mVoxels;
            std::mutex mSeenVoxelsMutex;
            std::map<std::uint64_t, VoxelId> mSeenVoxels;

            std::mutex mSeenPointsMutex;
            std::map<std::uint64_t, FieldPoint> mSeenPoints;

            std::mutex mSvMutex;
            std::unordered_map<std::uint64_t, SuperVoxel> mSuperVoxels;

            std::mutex mPointMutex;
            std::map<std::uint128_t, LinePoint> mComputedPoints;

            Lattice mLattice;
            tree::TreePointer mTree;

            atlas::utils::Mesh mMesh;

            std::stringstream mLog;
            std::string mName;
        };
    }
}

#endif