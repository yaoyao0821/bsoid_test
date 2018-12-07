#ifndef BSOID_INCLUDE_BSOID_VISUALIZER_MODEL_VIEW_HPP
#define BSOID_INCLUDE_BSOID_VISUALIZER_MODEL_VIEW_HPP

#pragma once

#include "bsoid/polygonizer/Bsoid.hpp"
#include "bsoid/polygonizer/MarchingCubes.hpp"

#include <atlas/utils/Geometry.hpp>
#include <atlas/utils/Mesh.hpp>
#include <atlas/gl/Buffer.hpp>
#include <atlas/gl/VertexArrayObject.hpp>

namespace bsoid
{
    namespace visualizer
    {
        class ModelView : public atlas::utils::Geometry
        {
        public:
//            ModelView(polygonizer::Bsoid&& soid);
            ModelView(polygonizer::Bsoid&& soid, polygonizer::MarchingCubes&& mc);
            ModelView(polygonizer::MarchingCubes&& mc);
            ModelView(polygonizer::Bsoid&& soid);

            ModelView(ModelView&& view) = default;
            ~ModelView() = default;

            std::string getModelName() const;

//            void renderGeometry() override;
            void renderGeometry(atlas::math::Matrix4 const& projection,
                                atlas::math::Matrix4 const& view) override;
            
            
            void drawGui() override;
//            glm::vec3 mModel;

        private:
            void initShaders();
            void constructLattices();
            void constructMesh();
            void constructMCMesh();

            polygonizer::Bsoid mSoid;
            polygonizer::MarchingCubes mMC;

            atlas::gl::VertexArrayObject mLatticeVao;
            atlas::gl::Buffer mLatticeData;
            atlas::gl::Buffer mLatticeIndices;
            std::size_t mLatticeNumIndices;

            atlas::gl::VertexArrayObject mMeshVao;
            atlas::gl::Buffer mMeshData;
            atlas::gl::Buffer mMeshIndices;
            std::size_t mMeshNumIndices;
            std::size_t mMeshNumVertices;

            atlas::gl::VertexArrayObject mMCVao;
            atlas::gl::Buffer mMCData;
            atlas::gl::Buffer mMCIndices;
            std::size_t mMCNumIndices;
            std::size_t mMCNumVertices;

            bool mShowLattices;
            bool mShowMesh;
            bool mShowMCMesh;
            bool mHasMC;
            int mRenderMode;
            int mSelectedSlice;
        };
    }
}

#endif
