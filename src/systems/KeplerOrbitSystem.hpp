
#pragma once

#include <string>
#include <iostream>
#include <algorithm>

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

            double t0 = time.sim_time;
            double dt = time.sim_dt;
            double t1 = t0 + dt;

            // 1) Orbital parameters
            double ecc = orbit.e;
            double rp = orbit.rp;
            double parentMass = coordinator.GetComponent<MassiveBody>(orbit.parentBodyId).mass;
            double mu = parentMass * G;

            double a = rp / (1 - ecc);
            orbit.a = a;

            // 2) Compute p (semilatus rectum, a.k.a. distance from the focus to the orbit at ta = +/- 90 degrees)
            double p = rp * (1 + ecc);

            // 3) Get tau (time since periapsis) from current ta
            double tau = TauFromTrueAnomaly(mu, p, ecc, a, t0, orbit.ta);

            // 4) Propagate true anomaly to t1
            double nu = TrueAnomalyAtTime(mu, p, ecc, a, t1, tau);
            orbit.ta = nu;

            // 5) Calculate the orbital radius
            double r = p / (1.0 + ecc * std::cos(nu));
            orbit.r = r;

            // 6) Calculate the position in the plane
            double x_pf = r * std::cos(nu);
            double y_pf = r * std::sin(nu);

            // 7) Calculate cartesian velocity
            double factor = std::sqrt(mu / p);

            double vx_pf = -factor * std::sin(nu);
            double vy_pf =  factor * (ecc + std::cos(nu));

            // 8) Rotate position and velocity by argument of periapsis
            double cw = std::cos(orbit.ap);
            double sw = std::sin(orbit.ap);

            double x = x_pf * cw - y_pf * sw;
            double y = x_pf * sw + y_pf * cw;

            double vx = vx_pf * cw - vy_pf * sw;
            double vy = vx_pf * sw + vy_pf * cw;

            // 9) Apply parent-body transform
            auto& parentTf = coordinator.GetComponent<Transform>(orbit.parentBodyId);

            transform.position.x = parentTf.position.x + x;
            transform.position.y = parentTf.position.y + y;

            transform.velocity.x = parentTf.velocity.x + vx;
            transform.velocity.y = parentTf.velocity.y + vy;
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
    }

private:

    static double TauFromTrueAnomaly(double mu, double p, double e, double a, double t0, double nu0)
    {
        const double eps = 1e-12;

        if (std::abs(e - 1.0) < eps)
        {
            // Parabola: dt = sqrt(p^3/mu) * (D + D^3/3), D = tan(nu/2)
            double D = std::tan(nu0 * 0.5);
            double dt_since = std::sqrt((p*p*p) / mu) * (D + (D*D*D)/3.0);
            return t0 - dt_since;
        }

        if (e < 1.0)
        {
            // Ellipse: M0 from E0, dt = M0 / n
            double n = std::sqrt(mu / (a*a*a));

            double sinE = (std::sqrt(1.0 - e*e) * std::sin(nu0)) / (1.0 + e*std::cos(nu0));
            double cosE = (e + std::cos(nu0)) / (1.0 + e*std::cos(nu0));
            double E0 = std::atan2(sinE, cosE);

            double M0 = Wrap2Pi(E0 - e * std::sin(E0));
            double dt_since = M0 / n;
            return t0 - dt_since;
        }
        else
        {
            // Hyperbola: M0 from H0, dt = M0 / n, n = sqrt(mu/(-a)^3)
            double abs_a = std::abs(a);
            double n = std::sqrt(mu / (abs_a*abs_a*abs_a));

            double tanhH2 = std::sqrt((e - 1.0)/(e + 1.0)) * std::tan(nu0 * 0.5);
            tanhH2 = std::clamp(tanhH2, -0.999999999999, 0.999999999999);
            double H0 = 2.0 * std::atanh(tanhH2);

            double M0 = e * std::sinh(H0) - H0;
            double dt_since = M0 / n;
            return t0 - dt_since;
        }
    }

    // Find out the true anomaly from tau (time since periapsis)
    // by applying Newton-Raphson with 10 iterations
    double TrueAnomalyAtTime(double mu, double p, double e, double a, double t, double tau)
    {
        double dt = t - tau;

        const double eps = 1e-9;
        if (std::abs(e - 1.0) < eps)
        {
            // For parabolic trajectories, we use Barker's equation
            // to find the parabolic anomaly D, and then use it for the true anomaly
            
            // Initial guess
            double B = std::sqrt(mu / (p*p*p)) * dt;

            double D = B;
            for (int i = 0; i < 10; ++i)
            {
                double f = D + pow(D, 3) / 3 - B;
                double fp = 1 + pow(D, 2);

                D -= f / fp;
            }

            double nu = 2.0 * atan(D);
            
            return nu;
        }
        else if (e < 1.0)
        {
            // For elliptical orbits, we first find the eccentric anomaly E
            // using Newton-Raphson, and then use it to find the true anomaly
            double n = std::sqrt(mu / (a*a*a));
            double M = n * dt;
            M = Wrap2Pi(M);
            
            // Initial guess
            double E = M;

            for (int i = 0; i < 10; ++i)
            {
                double f  = E - e * std::sin(E) - M;
                double fp = 1 - e * std::cos(E);
                E -= f / fp;
            }
            
            // Convert eccentric anomaly -> true anomaly
            double cosNu = (std::cos(E) - e) / (1 - e * std::cos(E));
            double sinNu = std::sqrt(1 - e*e) * std::sin(E) / (1 - e * std::cos(E));

            double nu = std::atan2(sinNu, cosNu);

            return nu;
        }
        else
        {
            // For hyperbolic trajectories, we use the hyperbolic Kepler's equation
            double abs_a = std::abs(a);
            double n = std::sqrt(mu / (abs_a*abs_a*abs_a));

            double M = n * dt;

            // Initial guess
            double H = std::asinh(M / e);

            for (int i = 0; i < 10; ++i)
            {
                double f = e * std::sinh(H) - H - M;
                double fp = e * std::cosh(H) - 1;
                H -= f / fp;
            }
            double nu = 2.0 * atan(std::sqrt((e + 1) / (e-1)) * std::tanh(H/2));

            return nu;
        }
        
    }

    static double Wrap2Pi(double x)
    {
        x = std::fmod(x, 2.0 * M_PI);
        if (x < 0)
        {
            x += 2.0 * M_PI;
        } 

        return x;
    }

};
