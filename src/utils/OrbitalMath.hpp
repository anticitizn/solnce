#pragma once

#include <string>
#include <iostream>
#include <algorithm>

#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/MassiveBody.hpp>

#define G 6.674e-11

double Wrap2Pi(double x)
{
    x = std::fmod(x, 2.0 * M_PI);
    if (x < 0)
    {
        x += 2.0 * M_PI;
    } 

    return x;
}

Transform GetTransform(const OrbitComponent& orbit, const Transform& parentTransform, double parentMass)
{
    double mu = parentMass * G;

    // 1) Calculate the position in the plane
    double x_pf = orbit.r * std::cos(orbit.ta);
    double y_pf = orbit.r * std::sin(orbit.ta);

    double p = orbit.rp * (1 + orbit.e);

    // 2) Calculate cartesian velocity
    double factor = std::sqrt(mu / p);

    double vx_pf = -factor * std::sin(orbit.ta);
    double vy_pf =  factor * (orbit.e + std::cos(orbit.ta));

    // 3) Rotate position and velocity by argument of periapsis
    double cw = std::cos(orbit.ap);
    double sw = std::sin(orbit.ap);

    double x = x_pf * cw - y_pf * sw;
    double y = x_pf * sw + y_pf * cw;

    double vx = vx_pf * cw - vy_pf * sw;
    double vy = vx_pf * sw + vy_pf * cw;

    // 4) Apply parent-body transform
    Transform transform {{0.0f, 0.0f, 0.0f}, {0,0}, 0 };

    transform.position.x = parentTransform.position.x + x;
    transform.position.y = parentTransform.position.y + y;

    transform.velocity.x = parentTransform.velocity.x + vx;
    transform.velocity.y = parentTransform.velocity.y + vy;

    return transform;
}

double TauFromTrueAnomaly(double mu, double p, double e, double a, double t0, double nu0)
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

void PropagateOrbit(OrbitComponent& o, double t0, double dt)
{
    // compute ta at t1
    double t1 = t0 + dt;
    o.ta = TrueAnomalyAtTime(o.mu, o.p, o.e, o.a, t1, o.t_periapsis);

    // radius at t1
    o.r = o.p / (1.0 + o.e * std::cos(o.ta));
}

