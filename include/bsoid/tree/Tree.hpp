#ifndef BSOID_INCLUDE_BSOID_TREE_TREE_HPP
#define BSOID_INCLUDE_BSOID_TREE_TREE_HPP

#pragma once

#include <memory>

namespace bsoid
{
    namespace tree
    {
        class Node;
        class BlobTree;

        using NodePtr = std::shared_ptr<Node>;
        using TreePointer = std::unique_ptr<BlobTree>;
    }
}

#endif