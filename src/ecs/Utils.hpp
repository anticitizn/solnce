
#pragma once

#include <bitset>

using namespace std;

const int MAX_COMPONENTS = 256;
const int MAX_SYSTEMS = 64;
const int MAX_ENTITIES = 1000;

using EntityID = uint32_t;
using ComponentType = uint16_t;
using Signature = bitset<MAX_COMPONENTS>;
