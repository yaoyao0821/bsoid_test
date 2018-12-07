#include "bsoid/polygonizer/Lattice.hpp"

#include <functional>
#include <unordered_set>

namespace bsoid
{
    namespace polygonizer
    {
        void Lattice::makeLattice(std::vector<Voxel> const& voxels)
        {
            using atlas::math::Point;

            struct LatticePoint
            {
                LatticePoint(Point const& p) :
                    point(p)
                { }

                LatticePoint(Point const& p, std::uint32_t i) :
                    point(p),
                    index(i)
                { }

                bool operator==(LatticePoint const& rhs) const
                {
                    return point == rhs.point;
                }

                bool operator!=(LatticePoint const& rhs) const
                {
                    return !(*this == rhs);
                }

                Point point;
                std::uint32_t index;
            };

            struct LatticeHash
            {
                std::size_t operator()(LatticePoint const& p) const
                {
                    return std::hash<float>()(p.point.x + p.point.y + 
                        p.point.z);
                }
            };

            clearBuffers();

            std::vector<Point> verts;
            std::vector<std::uint32_t> idxs;
            std::uint32_t idxStart = 0;
            for (auto& voxel : voxels)
            {
                for (auto& pt : voxel.points)
                {
                    verts.push_back(pt.value.xyz());
                }

                idxs.push_back(idxStart + 0);
                idxs.push_back(idxStart + 1);

                idxs.push_back(idxStart + 1);
                idxs.push_back(idxStart + 2);

                idxs.push_back(idxStart + 2);
                idxs.push_back(idxStart + 3);

                idxs.push_back(idxStart + 3);
                idxs.push_back(idxStart + 0);

                idxs.push_back(idxStart + 4);
                idxs.push_back(idxStart + 5);

                idxs.push_back(idxStart + 5);
                idxs.push_back(idxStart + 6);

                idxs.push_back(idxStart + 6);
                idxs.push_back(idxStart + 7);

                idxs.push_back(idxStart + 7);
                idxs.push_back(idxStart + 4);

                idxs.push_back(idxStart + 0);
                idxs.push_back(idxStart + 4);

                idxs.push_back(idxStart + 1);
                idxs.push_back(idxStart + 5);

                idxs.push_back(idxStart + 2);
                idxs.push_back(idxStart + 6);
                
                idxs.push_back(idxStart + 3);
                idxs.push_back(idxStart + 7);

                idxStart = static_cast<std::uint32_t>(verts.size());
            }

            std::unordered_set<LatticePoint, LatticeHash> uniqueVertices;
            std::uint32_t current = vertices.size();
            for (auto& idx : idxs)
            {
                auto pt = verts[idx];
                if (uniqueVertices.find(pt) == uniqueVertices.end())
                {
                    indices.push_back(current);
                    vertices.push_back(pt);

                    uniqueVertices.emplace(pt, current);
                    current++;
                }
                else
                {
                    auto uniqueV = *uniqueVertices.find(pt);
                    indices.push_back(uniqueV.index);
                }
            }
        }

        void Lattice::clearBuffers()
        {
            vertices.clear();
            indices.clear();
        }
    }
}