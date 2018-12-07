#include "bsoid/polygonizer/Bsoid.hpp"
#include "bsoid/polygonizer/Hash.hpp"
#include "bsoid/polygonizer/Tables.hpp"

#include <atlas/core/Timer.hpp>
#include <atlas/core/Macros.hpp>
#include <atlas/core/Assert.hpp>
#include <atlas/core/Log.hpp>
#include <atlas/core/Float.hpp>

#include <numeric>
#include <functional>
#include <unordered_set>
#include <fstream>
#include <queue>

#include <tbb/parallel_for.h>
#include <glm/gtx/component_wise.hpp>

#define DISABLE_PARALLEL 0


namespace bsoid
{
    namespace polygonizer
    {
        Bsoid::Bsoid() :
            mName("model")
        { }

        Bsoid::Bsoid(tree::BlobTree const& model, std::string const& name,
            float isoValue) :
            mMagic(isoValue),
            mTree(std::make_unique<tree::BlobTree>(model)),
            mName(name)
        { }

        Bsoid::Bsoid(Bsoid&& b) :
            mGridDelta(b.mGridDelta),
            mSvDelta(b.mSvDelta),
            mMin(b.mMin),
            mMax(b.mMax),
            mGridSize(b.mGridSize),
            mSvSize(b.mSvSize),
            mMagic(b.mMagic),
            mLattice(std::move(b.mLattice)),
            mTree(std::move(b.mTree)),
            mMesh(std::move(b.mMesh)),
            mLog(std::move(b.mLog)),
            mName(b.mName)
        { }

        void Bsoid::setModel(tree::BlobTree const& model)
        {
            mTree = std::make_unique<tree::BlobTree>(model);
        }

        void Bsoid::setIsoValue(float isoValue)
        {
            mMagic = isoValue;
        }

        void Bsoid::setResolution(std::uint64_t res, std::uint64_t svRes)
        {
            mGridSize = res;
            mSvSize = svRes;

            auto box = mTree->getTreeBox();
            auto const& end = box.pMax;
            auto const& start = box.pMin;

            mGridDelta = (end - start) / static_cast<float>(mGridSize);
            mSvDelta = (end - start) / static_cast<float>(mSvSize);
            mMin = start;
            mMax = end;
        }

        tree::BlobTree* Bsoid::tree() const
        {
            return mTree.get();
        }

        void Bsoid::constructLattice()
        {
            makeVoxels();
            mLattice.makeLattice(mVoxels);
            validateVoxels();
        }

        
        void Bsoid::constructMesh()
        {
            makeTriangles();
        }

        void Bsoid::polygonize()
        {
            using atlas::core::Timer;

            Timer<float> global;

            mLog << "Polygonizing model: " << mName << "\n";
            mLog << "Resolution: " << std::to_string(mGridSize) << ", "
                << std::to_string(mSvSize) << ".\n";
            mLog << "#===========================#\n";

            global.start();
            INFO_LOG("Bsoid: Starting Lattice generation.");
            // Generate lattices.
            {
                Timer<float> section;
                section.start();
                makeVoxels();
            }
            INFO_LOG("Bsoid: Lattice generation done.");

            INFO_LOG("Bsoid: Starting mesh generation.");
            {
                Timer<float> section;
                section.start();
                constructMesh();
            }
            INFO_LOG("Bsoid: Mesh generation done.");

            mLog << "\nSummary:\n";
            mLog << "#===========================#\n";
            mLog << "Total runtime: " << global.elapsed() << " seconds\n";
            mLog << "Total vertices generated: " << mMesh.vertices().size() << "\n";
            mLog << "Total memory usage: " << size() << " bytes\n";
            mLog << mTree->getFieldSummary();
        }

        Lattice const& Bsoid::getLattice() const
        {
            return mLattice;
        }

        atlas::utils::Mesh& Bsoid::getMesh()
        {
            return mMesh;
        }

        void Bsoid::setName(std::string const& name)
        {
            mName = name;
        }

        std::string Bsoid::getName() const
        {
            return mName;
        }

        std::string Bsoid::getLog() const
        { 
            return mLog.str();
        }

        void Bsoid::clearLog()
        {
            mLog.str(std::string());
        }

        void Bsoid::saveMesh()
        {
            mMesh.saveToFile(mName + ".obj");
        }

