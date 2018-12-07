#include "bsoid/models/Models.hpp"

#include "bsoid/fields/Sphere.hpp"
#include "bsoid/fields/Torus.hpp"

#include "bsoid/operators/Blend.hpp"
#include "bsoid/operators/Intersection.hpp"
#include "bsoid/operators/Union.hpp"
#include "bsoid/operators/Transform.hpp"

#include "bsoid/tree/BlobTree.hpp"
#include "bsoid/polygonizer/Bsoid.hpp"

#include <atlas/math/RandomGenerator.hpp>

#include <tuple>
#include <numeric>
#include <chrono>

namespace bsoid
{
    namespace models
    {
        // Global usings.
        using fields::ImplicitFieldPtr;
        using operators::ImplicitOperatorPtr;
        using tree::BlobTree;
        using polygonizer::Bsoid;
        using polygonizer::MarchingCubes;

        polygonizer::Bsoid makeSphere(Resolution const& res)
        {
            using fields::Sphere;

            ImplicitFieldPtr sphere = std::make_shared<Sphere>();
            BlobTree tree;
            tree.insertField(sphere);
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(sphere);
            tree.insertSkeletalField(sphere);

            Bsoid soid(tree, "sphere");
            soid.setResolution(std::get<0>(res),
                std::get<1>(res));
            return soid;
        }

        polygonizer::MarchingCubes makeMCSphere(Resolution const& res)
        {
            using fields::Sphere;

            ImplicitFieldPtr sphere = std::make_shared<Sphere>();
            BlobTree tree;
            tree.insertField(sphere);
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(sphere);
            tree.insertSkeletalField(sphere);

            MarchingCubes mc(tree, "sphere");
            mc.setResolution(std::get<0>(res));
            return mc;
        }

        polygonizer::Bsoid makeTorus(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Torus;

            ImplicitFieldPtr torus = std::make_shared<Torus>();
            BlobTree tree;
            tree.insertField(torus);
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(torus);
            tree.insertSkeletalField(torus);

            Bsoid soid(tree, "torus");
            soid.setResolution(std::get<0>(res),
                std::get<1>(res));
            return soid;
        }

        polygonizer::MarchingCubes makeMCTorus(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Torus;

            ImplicitFieldPtr torus = std::make_shared<Torus>();
            BlobTree tree;
            tree.insertField(torus);
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(torus);
            tree.insertSkeletalField(torus);

            MarchingCubes mc(tree, "torus");
            mc.setResolution(std::get<0>(res));
            return mc;
        }

