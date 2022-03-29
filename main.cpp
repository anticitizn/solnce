#include <iostream>
#include <string>

#include <src/WindowManager.hpp>

using namespace std;

int main(int argc, char *argv[]) 
{
    WindowManager wManager;
	wManager.Init("Test window", true);
}