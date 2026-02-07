
#pragma once

#include <string>
#include <iostream>
#include <algorithm>

#include <src/components/OrbitComponent.hpp>
#include <src/components/MassiveBody.hpp>
#include <src/components/Transform.hpp>
#include <src/resources/SimulationTime.hpp>

#include <src/ecs/System.hpp>

#define G 6.674e-11

extern Coordinator coordinator;

class SOISwitchingSystem : public System
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
            auto& orbitComponent = coordinator.GetComponent<OrbitComponent>(e);
            auto& tf = coordinator.GetComponent<Transform>(e);

            if ( !(coordinator.HasComponent<OrbitComponent>(orbitComponent.parentBodyId) 
                   && coordinator.HasComponent<MassiveBody>(orbitComponent.parentBodyId)) )
            {
                continue;
            }

            auto& parentMassiveBodyComponent = coordinator.GetComponent<MassiveBody>(orbitComponent.parentBodyId);
            auto& parentOrbitComponent = coordinator.GetComponent<OrbitComponent>(orbitComponent.parentBodyId);

            bool parentSwitched = false;

            // First, check if entity is about to enter the SOI of one of the bodies orbiting its current primary body
            for (auto childId : parentMassiveBodyComponent.childrenIds)
            {
                // The entity obviously shouldn't orbit itself
                if (childId == e)
                {
                    continue;
                }
                
                auto& childTf = coordinator.GetComponent<Transform>(childId);

                double distance = std::sqrt( std::pow(childTf.position.x - tf.position.x, 2) + std::pow(childTf.position.y - tf.position.y, 2) );

                double soi = CalculateSOI(childId);
                if (distance < soi * 0.999)
                {
                    orbitComponent.parentBodyId = childId;
                    
                    auto& childMassiveBody = coordinator.GetComponent<MassiveBody>(childId);
                    childMassiveBody.childrenIds.push_back(e);

                    // remove entity from old parent's list
                    auto& v = parentMassiveBodyComponent.childrenIds;
                    v.erase(std::remove(v.begin(), v.end(), e), v.end());

                    RecalculateOrbitalParameters(e);
                    
                    parentSwitched = true;
                    break;
                }
            }

            if (parentSwitched == true)
            {
                orbitComponent.dirty = true;
                continue;
            }

            // If the body is not entering a child SOI, then check if it is exiting
            // the SOI of its current primary body instead
            double parentSOI = CalculateSOI(orbitComponent.parentBodyId);

            if (orbitComponent.r > parentSOI * 1.001)
            {
                orbitComponent.parentBodyId = parentOrbitComponent.parentBodyId;

                auto& newParentMassiveBody = coordinator.GetComponent<MassiveBody>(orbitComponent.parentBodyId);
                newParentMassiveBody.childrenIds.push_back(e);

                // remove entity from old parent's list
                auto& v = parentMassiveBodyComponent.childrenIds;
                v.erase(std::remove(v.begin(), v.end(), e), v.end());

                RecalculateOrbitalParameters(e);
                orbitComponent.dirty = true;
                
                continue;
            }
        }
    }

private:
    static double CalculateSOI(uint32_t entity)
    {
        auto& orbitComponent = coordinator.GetComponent<OrbitComponent>(entity);
        auto& massiveBody = coordinator.GetComponent<MassiveBody>(entity);

        if (orbitComponent.parentBodyId != NULL)
        {
            auto& parentMassiveBody = coordinator.GetComponent<MassiveBody>(orbitComponent.parentBodyId);

            double SOI = orbitComponent.a * std::pow(massiveBody.mass / parentMassiveBody.mass, 0.4);
            return SOI;
        }
        else
        {
            // The entity is the star or otherwise root body in the system, so its SOI is infinite
            return std::numeric_limits<double>::infinity();
        }
    }

    static void RecalculateOrbitalParameters(uint32_t entity)
    {
        auto& orbitComponent = coordinator.GetComponent<OrbitComponent>(entity);
        auto& tf = coordinator.GetComponent<Transform>(entity);

        auto& parentTf = coordinator.GetComponent<Transform>(orbitComponent.parentBodyId);
        auto& parentMassiveBody = coordinator.GetComponent<MassiveBody>(orbitComponent.parentBodyId);

        glm::vec2 v = tf.velocity - parentTf.velocity;
        glm::vec2 pos = tf.position - parentTf.position;

        double distance = glm::length(pos);
        
        // specific angular momentum H
        double H = pos.x * v.y - pos.y * v.x;

        // standard gravitational parameter of parent body
        double mu = (G * parentMassiveBody.mass);

        // specific orbital energy
        double epsilon = std::pow(glm::length(v), 2) / 2 - mu / distance;

        // semilatus rectum
        double p = (H * H) / mu;

        // Calculate the eccentricity vector and its magnitude
        glm::vec2 e_vec = glm::vec2{v.y * H * (1/mu), -v.x * H * (1/mu)} - (pos / (float)distance);
        double e = glm::length(e_vec);

        double ap = std::atan2(e_vec.y, e_vec.x);

        // Get the resulting true anomaly using the sin,cos trick
        double cos_ta = glm::dot(e_vec, pos) / (e * distance);
        double sin_ta = (e_vec.x * pos.y - e_vec.y * pos.x) / (e * distance);
        double ta = std::atan2(sin_ta, cos_ta);

        // Radius of periapsis
        double rp = p / (1 + e);

        // Semi-major axis
        double a = -mu / (2 * epsilon);

        orbitComponent.a = a;
        orbitComponent.ap = ap;
        orbitComponent.e = e;
        orbitComponent.r = distance;
        orbitComponent.rp = rp;
        orbitComponent.ta = ta;
    }

};
