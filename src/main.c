#include <ncurses.h>
#include <string.h>

int isDir(int d) {
    mvprintw(2, 0, "got dir: %c", d);
    return (d == 'h' | d == 'j' | d == 'k' | d == 'l');
}

int main() {
    // init functions
    initscr();
    noecho();
    nodelay(stdscr, 1);
    curs_set(0);

    const int TICK = 8000;
    // HACK: Since characters are taller than they are wider,
    // the percieved horizontal speed > vertical speed.
    // Treating each horizontal tile as 2 characters smooths it.
    const int HORIZONTAL_MULTIPLIER = 2;
    int running = 0;

    // snake start pos
    int head_x = COLS / 2;
    int head_y = LINES / 2;

    enum directions {
        DIR_LEFT = 'h',
        DIR_DOWN = 'j',
        DIR_UP = 'k',
        DIR_RIGHT = 'l'
    };

    // Snake head char
    char curHead = '+';
    int curDirection;

    unsigned int clk = 0;
    unsigned int lastUpdate = 0;

    int ch;
    while (1) {
        char *titleString = "Press hjkl to start!";
        mvprintw(head_y, head_x - strlen(titleString), "%s", titleString);
        ch = getch();
        if (isDir(ch)) {
            running = 1;
            curDirection = ch;
            break;
        }
    }

    clear();
    while (running) {
        // clear last frame

        ch = getch();
        // update state
        switch (ch) {
        case 'h':
            curDirection = DIR_LEFT;
            // cur_head = '<';
            break;
        case 'j':
            curDirection = DIR_DOWN;
            // cur_head = 'v';
            break;
        case 'k':
            curDirection = DIR_UP;
            // cur_head = '^';
            break;
        case 'l':
            curDirection = DIR_RIGHT;
            // cur_head = '>';
            break;
        default:
            mvprintw(3, 0, "no char read: %d", clk);
            break;
        }

        // update position
        switch (curDirection) {
        case DIR_LEFT:
            if (clk > lastUpdate + TICK) {
                head_x -= 1 * HORIZONTAL_MULTIPLIER;
                lastUpdate = clk;
            }
            break;
        case DIR_DOWN:
            if (clk > lastUpdate + TICK) {
                head_y += 1;
                lastUpdate = clk;
            }
            break;
        case DIR_UP:
            if (clk > lastUpdate + TICK) {
                head_y -= 1;
                lastUpdate = clk;
            }
            break;
        case DIR_RIGHT:
            if (clk > lastUpdate + TICK) {
                head_x += 1 * HORIZONTAL_MULTIPLIER;
                lastUpdate = clk;
            }
            break;
        }

        // Edge wrap-around
        if (head_x > COLS - 1) {
            head_x = 0;
        }
        if (head_x < 0) {
            head_x = COLS - 1;
        }

        if (head_y > LINES - 1) {
            head_y = 0;
        }
        if (head_y < 0) {
            head_y = LINES - 1;
        }

        erase();
        // Print stats at top-left
        mvprintw(0, 0, "");
        printw("head Y: %d\n", head_y);
        printw("head X: %d\n", head_x);

        // Print the snake head
        mvaddch(head_y, head_x, curHead);

        // draw the buffer
        refresh();
        clk++;
    }

    endwin();
    return 0;
}
