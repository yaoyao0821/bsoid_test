#include "bsoid/visualizer/ModelView.hpp"
#include "bsoid/ShaderPaths.hpp"
#include "LayoutLocations.glsl"

#include <atlas/utils/GUI.hpp>
#include <atlas/core/Enum.hpp>
#include <ctime>
#include<iostream>

#if defined ATLAS_DEBUG
#define ATHENA_DEBUG_CONTOURS 1 
#endif

enum class ShaderNames : int
{
//    Lattice = 0,
//    Wireframe = 1,
    Mesh = 0
};

namespace gl = atlas::gl;
namespace math = atlas::math;

namespace bsoid
{
    namespace visualizer
    {
        
        ModelView::ModelView(polygonizer::Bsoid&& soid) :
        mSoid(std::move(soid)),
//        mMC(std::move(mc)),
        mLatticeData(GL_ARRAY_BUFFER),
        mLatticeIndices(GL_ELEMENT_ARRAY_BUFFER),
        mLatticeNumIndices(0),
        mMeshData(GL_ARRAY_BUFFER),
        mMeshIndices(GL_ELEMENT_ARRAY_BUFFER),
        mMeshNumIndices(0),
        mMCData(GL_ARRAY_BUFFER),
        mMCIndices(GL_ELEMENT_ARRAY_BUFFER),
        mMCNumIndices(0),
        mShowLattices(false),
        mShowMesh(false),
        mShowMCMesh(true),
        mHasMC(true),
        mRenderMode(0),
        mSelectedSlice(0)
        {
            initShaders();
        }
        
        ModelView::ModelView(polygonizer::Bsoid&& soid, polygonizer::MarchingCubes&& mc) :
        mSoid(std::move(soid)),
        mMC(std::move(mc)),
        mLatticeData(GL_ARRAY_BUFFER),
        mLatticeIndices(GL_ELEMENT_ARRAY_BUFFER),
        mLatticeNumIndices(0),
        mMeshData(GL_ARRAY_BUFFER),
        mMeshIndices(GL_ELEMENT_ARRAY_BUFFER),
        mMeshNumIndices(0),
        mMCData(GL_ARRAY_BUFFER),
        mMCIndices(GL_ELEMENT_ARRAY_BUFFER),
        mMCNumIndices(0),
        mShowLattices(false),
        mShowMesh(false),
        mShowMCMesh(true),
        mHasMC(true),
        mRenderMode(0),
        mSelectedSlice(0)
        {
            initShaders();
        }
 

        ModelView::ModelView(polygonizer::MarchingCubes&& mc) :
//        mSoid(std::move(soid)),
        mMC(std::move(mc)),
        mLatticeData(GL_ARRAY_BUFFER),
        mLatticeIndices(GL_ELEMENT_ARRAY_BUFFER),
        mLatticeNumIndices(0),
        mMeshData(GL_ARRAY_BUFFER),
        mMeshIndices(GL_ELEMENT_ARRAY_BUFFER),
        mMeshNumIndices(0),
        mMCData(GL_ARRAY_BUFFER),
        mMCIndices(GL_ELEMENT_ARRAY_BUFFER),
        mMCNumIndices(0),
        mShowLattices(false),
        mShowMesh(false),
        mShowMCMesh(true),
        mHasMC(true),
        mRenderMode(0),
        mSelectedSlice(0)
        {
            initShaders();
        }

        std::string ModelView::getModelName() const
        {
            return mSoid.getName();
        }


        void ModelView::renderGeometry(atlas::math::Matrix4 const& projection,
                                       atlas::math::Matrix4 const& view)
        {
            if(!mShaders[0].getShaderProgram())
            {
                printf("Render ERROR\n");

            }
            printf("\ninside render, print: %lu\n",mMCNumIndices);

            printf("ha1");

                mShaders[0].hotReloadShaders();
            printf("ha2");

                if (!mShaders[0].shaderProgramValid())
                {
                    printf("ha3");
                    return;
                }
            
            printf("ha");
            using atlas::core::enumToUnderlyingType;

            mShaders[0].enableShaders();

                mMCVao.bindVertexArray();
                mMCIndices.bindBuffer();

//                    auto meshIndex = enumToUnderlyingType(ShaderNames::Mesh);
                    mShaders[0].enableShaders();

            glUniformMatrix4fv(mUniforms["model"], 1, GL_FALSE,
                        &mModel[0][0]);
            glUniformMatrix4fv(mUniforms["projection"], 1, GL_FALSE,
                               &projection[0][0]);
            glUniformMatrix4fv(mUniforms["view"], 1, GL_FALSE, &view[0][0]);
            
//                    glUniform1i(var, mRenderMode);
            
            
//            mMCVao.bindVertexArray();
//                    mMCIndices.bindBuffer();

                    glDrawElements(GL_TRIANGLES, (GLsizei)mMCNumIndices,
                        GL_UNSIGNED_INT, 0);


                mMCIndices.unBindBuffer();
                mMCVao.unBindVertexArray();
            mShaders[0].disableShaders();


            printf("\ninside render, print: %lu\n",mMCNumIndices);
        }

