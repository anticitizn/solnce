
#pragma once

#include <memory>
#include <unordered_map>

class ResourceManager
{
public:
    template<typename T, typename... Args>
    void Register(Args&&... args)
    {
        resources[typeid(T).hash_code()] = std::make_shared<T>(std::forward<Args(args)...);
    }

    template<typename T>
    T& Get()
    {
        return *std::static_pointer_cast<T>(resources.at(typeid(T).hash_code()));
    }

private:
    std::unordered_map<size_t, std::shared_ptr<void>> resources;
};
