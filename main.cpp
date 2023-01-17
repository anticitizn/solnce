#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <memory>
#include <math.h>
#include <typeinfo>

#include <src/ecs/ECS.hpp>
#include <src/systems/RenderingSystem.hpp>
#include <src/systems/MovingSystem.hpp>
#include <src/components/Texture.hpp>
#include <src/components/Quad.hpp>

#include <external/pugixml/pugixml.hpp>

using namespace std;

extern Coordinator coordinator;

int main(int argc, char *argv[])
{
    pugi::xml_document doc;
    auto declarationNode = doc.append_child(pugi::node_declaration);

    auto root = doc.append_child("root");
    auto nodeChild = root.append_child("child1");
    nodeChild.append_child(pugi::node_pcdata).set_value(to_string(15.374).c_str());
    doc.save_file("test2.xml", PUGIXML_TEXT("  "));

    coordinator.RegisterComponent<Texture>();
    coordinator.RegisterComponent<Quad>();

    shared_ptr<RenderingSystem> renderingSystem = coordinator.RegisterSystem<RenderingSystem>();
    
    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Texture>());
        signature.set(coordinator.GetComponentType<Quad>());
        coordinator.SetSystemSignature<RenderingSystem>(signature);
    }

    renderingSystem->Init("assets/", "src/shaders/");

    shared_ptr<MovingSystem> movingSystem = coordinator.RegisterSystem<MovingSystem>();

    {
        Signature signature;
        signature.set(coordinator.GetComponentType<Quad>());
        coordinator.SetSystemSignature<MovingSystem>(signature); 
    }
    
    for (int i = 0; i < 10; i++)
    {
        Entity entity = coordinator.CreateEntity();
        coordinator.AddComponent<Quad>(entity, Quad {50.0f * i, 50.0f * i, 0, 50, 50, 255, 255, 255, 0});
        coordinator.AddComponent<Texture>(entity, Texture{"", 0});
    }

    while(true)
    {
        movingSystem->Update();
        renderingSystem->Render();
    }
    
    cout << coordinator.GetEntitiesCount() << endl;
    
}