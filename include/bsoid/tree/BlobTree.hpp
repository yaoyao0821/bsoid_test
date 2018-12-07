#ifndef BSOID_INCLUDE_BSOID_TREE_BLOB_TREE_HPP
#define BSOID_INCLUDE_BSOID_TREE_BLOB_TREE_HPP

#pragma once

#include "Tree.hpp"
#include "Node.hpp"
#include "bsoid/fields/ImplicitField.hpp"

#include <vector>

namespace bsoid
{
    namespace tree
    {
        class BlobTree
        {
        public:
            BlobTree();
            ~BlobTree() = default;

            void insertField(fields::ImplicitFieldPtr const& field);
            void insertFields(
                std::vector<fields::ImplicitFieldPtr> const& fields);

            void insertSkeletalField(fields::ImplicitFieldPtr const& field);
            void insertSkeletalFields(
                std::vector<fields::ImplicitFieldPtr> const& fields);

            void insertNodeTree(std::vector<std::vector<int>> const& tree);
            void insertFieldTree(fields::ImplicitFieldPtr const& tree);

            float eval(atlas::math::Point const& p) const;
            atlas::math::Normal grad(atlas::math::Point const& p) const;

            fields::ImplicitFieldPtr getSubTree(
                atlas::utils::BBox const& box) const;

            atlas::utils::BBox getTreeBox() const;
            std::vector<atlas::math::Point> getSeeds() const;

            std::string getFieldSummary() const;

        private:
            std::vector<NodePtr> mNodes;
            NodePtr mVolumeTree;
            fields::ImplicitFieldPtr mFieldTree;
            std::vector<fields::ImplicitFieldPtr> mSkeletalFields;
        };
    }
}

#endif