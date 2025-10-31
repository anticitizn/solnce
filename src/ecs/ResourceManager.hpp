
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
    template<typename T>
    T& Get()
    {
        return *std::static_pointer_cast<T>(resources.at(typeid(T).hash_code()));
    }

private:
    std::unordered_map<size_t, std::shared_ptr<void>> resources;
};
