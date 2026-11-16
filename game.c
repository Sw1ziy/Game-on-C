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
        
void load_start_screan(void){

    int logo_x = (COLS - 91) / 2;
    int logo_y = (LINES - 15) / 2;

    erase();
    
    int row = logo_y;
    int col = logo_x;
    
    for(int i = 0; logo[i] != '\0'; i++){
        if(logo[i] == '\n'){
            row++;
            col = logo_x;
        } else {
            mvaddch(row, col, logo[i]);
            col++;
        }
    }

    int bar_width = 40;
    int bar_x = (COLS - bar_width - 2) / 2;
    int bar_y = logo_y + 13;

    mvaddch(bar_y, bar_x, '[');
    mvaddch(bar_y, bar_x + bar_width + 1, ']');
    

    for(int percent = 0; percent <= 100; percent++){
        int filled = bar_width * percent / 100;
        for(int i = 0; i < bar_width; i++){
           if(i < filled){
                mvaddch(bar_y, bar_x + 1 + i, '#');
            } else {
                mvaddch(bar_y, bar_x + 1 + i, ' ');
            }   
        }

        mvprintw(bar_y + 1, (COLS - 4) / 2, "%3d%%", percent);

        refresh();

        if(percent < 100){
            napms(40);
        } else if(percent == 100){
            mvaddstr(bar_y + 2, (COLS - 5) / 2, "START");
            refresh();
        }    
    }

    refresh();
    getch();
}

int flip_gravity(int gravity, int key, int y){
    if(y == LINES - 2 || y == 1){
        if(key == ' '){
            gravity = -gravity;           
        }
    }
    return gravity;
}

int move_player_x(int x, int key){
    if ((key == 'a' || key == 'A') && x > 1){
        x--;
    } else if((key == 'd' || key == 'D') && x < COLS - 2){
        x++;
    }   
    return x;
}

int move_player_y(int y, int gravity){
    int jump = y + gravity;
    if(jump >= 1 && jump <= LINES - 2){
        y = jump;
    }
    return y;
}

int main(void) {
    if (initscr() == NULL) return 1;

    load_start_screan();

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int key = 0;
    int x = 1;
    int y = LINES - 2;
    int gravity = 1;

    while (key != 'q' && key != 'Q' && key != 27){
        erase();
        box(stdscr, 0, 0);
        mvaddch(y, x, 'M');
        x = move_player_x(x, key);
        y = move_player_y(y, gravity);
        gravity = flip_gravity(gravity, key, y);
        refresh();
        key = getch();
    }

    endwin();
    return 0;
}
