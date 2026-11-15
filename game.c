#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void) {
    if (initscr() == NULL) {
        fprintf(stderr, "Cannot initialize ncurses. Run in a terminal.\n");
        return 1;
    }

    if (cbreak() == ERR || noecho() == ERR || keypad(stdscr, TRUE) == ERR) {
        endwin();
        fprintf(stderr, "Cannot configure terminal input.\n");
        return 1;
    }

    curs_set(0);
    timeout(50);

    bool colors = has_colors() && start_color() != ERR && COLOR_PAIRS >= 4;
    if (colors) {
        colors = init_pair(1, COLOR_CYAN, COLOR_BLACK) != ERR
              && init_pair(2, COLOR_GREEN, COLOR_BLACK) != ERR
              && init_pair(3, COLOR_YELLOW, COLOR_BLACK) != ERR;
    }

    int x = 4;
    int y = 6;
    int color = 1;
    bool running = true;

    while (running) {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        bool fits = rows >= 10 && cols >= 58;

        erase();
        if (fits) {
            if (x > cols - 3) x = cols - 3;
            if (y > rows - 3) y = rows - 3;

            box(stdscr, 0, 0);
            mvprintw(1, 2, "BETFLIP - ncurses test");
            mvprintw(2, 2, "Arrows/WASD: move | Space: color | Q/Esc: quit");
            mvprintw(3, 2, "Colors: %s | Size: %d x %d", colors ? "yes" : "no", cols, rows);

            if (colors) attron(COLOR_PAIR(color));
            mvaddch(y, x, '@');
            if (colors) attroff(COLOR_PAIR(color));
        } else {
            mvaddnstr(0, 0, "Resize to 58x10 or larger. Q: quit", cols > 1 ? cols - 1 : 0);
        }
        refresh();

        int key = getch();
        if (key == 'q' || key == 'Q' || key == 27) {
            running = false;
        } else if (fits) {
            switch (key) {
                case KEY_LEFT:  case 'a': case 'A': if (x > 2) x--; break;
                case KEY_RIGHT: case 'd': case 'D': if (x < cols - 3) x++; break;
                case KEY_UP:    case 'w': case 'W': if (y > 5) y--; break;
                case KEY_DOWN:  case 's': case 'S': if (y < rows - 3) y++; break;
                case ' ': color = color % 3 + 1; break;
            }
        }
    }

    endwin();
    puts("ncurses test finished.");
    return 0;
}

/*
 * RU: Это тестовый код для проверки ncurses, а не готовая игра «Бетфлип».
 * EN: This is test code for checking ncurses, not the finished Betflip game.
 */
