
#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <random>

#include <glm/gtc/constants.hpp>

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

            double t0 = time.sim_time / 1000.0;
            double dt = time.sim_dt / 1000.0;

            double& parentMass = coordinator.GetComponent<MassiveBody>(orbit.parentBodyId).mass;
            Transform& parentTransform = coordinator.GetComponent<Transform>(orbit.parentBodyId);

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
        std::mt19937 rng(1337);

        auto randd = [&](double minv, double maxv) -> double
        {
            std::uniform_real_distribution<double> dist(minv, maxv);
            return dist(rng);
        };

        auto randf = [&](float minv, float maxv) -> float
        {
            std::uniform_real_distribution<float> dist(minv, maxv);
            return dist(rng);
        };

        auto randBool = [&](double p = 0.5) -> bool
        {
            std::bernoulli_distribution dist(p);
            return dist(rng);
        };

        // -------------------------------------------------------------------------
        // Star
        // -------------------------------------------------------------------------
        Entity star = coordinator.CreateEntity();
        star.Assign<MassiveBody>(MassiveBody{ 1.989e30 });
        star.Assign<Transform>(Transform{ {0.0f, 0.0f}, {0,0}, 0 });
        star.Assign<Quad>(Quad{80, 80, 1.0, 0.9, 0.4});
        star.Assign<Texture>(Texture{"sun.png", 0});
        star.Assign<Polyline>(Polyline{ {}, {{1,1,1,1}, 2.0f, 1.0f, 0, 0} });

        auto createOrbiter = [&](Entity parent,
                                double mass,
                                double rp,
                                double e,
                                double ap,
                                double ta,
                                float quadW,
                                float quadH,
                                float r,
                                float g,
                                float b,
                                const std::string& texture) -> Entity
        {
            Entity body = coordinator.CreateEntity();
            body.Assign<Transform>(Transform{ {0.0f, 0.0f}, {0,0}, 0 });
            body.Assign<Quad>(Quad{quadW, quadH, r, g, b});
            body.Assign<MassiveBody>(MassiveBody{ mass });
            body.Assign<OrbitComponent>(OrbitComponent{
                parent.GetId(),
                rp,   // periapsis distance
                e,    // eccentricity
                ap,   // argument of periapsis
                ta,   // true anomaly
                0.0,  // a (derived)
                0.0   // r (derived)
            });
            body.Assign<Texture>(Texture{texture, 0});
            body.Assign<Polyline>(Polyline{ {}, {{1,1,1,1}, 1.5f, 0.8f, 0, 0} });

            auto& parentMassiveBody = parent.GetComponent<MassiveBody>();
            parentMassiveBody.childrenIds.push_back(body.GetId());

            return body;
        };

        // -------------------------------------------------------------------------
        // 8 closer-packed planets
        // Distances are intentionally compressed compared to a real solar system for now
        // -------------------------------------------------------------------------
        struct PlanetDef
        {
            double rpAU;
            double e;
            double mass;
            float  size;
            float  cr, cg, cb;
        };

        const PlanetDef planets[6] =
        {
            {0.18, 0.020, 0.33e24, 12.0f, 0.70f, 0.70f, 0.75f},
            {0.28, 0.015, 0.81e24, 14.0f, 0.90f, 0.75f, 0.45f},
            {0.40, 0.018, 1.00e29, 16.0f, 0.35f, 0.60f, 1.00f},
            {0.55, 0.025, 0.11e24, 13.0f, 0.95f, 0.40f, 0.30f},
            {1.3, 0.030, 8.00e24, 24.0f, 0.90f, 0.80f, 0.55f},
            {1.7, 0.020, 6.00e24, 22.0f, 0.65f, 0.75f, 0.95f},
        };

        for (int i = 0; i < 6; ++i)
        {
            const auto& p = planets[i];

            Entity planet = createOrbiter(
                star,
                p.mass,
                p.rpAU * AU,
                p.e,
                randd(0.0, glm::two_pi<double>()),
                randd(0.0, glm::two_pi<double>()),
                p.size,
                p.size,
                p.cr, p.cg, p.cb,
                "planet.png"
            );

            const int moonCount = randBool(0.55) ? 2 : 1;

            // Approximate semi-major axis of the planet around the star
            const double planetA = (p.rpAU * AU) / (1.0 - p.e);

            // Hill radius r_H ~= a * (m / (3M))^(1/3)
            const double starMass   = 1.989e30;
            const double hillRadius = planetA * std::cbrt(p.mass / (3.0 * starMass));

            // Keep moons comfortably inside the Hill sphere (<= ~0.25-0.35 r_H)
            const double safeOuterMoonRp = hillRadius * 0.18;
            const double safeInnerMoonRp = hillRadius * 0.06;

            for (int m = 0; m < moonCount; ++m)
            {
                double moonRp;
                if (m == 0)
                {
                    moonRp = randd(safeInnerMoonRp, hillRadius * 0.12);
                }
                else
                {
                    moonRp = randd(hillRadius * 0.12, safeOuterMoonRp);
                }

                const double moonMass =
                    (m == 0) ? randd(2.0e21, 9.0e22)
                            : randd(5.0e20, 2.5e22);

                const float moonSize =
                    (m == 0) ? randf(5.0f, 9.0f)
                            : randf(3.0f, 6.0f);

                createOrbiter(
                    planet,
                    moonMass,
                    moonRp,
                    randd(0.0, 0.03), // keep moon eccentricity low
                    randd(0.0, glm::two_pi<double>()),
                    randd(0.0, glm::two_pi<double>()),
                    moonSize,
                    moonSize,
                    randf(0.65f, 0.95f),
                    randf(0.65f, 0.95f),
                    randf(0.65f, 0.95f),
                    "moon.png"
                );
            }
        }

        // -------------------------------------------------------------------------
        // Asteroid belt between 4th and 5th planets
        // -------------------------------------------------------------------------
        for (int i = 0; i < 36; ++i)
        {
            const double rpAU = randd(0.60, 0.68);
            const double e    = randd(0.02, 0.10);
            const float  size = randf(4.0f, 8.0f);

            Entity asteroid = createOrbiter(
                star,
                randd(5.0e13, 8.0e16),
                rpAU * AU,
                e,
                randd(glm::radians(5.0), glm::radians(35.0)),
                randd(0.0, glm::two_pi<double>()),
                size,
                size,
                0.55f, 0.55f, 0.58f,
                "asteroid.png"
            );

            asteroid.GetComponent<Polyline>() =
                Polyline{ {}, {{0.35f, 0.35f, 0.35f, 1.0f}, 1.2f, 0.6f, 0, 0} };
        }

        // -------------------------------------------------------------------------
        // Highly elliptical bodies
        // -------------------------------------------------------------------------
        for (int i = 0; i < 3; ++i)
        {
            const double rpAU = randd(0.55, 0.95);
            const double e    = randd(0.58, 0.82);
            const float  size = randf(6.0f, 11.0f);

            Entity asteroid = createOrbiter(
                star,
                randd(1.0e14, 5.0e16),
                rpAU * AU,
                e,
                randd(0.0, glm::two_pi<double>()),
                randd(0.0, glm::two_pi<double>()),
                size,
                size,
                1.0f, 0.35f, 0.35f,
                "asteroid.png"
            );

            asteroid.GetComponent<Polyline>() =
                Polyline{ {}, {{0.3f, 0.3f, 0.3f, 1.0f}, 1.2f, 0.6f, 0, 0} };
        }
    }

private:

    // Update the derived parameters of an orbit component
    // They should change only upon SOI switch or during propulsive events
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
