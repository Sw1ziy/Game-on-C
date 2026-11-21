#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int move_key;
    int flip_key;
    bool reset;
    bool quit;
}GameInput;

void load_start_screen(void);
int move_player_x(int x, int key);
int move_player_y(int y, int gravity);
int flip_gravity(int gravity, int key, int y);
int read_move_key(int key);
GameInput read_input_key();




int main(void) {
    if (initscr() == NULL) return 1;

    curs_set(0);
    load_start_screen();

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(0);

    int key = 0;
    int x = 1;
    int y = LINES - 2;
    int gravity = 1;
    char bat = 'M';

    while (key != 'q' && key != 'Q' && key != 27){
        erase();
        box(stdscr, 0, 0);
        mvaddstr(0, 2, "A/D or </> : Move   Space: Flip   Q/Esc: Quit  R: Reset");

        if(key == 'r' || key == 'R'){
            x = 1;
            y = LINES - 2;
            gravity = 1;
        }

        int move_key = read_move_key(key);


        x = move_player_x(x, move_key);
        gravity = flip_gravity(gravity, key, y);
        y = move_player_y(y, gravity);


        if(y == 1){
            bat = 'W';
        } else if(y == LINES - 2){
            bat = 'M';
        }
        mvaddch(y, x, bat);
        refresh();
        napms(50);
        key = getch();
    }

    endwin();
    return 0;
}

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
        
void load_start_screen(void){

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
    int percent_x = (COLS - 4) / 2;

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

        mvprintw(bar_y + 1, percent_x, "%3d%%", percent);

        refresh();

        if(percent < 100){
            napms(40);
        }
    }

    for (int frame = 0; frame < 6; frame++) {
        if (frame % 2 == 0) {
            mvaddstr(bar_y + 1, percent_x, "100%");
        } else {
            mvaddstr(bar_y + 1, percent_x, "    ");
        }
        refresh();
        napms(250);
    }

    mvhline(bar_y, bar_x, ' ', bar_width + 2);
    mvaddstr(bar_y + 1, percent_x, "    ");
    mvaddstr(bar_y + 2, (COLS - 5) / 2, "START");
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
    if ((key == 'a' || key == 'A' || key == KEY_LEFT) && x > 1){
        x--;
    } else if((key == 'd' || key == 'D' || key == KEY_RIGHT) && x < COLS - 2){
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

int read_move_key(int key){
    if(key == 'a' ||
        key == 'A' ||
        key == 'd' ||
        key == 'D' ||
        key == KEY_LEFT ||
        key == KEY_RIGHT){
        
        return key;
    }
    return ERR;
}

GameInput read_input_key(void){

    
}