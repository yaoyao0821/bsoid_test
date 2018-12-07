#include "bsoid/Bsoid.hpp"

#include "bsoid/visualizer/ModelView.hpp"
#include "bsoid/visualizer/ModelVisualizer.hpp"
#include "bsoid/models/Models.hpp"

#include <atlas/core/Log.hpp>
#include <atlas/utils/Application.hpp>
#include <atlas/utils/WindowSettings.hpp>
#include <atlas/gl/ErrorCheck.hpp>

#include <atlas/tools/ModellingScene.hpp>

#include <fstream>
#include <chrono>
#include <thread>
#include <tbb/parallel_for.h>
#include <iostream>

//int main()
//{
//    printf("hey");
//    tbb::parallel_for(0, 8, [=](const int r)
//                      {
//                          printf("hi %d",r);
//                      });
//    
//
//    return 0;
//
//}


std::vector<bsoid::models::ModelFn> getModels(
    bsoid::models::Resolution const& res = bsoid::models::currentResolution)
{
    using namespace bsoid::models;
    using namespace std;
    std::vector<ModelFn> result;
//
    //result.push_back([res]() { return makeSphere(res); });
    //result.push_back([res]() { return makeTorus(res); });

    //result.push_back([res]() { return makeBlend(res); });
    //result.push_back([res]() { return makeIntersection(res); });
    //result.push_back([res]() { return makeUnion(res); });
    //result.push_back([res]() { return makeTransform(res); });

    //result.push_back([res]() { return makeButterfly(res); });

    result.push_back([res]() { return makeParticles(res); });

    //result.push_back([res]() { return makeChain(res); });
    printf("res \n");
    std::cout << get<0>(res) << "," <<get<1>(res) << std::endl;

//    std::cout << res;
    printf("size \n");

    std::cout << result.size() << endl;
//    res
//    256,64
//    size
//    1

    return result;
}

std::vector<bsoid::models::MCModelFn> getMCModels(
    bsoid::models::Resolution const& res = bsoid::models::currentResolution)
{
    using namespace bsoid::models;
    
    std::vector<MCModelFn> result;

    //result.push_back([res]() { return makeMCSphere(res); });
    //result.push_back([res]() { return makeMCTorus(res); });

    //result.push_back([res]() { return makeMCBlend(res); });
    //result.push_back([res]() { return makeMCIntersection(res); });
    //result.push_back([res]() { return makeMCUnion(res); });
    //result.push_back([res]() { return makeMCTransform(res); });

    //result.push_back([res]() { return makeMCButterfly(res); });

    result.push_back([res]() {return makeMCParticles(res); });

    //result.push_back([res]() {return makeMCChain(res); });

    return result;
}


#if (BSOID_USE_GUI)
int main()
{
    // Atlas using.
    using atlas::utils::WindowSettings;
    using atlas::utils::ContextVersion;
    using atlas::utils::Application;
    using atlas::utils::ScenePointer;

    using bsoid::polygonizer::Bsoid;
    using bsoid::polygonizer::MarchingCubes;
    using bsoid::visualizer::ModelVisualizer;

    std::vector<Bsoid> models;
    std::vector<MarchingCubes> mcModels;

    auto modelFns = getModels();
    auto mcModelFns = getMCModels();

    for (auto& modelFn : modelFns)
    {
        models.emplace_back(modelFn());
    }
    
    for (auto& mcFn : mcModelFns)
    {
        mcModels.emplace_back(mcFn());
    }
    printf("jojo!%lu",sizeof(models));

    atlas::gl::setGLErrorSeverity(ATLAS_GL_ERROR_SEVERITY_HIGH |
        ATLAS_GL_ERROR_SEVERITY_MEDIUM);

    WindowSettings settings;
    settings.contextVersion = ContextVersion(3, 3);
//    settings.contextVersion = ContextVersion(4, 5);

    settings.isForwardCompat = true;
    settings.isMaximized = true;
    settings.title = "Bsoid " + std::string(BSOID_VERSION_STRING);
    printf("hey jojo!\n");

    Application::getInstance().createWindow(settings);
    Application::getInstance().addScene(ScenePointer(
        new ModelVisualizer(mcModels)));
//    Application::getInstance().addScene(ScenePointer(
//                                                     new ModelVisualizer(models)));
    Application::getInstance().runApplication();

    return 0;
}

#else

int main()
{
    INFO_LOG_V("Welcome to Bsoid %s", BSOID_VERSION_STRING);

    constexpr auto TestMode = 0;

    if (TestMode == 0)
    {
        std::fstream soidFile("bsoid_performance_summary.txt", 
            std::fstream::out);
        std::size_t res = 0;
        for (std::size_t i = 12; i < 253; i++)
        {
            res = 8 + (2 * i);
            INFO_LOG_V("Starting Bsoid run %d.", (i + 1));
            auto modelFns = getModels({ res, res / 4 });
            for (auto& modelFn : modelFns)
            {
                auto soid = modelFn();
                soid.polygonize();
                std::string log = soid.getLog();
                soidFile << log;
                soidFile << "\n\n";
            }
        }
        soidFile.flush();

        // Now run MC
        res = 0;
        std::fstream mcFile("mc_performance_summary.txt", std::fstream::out);
        for (std::size_t i = 12; i < 253; i++)
        {
            res = 8 + (2 * i);
            INFO_LOG_V("Staring MC run %d.", (i + 1));
            auto modelFns = getMCModels({ res, res / 4 });
            for (auto& modelFn : modelFns)
            {
                auto mc = modelFn();
                mc.polygonize();
                std::string log = mc.getLog();
                mcFile << log;
                mcFile << "\n\n";
            }
        }
        mcFile.flush();
    }
    else
    {
        auto modelFns = getModels({ 178, 45 });
        auto mcModelFns = getMCModels();

        std::fstream file("summary.txt", std::fstream::out);

        for (std::size_t i = 0; i < modelFns.size(); ++i)
        {
            {
                auto soid = modelFns[i]();
                INFO_LOG_V("Polygonizing model %s", soid.getName().c_str());
                soid.polygonize();
                std::string log = soid.getLog();
                file << log;
            }

            //INFO_LOG_V("Finished Bsoid, starting MC.");
            //{
            //    auto mc = mcModelFns[i]();
            //    mc.polygonize();
            //    std::string log = mc.getLog();
            //    file << "\n";
            //    file << log;
            //}
            
            file << "\n\n";
            INFO_LOG("");
            INFO_LOG("");

        }
    }

    return 0;
}

#endif
