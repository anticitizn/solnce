#pragma once

#include <src/ui/Window.hpp>
#include <src/ecs/ECS.hpp>
#include <src/components/MassiveBody.hpp>
#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>

extern Coordinator coordinator;

class OrbitalBodyCreationWindow : public Window, public System
{
public:
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<OrbitComponent, Transform, MassiveBody>();
    }

    virtual void Draw() override
    {
        if (!display)
        {
            return;
        }

        ImGui::Begin("Create Orbital Body");

        // No parent bodies -> nothing can orbit
        if (entities.empty())
        {
            ImGui::Text("No parent MassiveBody entities exist.");
            ImGui::End();
            return;
        }

        // Ensure selectedParentId is valid (first time, or if selection was removed)
        if (selectedParentId == kInvalidId || entities.find(selectedParentId) == entities.end())
        {
            selectedParentId = *entities.begin();
        }

        // Combo preview text
        std::string previewStr = std::to_string(selectedParentId);
        const char* preview = previewStr.c_str();

        if (ImGui::BeginCombo("Parent Body", preview))
        {
            for (uint32_t id : entities)
            {
                bool selected = (id == selectedParentId);
                std::string label = std::to_string(id);

                if (ImGui::Selectable(label.c_str(), selected))
                {
                    selectedParentId = id;
                }
                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        uint32_t parentId = 0;

        ImGui::InputDouble("Mass (kg)", &mass);
        ImGui::InputDouble("a (semi-major axis)", &a);
        ImGui::InputDouble("e (eccentricity)", &e);
        ImGui::InputDouble("ap (argument of periapsis)", &ap);
        ImGui::InputDouble("ta (true anomaly)", &ta);

        ImGui::InputFloat("Quad Size", &size);
        ImGui::ColorEdit3("Quad Color", color);

        if (ImGui::Button("Create Body"))
        {
            CreateEntity(selectedParentId);
        }

        ImGui::End();
    }

private:
    static constexpr uint32_t kInvalidId = 0xFFFFFFFFu;
    uint32_t selectedParentId = kInvalidId;

    double mass = 1e15;
    double a = 100.0;
    double e = 0.0;
    double ap = 0.0;
    double ta = 0.0;

    float size = 20.0f;
    float color[3] = {1,1,1};

    void CreateEntity(uint32_t parentId)
    {
        Entity ent = coordinator.CreateEntity();
        uint32_t id = ent.GetId();

        coordinator.AddComponent(id, MassiveBody{ mass });

        coordinator.AddComponent(id, OrbitComponent{
            parentId,
            a, e, ap, ta,
            0.0, 0.0, 0.0
        });

        coordinator.AddComponent(id, Transform{
            {0,0,0},
            {0,0},
            0.0f
        });

        coordinator.AddComponent(id, Quad{
            size, size,
            color[0], color[1], color[2],
        });

        coordinator.AddComponent(id, Texture{"planet.png", 0});

        ent.Assign<Polyline>(Polyline{
            {}, // segments (empty, should be filled by OrbitPathSystem)
            { {1,1,1,1}, 2.0f, 1.0f, 0, 0 }  // attributes (color/width)
        });


        // Update registry
        auto& reg = coordinator.GetResource<OrbitalRegistry>();
        reg.massiveBodies.push_back(id);
        reg.orbitingBodies.push_back(id);
    }
};
