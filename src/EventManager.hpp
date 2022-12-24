#pragma once

#include <string>

#include <src/Event.hpp>

using namespace std;

class EventManager
{
public:
    template<typename T>
    void Subscribe(string topic, function<void()> func)
    {
        string typeName(typeid(T).name());
        typeName.append(topic);
        eventSubscribers.insert({typename, func});
    }

    template<typename T>
    void Unsubscribe(string topic, function<void()> func)
    {
        
    }

    template<typename T>
    void Publish(string topic, Event event)
    {
        string typeName(typeid(T).name());
        typeName.append(topic);
        for (auto subscriber : eventSubscribers[typename])
        {
            subscriber(event);
        }
    }

private:
    unordered_map<string, vector<function<void(Event)>>> eventSubscribers {};
}