#include "coordQueue.h"
#include "globals.h"
#include "snake.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUGINFO

const int TICK = 8000;
// HACK: Since characters are taller than they are wider,
// the percieved horizontal speed > vertical speed.
// Treating each horizontal tile as 2 characters smooths it.
const int HORIZONTAL_MULTIPLIER = 2;

int coordIsEqual(coord a, coord b) { return a.x == b.x && a.y == b.y; }

int isDir(int d) { return (d == 'h' | d == 'j' | d == 'k' | d == 'l'); }

// Generates a valid coord for the game grid.
void generateRandomCoord(coord *coord) {
    coord->x = rand() / ((RAND_MAX + 1u) / COLS);
    coord->y = rand() / ((RAND_MAX + 1u) / LINES);
    // ensure we're on even grid
    if (coord->x % 2) {
        coord->x--;
    }
}

// linear search, returns true if head is on tail
int isEatingItself(Snake *snake) {
    struct Node *tmp = snake->tail.front;
    while (tmp != NULL) {
        if (coordIsEqual(snake->pos, tmp->data)) {
            return 1;
        }
        tmp = tmp->next;
    }
    return 0;
}

void init() {
    initscr();
    noecho();
    curs_set(0);
    srand(time(NULL));
}

// Displays the title screen and waits for input.
int titleScreen(direction *dir) {
    // Print the title
    char *titleString = "Press hjkl to start!";
    mvprintw(LINES / 2, COLS / 2 - strlen(titleString) / 2, "%s", titleString);

    // Wait for initial input
    int ch;
    // if it is not valid, keep waiting for key presses
    while (1) {
        ch = getch();
        if (isDir(ch)) {
            *dir = ch;
            nodelay(stdscr, 1);
            return 0;
        }
    }
    return 1;
}

void endScreen(int score) {
    const int countdown = 5;

    int i = countdown;
    time_t curTime = time(NULL);
    time_t startTime = time(NULL);

    while (curTime < startTime + countdown) {
        erase();
        mvprintw(LINES / 2, (COLS / 2) - 9, "oops you died %ld...",
                 countdown - (curTime - startTime));
        curTime = time(NULL);
        refresh();
    }

    erase();
    mvprintw(LINES / 2, (COLS / 2) - 5, "Score: %d", score);
    mvprintw((LINES / 2) + 1, (COLS / 2) - 11, "Press any key to exit.");
    refresh();
    nodelay(stdscr, 0);
    int ch = getch();
    printw("\nexiting..\n");
}

int main() {
    init();

    unsigned int clk = 0;
    unsigned int lastUpdate = 0;
    int updatedThisCycle = 0;

    coord curFoodPos;
    generateRandomCoord(&curFoodPos);
    int isEaten = 0;

    // the key pressed in the title screen is the initial direction
    direction initialDir;
    if (titleScreen(&initialDir)) {
        return 1;
    }

    Snake snake;
    snakeInit(&snake, initialDir);

    direction inputDir;
    int isOnFood = 0;

    ////////////////////
    // Main game loop //
    ////////////////////
    while (1) {

        // food eating
        if (coordIsEqual(snake.pos, curFoodPos)) {
            isOnFood = 1;
        }

        // Get input. Last input before the next frame is drawn is valid.
        switch (getch()) {
        case 'h':
            inputDir = DIR_LEFT;
            break;
        case 'j':
            inputDir = DIR_DOWN;
            break;
        case 'k':
            inputDir = DIR_UP;
            break;
        case 'l':
            inputDir = DIR_RIGHT;
            break;
        }
        // EVERY FRAME
        if (clk > lastUpdate + TICK) {
            // inputDir = charToDir(ch);
            snakeUpdatePosition(&snake, inputDir);
            if (isOnFood) {
                enqueue(&snake.tail, curFoodPos);
                generateRandomCoord(&curFoodPos);
                isOnFood = 0;
            }
            lastUpdate = clk;
        }

        // end game if snake eats itself
        if (isEatingItself(&snake)) {
            endScreen(snake.tail.size);
            break;
        }

        // Erase previous frame
        erase();

// Debug info
#ifdef DEBUGINFO
        mvprintw(0, 0, "");
        printw("LINES: %d COLS: %d\n", LINES, COLS);
        printw("head Y: %d\n", snake.pos.y);
        printw("head X: %d\n", snake.pos.x);
        printw("Tail Length: %d\n", snake.tail.size);
        printw("Food Pos: y=%d x=%d\n", curFoodPos.y, curFoodPos.x);
        printw("inputDir: %c\n", inputDir);
        printw("curdir: %c\n", snake.curDirection);

        printw("%d\n", clk);
        printw("%d\n", lastUpdate);

#endif /* ifdef DEBUGINFO */

        // Draw the food
        mvprintw(curFoodPos.y, curFoodPos.x, "*");

        drawSnake(&snake);

        refresh();
        clk++;
    }

    endwin();
    return 0;
}
