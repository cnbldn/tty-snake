#ifndef GLOBALS_H
#define GLOBALS_H

typedef enum {
    DIR_LEFT = 'h',
    DIR_DOWN = 'j',
    DIR_UP = 'k',
    DIR_RIGHT = 'l',
} direction;

typedef struct {
    int y;
    int x;
} coord;

#endif // !GLOBALS_H
