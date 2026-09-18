#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#define RED (Color){255, 0, 0, 255}

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 650

typedef struct{
    int r;
    int g;
    int b;
    int a;
}Color;

typedef struct{
    float x;
    float y;
}Vector2;

#endif
