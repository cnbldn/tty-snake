#ifndef SNAKE_H
#define SNAKE_H

#include "coordQueue.h"
#include "globals.h"

typedef struct {
    coord pos;
    coord lastPos;
    char curHeadSymbol;
    direction curDirection;
    Queue tail;

} Snake;

void snakeInit(Snake *, direction startingDir);
void snakeUpdatePosition(Snake *, direction);
void drawSnake(const Snake *);

#endif // !SNAKE_H
