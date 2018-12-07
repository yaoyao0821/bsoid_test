#ifndef BSOID_INCLUDE_BSOID_VISUALIZER_MODEL_VISUALIZER_HPP
#define BSOID_INCLUDE_BSOID_VISUALIZER_MODEL_VISUALIZER_HPP

#pragma once

#include "ModelView.hpp"

#include <atlas/tools/ModellingScene.hpp>

namespace bsoid
{
    namespace visualizer
    {
        class ModelVisualizer : public atlas::tools::ModellingScene
        {
        public:
            ModelVisualizer(std::vector<polygonizer::Bsoid>& models);
            ModelVisualizer(std::vector<polygonizer::Bsoid>& models,
                std::vector<polygonizer::MarchingCubes>& modelsMC);
            ModelVisualizer(std::vector<polygonizer::MarchingCubes>& modelsMC);
            void renderScene() override;

        private:
            void takeSnapshot(std::string const& name);
//            std::vector<ModelView> mViews;
            int mCurrentView;
            ModelView mViews;
        };
    }
}

#endif
