
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>

#include <external/cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>
#include <external/cereal/types/set.hpp>

#include <src/ecs/System.hpp>

using namespace std;

extern Coordinator coordinator;

class SaveSystem : public System
{
public:
    void PrintEntities()
    {
        for (const auto& entity : entities)
        {
            cout << "Entity " << entity << endl;
        }
    }

    void Save(string filename)
    {
        ofstream filestream(filename.c_str());
        {
            cereal::XMLOutputArchive archive(filestream);
            archive(entities);
        }
    }

    void Load(string filename)
    {
        while (entities.size() != 0)
        {
            coordinator.DestroyEntity(*entities.begin());
        }
        
        ifstream filestream(filename.c_str());
        {
            cereal::XMLInputArchive archive(filestream);
            archive(entities);
        }       
    }

private:

};