        std::size_t Bsoid::size() const
        {
            std::size_t voxelSize = mVoxels.size() * sizeof(Voxel);
            std::size_t seenVoxelSize = mSeenVoxels.size() *
                sizeof(std::pair<std::uint64_t, VoxelId>);
            std::size_t seenPointsSize = mSeenPoints.size() *
                sizeof(std::pair<std::uint64_t, VoxelId>);
            std::size_t svSize = mSuperVoxels.size() *
                sizeof(std::pair<std::uint64_t, VoxelId>);
            std::size_t computedSize = mComputedPoints.size() *
                sizeof(std::pair<std::uint128_t, LinePoint>);

            return voxelSize + seenVoxelSize + seenPointsSize + svSize +
                computedSize;
        }

        void Bsoid::makeVoxels()
        {
            using atlas::math::Point;
            using atlas::utils::BBox;

            atlas::core::Timer<float> global;
            atlas::core::Timer<float> t;

#if (DISABLE_PARALLEL)
            for (std::size_t x = 0; x < mSvSize; ++x)
            {
                for (std::size_t y = 0; y < mSvSize; ++y)
                {
                    for (std::size_t z = 0; z < mSvSize; ++z)
                    {
                        auto pt = createCellPoint(x, y, z, mSvDelta);
                        BBox cell(pt, pt + mSvDelta);

                        SuperVoxel sv;
                        sv.field = mTree->getSubTree(cell);
                        sv.id = { x, y, z };

                        if (sv.field)
                        {
                            auto idx = BsoidHash64::hash(x, y, z);
                            mSuperVoxels.insert({ idx, sv });
                        }
                    }
                }
            }

#else
            // First construct the grid of super-voxels.
            tbb::parallel_for(static_cast<std::uint64_t>(0), mSvSize, 
                [this](std::uint64_t x) {
                tbb::parallel_for(static_cast<std::uint64_t>(0), mSvSize,
                    [this, x](std::uint64_t y) {
                    tbb::parallel_for(static_cast<std::uint64_t>(0), mSvSize,
                        [this, x, y](std::uint64_t z)
                    {
                        auto pt = createCellPoint(x, y, z, mSvDelta);
                        BBox cell(pt, pt + mSvDelta);

                        SuperVoxel sv;
                        sv.field = mTree->getSubTree(cell);
                        sv.id = { x, y, z };

                        if (sv.field)
                        {
                            // critical section.
                            std::lock_guard<std::mutex> lock(mSvMutex);
                            auto idx = BsoidHash64::hash(x, y, z);
                            mSuperVoxels.insert({ idx, sv });
                        }
                    });
                });
            });
#endif


            // Now that we have the grid of super-voxels, we can grab the seeds
            // and convert them into voxels in parallel.
            auto seedPoints = mTree->getSeeds();
            std::vector<Voxel> seedVoxels(seedPoints.size());
#if (DISABLE_PARALLEL)
            std::size_t i = 0;
            for (auto& pt : seedPoints)
            {
                auto v = (pt - mMin) / mGridDelta;
                PointId id;
                id.x = static_cast<std::uint64_t>(v.x);
                id.y = static_cast<std::uint64_t>(v.y);
                id.z = static_cast<std::uint64_t>(v.z);
                seedVoxels[i] = Voxel(id);
                ++i;
            }
#else
            tbb::parallel_for(static_cast<std::size_t>(0), seedVoxels.size(),
                [this, seedPoints, &seedVoxels](std::size_t i) 
            {
                auto pt = seedPoints[i];
                auto v = (pt - mMin) / mGridDelta;
                PointId id;
                id.x = static_cast<std::uint64_t>(v.x);
                id.y = static_cast<std::uint64_t>(v.y);
                id.z = static_cast<std::uint64_t>(v.z);
                seedVoxels[i] = Voxel(id);
            });
#endif

            marchVoxelOnSurface(seedVoxels);
        }


        atlas::math::Point Bsoid::createCellPoint(std::uint64_t x,
            std::uint64_t y, std::uint64_t z, atlas::math::Point const& delta)
        {
            atlas::math::Point pt;
            atlas::math::Point p(x, y, z);
            auto const& start = mMin;

            pt = start + p * delta;
            return pt;
        }

