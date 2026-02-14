#pragma once

#include <src/ecs/System.hpp>
#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/Polyline.hpp>
#include <src/components/MassiveBody.hpp>
#include <src/utils/OrbitalMath.hpp>

extern Coordinator coordinator;

struct TaRange
{
    // Is any portion of the orbit inside the SOI? This should always be true, but you never know
    bool valid = false;   
    double nu_min = 0.0;
    double nu_max = 0.0;
};

class OrbitPathSystem : public System
{
public:
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<OrbitComponent, Transform, Polyline>();
    }

    void Update()
    {
        for (auto e : entities)
        {
            auto& orbit = coordinator.GetComponent<OrbitComponent>(e);
            auto& line = coordinator.GetComponent<Polyline>(e);

            auto& parentTf = coordinator.GetComponent<Transform>(orbit.parentBodyId);
            auto& parentMassiveBody = coordinator.GetComponent<MassiveBody>(orbit.parentBodyId);
            
            TaRange range = TrueAnomalyRangeInsideRadius(orbit.rp, orbit.e, parentMassiveBody.soi);

            if (range.valid)
            {
                SampleOrbitPositionsTa(line.segments, orbit, parentTf, 2000, range.nu_min, range.nu_max);
            }
            else
            {
                // If something goes wrong, don't draw anything
                line.segments.clear();
            }
        }
    }


private:
    inline void SampleOrbitPositionsTa(
        std::vector<glm::dvec2>& out,
        const OrbitComponent& orbitIn,
        const Transform& parentTf,
        int sampleCount,
        double nuStart = 0.0,
        double nuEnd   = 2.0 * M_PI
    )
    {
        out.clear();
        out.reserve(sampleCount + 1);

        OrbitComponent o = orbitIn;

        const double p  = (o.p != 0.0) ? o.p : (o.rp * (1.0 + o.e));

        for (int i = 0; i <= sampleCount; ++i)
        {
            double u = double(i) / double(sampleCount);
            double nu = nuStart + (nuEnd - nuStart) * u;

            o.ta = nu;
            o.r  = RadiusFromTrueAnomaly(p, o.e, nu);

            Transform tf = GetTransform(o, parentTf);
            out.push_back(glm::dvec2(tf.position.x, tf.position.y));
        }
    }

    // Returns range in true anomaly (nu) where r(nu) <= R
    // Assumes periapsis at nu=0
    inline TaRange TrueAnomalyRangeInsideRadius(double rp, double e, double R)
    {
        TaRange out;

        // Sanity checks
        if (R <= 0.0 || rp > R) 
        {
            return out;
        }

        // Near-circular: r is approximately constant; if rp<=R then whole orbit is inside SOI
        if (std::abs(e) < 1e-12)
        {
            out.valid  = true;
            out.nu_min = 0.0;
            out.nu_max = 2.0 * M_PI;
            return out;
        }

        const double p = rp * (1.0 + e);

        // Solve cos(nu) >= k
        double k = ((p / R) - 1.0) / e;

        if (k >= 1.0)
        {
            // Only touches SOI at periapsis when k==1
            // Treat as degenerate range
            out.valid  = true;
            out.nu_min = 0.0;
            out.nu_max = 0.0;
            return out;
        }

        if (k <= -1.0)
        {
            // Orbit inside SOI for entire physically valid branch
            out.valid = true;

            if (e < 1.0)
            {
                out.nu_min = -M_PI;
                out.nu_max =  M_PI;
            }
            else
            {
                // hyperbola/parabola: physically valid branch is limited
                // parabola (e==1): nu in (-pi, pi)
                // hyperbola (e>1): nu in (-nu_inf, nu_inf)
                if (std::abs(e - 1.0) < 1e-12)
                {
                    out.nu_min = -M_PI;
                    out.nu_max =  M_PI;
                }
                else
                {
                    double nu_inf = std::acos(-1.0 / e);
                    out.nu_min = -nu_inf;
                    out.nu_max =  nu_inf;
                }
            }
            return out;
        }

        // Normal case: bounded by arccos(k)
        double nu_max = std::acos(std::clamp(k, -1.0, 1.0));

        // Clamp to physical branch for hyperbolas/parabolas
        if (e > 1.0)
        {
            double nu_inf = std::acos(-1.0 / e);
            const double eps = 1e-9;
            nu_max = std::min(nu_max, nu_inf - eps);
        }
        else if (std::abs(e - 1.0) < 1e-12)
        {
            // parabola: nu in (-pi, pi)
            const double eps = 1e-9;
            nu_max = std::min(nu_max, M_PI - eps);
        }

        out.valid  = true;
        out.nu_min = -nu_max;
        out.nu_max =  nu_max;
        return out;
    }

};