
#pragma once

#include <string>
#include <iostream>
#include <algorithm>

#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/MassiveBody.hpp>
#include <src/resources/SimulationTime.hpp>
#include <src/utils/OrbitalMath.hpp>

#include <src/ecs/System.hpp>

extern Coordinator coordinator;

class KeplerOrbitSystem : public System
{
public:
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<OrbitComponent, Transform>();
    }

    void Update()
    {
        for (auto e : entities)
        {
            auto& orbit = coordinator.GetComponent<OrbitComponent>(e);
            auto& transform = coordinator.GetComponent<Transform>(e);
            auto& time  = coordinator.GetResource<SimulationTime>();

            double t0 = time.sim_time;
            double dt = time.sim_dt;

            double parentMass = coordinator.GetComponent<MassiveBody>(orbit.parentBodyId).mass;
            Transform parentTransform = coordinator.GetComponent<Transform>(orbit.parentBodyId);

            if (orbit.dirty)
            {
                RebuildOrbitCache(orbit, parentMass, t0);
            }

            PropagateOrbit(orbit, t0, dt);
            transform = GetTransform(orbit, parentTransform);
        }
    }


    void AddTestBodies()
    {
        // Star
        Entity star = coordinator.CreateEntity();
        star.Assign<MassiveBody>(MassiveBody{ 1.0e20 });
        star.Assign<Transform>(Transform{ {0.0f, 0.0f, 0.0f}, {0,0}, 0 });
        star.Assign<Quad>(Quad{80, 80, 0.4, 0.6, 1.0});
        star.Assign<Texture>(Texture{"sun.png", 0});
        star.Assign<Polyline>(Polyline{ {}, {{1,1,1,1}, 2.0f, 1.0f, 0, 0} });

        // Planet
        Entity planet = coordinator.CreateEntity();
        planet.Assign<Transform>(Transform{ {200.0f, 0.0f, 0.0f}, {0,0}, 0 });
        planet.Assign<Quad>(Quad{20, 20, 0.4, 0.6, 1.0});
        planet.Assign<MassiveBody>(MassiveBody{ 5.0e15 });
        planet.Assign<OrbitComponent>(OrbitComponent{
            star.GetId(),   // parent
            200.0,          // pr
            0.0,            // e
            0.0,            // ap
            0.0,            // ta
            0.0,            // a
            0.0,            // r
        });
        planet.Assign<Texture>(Texture{"planet.png", 0});
        planet.Assign<Polyline>(Polyline{ {}, {{1,1,1,1}, 2.0f, 1.0f, 0, 0} });

        // Moon
        Entity moon = coordinator.CreateEntity();
        moon.Assign<Transform>(Transform{ {240.0f, 0.0f, 0.0f}, {0,0}, 0 });
        moon.Assign<Quad>(Quad{10, 10, 0.8, 0.8, 0.8});
        moon.Assign<MassiveBody>(MassiveBody{5.0e10});
        moon.Assign<OrbitComponent>(OrbitComponent{
            planet.GetId(),
            20.0,                // pr
            0.3,                 // e
            glm::radians(30.0f), // ap
            0.0,
            0.0,
            0.0,
        });
        moon.Assign<Texture>(Texture{"moon.png", 0});
        moon.Assign<Polyline>(Polyline{ {}, {{1,1,1,1}, 2.0f, 1.0f, 0, 0} });

        // Elliptical planet
        Entity ellipse = coordinator.CreateEntity();
        ellipse.Assign<Transform>(Transform{ {300.0f, 0.0f, 0.0f}, {0,0}, 0 });
        ellipse.Assign<Quad>(Quad{15, 15, 1.0, 0.3, 0.3});
        ellipse.Assign<MassiveBody>(MassiveBody{ 5.0e15 });
        ellipse.Assign<OrbitComponent>(OrbitComponent{
            star.GetId(),
            300.0,                // pr
            0.6,                  // e
            glm::radians(45.0f),  // ap
            0.0,
            0.0,
            0.0,
        });
        ellipse.Assign<Texture>(Texture{"asteroid.png", 0});
        ellipse.Assign<Polyline>(Polyline{ {}, {{1,1,1,1}, 2.0f, 1.0f, 0, 0} });

        auto& starMassiveBody = star.GetComponent<MassiveBody>();
        starMassiveBody.childrenIds.push_back(planet.GetId());
        starMassiveBody.childrenIds.push_back(ellipse.GetId());

        auto& planetMassiveBody = planet.GetComponent<MassiveBody>();
        planetMassiveBody.childrenIds.push_back(moon.GetId());
    }

private:
    inline void RebuildOrbitCache(OrbitComponent& orbit, double parentMass, double t_now)
    {
        orbit.mu = parentMass * G;
        orbit.p  = orbit.rp * (1.0 + orbit.e);

        if (orbit.e < 1.0)
        {
            // Ellipse
            orbit.a = orbit.rp / (1.0 - orbit.e);
        }
        else if (orbit.e > 1.0)
        {
            // Hyperbola
            orbit.a = -(orbit.rp / (orbit.e - 1.0));
        }
        else
        {
            // Parabola
            orbit.a = std::numeric_limits<double>::infinity();
        }

        orbit.t_periapsis = TauFromTrueAnomaly(orbit.mu, orbit.p, orbit.e, orbit.a, t_now, orbit.ta);
        orbit.dirty = false;
    }

};
