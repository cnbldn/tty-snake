#include "coordQueue.h"
#include "globals.h"

#include <ncurses.h>
#include <string.h>

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

void printQueue(const Queue *tail) {
    struct Node *curNode = tail->front;
    while (curNode != NULL) {
        printw("y:%d x:%d\n", curNode->data.y, curNode->data.x);
        curNode = curNode->next;
    }
}

int isDir(int d) { return (d == 'h' | d == 'j' | d == 'k' | d == 'l'); }

void init() {
    initscr();
    noecho();
    curs_set(0);
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

    if (titleScreen(&curDirection)) {
        return 1;
    }

    coord aaa;
    for (int i = 0; i < 50; i++) {
        aaa.y = snakeCoords.y;
        aaa.x = snakeCoords.x - i * 2;
        enqueue(&snakeTail, aaa);
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

    // Main game loop
    while (1) {
        int ch = getch();

        // update state
        switch (ch) {
        case 'h':
            if (curDirection != DIR_RIGHT) {
                curDirection = DIR_LEFT;
                curHead = '<';
            }
            break;
        case 'j':
            if (curDirection != DIR_UP) {
                curDirection = DIR_DOWN;
                curHead = 'v';
            }
            break;
        case 'k':
            if (curDirection != DIR_DOWN) {
                curDirection = DIR_UP;
                curHead = '^';
            }
            break;
        case 'l':
            if (curDirection != DIR_LEFT) {
                curDirection = DIR_RIGHT;
                curHead = '>';
            }
            break;
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
        printw("head Y: %d\n", snakeCoords.y);
        printw("head X: %d\n", snakeCoords.x);
        printw("Queue Size %d:\n", snakeTail.size);

        // Draw the snake
        mvaddch(snakeCoords.y, snakeCoords.x, curHead);
        drawTail(&snakeTail);

        // draw the buffer
        refresh();
        clk++;
    }

    endwin();
    return 0;
}
