#include <iostream>
#include <string>

#include <src/WindowManager.hpp>
#include <src/ecs/ComponentContainer.hpp>
#include <src/ecs/EntityManager.hpp>
#include <src/ecs/ComponentManager.hpp>
#include <src/ecs/SystemManager.hpp>

using namespace std;

int main(int argc, char *argv[]) 
{
    WindowManager wManager;
	wManager.Init("Test window", true);
}