        atlas::math::Point Bsoid::createCellPoint(glm::u64vec3 const& p,
            atlas::math::Point const& delta)
        {
            return createCellPoint(p.x, p.y, p.z, delta);
        }

        FieldPoint Bsoid::findVoxelPoint(PointId const& id)
        {
            using atlas::math::Point4;
            using atlas::math::Point;

            // First check if we have seen this point before.
            auto entry = mSeenPoints.find(BsoidHash64::hash(id.x, id.y, id.z));
            if (entry != mSeenPoints.end())
            {
                return (*entry).second;
            }
            else
            {
                auto pt = createCellPoint(id, mGridDelta);

                PointId svId;
                {
                    auto v = (pt - mMin) / mSvDelta;
                    svId.x = static_cast<std::uint64_t>(v.x);
                    svId.y = static_cast<std::uint64_t>(v.y);
                    svId.z = static_cast<std::uint64_t>(v.z);

                    // Check any of the coordinates of the id are beyond the edge
                    // of the grid.
                    svId.x = (svId.x < mSvSize) ? svId.x : svId.x - 1;
                    svId.y = (svId.y < mSvSize) ? svId.y : svId.y - 1;
                    svId.z = (svId.z < mSvSize) ? svId.z : svId.z - 1;
                }

                FieldPoint fp;
                {
                    auto svHash = BsoidHash64::hash(svId.x, svId.y, svId.z);
                    SuperVoxel sv = mSuperVoxels.at(svHash);
                    auto val = sv.eval(pt);
                    auto g = sv.grad(pt);
                    fp = { pt, val, g, svHash };
                }

                // Now that we have the point, let's add it to our list and
                // return it.
                {
                    std::lock_guard<std::mutex> lock(mSeenPointsMutex);
                    auto hash = BsoidHash64::hash(id.x, id.y, id.z);
                    mSeenPoints.insert(
                        std::pair<std::uint64_t, FieldPoint>(hash, fp));
                }

                return fp;
            }
        }

        void Bsoid::fillVoxel(Voxel& v)
        {
#if (DISABLE_PARALLEL)
            std::size_t d = 0;
            for (auto& id : VoxelDecals)
            {
                auto decalId = v.id + id;
                v.points[d] = findVoxelPoint(decalId);
                ++d;
            }
#else
            tbb::parallel_for(static_cast<std::size_t>(0), 
                VoxelDecals.size(), [this, &v](std::size_t d) {
                auto decalId = v.id + VoxelDecals[d];
                v.points[d] = findVoxelPoint(decalId);
            });
#endif
        }

        bool Bsoid::seenVoxel(VoxelId const& id)
        {
            if (mSeenVoxels.find(BsoidHash64::hash(id.x, id.y, id.z)) !=
                mSeenVoxels.end())
            {
                return true;
            }
            else
            {
                std::lock_guard<std::mutex> lock(mSeenVoxelsMutex);
                mSeenVoxels.insert(
                    std::pair<std::uint64_t, VoxelId>(
                        BsoidHash64::hash(id.x, id.y, id.z), id));
                return false;
            }
        }

        FieldPoint Bsoid::interpolate(FieldPoint const& p1, FieldPoint const& p2)
        {
            auto pt = glm::mix(p1.value.xyz(), p2.value.xyz(),
                (mMagic - p1.value.w) / (p2.value.w - p1.value.w));

            auto hash = p1.svHash;
            auto sv = mSuperVoxels[hash];
            auto val = sv.eval(pt);
            auto grad = sv.grad(pt);
            return FieldPoint(pt, val, grad, hash);
        }

        Bsoid::LinePoint Bsoid::generateLinePoint(PointId const& p1, 
            PointId const& p2, FieldPoint const& fp1, FieldPoint const& fp2)
        {
            auto h1 = BsoidHash64::hash(p1.x, p1.y, p1.z);
            auto h2 = BsoidHash64::hash(p2.x, p2.y, p2.z);

            auto edgeHash1 = BsoidHash128::hash(h1, h2);
            auto edgeHash2 = BsoidHash128::hash(h2, h1);

            auto entry1 = mComputedPoints.find(edgeHash1);
            auto entry2 = mComputedPoints.find(edgeHash2);

            if (entry1 != mComputedPoints.end() ||
                entry2 != mComputedPoints.end())
            {
                if (entry1 != mComputedPoints.end())
                {
                    return (*entry1).second;
                }
                
                return (*entry2).second;
            }
            else
            {
                auto edgeHash = edgeHash1;
                auto pt = interpolate(fp1, fp2);

                LinePoint p(pt, edgeHash);
                std::lock_guard<std::mutex> lock(mPointMutex);
                mComputedPoints.insert(
                    std::pair<std::uint128_t, LinePoint>(edgeHash, p));
                return p;
            }
        }

