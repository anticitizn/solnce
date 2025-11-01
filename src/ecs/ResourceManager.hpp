
#pragma once

#include <memory>
#include <unordered_map>
#include <typeinfo>
#include <utility>

class ResourceManager
{
public:
    // Create and register a new resource of type T
    template<typename T, typename... Args>
    void Register(Args&&... args)
    {
        resources[typeid(T).hash_code()] = std::make_shared<T>(std::forward<Args>(args)...);
    }

    // Modify the value of an existing resource
    template<typename T>
    void Set(const T& value)
    {
        Get<T>() = value;
    }

    // Retrieve a reference to the stored resource
    // If not registered yet, register a default-constructed instance first
    template<typename T>
    T& Get()
    {
        auto key = typeid(T).hash_code();
        auto it = resources.find(key);

        if (it == resources.end()) 
        {
            // Automatically register a default instance if not found
            Register<T>();
            it = resources.find(key);
        }

        return *std::static_pointer_cast<T>(it->second);
    }

private:
    std::unordered_map<size_t, std::shared_ptr<void>> resources;
};
