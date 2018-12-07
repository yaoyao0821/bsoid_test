#include "bsoid/visualizer/ModelVisualizer.hpp"

#include <atlas/gl/GL.hpp>
#include <atlas/utils/GUI.hpp>
#include <atlas/core/STB.hpp>
#include <atlas/core/Log.hpp>
#include <atlas/core/Assert.hpp>

#include <algorithm>
#include <fstream>

namespace gl = atlas::gl;
namespace math = atlas::math;
namespace utils = atlas::utils;
namespace tools = atlas::tools;

namespace bsoid
{
    namespace visualizer
    {
        ModelVisualizer::ModelVisualizer(std::vector<polygonizer::Bsoid>& models,
            std::vector<polygonizer::MarchingCubes>& modelsMC) :
            ModellingScene(),
            mViews(std::move(modelsMC[0])),
            mCurrentView(0)
        {
//            if (modelsMC.empty())
//            {
//                for (auto&& model : models)
//                {
//                    mViews.emplace_back(std::move(model));
//                }
//            }
//            else
//            {
//                ATLAS_ASSERT(models.size() == modelsMC.size(),
//                    "When comparing models, the number must match.");
                for (std::size_t i = 0; i < models.size(); ++i)
                {
//                    mViews.emplace_back(std::move(models[i]), std::move(modelsMC[i]));
                }
//            }

        }
        
        ModelVisualizer::ModelVisualizer(std::vector<polygonizer::Bsoid>& models) :
        ModellingScene(),
        mViews(std::move(models[0])),
        mCurrentView(0)
        {
//            for (std::size_t i = 0; i < modelsMC.size(); ++i)
//            {
//                mViews.emplace_back(std::move(modelsMC[0]));
//            }
            
            
        }
        
        ModelVisualizer::ModelVisualizer(std::vector<polygonizer::MarchingCubes>& modelsMC) :
        ModellingScene(),
        mViews(std::move(modelsMC[0])),
        mCurrentView(0)
        {
            //            for (std::size_t i = 0; i < modelsMC.size(); ++i)
            //            {
            //                mViews.emplace_back(std::move(modelsMC[0]));
            //            }
            
            
        }
        

        void ModelVisualizer::renderScene()
        {
            utils::Gui::getInstance().newFrame();
            const float grey = 92.0f / 255.0f;
            glClearColor(grey, grey, grey, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mProjection = glm::perspective(
                glm::radians(mCamera.getCameraFOV()),
                (float)mWidth / mHeight, 1.0f, 1000000.0f);

            mUniformMatrixBuffer.bindBuffer();
            mUniformMatrixBuffer.bufferSubData(0,
                gl::size<math::Matrix4>(1), &mProjection[0][0]);
            mUniformMatrixBuffer.unBindBuffer();

            mView = mCamera.getCameraMatrix();
            mUniformMatrixBuffer.bindBuffer();
            mUniformMatrixBuffer.bufferSubData(gl::size<math::Matrix4>(1),
                gl::size<math::Matrix4>(1), &mView[0][0]);

            if (mShowGrid)
            {
                mGrid.renderGeometry(mProjection, mView);
            }

            // Now render the view.
//            mViews[mCurrentView].renderGeometry();
            mViews.renderGeometry(mProjection, mView);


            // Global HUD.
            ImGui::SetNextWindowSize(ImVec2(350, 140), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Global HUD");
            ImGui::Checkbox("Show grid", &mShowGrid);
            if (ImGui::Button("Reset Camera"))
            {
                mCamera.resetCamera();
            }

//            if (ImGui::Button("Take snapshot"))
//            {
//                takeSnapshot(mViews[mCurrentView].getModelName());
//            }

            // Model select.
            std::vector<std::string> strNames;
//            for (auto& view : mViews)
//            {
//                strNames.push_back(view.getModelName());
//            }
            strNames.push_back(mViews.getModelName());
            std::vector<const char*> names;
            std::transform(strNames.begin(), strNames.end(),
                std::back_inserter(names),
                [](std::string const& str) { return str.c_str(); });

            ImGui::Combo("Select model", &mCurrentView, names.data(),
                ((int)names.size()));

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            // Render the GUI for the current view.
            mViews.drawGui();

            ImGui::Render();
        }

        void ModelVisualizer::takeSnapshot(std::string const& name)
        {
//            // Determine what the name of the snapshot image will be.
//            int num = 0;
//            std::string filename = name + "_image_";
//            while (true)
//            {
//                std::ifstream file(filename + std::to_string(num) + ".png");
//                if (!file)
//                {
//                    break;
//                }
//                num++;
//            }
//
//            filename = filename + std::to_string(num) + ".png";
//
//            // We essentially render the entire scene again.
//            {
//                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
////                mViews.renderGeometry();
//            }
//
//            int width = static_cast<int>(mWidth);
//            int height = static_cast<int>(mHeight);
//
//            unsigned char* buffer = new unsigned char[width * height * 4];
//            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
//            unsigned char* lastRow = buffer + (width * 4 * (height - 1));
//            if (!stbi_write_png(filename.c_str(), width, height, 4, lastRow,
//                -4 * width))
//            {
//                ERROR_LOG_V("Could not write image to %s", filename.c_str());
//            }
        }
    }
}
