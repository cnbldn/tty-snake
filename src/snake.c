#include "snake.h"
#include "coordQueue.h"
#include "globals.h"
#include <ncurses.h>

// helper functions
char getHeadChar(direction);
int isValidDirection(direction cur, direction dir);

// class functions

void snakeInit(Snake *snake, direction startingDir) {
    snake->pos = (coord){.y = LINES / 2, .x = COLS / 2};
    if (snake->pos.x % 2)
        snake->pos.x--;
    snake->lastPos = snake->pos;

    snake->curDirection = startingDir;
    snake->curHeadSymbol = getHeadChar(startingDir);

    queueInit(&snake->tail);
}

// Updates the position of the snake and its tail
// If snake goes oob, it wraps around
void snakeUpdatePosition(Snake *snake, direction dir) {
    snake->lastPos = snake->pos;
    if (dir != 0 && isValidDirection(snake->curDirection, dir)) {
        snake->curDirection = dir;
        snake->curHeadSymbol = getHeadChar(dir);
    }

    switch (snake->curDirection) {
    case DIR_LEFT:
        snake->pos.x -= 2;
        break;
    case DIR_DOWN:
        snake->pos.y += 1;
        break;
    case DIR_UP:
        snake->pos.y -= 1;
        break;
    case DIR_RIGHT:
        snake->pos.x += 2;
        break;
    default:
        break;
    }

    // Update Tail
    coord tmp;
    dequeue(&snake->tail, &tmp);
    enqueue(&snake->tail, snake->lastPos);

    // Wraparound
    if (snake->pos.x > COLS - 1) {
        snake->pos.x = 0;
    }
    if (snake->pos.x < 0) {
        snake->pos.x = COLS % 2 ? COLS - 1 : COLS - 2;
    }
    if (snake->pos.y > LINES - 1) {
        snake->pos.y = 0;
    }
    if (snake->pos.y < 0) {
        snake->pos.y = LINES - 1;
    }
}

void drawSnake(const Snake *snake) {
    // Draw the tail
    struct Node *curNode = snake->tail.front;
    while (curNode != NULL) {
        mvprintw(curNode->data.y, curNode->data.x, "o");
        curNode = curNode->next;
    }
    // Draw the head
    mvaddch(snake->pos.y, snake->pos.x, snake->curHeadSymbol);
}

int isValidDirection(direction cur, direction dir) {
    if ((cur == DIR_RIGHT && dir == DIR_LEFT) ||
        (cur == DIR_LEFT && dir == DIR_RIGHT) ||
        (cur == DIR_UP && dir == DIR_DOWN) ||
        (cur == DIR_DOWN && dir == DIR_UP)) {
        return 0;
    }
    return 1;
}

// Returns the corresponding symbol for the direction
char getHeadChar(direction dir) {
    switch (dir) {
    case DIR_LEFT:
        return '<';
    case DIR_DOWN:
        return 'v';
    case DIR_UP:
        return '^';
    case DIR_RIGHT:
        return '>';
    }
}
