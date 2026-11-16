#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const char *logo =
    "      _____          ____   _________________       _____   ____         ____      _____\n"
    " ___|\\     \\    ____|\\   \\ /                 \\ ____|\\    \\ |    |       |    | ___|\\    \\\n"
    "|    |\\     \\  /    /\\    \\\\______     ______/|    | \\    \\|    |       |    ||    |\\    \\\n"
    "|    | |     ||    |  |    |  \\( /    /  )/   |    |______/|    |       |    ||    | |    |\n"
    "|    | /_ _ / |    |__|    |   ' |   |   '    |    |----'\\ |    |  ____ |    ||    |/____/|\n"
    "|    |\\    \\  |    .--.    |     |   |        |    |_____/ |    | |    ||    ||    ||    ||\n"
    "|    | |    | |    |  |    |    /   //        |    |       |    | |    ||    ||    ||____|/\n"
    "|____|/____/| |____|  |____|   /___//         |____|       |____|/____/||____||____|\n"
    "|    /     || |    |  |    |  |`   |          |    |       |    |     |||    ||    |\n"
    "|____|_____|/ |____|  |____|  |____|          |____|       |____|_____|/|____||____|\n"
    "  \\(    )/      \\(      )/      \\(              )/           \\(    )/     \\(    \\(\n"
    "   '    '        '      '        '              '             '    '       '     '\n";

int main(void) {
    if (initscr() == NULL) return 1;

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int key;
    do {
        erase();
        box(stdscr, 0, 0);
        mvaddch(LINES / 2, COLS / 2, '@');
        refresh();
        key = getch();
    } while (key != 'q' && key != 'Q' && key != 27);

    endwin();
    return 0;
}

/*
 * RU: Это тестовый каркас: поле и неподвижный персонаж. Логотип сохранён на будущее.
 * EN: This is a test skeleton: a field and a stationary character. The logo is saved for later.
 */