        void Bsoid::marchVoxelOnSurface(std::vector<Voxel> const& seeds)
        {
            using atlas::math::Point4;
            using atlas::math::Point;

            auto getEdges = [this](Voxel const& v)
            {
                int edgeId = 0;
                std::vector<int> edges;
                std::mutex edgesMutex;
#if (DISABLE_PARALLEL)
                FieldPoint start, end;
                for (auto& decal : EdgeDecals)
                {
                    start = v.points[decal.x];
                    end = v.points[decal.y];
                    float val1 = start.value.w - mMagic;
                    float val2 = end.value.w - mMagic;

                    if (glm::sign(val1) != glm::sign(val2))
                    {
                        auto map = NeighbourMap[edgeId];
                        edges.insert(edges.end(), map.begin(), map.end());
                    }
                    edgeId++;
                }
#else
                tbb::parallel_for(static_cast<std::size_t>(0), EdgeDecals.size(),
                    [this, &edges, &edgesMutex, &v](std::size_t edgeId)
                    {
                        FieldPoint start, end;
                        auto decal = EdgeDecals[edgeId];
                        start = v.points[decal.x];
                        end = v.points[decal.y];
                        float val1 = start.value.w - mMagic;
                        float val2 = end.value.w - mMagic;

                        if (glm::sign(val1) != glm::sign(val2))
                        {
                            auto map = NeighbourMap[edgeId];
                            std::lock_guard<std::mutex> lock(edgesMutex);
                            edges.insert(edges.end(), map.begin(), map.end());
                        }

                    });
#endif

                return edges;
            };

            if (seeds.empty())
            {
                return;
            }

            std::queue<VoxelId> frontier;
            std::mutex frontierMutex;
            {
                auto containsSurface = [this, getEdges](Voxel const& v)
                {
                    Voxel voxel = v;
                    fillVoxel(voxel);
                    auto edges = getEdges(voxel);
                    return !edges.empty();
                };

                auto findSurface = [this, containsSurface](Voxel const& v)
                {
                    bool found = false;
                    Voxel last, current;
                    current = v;

                    while (!found)
                    {
                        auto cPos = (2llu * current.id) + glm::u64vec3(1, 1, 1);
                        Point origin = createCellPoint(cPos, mGridDelta / 2.0f);
                        float originVal = mTree->eval(origin);
                        auto norm = mTree->grad(origin);
                        norm = glm::normalize(norm);
                        norm = (originVal > mMagic) ? -norm : norm;

                        // Now find the voxel that we are pointing to.
                        glm::ivec3 next = glm::sign(norm);

                        current.id.x += static_cast<std::uint64_t>(next.x);
                        current.id.y += static_cast<std::uint64_t>(next.y);
                        current.id.z += static_cast<std::uint64_t>(next.z);

                        // Check if the voxel hasn't run off the edge of the grid.
                        if (!validVoxel(current))
                        {
                            break;
                        }

                        if (containsSurface(current))
                        {
                            break;
                        }
                    }

                    return current;
                };

#if (DISABLE_PARALLEL)
                std::size_t i = 0;
                for (auto& seed : seeds)
                {
                    auto v = seeds[i];
                    if (!containsSurface(seed))
                    {
                        v = findSurface(v);
                        if (!validVoxel(v))
                        {
                            return;
                        }
                    }
                    frontier.push(v.id);
                    ++i;
                }
#else
                tbb::parallel_for(static_cast<std::size_t>(0), seeds.size(),
                    [this, containsSurface, findSurface, &frontierMutex, 
                    &frontier, seeds](std::size_t i) {
                    auto& seed = seeds[i];
                    auto v = seeds[i];
                    if (!containsSurface(seed))
                    {
                        v = findSurface(v);
                        if (!validVoxel(v))
                        {
                            return;
                        }
                    }

                    {
                        std::lock_guard<std::mutex> lock(frontierMutex);
                        frontier.push(v.id);
                    }
                });
#endif
            }

            // See whether there is a sensible way of parallelizing this later.
            if (frontier.empty())
            {
                DEBUG_LOG("Exiting on empty queue.");
                return;
            }

            while (!frontier.empty())
            {
                auto top = frontier.front();
                frontier.pop();

                if (seenVoxel(top))
                {
                    continue;
                }

                Voxel v(top);
                fillVoxel(v);

                auto edges = getEdges(v);
                if (edges.empty())
                {
                    continue;
                }

#if (DISABLE_PARALLEL)
                for (auto& edge : edges)
                {
                    auto decal = NeighbourDecals[edge];

                    auto neighbourDecal = v.id;
                    neighbourDecal.x += decal.x;
                    neighbourDecal.y += decal.y;
                    neighbourDecal.z += decal.z;

                    if (!validVoxel(Voxel(neighbourDecal)))
                    {
                        continue;
                    }
                    frontier.push(neighbourDecal);
                }
#else
                tbb::parallel_for(static_cast<std::size_t>(0), edges.size(),
                    [v, &frontier, &frontierMutex, edges, this](std::size_t i)
                {
                    auto decal = NeighbourDecals[edges[i]];

                    auto neighbourDecal = v.id;
                    neighbourDecal.x += decal.x;
                    neighbourDecal.y += decal.y;
                    neighbourDecal.z += decal.z;

                    if (!validVoxel(Voxel(neighbourDecal)))
                    {
                        return;
                    }

                    {
                        std::lock_guard<std::mutex> lock(frontierMutex);
                        frontier.push(neighbourDecal);
                    }
                });
#endif

                mVoxels.push_back(v);
            }
        }

