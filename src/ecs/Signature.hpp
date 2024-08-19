#pragma once

#include "Signature.inc.hpp"
#include "Coordinator.inc.hpp"

template <typename... Args>
Signature<Args...>::Signature(Coordinator* _coordinator)
{
    (signature.set(_coordinator->GetComponentType<Args>()), ...);
}

template <typename... Args>
std::bitset<MAX_COMPONENTS> Signature<Args...>::GetSignature() const
{
    return signature;
}

template <typename... Args>
Signature<Args...>::operator std::bitset<MAX_COMPONENTS>() const
{
    return signature;
}
