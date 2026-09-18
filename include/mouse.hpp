#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <SDL3/SDL.h>
#include "globals.hpp"

class Mouse{
private:
    Vector2 lastPos = {0.0f, 0.0f};
    Vector2 pos = {0.0f, 0.0f};
public:
    void MouseUpdate(void);
    Vector2 GetMouseDifference(void);
};

#endif