        polygonizer::Bsoid makeBlend(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Blend;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(1.0f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-1.0f, 0, 0));
            ImplicitOperatorPtr blend = std::make_shared<Blend>();
            blend->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, blend });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(blend);
            tree.insertSkeletalFields({ sphere1, sphere2 });

            Bsoid soid(tree, "blend");
            soid.setResolution(std::get<0>(res),
                std::get<1>(res));
            return soid;
        }

        polygonizer::MarchingCubes makeMCBlend(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Blend;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(1.0f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-1.0f, 0, 0));
            ImplicitOperatorPtr blend = std::make_shared<Blend>();
            blend->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, blend });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(blend);
            tree.insertSkeletalFields({ sphere1, sphere2 });

            MarchingCubes mc(tree, "blend");
            mc.setResolution(std::get<0>(res));

            return mc;
        }

        polygonizer::Bsoid makeIntersection(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Intersection;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(0.5f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-0.5f, 0, 0));
            ImplicitOperatorPtr intersection = std::make_shared<Intersection>();
            intersection->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, intersection });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(intersection);
            tree.insertSkeletalFields({ sphere1, sphere2 });


            Bsoid soid(tree, "intersection");
            soid.setResolution(std::get<0>(res),
                std::get<1>(res));
            return soid;
        }

        polygonizer::MarchingCubes makeMCIntersection(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Intersection;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(0.5f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-0.5f, 0, 0));
            ImplicitOperatorPtr intersection = std::make_shared<Intersection>();
            intersection->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, intersection });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(intersection);
            tree.insertSkeletalFields({ sphere1, sphere2 });

            MarchingCubes mc(tree, "intersection");
            mc.setResolution(std::get<0>(res));

            return mc;
        }

        polygonizer::Bsoid makeUnion(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Union;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(0.5f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-0.5f, 0, 0));
            ImplicitOperatorPtr op = std::make_shared<Union>();
            op->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, op });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(op);
            tree.insertSkeletalFields({ sphere1, sphere2 });

            Bsoid soid(tree, "union");
            soid.setResolution(std::get<0>(res),
                std::get<1>(res));
            return soid;
        }

        polygonizer::MarchingCubes makeMCUnion(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Union;

            ImplicitFieldPtr sphere1 =
                std::make_shared<Sphere>(1.0f, Point(0.5f, 0, 0));
            ImplicitFieldPtr sphere2 =
                std::make_shared<Sphere>(1.0f, Point(-0.5f, 0, 0));
            ImplicitOperatorPtr op = std::make_shared<Union>();
            op->insertFields({ sphere1, sphere2 });

            BlobTree tree;
            tree.insertFields({ sphere1, sphere2, op });
            tree.insertNodeTree({ { -1 }, { -1 }, { 0, 1 } });
            tree.insertFieldTree(op);
            tree.insertSkeletalFields({ sphere1, sphere2 });

            MarchingCubes mc(tree, "union");
            mc.setResolution(std::get<0>(res));
            return mc;
        }

        polygonizer::Bsoid makeTransform(Resolution const& res)
        {
            using atlas::math::Matrix4;
            using atlas::math::Vector;
            using fields::Torus;
            using operators::Transform;

            ImplicitFieldPtr torus =
                std::make_shared<Torus>();
            ImplicitOperatorPtr op = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(0.0f, 0.0f, 3.0f)) * 
                glm::rotate(Matrix4(1.0f), glm::radians(90.0f), Vector(1, 0, 0)) * 
                glm::scale(Matrix4(1.0f), Vector(0.5f)) * 
                glm::scale(Matrix4(1.0f), Vector(2.5f, 1.0f, 1.0f)));
            op->insertField(torus);

            BlobTree tree;
            tree.insertFields({ op });
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(op);
            tree.insertSkeletalField(torus);

            Bsoid soid(tree, "transform");
            soid.setResolution(std::get<0>(res),
                std::get<1>(res));
            return soid;
        }

        polygonizer::MarchingCubes makeMCTransform(Resolution const& res)
        {
            using atlas::math::Matrix4;
            using atlas::math::Vector;
            using fields::Torus;
            using operators::Transform;

            ImplicitFieldPtr torus =
                std::make_shared<Torus>();
            ImplicitOperatorPtr op = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(0.0f, 0.0f, 3.0f)) * 
                glm::rotate(Matrix4(1.0f), glm::radians(90.0f), Vector(1, 0, 0)) * 
                glm::scale(Matrix4(1.0f), Vector(0.5f)) * 
                glm::scale(Matrix4(1.0f), Vector(2.5f, 1.0f, 1.0f)));
            op->insertField(torus);

            BlobTree tree;
            tree.insertFields({ op });
            tree.insertNodeTree({ { -1 } });
            tree.insertFieldTree(op);
            tree.insertSkeletalField(torus);

            MarchingCubes mc(tree, "transform");
            mc.setResolution(std::get<0>(res));
            return mc;
        }

        polygonizer::Bsoid makeButterfly(Resolution const& res)
        {
            using atlas::math::Matrix4;
            using atlas::math::Vector;

            using fields::Torus;
            using fields::Sphere;

            using operators::Blend;
            using operators::Transform;
            using operators::Union;

            // Base primitives.
            ImplicitFieldPtr sphere = std::make_shared<Sphere>();
            ImplicitFieldPtr torus = std::make_shared<Torus>();

            // Butterfly body.
            ImplicitOperatorPtr head = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(-1.0f, 0.0f, 0.0f)));
            ImplicitOperatorPtr torso = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(1.0f, 0.0f, 0.0f)) *
                glm::scale(Matrix4(1.0f), Vector(2.0f, 1.0f, 1.0f)));
            ImplicitOperatorPtr tail = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(3.0f, 0.0f, 0.0f)) *
                glm::scale(Matrix4(1.0f), Vector(4.0f, 1.0f, 1.0f)));

            head->insertField(sphere);
            torso->insertField(sphere);
            tail->insertField(sphere);

            ImplicitOperatorPtr body = std::make_shared<Union>();
            body->insertFields({ head, torso, tail });

            // Now make the wings.
            ImplicitOperatorPtr topWing = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(0.0f, 0.0f, 3.0f)) *
                glm::rotate(Matrix4(1.0f), glm::radians(90.0f), Vector(1, 0, 0)) * 
                glm::rotate(Matrix4(1.0f), glm::radians(45.0f), Vector(0, 0, -1)) * 
                glm::scale(Matrix4(1.0f), Vector(0.5f)) *
                glm::scale(Matrix4(1.0f), Vector(2.5f, 1.0f, 1.0f)));

            ImplicitOperatorPtr bottomWing = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(3.0f, 0.0f, 2.5f)) *
                glm::rotate(Matrix4(1.0f), glm::radians(-90.0f), Vector(1, 0, 0)) *
                glm::rotate(Matrix4(1.0f), glm::radians(-90.0f), Vector(0, 0, -1)) *
                glm::scale(Matrix4(1.0f), Vector(0.5f)) *
                glm::scale(Matrix4(1.0f), Vector(2.0f, 1.0f, 1.0f)));

            topWing->insertField(torus);
            bottomWing->insertField(torus);

            ImplicitOperatorPtr wingR = std::make_shared<Blend>();
            wingR->insertFields({ topWing, bottomWing });

            ImplicitOperatorPtr wingL = std::make_shared<Transform>(
                glm::rotate(Matrix4(1.0f), glm::radians(180.0f), Vector(1, 0, 0)));
            wingL->insertField(wingR);

            ImplicitOperatorPtr butterfly = std::make_shared<Union>();
            butterfly->insertFields({ body, wingR, wingL });

            BlobTree tree;
            tree.insertFields({ body, wingR, wingL, butterfly });
            tree.insertNodeTree({ { -1 }, { -1 }, { -1 }, { 0, 1, 2 } });
            tree.insertFieldTree(butterfly);
            tree.insertSkeletalFields({ sphere, torus });

            Bsoid soid(tree, "butterfly");
            soid.setResolution(std::get<0>(res),
                std::get<1>(res));
            return soid;
        }

        polygonizer::MarchingCubes makeMCButterfly(Resolution const& res)
        {
            using atlas::math::Matrix4;
            using atlas::math::Vector;

            using fields::Torus;
            using fields::Sphere;

            using operators::Blend;
            using operators::Transform;
            using operators::Union;

            // Base primitives.
            ImplicitFieldPtr sphere = std::make_shared<Sphere>();
            ImplicitFieldPtr torus = std::make_shared<Torus>();

            // Butterfly body.
            ImplicitOperatorPtr head = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(-1.0f, 0.0f, 0.0f)));
            ImplicitOperatorPtr torso = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(1.0f, 0.0f, 0.0f)) *
                glm::scale(Matrix4(1.0f), Vector(2.0f, 1.0f, 1.0f)));
            ImplicitOperatorPtr tail = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(3.0f, 0.0f, 0.0f)) *
                glm::scale(Matrix4(1.0f), Vector(4.0f, 1.0f, 1.0f)));

            head->insertField(sphere);
            torso->insertField(sphere);
            tail->insertField(sphere);

            ImplicitOperatorPtr body = std::make_shared<Union>();
            body->insertFields({ head, torso, tail });

            // Now make the wings.
            ImplicitOperatorPtr topWing = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(0.0f, 0.0f, 3.0f)) *
                glm::rotate(Matrix4(1.0f), glm::radians(90.0f), Vector(1, 0, 0)) * 
                glm::rotate(Matrix4(1.0f), glm::radians(45.0f), Vector(0, 0, -1)) * 
                glm::scale(Matrix4(1.0f), Vector(0.5f)) *
                glm::scale(Matrix4(1.0f), Vector(2.5f, 1.0f, 1.0f)));

            ImplicitOperatorPtr bottomWing = std::make_shared<Transform>(
                glm::translate(Matrix4(1.0f), Vector(3.0f, 0.0f, 2.5f)) *
                glm::rotate(Matrix4(1.0f), glm::radians(-90.0f), Vector(1, 0, 0)) *
                glm::rotate(Matrix4(1.0f), glm::radians(-90.0f), Vector(0, 0, -1)) *
                glm::scale(Matrix4(1.0f), Vector(0.5f)) *
                glm::scale(Matrix4(1.0f), Vector(2.0f, 1.0f, 1.0f)));

            topWing->insertField(torus);
            bottomWing->insertField(torus);

            ImplicitOperatorPtr wingR = std::make_shared<Blend>();
            wingR->insertFields({ topWing, bottomWing });

            ImplicitOperatorPtr wingL = std::make_shared<Transform>(
                glm::rotate(Matrix4(1.0f), glm::radians(180.0f), Vector(1, 0, 0)));
            wingL->insertField(wingR);

            ImplicitOperatorPtr butterfly = std::make_shared<Union>();
            butterfly->insertFields({ body, wingR, wingL });

            BlobTree tree;
            tree.insertFields({ body, wingR, wingL, butterfly });
            tree.insertNodeTree({ { -1 }, { -1 }, { -1 }, { 0, 1, 2 } });
            tree.insertFieldTree(butterfly);
            tree.insertSkeletalFields({ sphere, torus });

            MarchingCubes mc(tree, "butterfly");
            mc.setResolution(std::get<0>(res));
            return mc;
        }

        bsoid::polygonizer::Bsoid makeParticles(Resolution const& res)
        {
            using atlas::math::Matrix4;
            using atlas::math::Vector;

            using fields::Sphere;
            using operators::Blend;
            using atlas::math::RandomGenerator;

            auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

            RandomGenerator<float> random{ 2018 };
            static constexpr auto numParticles = 100;
            static constexpr auto maxVal = 10.0f;
            static constexpr auto minVal = -10.0f;
            std::vector<bsoid::fields::ImplicitFieldPtr> particles;
            std::vector<std::vector<int>> nodes;
            for (int i = 0; i < numParticles; ++i)
            {
                float x = random.getRandomReal(minVal, maxVal);
                float y = random.getRandomReal(minVal, maxVal);
                float z = random.getRandomReal(minVal, maxVal);

                particles.emplace_back(std::make_shared<Sphere>(1.0f,
                    Vector(x, y, z)));
                nodes.push_back({ -1 });
            }

            ImplicitOperatorPtr blend = std::make_shared<Blend>();
            blend->insertFields(particles);
            std::vector<int> parent(nodes.size());
            std::iota(parent.begin(), parent.end(), 0);
            nodes.push_back(parent);

            BlobTree tree;
            tree.insertSkeletalFields(particles);

            particles.push_back(blend);
            tree.insertFields(particles);
            tree.insertNodeTree(nodes);
            tree.insertFieldTree(blend);

            Bsoid soid(tree, "particles");
            soid.setResolution(std::get<0>(res), 
                std::get<1>(res));
            return soid;
        }

        bsoid::polygonizer::MarchingCubes makeMCParticles(Resolution const& res)
        {
            using atlas::math::Matrix4;
            using atlas::math::Vector;

            using fields::Sphere;
            using operators::Blend;
            using atlas::math::RandomGenerator;

//            auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            RandomGenerator<float> random{ 2018 };
            static constexpr auto numParticles = 100;
            static constexpr auto maxVal = 10.0f;
            static constexpr auto minVal = -10.0f;
            std::vector<bsoid::fields::ImplicitFieldPtr> particles;
            std::vector<std::vector<int>> nodes;
            for (int i = 0; i < numParticles; ++i)
            {
                float x = random.getRandomReal(minVal, maxVal);
                float y = random.getRandomReal(minVal, maxVal);
                float z = random.getRandomReal(minVal, maxVal);

                particles.emplace_back(std::make_shared<Sphere>(1.0f,
                    Vector(x, y, z)));
                nodes.push_back({ -1 });
                printf("%d: %f%f%f\n",i,x,y,z);
            }

            ImplicitOperatorPtr blend = std::make_shared<Blend>();
            blend->insertFields(particles);
            std::vector<int> parent(nodes.size());
            std::iota(parent.begin(), parent.end(), 0);
            nodes.push_back(parent);

            BlobTree tree;
            tree.insertSkeletalFields(particles);

            particles.push_back(blend);
            tree.insertFields(particles);
            tree.insertNodeTree(nodes);
            tree.insertFieldTree(blend);

            MarchingCubes mc(tree, "particles");
            mc.setResolution(std::get<0>(res));
            return mc;
        }

        bsoid::polygonizer::Bsoid makeChain(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Blend;

            static constexpr auto chainLength = 20;

            std::vector<ImplicitFieldPtr> chain;
            std::vector<std::vector<int>> nodes;
            for (int i = 0; i < chainLength; ++i)
            {
                chain.emplace_back(std::make_shared<Sphere>(1.0f,
                    Point(-10.0f + 2.0f * i, 0, 0)));

                nodes.push_back({ -1 });
            }

            ImplicitOperatorPtr blend = std::make_shared<Blend>();
            blend->insertFields(chain);

            std::vector<int> roots(chainLength);
            std::iota(roots.begin(), roots.end(), 0);
            nodes.push_back(roots);

            BlobTree tree;
            tree.insertSkeletalFields(chain);

            chain.push_back(blend);
            tree.insertFields(chain);
            tree.insertNodeTree(nodes);
            tree.insertFieldTree(blend);

            Bsoid soid(tree, "chain");
            soid.setResolution(std::get<0>(res),
                std::get<1>(res));
            return soid;
        }

        bsoid::polygonizer::MarchingCubes makeMCChain(Resolution const& res)
        {
            using atlas::math::Point;
            using fields::Sphere;
            using operators::Blend;

            static constexpr auto chainLength = 20;

            std::vector<ImplicitFieldPtr> chain;
            std::vector<std::vector<int>> nodes;
            for (int i = 0; i < chainLength; ++i)
            {
                chain.emplace_back(std::make_shared<Sphere>(1.0f,
                    Point(-10.0f + 2.0f * i, 0, 0)));

                nodes.push_back({ -1 });
            }

            ImplicitOperatorPtr blend = std::make_shared<Blend>();
            blend->insertFields(chain);

            std::vector<int> roots(chainLength);
            std::iota(roots.begin(), roots.end(), 0);
            nodes.push_back(roots);

            BlobTree tree;
            tree.insertSkeletalFields(chain);

            chain.push_back(blend);
            tree.insertFields(chain);
            tree.insertNodeTree(nodes);
            tree.insertFieldTree(blend);


            MarchingCubes mc(tree, "chain");
            mc.setResolution(std::get<0>(res));
            return mc;
        }
    }
}