        void Bsoid::makeTriangles()
        {
            using atlas::math::Point;
            using atlas::math::Normal;

            // Iterate over the set of voxels.
            std::map<std::uint128_t, std::uint32_t> indexMap;


#if (DISABLE_PARALLEL)
            for (auto& voxel : mVoxels)
            {
                std::uint32_t voxelIndex = 0;
                std::vector<std::uint32_t> coeffs =
                { 1, 2, 4, 8, 16, 32, 64, 128 };
                for (std::size_t i = 0; i < voxel.points.size(); ++i)
                {
                    voxelIndex |= (voxel.points[i].value.w < mMagic) ?
                        coeffs[i] : 0;
                }

                if (EdgeTable[voxelIndex] == 0)
                {
                    continue;
                }

                std::vector<LinePoint> vertList(12);
                if (EdgeTable[voxelIndex] & 1)
                {
                    vertList[0] = generateLinePoint(
                        voxel.id + VoxelDecals[0],
                        voxel.id + VoxelDecals[1],
                        voxel.points[0],
                        voxel.points[1]);
                }

                if (EdgeTable[voxelIndex] & 2)
                {
                    vertList[1] = generateLinePoint(
                        voxel.id + VoxelDecals[1],
                        voxel.id + VoxelDecals[2],
                        voxel.points[1],
                        voxel.points[2]);
                }

                if (EdgeTable[voxelIndex] & 4)
                {
                    vertList[2] = generateLinePoint(
                        voxel.id + VoxelDecals[2],
                        voxel.id + VoxelDecals[3],
                        voxel.points[2],
                        voxel.points[3]);
                }

                if (EdgeTable[voxelIndex] & 8)
                {
                    vertList[3] = generateLinePoint(
                        voxel.id + VoxelDecals[3],
                        voxel.id + VoxelDecals[0],
                        voxel.points[3],
                        voxel.points[0]);
                }

                if (EdgeTable[voxelIndex] & 16)
                {
                    vertList[4] = generateLinePoint(
                        voxel.id + VoxelDecals[4],
                        voxel.id + VoxelDecals[5],
                        voxel.points[4],
                        voxel.points[5]);
                }

                if (EdgeTable[voxelIndex] & 32)
                {
                    vertList[5] = generateLinePoint(
                        voxel.id + VoxelDecals[5],
                        voxel.id + VoxelDecals[6],
                        voxel.points[5],
                        voxel.points[6]);
                }

                if (EdgeTable[voxelIndex] & 64)
                {
                    vertList[6] = generateLinePoint(
                        voxel.id + VoxelDecals[6],
                        voxel.id + VoxelDecals[7],
                        voxel.points[6],
                        voxel.points[7]);
                }

                if (EdgeTable[voxelIndex] & 128)
                {
                    vertList[7] = generateLinePoint(
                        voxel.id + VoxelDecals[7],
                        voxel.id + VoxelDecals[4],
                        voxel.points[7],
                        voxel.points[4]);
                }

                if (EdgeTable[voxelIndex] & 256)
                {
                    vertList[8] = generateLinePoint(
                        voxel.id + VoxelDecals[0],
                        voxel.id + VoxelDecals[4],
                        voxel.points[0],
                        voxel.points[4]);
                }

                if (EdgeTable[voxelIndex] & 512)
                {
                    vertList[9] = generateLinePoint(
                        voxel.id + VoxelDecals[1],
                        voxel.id + VoxelDecals[5],
                        voxel.points[1],
                        voxel.points[5]);
                }

                if (EdgeTable[voxelIndex] & 1024)
                {
                    vertList[10] = generateLinePoint(
                        voxel.id + VoxelDecals[2],
                        voxel.id + VoxelDecals[6],
                        voxel.points[2],
                        voxel.points[6]);
                }

                if (EdgeTable[voxelIndex] & 2048)
                {
                    vertList[11] = generateLinePoint(
                        voxel.id + VoxelDecals[3],
                        voxel.id + VoxelDecals[7],
                        voxel.points[3],
                        voxel.points[7]);
                }

                for (int i = 0; TriangleTable[voxelIndex][i] != -1; i += 3)
                {
                    std::vector<LinePoint> pts;
                    pts.push_back(vertList[TriangleTable[voxelIndex][i + 0]]);
                    pts.push_back(vertList[TriangleTable[voxelIndex][i + 1]]);
                    pts.push_back(vertList[TriangleTable[voxelIndex][i + 2]]);

                    for (auto& pt : pts)
                    {
                        // Check if we have seen this point before.
                        auto entry = indexMap.find(pt.edge);
                        if (entry != indexMap.end())
                        {
                            // We have, so just insert the index.
                            mMesh.indices().push_back((*entry).second);
                        }
                        else
                        {
                            // We haven't, so insert it.
                            mMesh.vertices().push_back(pt.point.value.xyz());
                            mMesh.normals().push_back(-pt.point.g);
                            mMesh.indices().push_back(mMesh.vertices().size() - 1);
                            indexMap.insert(
                                std::pair<std::uint128_t, std::uint32_t>(pt.edge,
                                    mMesh.vertices().size() - 1));
                        }
                    }
                }

            }
#else
            std::mutex indexMapMutex;
            auto loop = [&indexMap, &indexMapMutex, this](std::size_t i)
            {
                Voxel& voxel = mVoxels[i];
                std::uint32_t voxelIndex = 0;
                std::vector<std::uint32_t> coeffs =
                { 1, 2, 4, 8, 16, 32, 64, 128 };
                for (std::size_t i = 0; i < voxel.points.size(); ++i)
                {
                    voxelIndex |= (voxel.points[i].value.w < mMagic) ?
                        coeffs[i] : 0;
                }

                if (EdgeTable[voxelIndex] == 0)
                {
                    return;
                }

                std::vector<LinePoint> vertList(12);
                if (EdgeTable[voxelIndex] & 1)
                {
                    vertList[0] = generateLinePoint(
                        voxel.id + VoxelDecals[0],
                        voxel.id + VoxelDecals[1],
                        voxel.points[0],
                        voxel.points[1]);
                }

                if (EdgeTable[voxelIndex] & 2)
                {
                    vertList[1] = generateLinePoint(
                        voxel.id + VoxelDecals[1],
                        voxel.id + VoxelDecals[2],
                        voxel.points[1],
                        voxel.points[2]);
                }

                if (EdgeTable[voxelIndex] & 4)
                {
                    vertList[2] = generateLinePoint(
                        voxel.id + VoxelDecals[2],
                        voxel.id + VoxelDecals[3],
                        voxel.points[2],
                        voxel.points[3]);
                }

                if (EdgeTable[voxelIndex] & 8)
                {
                    vertList[3] = generateLinePoint(
                        voxel.id + VoxelDecals[3],
                        voxel.id + VoxelDecals[0],
                        voxel.points[3],
                        voxel.points[0]);
                }

                if (EdgeTable[voxelIndex] & 16)
                {
                    vertList[4] = generateLinePoint(
                        voxel.id + VoxelDecals[4],
                        voxel.id + VoxelDecals[5],
                        voxel.points[4],
                        voxel.points[5]);
                }

                if (EdgeTable[voxelIndex] & 32)
                {
                    vertList[5] = generateLinePoint(
                        voxel.id + VoxelDecals[5],
                        voxel.id + VoxelDecals[6],
                        voxel.points[5],
                        voxel.points[6]);
                }

                if (EdgeTable[voxelIndex] & 64)
                {
                    vertList[6] = generateLinePoint(
                        voxel.id + VoxelDecals[6],
                        voxel.id + VoxelDecals[7],
                        voxel.points[6],
                        voxel.points[7]);
                }

                if (EdgeTable[voxelIndex] & 128)
                {
                    vertList[7] = generateLinePoint(
                        voxel.id + VoxelDecals[7],
                        voxel.id + VoxelDecals[4],
                        voxel.points[7],
                        voxel.points[4]);
                }

                if (EdgeTable[voxelIndex] & 256)
                {
                    vertList[8] = generateLinePoint(
                        voxel.id + VoxelDecals[0],
                        voxel.id + VoxelDecals[4],
                        voxel.points[0],
                        voxel.points[4]);
                }

                if (EdgeTable[voxelIndex] & 512)
                {
                    vertList[9] = generateLinePoint(
                        voxel.id + VoxelDecals[1],
                        voxel.id + VoxelDecals[5],
                        voxel.points[1],
                        voxel.points[5]);
                }

                if (EdgeTable[voxelIndex] & 1024)
                {
                    vertList[10] = generateLinePoint(
                        voxel.id + VoxelDecals[2],
                        voxel.id + VoxelDecals[6],
                        voxel.points[2],
                        voxel.points[6]);
                }

                if (EdgeTable[voxelIndex] & 2048)
                {
                    vertList[11] = generateLinePoint(
                        voxel.id + VoxelDecals[3],
                        voxel.id + VoxelDecals[7],
                        voxel.points[3],
                        voxel.points[7]);
                }

                for (int i = 0; TriangleTable[voxelIndex][i] != -1; i += 3)
                {
                    std::vector<LinePoint> pts;
                    pts.push_back(vertList[TriangleTable[voxelIndex][i + 0]]);
                    pts.push_back(vertList[TriangleTable[voxelIndex][i + 1]]);
                    pts.push_back(vertList[TriangleTable[voxelIndex][i + 2]]);

                    std::lock_guard<std::mutex> lock(indexMapMutex);
                    for (auto& pt : pts)
                    {
                        auto entry = indexMap.find(pt.edge);
                        if (entry != indexMap.end())
                        {
                            mMesh.indices().push_back((*entry).second);
                        }
                        else
                        {
                            mMesh.vertices().push_back(pt.point.value.xyz());
                            mMesh.normals().push_back(-pt.point.g);
                            mMesh.indices().push_back(mMesh.vertices().size() - 1);
                            indexMap.insert(
                                std::pair<std::uint128_t, std::uint32_t>(
                                    pt.edge, mMesh.vertices().size() - 1));
                        }
                    }
                }
            };

            tbb::parallel_for(static_cast<std::size_t>(0), mVoxels.size(), loop);
#endif
        }

        bool Bsoid::validVoxel(Voxel const& v)
        {
            return (
                v.isValid() &&
                v.id.x < mGridSize &&
                v.id.y < mGridSize &&
                v.id.z < mGridSize);
        }

        void Bsoid::validateVoxels()
        {
            std::map<std::uint64_t, Voxel> seenMap;
            for (auto& voxel : mVoxels)
            {
                auto hash = BsoidHash64::hash(voxel.id.x, voxel.id.y, voxel.id.z);
                seenMap.insert(std::pair<std::uint64_t, Voxel>(hash, voxel));
            }

            ATLAS_ASSERT(seenMap.size() == mVoxels.size(),
                "There should be no repeated voxels in the lattice.");
            DEBUG_LOG("Voxel validation succeeded.");
        }

    }
}