        void ModelView::drawGui()
        {
            // Polygonizer controls window.
            ImGui::SetNextWindowSize(ImVec2(470, 400), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Polygonizer Controls");

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Generation controls");
            ImGui::Separator();
//            if (mHasMC)
//            {
//                if (ImGui::Button("Construct MC mesh"))
//                {
//                        constructMCMesh();
//                }
//            }

//            if (ImGui::Button("Save mesh"))
//            {
//                mMesh.saveMesh();
//            }

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Visualization Options");
            ImGui::Separator();
            ImGui::Checkbox("Show lattices", &mShowLattices);
            ImGui::Checkbox("Show mesh", &mShowMesh);
            ImGui::Checkbox("Show MC mesh", &mShowMCMesh);

            ImGui::Dummy(ImVec2(0, 10));
            ImGui::Text("Log");
            ImGui::Separator();
            ImGui::BeginChild("Log", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                false, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
//            ImGui::TextWrapped(mSoid.getLog().c_str());
            ImGui::EndChild();
            ImGui::End();

            // Render controls.
            ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Render Controls");

            std::vector<const char*> renderNames = { "Vertices", "Wireframe", 
                "Shaded", "Wireframe on Shaded", "Normals" };
            ImGui::Combo("Render mode", &mRenderMode, renderNames.data(),
                ((int)renderNames.size()));
            ImGui::End();
        }

        void ModelView::initShaders()
        {
            using atlas::core::enumToUnderlyingType;
//            constructMCMesh();
//            mMC.polygonize();
//            
//            auto verts = mMC.getMesh().vertices();
//            auto normals = mMC.getMesh().normals();
//            auto idx = mMC.getMesh().indices();
//            mMC.saveMesh();
       //bsoid test
            using namespace std;
            
            clock_t start = clock();
            mMC.polygonize();
//            mSoid.polygonize();
            clock_t end = clock();
            cout << "花费了" << (double)(end - start) / CLOCKS_PER_SEC << "秒" << endl;

            auto verts = mMC.getMesh().vertices();
            auto normals = mMC.getMesh().normals();
            auto idx = mMC.getMesh().indices();
            mMC.saveMesh();
            //=======
            
            mMCNumVertices = verts.size();
            mMCNumIndices = idx.size();
            printf("data!\n ");
            
            std::vector<float> data;
            for (std::size_t i = 0; i < verts.size(); ++i)
            {
                data.push_back(verts[i].x);
                data.push_back(verts[i].y);
                data.push_back(verts[i].z);
                
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
                printf("%f-%f-%f\n",verts[i].x,verts[i].y,verts[i].z);
            }
            printf("size %lu %lu",verts.size(),mMCNumIndices);
            
            mMCVao.bindVertexArray();
            mMCData.bindBuffer();
            mMCData.bufferData(gl::size<float>(data.size()), data.data(),
                               GL_STATIC_DRAW);
            mMCData.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3,
                                        GL_FLOAT, GL_FALSE, gl::stride<float>(6),
                                        gl::bufferOffset<float>(0));
            mMCData.vertexAttribPointer(NORMALS_LAYOUT_LOCATION, 3,
                                        GL_FLOAT, GL_FALSE, gl::stride<float>(6),
                                        gl::bufferOffset<float>(3));
            mMCVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
            mMCVao.enableVertexAttribArray(NORMALS_LAYOUT_LOCATION);
            
            mMCIndices.bindBuffer();
            mMCIndices.bufferData(
                                  gl::size<GLuint>(idx.size()), idx.data(), GL_STATIC_DRAW);
            glDrawElements(GL_TRIANGLES, (GLsizei)mMCNumIndices,
                           GL_UNSIGNED_INT, 0);
            mMCIndices.unBindBuffer();
            mMCData.unBindBuffer();
            mMCVao.unBindVertexArray();

            
            
            
            
            
            
            
            
            
            
            
            
//            mMCVao.bindVertexArray();
//            mMCIndices.bindBuffer();
//            
//            auto meshIndex = enumToUnderlyingType(ShaderNames::Mesh);
//            mShaders[meshIndex].enableShaders();
//            auto var = mUniforms["mesh_renderMode"];
//            
//            glUniformMatrix4fv(mUniforms["mesh_model"], 1, GL_FALSE,
//                               &mModel[0][0]);
//            glUniform1i(var, mRenderMode);
//            mMCVao.bindVertexArray();
//            mMCIndices.bindBuffer();
//            glDrawElements(GL_TRIANGLES, (GLsizei)mMCNumIndices,
//                           GL_UNSIGNED_INT, gl::bufferOffset<GLuint>(0));
//            mShaders[meshIndex].disableShaders();
//            
//            printf("test \n");
//            mMCIndices.unBindBuffer();
//            mMCVao.unBindVertexArray();
//            
//            
//            
//            mMCVao.bindVertexArray();
//            mVertexBuffer.bindBuffer();
//            mVertexBuffer.bufferData(gl::size<glm::vec3>(particlesPos.size()), particlesPos.data(), GL_DYNAMIC_DRAW);
//            mVertexBuffer.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3, GL_FLOAT,
//                                              GL_FALSE, 0, gl::bufferOffset<float>(0));
//            mVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
//            
//            mVertexBuffer.unBindBuffer();
//            mVao.unBindVertexArray();
//            
            
//            std::vector<float> data;
//            data.push_back(0.f);
//            data.push_back(0.f);
//            data.push_back(0.f);
//
//            data.push_back(0.f);
//            data.push_back(1.f);
//            data.push_back(0.f);


//            mMCVao.bindVertexArray();
//            mMCData.bindBuffer();
//            mMCData.bufferData(gl::size<float>(data.size()), data.data(),
//                                           GL_DYNAMIC_DRAW);
//                    mMCData.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3, GL_FLOAT,
//                        GL_FALSE, gl::stride<float>(8), gl::bufferOffset<float>(0));
//                    mMCData.vertexAttribPointer(NORMALS_LAYOUT_LOCATION, 3, GL_FLOAT,
//                        GL_FALSE, gl::stride<float>(8), gl::bufferOffset<float>(3));
//                    mMCVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
//                    mMCVao.enableVertexAttribArray(NORMALS_LAYOUT_LOCATION);
//            
////                    mMCNumIndices.bindBuffer();
////                    mMCNumIndices.bufferData(gl::size<GLuint>(data.size()),
////                        data.indices().data(), GL_STATIC_DRAW);
//            
////                    mIndexBuffer.unBindBuffer();
//                    mMCData.unBindBuffer();
//                    mMCVao.unBindVertexArray();
//////
//
//            
//            printf("start init shader");

            // Load the lattice shaders first.
//            std::vector<gl::ShaderUnit> latticeShaders
//            {
//                { std::string(ShaderDirectory) +
//                "bsoid/visualizer/Lattice.vs.glsl", GL_VERTEX_SHADER },
//                { std::string(ShaderDirectory) +
//                "bsoid/visualizer/Lattice.fs.glsl", GL_FRAGMENT_SHADER }
//            };
//
//            std::vector<gl::ShaderUnit> wireframeShaders
//            {
//                { std::string(ShaderDirectory) +
//                "bsoid/visualizer/Wireframe.vs.glsl", GL_VERTEX_SHADER },
//                { std::string(ShaderDirectory) +
//                "bsoid/visualizer/Wireframe.fs.glsl", GL_FRAGMENT_SHADER }
//            };

            // Finally the mesh shaders.
            
            
            
            std::vector<gl::ShaderUnit> meshShaders
            {
                { std::string(ShaderDirectory) + "Dragon.vs.glsl", GL_VERTEX_SHADER },
//                { std::string(ShaderDirectory) +
//                "bsoid/visualizer/Mesh.gs.glsl", GL_GEOMETRY_SHADER },
                { std::string(ShaderDirectory) + "Dragon.fs.glsl", GL_FRAGMENT_SHADER}
            };
            printf("1\n");

            mShaders.emplace_back(meshShaders);
//            mShaders[0].setShaderIncludeDir(ShaderDirectory);
            mShaders[0].setShaderIncludeDir(ShaderDirectory);

            mShaders[0].compileShaders();
            printf("2\n");

            mShaders[0].linkShaders();
            printf("3\n");

            //
            //        auto var = mShaders[0].getUniformVariable("model");
            //        mUniforms.insert(UniformKey("model", var));
            //        var = mShaders[0].getUniformVariable("projection");
            //        mUniforms.insert(UniformKey("projection", var));
            //        var = mShaders[0].getUniformVariable("view");
            //        mUniforms.insert(UniformKey("view", var));
            //        var = mShaders[0].getUniformVariable("materialColour");
            //        mUniforms.insert(UniformKey("materialColour", var));
            //
            //        mShaders[0].disableShaders();

            
            
//            mShaders.push_back(gl::Shader(latticeShaders));
//            mShaders.push_back(gl::Shader(wireframeShaders));
//            mShaders.push_back(gl::Shader(meshShaders));

//            for (auto& shader : mShaders)
//            {
//                shader.setShaderIncludeDir(ShaderDirectory);
//                shader.compileShaders();
//                shader.linkShaders();
//            }

            // Grab the lattice uniforms.
//            auto latticeIndex = 0;
//            auto var = mShaders[latticeIndex].getUniformVariable("model");
////            mUniforms.insert(UniformKey("model", var));
//
//            mUniforms.insert(UniformKey("lattice_model", var));
//            
//            mShaders[0].disableShaders();

//
//            auto wireframeIndex = enumToUnderlyingType(ShaderNames::Wireframe);
//            var = mShaders[wireframeIndex].getUniformVariable("model");
//            mUniforms.insert(UniformKey("wireframe_model", var));
//            var = mShaders[wireframeIndex].getUniformVariable("renderMode");
//            mUniforms.insert(UniformKey("wireframe_renderMode", var));
//
            // Finally the mesh uniforms.
            auto var = mShaders[0].getUniformVariable("model");
            mUniforms.insert(UniformKey("model", var));
            var = mShaders[0].getUniformVariable("projection");
            mUniforms.insert(UniformKey("projection", var));
            var = mShaders[0].getUniformVariable("view");
            mUniforms.insert(UniformKey("view", var));
            mModel = glm::scale(math::Matrix4(1.0f), math::Vector(1.0f));

//
//            
//            auto meshIndex = enumToUnderlyingType(ShaderNames::Mesh);
//            auto var = mShaders[meshIndex].getUniformVariable("model");
//            mUniforms.insert(UniformKey("mesh_model", var));
            mShaders[0].disableShaders();

//
//            var = mShaders[meshIndex].getUniformVariable("renderMode");
//            mUniforms.insert(UniformKey("mesh_renderMode", var));
//
//            for (auto& shader : mShaders)
//            {
//                shader.disableShaders();
//            }
        }

   

        void ModelView::constructMCMesh()
        {
            mMC.polygonize();

            auto verts = mMC.getMesh().vertices();
            auto normals = mMC.getMesh().normals();
            auto idx = mMC.getMesh().indices();
            mMC.saveMesh();
            mMCNumVertices = verts.size();
            mMCNumIndices = idx.size();
            printf("data!\n ");

            std::vector<float> data;
            for (std::size_t i = 0; i < verts.size(); ++i)
            {
                data.push_back(verts[i].x);
                data.push_back(verts[i].y);
                data.push_back(verts[i].z);

                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
                printf("%f-%f-%f\n",verts[i].x,verts[i].y,verts[i].z);
            }
            printf("size %lu %lu",verts.size(),mMCNumIndices);

            mMCVao.bindVertexArray();
            mMCData.bindBuffer();
            mMCData.bufferData(gl::size<float>(data.size()), data.data(),
                GL_STATIC_DRAW);
            mMCData.vertexAttribPointer(VERTICES_LAYOUT_LOCATION, 3,
                GL_FLOAT, GL_FALSE, gl::stride<float>(6),
                gl::bufferOffset<float>(0));
            mMCData.vertexAttribPointer(NORMALS_LAYOUT_LOCATION, 3,
                GL_FLOAT, GL_FALSE, gl::stride<float>(6), 
                gl::bufferOffset<float>(3));
            mMCVao.enableVertexAttribArray(VERTICES_LAYOUT_LOCATION);
            mMCVao.enableVertexAttribArray(NORMALS_LAYOUT_LOCATION);

            mMCIndices.bindBuffer();
            mMCIndices.bufferData(
                gl::size<GLuint>(idx.size()), idx.data(), GL_STATIC_DRAW);
            glDrawElements(GL_TRIANGLES, (GLsizei)mMCNumIndices,
                           GL_UNSIGNED_INT, 0);
            mMCIndices.unBindBuffer();
            mMCData.unBindBuffer();
            mMCVao.unBindVertexArray();
        }
    }
}
