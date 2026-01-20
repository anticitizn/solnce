
#pragma once

#include <memory>
#include <unordered_map>
#include "Utils.hpp"
#include "System.hpp"

using namespace std;

class SystemManager
{
public:
    template <typename T>
    shared_ptr<T> RegisterSystem()
    {
        const char* typeName = typeid(T).name();
        auto system = make_shared<T>();
        systems.insert({typeName, system});
        return system;
    }

    template <typename T>
    shared_ptr<T> GetSystem()
    {
        const char* typeName = typeid(T).name();
        auto it = systems.find(typeName);

        assert(it != systems.end() && "System not registered");
        return std::static_pointer_cast<T>(it->second);
    }

    template <typename T>
    void SetSignature(std::bitset<MAX_COMPONENTS> signature)
    {
        const char* typeName = typeid(T).name();
        signatures.insert({typeName, signature});
    }

    void EntitySignatureChanged(EntityID entity, std::bitset<MAX_COMPONENTS> signature)
    {
        for (auto const& pair : systems)
        {
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& systemSignature = signatures[type];

            if ((signature & systemSignature) == systemSignature)
            {
                system->entities.insert(entity);
            }

            else
            {
                system->entities.erase(entity);
            }
        }
    }

    void EntityDestroyed(EntityID entity)
    {
        for (auto const& pair : systems)
        {
            auto const& system = pair.second;
            system->entities.erase(entity);
        }
    }


private:
    unordered_map<const char*, std::bitset<MAX_COMPONENTS>> signatures {};
    unordered_map<const char*, shared_ptr<System>> systems {};
};
