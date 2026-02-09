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

int isDir(int d) { return (d == 'h' | d == 'j' | d == 'k' | d == 'l'); }

int isOnfood(coord snakePos, coord foodPos) {
    return snakePos.y == foodPos.y && snakePos.x == foodPos.x;
}

// Generates a valid coord for the game grid.
void generateRandomCoord(coord *coord) {
    coord->x = rand() / ((RAND_MAX + 1u) / COLS);
    coord->y = rand() / ((RAND_MAX + 1u) / LINES);
    // ensure we're on even grid
    if (coord->x % 2) {
        coord->x--;
    }
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
    ////////////////////
    // Main game loop //
    ////////////////////
    while (1) {

        // food eating
        if (isOnfood(snake.pos, curFoodPos)) {
            enqueue(&snake.tail, curFoodPos);
            generateRandomCoord(&curFoodPos);
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
            lastUpdate = clk;
        }

        // end game if snake eats itself
        if (isEatingItself(*snake)) {
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
