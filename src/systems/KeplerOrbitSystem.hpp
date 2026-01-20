
#pragma once

#include <string>
#include <iostream>

#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/MassiveBody.hpp>
#include <src/resources/SimulationTime.hpp>

#include <src/ecs/System.hpp>

#define G 6.674e-11

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

            double dt = time.sim_dt;

            // 1) orbital parameters
            double a  = orbit.a;
            double ecc = orbit.e;
            double parentMass = coordinator.GetComponent<MassiveBody>(orbit.parentBodyId).mass;
            double mu = parentMass * G;

            // 2) Compute the mean motion
            double n = std::sqrt(mu / (a * a * a));

            // 3) Convert current true anomaly into eccentric anomaly
            double cosE = (ecc + std::cos(orbit.ta)) / (1 + ecc * std::cos(orbit.ta));
            double sinE = std::sqrt(1 - ecc*ecc) * std::sin(orbit.ta) / (1 + ecc * std::cos(orbit.ta));
            double E = std::atan2(sinE, cosE);

            // 4) Compute mean anomaly
            double M = E - ecc * std::sin(E);

            // 5) Advance mean anomaly
            M += n * dt;
            M = std::fmod(M, 2.0 * M_PI);  // normalize just in case

            // 6) Solve Kepler’s equation | M = E - e sin(E)
            E = SolveKepler(M, ecc);  

            // 7) Convert eccentric anomaly -> true anomaly
            double cosNu = (std::cos(E) - ecc) / (1 - ecc * std::cos(E));
            double sinNu = std::sqrt(1 - ecc*ecc) * std::sin(E) / (1 - ecc * std::cos(E));

            double nu = std::atan2(sinNu, cosNu);

            orbit.ta = std::atan2(sinNu, cosNu);
            orbit.E  = E;
            orbit.M  = M;

            // 8) Calculate the orbital radius
            double r = a * (1 - ecc * ecc) / (1 + ecc * cosNu);
            orbit.r = r;

            // 9) Calculate the position in the plane
            double x_pf = r * std::cos(nu);
            double y_pf = r * std::sin(nu);

            // 10) Rotate by argument of periapsis
            double cw = std::cos(orbit.ap);
            double sw = std::sin(orbit.ap);

            double x = x_pf * cw - y_pf * sw;
            double y = x_pf * sw + y_pf * cw;

            // 11) Apply parent-body position
            auto& parentTf = coordinator.GetComponent<Transform>(orbit.parentBodyId);

            transform.position.x = parentTf.position.x + x;
            transform.position.y = parentTf.position.y + y;
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
            200.0,          // a
            0.0,            // e
            0.0,            // ap
            0.0,            // ta
            0.0,            // E
            0.0,            // M
            0.0             // r
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
            20.0,                // a
            0.3,                 // e
            glm::radians(30.0f), // ap
            0.0,
            0.0,
            0.0,
            0.0
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
            300.0,                // a
            0.6,                  // e
            glm::radians(45.0f),  // ap
            0.0,
            0.0,
            0.0,
            0.0
        });
        ellipse.Assign<Texture>(Texture{"asteroid.png", 0});
        ellipse.Assign<Polyline>(Polyline{ {}, {{1,1,1,1}, 2.0f, 1.0f, 0, 0} });
    }

private:

    // Find out the eccentric anomaly from the mean anomaly
    // by applying Newton-Raphson with 10 iterations
    double SolveKepler(double M, double e)
    {
        double E = M;  // only works well for low eccentricity

        for (int i = 0; i < 10; ++i)
        {
            double f  = E - e * std::sin(E) - M;
            double fp = 1 - e * std::cos(E);
            E = E - f / fp;
        }
        return E;
    }



};


