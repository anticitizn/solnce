
#include <SDL3/SDL.h>

struct InputState 
{
    // Keyboard
    std::bitset<SDL_SCANCODE_COUNT> down;      // buttons that were previously pressed and haven't been released yet
    std::bitset<SDL_SCANCODE_COUNT> pressed;   // buttons that transitioned 0->1 this frame
    std::bitset<SDL_SCANCODE_COUNT> released;  // buttons that transitioned 1->0 this frame

    // Mouse
    float mouseX = 0; // absolute motion position
    float mouseY = 0;               
    float dx = 0; // total motion during this frame
    float dy = 0;
    uint32_t mouseDown = 0; // SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT or SDL_BUTTON_MIDDLE
    uint32_t mousePressed = 0;
    uint32_t mouseReleased = 0;
    float wheelX = 0;
    float wheelY = 0;

    void Reset() 
    {
        pressed.reset(); 
        released.reset();
        mousePressed = mouseReleased = 0;
        dx = dy = 0; 
        wheelX = wheelY = 0;
    }
};
