#pragma once

#include <src/ui/Window.hpp>
#include <src/ecs/ECS.hpp>
#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/MassiveBody.hpp>
#include <src/resources/OrbitalRegistry.hpp>

extern Coordinator coordinator;

class OrbitalDebugWindow : public Window, public System
{
public:
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<OrbitComponent, Transform, MassiveBody>();
    }

    virtual void Draw() override
    {
        if (!display)
            return;

        auto& reg = coordinator.GetResource<OrbitalRegistry>();

        ImGui::Begin("Orbital Debug");

        if (reg.orbitingBodies.empty())
        {
            ImGui::Text("No orbiting bodies found.");
            ImGui::End();
            return;
        }

        for (uint32_t id : reg.orbitingBodies)
        {
            std::string label = "Entity " + std::to_string(id);

            if (ImGui::TreeNode(label.c_str()))
            {
                auto& orbit = coordinator.GetComponent<OrbitComponent>(id);

                ImGui::Text("Parent ID: %u", orbit.parentBodyId);
                ImGui::Text("a = %.6f", orbit.a);
                ImGui::Text("e = %.6f", orbit.e);
                ImGui::Text("ap = %.6f", orbit.ap);
                ImGui::Text("ta = %.6f", orbit.ta);

                ImGui::Separator();
                ImGui::Text("E = %.6f", orbit.E);
                ImGui::Text("M = %.6f", orbit.M);
                ImGui::Text("r = %.6f", orbit.r);

                if (coordinator.HasComponent<Transform>(id))
                {
                    auto& tf = coordinator.GetComponent<Transform>(id);
                    ImGui::Separator();
                    ImGui::Text("Transform:");
                    ImGui::Text("  position = (%.6f, %.6f, %.6f)",
                                tf.position.x, tf.position.y, tf.position.z);
                    ImGui::Text("  velocity = (%.6f, %.6f)",
                                tf.velocity.x, tf.velocity.y);
                }

                if (coordinator.HasComponent<MassiveBody>(id))
                {
                    auto& mb = coordinator.GetComponent<MassiveBody>(id);
                    ImGui::Separator();
                    ImGui::Text("Mass: %.6e kg", mb.mass);
                }

                ImGui::TreePop();
            }
        }

        ImGui::End();
    }
};
