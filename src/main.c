#include "coordQueue.h"
#include "globals.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const int TICK = 8000;
// HACK: Since characters are taller than they are wider,
// the percieved horizontal speed > vertical speed.
// Treating each horizontal tile as 2 characters smooths it.
const int HORIZONTAL_MULTIPLIER = 2;

void updateTail(Queue *tail, coord lastHeadPos) {
    coord tmp;
    // remove the last part of tail
    dequeue(tail, &tmp);

    // add
    enqueue(tail, lastHeadPos);
}

void drawTail(const Queue *tail) {
    struct Node *curNode = tail->front;
    while (curNode != NULL) {
        mvprintw(curNode->data.y, curNode->data.x, "o");
        curNode = curNode->next;
    }
}

int isDir(int d) { return (d == 'h' | d == 'j' | d == 'k' | d == 'l'); }

int isOnfood(coord snakeCoords, coord curFoodPos) {
    return snakeCoords.y == curFoodPos.y && snakeCoords.x == curFoodPos.x;
}

// Generates a valid coord for the game grid.
void generateRandomCoord(coord *coord) {
    coord->x = rand() / ((RAND_MAX + 1u) / COLS);
    coord->y = rand() / ((RAND_MAX + 1u) / LINES);
    // TODO: make sure food doesn't end up oob
    // repurpose the snake oob code into a function
    if (((COLS / 2) % 2) != coord->x % 2)
        coord->x++;
}

void init() {
    initscr();
    noecho();
    curs_set(0);
    srand(time(NULL));
}

// Displays the title screen and waits for input.
int titleScreen(int *curDirection) {
    // Print the title
    char *titleString = "Press hjkl to start!";
    mvprintw(LINES / 2, COLS / 2 - strlen(titleString) / 2, "%s", titleString);

    // Wait for initial input
    int ch;
    while (1) {
        ch = getch();
        if (isDir(ch)) {
            *curDirection = ch;
            nodelay(stdscr, 1);
            return 0;
        }
    }
    return 1;
}

int main() {
    init();

    coord snakeCoords = {.y = LINES / 2, .x = COLS / 2};
    coord lastSnakeCoords = snakeCoords;

    // Snake head char
    char curHead = '+';
    int curDirection;

    Queue snakeTail;
    queueInit(&snakeTail);

    unsigned int clk = 0;
    unsigned int lastUpdate = 0;
    int updatedThisCycle = 0;

    coord curFoodPos;
    generateRandomCoord(&curFoodPos);
    int isEaten = 0;

    if (titleScreen(&curDirection)) {
        return 1;
    }

    // Initial direction of the head
    switch (curDirection) {
    case DIR_LEFT:
        curHead = '<';
        break;
    case DIR_DOWN:
        curHead = 'v';
        break;
    case DIR_UP:
        curHead = '^';
        break;
    case DIR_RIGHT:
        curHead = '>';
        break;
    }

    ////////////////////
    // Main game loop //
    ////////////////////
    while (1) {
        if (isOnfood(snakeCoords, curFoodPos)) {
            enqueue(&snakeTail, curFoodPos);
            generateRandomCoord(&curFoodPos);
        }

        int ch = getch();
        // NOTE: This way, only the first input in the current cycle is valid.
        if (!updatedThisCycle) {
            // update state
            switch (ch) {
            case 'h':
                if (curDirection != DIR_RIGHT) {
                    curDirection = DIR_LEFT;
                    curHead = '<';
                    updatedThisCycle = 1;
                }
                break;
            case 'j':
                if (curDirection != DIR_UP) {
                    curDirection = DIR_DOWN;
                    curHead = 'v';
                    updatedThisCycle = 1;
                }
                break;
            case 'k':
                if (curDirection != DIR_DOWN) {
                    curDirection = DIR_UP;
                    curHead = '^';
                    updatedThisCycle = 1;
                }
                break;
            case 'l':
                if (curDirection != DIR_LEFT) {
                    curDirection = DIR_RIGHT;
                    curHead = '>';
                    updatedThisCycle = 1;
                }
                break;
            }
        }

        if (clk > lastUpdate + TICK) {
            // update position

            switch (curDirection) {
            case DIR_LEFT:
                lastSnakeCoords = snakeCoords;
                snakeCoords.x -= 1 * HORIZONTAL_MULTIPLIER;
                lastUpdate = clk;
                break;
            case DIR_DOWN:
                lastSnakeCoords = snakeCoords;
                snakeCoords.y += 1;
                lastUpdate = clk;
                break;
            case DIR_UP:
                lastSnakeCoords = snakeCoords;
                snakeCoords.y -= 1;
                lastUpdate = clk;
                break;
            case DIR_RIGHT:
                lastSnakeCoords = snakeCoords;
                snakeCoords.x += 1 * HORIZONTAL_MULTIPLIER;
                lastUpdate = clk;
                break;
            }

            // update tail
            updateTail(&snakeTail, lastSnakeCoords);

            updatedThisCycle = 0;
        }

        // Edge wrap-around
        if (snakeCoords.x > COLS - 1) {
            snakeCoords.x = 0;
        }
        if (snakeCoords.x < 0) {
            snakeCoords.x = COLS - 1;
        }

        if (snakeCoords.y > LINES - 1) {
            snakeCoords.y = 0;
        }
        if (snakeCoords.y < 0) {
            snakeCoords.y = LINES - 1;
        }

        // Erase previous frame
        erase();

        // Debug info
        mvprintw(0, 0, "");
        printw("LINES: %d COLS: %d\n", LINES, COLS);
        printw("head Y: %d\n", snakeCoords.y);
        printw("head X: %d\n", snakeCoords.x);
        printw("Tail Length: %d\n", snakeTail.size);
        printw("Food Pos: y=%d x=%d\n", curFoodPos.y, curFoodPos.x);

        mvprintw(curFoodPos.y, curFoodPos.x, "*");      // Draw food
        mvaddch(snakeCoords.y, snakeCoords.x, curHead); // Draw snake head
        drawTail(&snakeTail);

        // draw the buffer
        refresh();
        clk++;
    }

    endwin();
    return 0;
}
