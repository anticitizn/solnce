#pragma once

#include <string>
#include <unordered_set>

#include <src/ui/Window.hpp>
#include <src/ecs/ECS.hpp>

#include <src/components/OrbitComponent.hpp>
#include <src/components/Transform.hpp>
#include <src/components/MassiveBody.hpp>
#include <src/components/Quad.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Polyline.hpp>

#include <src/resources/BodySelection.hpp>

extern Coordinator coordinator;

class OrbitalInspectorWindow : public Window, public System
{
public:
    // Show bodies that can appear in the hierarchy (MassiveBody). Details pane can show Orbit if present.
    Signature InitialSignature()
    {
        return coordinator.BuildSignature<MassiveBody>();
    }

    virtual void Draw() override
    {
        if (!display) return;

        auto& bodySelection = coordinator.GetResource<BodySelection>();

        ImGui::Begin("Orbital Inspector");

        // Two-pane layout
        if (ImGui::BeginTable("##orbital_inspector_split", 2,
                              ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
        {
            ImGui::TableSetupColumn("Hierarchy", ImGuiTableColumnFlags_WidthFixed, 260.0f);
            ImGui::TableSetupColumn("Details",   ImGuiTableColumnFlags_WidthStretch);

            // LEFT: hierarchy
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            DrawHierarchyPane(bodySelection);

            // RIGHT: details + create
            ImGui::TableSetColumnIndex(1);
            DrawDetailsAndCreatePane(bodySelection);

            ImGui::EndTable();
        }

        ImGui::End();
    }

private:
    // Creation state
    double mass = 1000;
    double pr   = 1000000.0;
    double ecc  = 0.0;
    double ap   = 0.0;
    double ta   = 0.0;

    float size = 20.0f;
    float color[3] = {1,1,1};
    char texturePath[128] = "planet.png";

    // Optional: UI state
    char filter[64] = "";

    void DrawHierarchyPane(BodySelection& bodySelection)
    {
        ImGui::TextUnformatted("Bodies");
        ImGui::Separator();

        ImGui::BeginChild("##hierarchy_child", ImVec2(0, 0), false);

        if (entities.empty())
        {
            ImGui::TextUnformatted("No MassiveBody entities found.");
            ImGui::EndChild();
            return;
        }

        // Find roots: MassiveBody entities that are not children of any other MassiveBody
        std::unordered_set<uint32_t> isChild;
        isChild.reserve(entities.size() * 2);

        for (uint32_t id : entities)
        {
            auto& mb = coordinator.GetComponent<MassiveBody>(id);
            for (uint32_t c : mb.childrenIds)
            {
                isChild.insert(c);
            }
        }

        bool anyRoot = false;
        for (uint32_t id : entities)
        {
            if (isChild.find(id) != isChild.end())
            {
                continue;
            }

            anyRoot = true;
            DrawBodyNodeRecursive(id, bodySelection);
        }

        // Fallback: if somehow data is incomplete and there is no root, just list all
        if (!anyRoot)
        {
            for (uint32_t id : entities)
            {
                DrawBodyNodeRecursive(id, bodySelection);
            }
        }

        ImGui::EndChild();
    }

    void DrawBodyNodeRecursive(uint32_t id, BodySelection& bodySelection)
    {
        if (!coordinator.HasComponent<MassiveBody>(id))
        {
            return;
        }

        const bool isSelected = (bodySelection.selectedEntity == id);

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

        auto& mb = coordinator.GetComponent<MassiveBody>(id);
        if (mb.childrenIds.empty())
        {
            flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        if (isSelected)
        {
            flags |= ImGuiTreeNodeFlags_Selected;
        }

        std::string label = "Entity " + std::to_string(id);

        // Use stable ID scope so ImGui can distinguish same labels
        ImGui::PushID((int)id);
        bool open = ImGui::TreeNodeEx(label.c_str(), flags);

        // Clicking anywhere on the item selects the entity
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            bodySelection.selectedEntity = id;
        }

        if (!mb.childrenIds.empty() && open)
        {
            for (uint32_t child : mb.childrenIds)
            {
                DrawBodyNodeRecursive(child, bodySelection);
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void DrawDetailsAndCreatePane(BodySelection& bodySelection)
    {
        const uint32_t selected = bodySelection.selectedEntity;

        if (selected == 0)
        {
            ImGui::TextUnformatted("No entity selected.");
            ImGui::Separator();
            DrawCreateSection(bodySelection, INVALID_ENTITY);
            return;
        }

        ImGui::Text("Entity %u", selected);

        if (coordinator.HasComponent<OrbitComponent>(selected))
        {
            auto& orbit = coordinator.GetComponent<OrbitComponent>(selected);
            ImGui::Separator();
            ImGui::TextUnformatted("Orbit");
            ImGui::Text("Parent ID: %u", orbit.parentBodyId);
            ImGui::Text("a  = %.2f", orbit.a);
            ImGui::Text("e  = %.2f", orbit.e);
            ImGui::Text("ap = %.2f", orbit.ap);
            ImGui::Text("ta = %.2f", orbit.ta);
            ImGui::Text("r  = %.2f", orbit.r);
        }

        if (coordinator.HasComponent<Transform>(selected))
        {
            auto& tf = coordinator.GetComponent<Transform>(selected);
            ImGui::Separator();
            ImGui::TextUnformatted("Transform");
            ImGui::Text("position = (%.2f, %.2f)", tf.position.x, tf.position.y);
            ImGui::Text("velocity = (%.2f, %.2f)", tf.velocity.x, tf.velocity.y);
            ImGui::Text("rotation = %.2f", tf.rotation);
        }

        if (coordinator.HasComponent<MassiveBody>(selected))
        {
            auto& mb = coordinator.GetComponent<MassiveBody>(selected);
            ImGui::Separator();
            ImGui::TextUnformatted("MassiveBody");
            ImGui::Text("mass = %.2e kg", mb.mass);
            ImGui::Text("children = %d", (int)mb.childrenIds.size());
        }

        ImGui::Separator();
        DrawCreateSection(bodySelection, selected);
    }

    void DrawCreateSection(BodySelection& bodySelection, uint32_t fallbackParent)
    {
        // Parent selection: default to currently selected entity if it’s a MassiveBody; else first available MassiveBody.
        uint32_t parentId = INVALID_ENTITY;

        if (fallbackParent != INVALID_ENTITY && coordinator.HasComponent<MassiveBody>(fallbackParent))
        {
            parentId = fallbackParent;
        }
        else if (!entities.empty())
        {
            parentId = *entities.begin();
        }

        ImGui::PushItemWidth(160.0f);

        // No MassiveBody exists at all, this just shouldn't happen but still
        if (parentId == INVALID_ENTITY)
        {
            ImGui::TextUnformatted("No MassiveBody exists to parent a new orbiting body.");
            return;
        }

        // Let user override parent
        std::string previewStr = std::to_string(parentId);
        if (ImGui::BeginCombo("Parent Body", previewStr.c_str()))
        {
            for (uint32_t id : entities)
            {
                bool selected = (id == parentId);
                std::string label = std::to_string(id);

                if (ImGui::Selectable(label.c_str(), selected))
                {
                    parentId = id;
                }
                    
                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::InputDouble("Mass (kg)", &mass, 1000, 100000, "%.2f");
        ImGui::InputDouble("pr (periapsis radius)", &pr, 1000, 100000, "%.2f");
        ImGui::InputDouble("e (eccentricity)", &ecc, 0.01, 0.1, "%.2f");
        ImGui::InputDouble("ap (argument of periapsis)", &ap, 1000, 100000, "%.2f");
        ImGui::InputDouble("ta (true anomaly)", &ta, 0.01, 0.1, "%.2f");

        ImGui::InputFloat("Quad Size", &size);
        ImGui::ColorEdit3("Quad Color", color);
        ImGui::InputText("Texture", texturePath, sizeof(texturePath));

        if (ImGui::Button("Create Child Body"))
        {
            uint32_t created = CreateOrbitingBody(parentId);
        }

        ImGui::PopItemWidth();
    }

    uint32_t CreateOrbitingBody(uint32_t parentId)
    {
        Entity ent = coordinator.CreateEntity();
        uint32_t id = ent.GetId();

        coordinator.AddComponent(id, MassiveBody{ mass });

        coordinator.AddComponent(id, OrbitComponent{
            parentId,
            pr, ecc, ap, ta,
            0.0, 0.0,
        });

        coordinator.AddComponent(id, Transform{
            {0,0},
            {0,0},
            0.0f
        });

        coordinator.AddComponent(id, Quad{
            size, size,
            color[0], color[1], color[2],
        });

        coordinator.AddComponent(id, Texture{ texturePath, 0 });

        ent.Assign<Polyline>(Polyline{
            {},
            { {1,1,1,1}, 2.0f, 1.0f, 0, 0 }
        });

        // Keep hierarchy consistent
        if (coordinator.HasComponent<MassiveBody>(parentId))
        {
            auto& parentMassiveBody = coordinator.GetComponent<MassiveBody>(parentId);
            parentMassiveBody.childrenIds.push_back(id);
        }

        return id;
    }
};
