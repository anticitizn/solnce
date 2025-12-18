#pragma once

#include <src/ecs/System.hpp>
#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/Polyline.hpp>
#include <src/components/MassiveBody.hpp>

extern Coordinator coordinator;

class OrbitPathSystem : public System
{
public:
    void Update()
    {
        for (const auto& entity : entities)
        {
            auto& orbit = coordinator.GetComponent<OrbitComponent>(entity);
            auto& line  = coordinator.GetComponent<Polyline>(entity);

            // Parent position
            auto& parentTf = coordinator.GetComponent<Transform>(orbit.parentBodyId);

            // Rebuild the polyline
            BuildEllipse(line, orbit, parentTf);
        }
    }

private:
    void BuildEllipse(Polyline& line, const OrbitComponent& orbit, const Transform& parentTf)
    {
        const int sampleCount = 200;
        line.segments.clear();
        line.segments.reserve(sampleCount);

        double a  = orbit.a;
        double e  = orbit.e;
        double ap = orbit.ap;

        float cw = std::cos(ap);
        float sw = std::sin(ap);

        for (int i = 0; i <= sampleCount; i++)
        {
            double nu = (double(i) / sampleCount) * 2.0 * M_PI;

            // radius
            double r = a * (1 - e*e) / (1 + e * std::cos(nu));

            // perifocal position
            double xpf = r * std::cos(nu);
            double ypf = r * std::sin(nu);

            // ap rotation
            double x = xpf * cw - ypf * sw;
            double y = xpf * sw + ypf * cw;

            // transform to world space
            glm::vec2 p = {parentTf.position.x + float(x), parentTf.position.y + float(y)};

            line.segments.push_back(p);
        }
    }
};
