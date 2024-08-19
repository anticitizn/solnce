#pragma once

#include <bitset>
#include "Utils.hpp"

class Coordinator;

template <typename... Args>
class Signature
{
public:
    Signature(Coordinator* _coordinator);

    std::bitset<MAX_COMPONENTS> GetSignature() const;

    operator std::bitset<MAX_COMPONENTS>() const;

private:
    std::bitset<MAX_COMPONENTS> signature;
};
