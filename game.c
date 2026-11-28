#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int move_key;
    int flip_key;
    bool reset;
    bool quit;
    bool pause;
}GameInput;

typedef struct {
    int start_x;
    int start_y;
    int exit_x;
    int exit_y;
}Levels;

typedef struct{
    int x;
    int y;
    bool collected;
}Moth;

void start_game(void);
void draw_game(int x, int y, char bat, int level, int exit_x, int exit_y, const Moth moth[], int collected);
void win_screen(int level, bool has_next_level);
void about_screen(void);
void setup_controls(void);

int move_player_x(int x, int key);
int move_player_y(int y, int gravity);
int flip_gravity(int gravity, int key, int y);
int read_move_key(int key);

bool load_start_screen(void);
bool pause_screen(void);
bool secret_level(int number, int total_collected);

int moth_collected(Moth moth[], int x, int y);
GameInput read_input_key(void);
Levels read_level(int number);

int main(void) {
    if (initscr() == NULL) return 1;

    curs_set(0);
    setup_controls();

    if (load_start_screen()) {
        start_game();
    }
    
    endwin();
    return 0;
}

bool load_start_screen(void){
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

    for (int i = 0; i < bar_width + 2; i++) {
        mvaddch(bar_y, bar_x + i, ' ');
    }
    mvaddstr(bar_y + 1, percent_x, "    ");

    int selected = 0;
    int key;

    timeout(-1);
    while (true) {
        int menu_y = LINES / 2 - 1;
        int menu_x = (COLS - 12) / 2;

        erase();
        mvaddstr(menu_y, menu_x + 2, "START");
        mvaddstr(menu_y + 1, menu_x + 2, "ABOUT GAME");
        mvaddstr(menu_y + 2, menu_x + 2, "EXIT");
        mvaddstr(LINES - 1, 0, "Beta 1.8.1");
        mvaddch(menu_y + selected, menu_x, '>');
        refresh();

        key = getch();
        if (key == KEY_UP && selected > 0) {
            selected--;
        } else if (key == KEY_DOWN && selected < 2) {
            selected++;
        } else if (key == '\n' || key == '\r' || key == KEY_ENTER) {
            if (selected == 1) {
                about_screen();
                continue;
            }
            break;
        } else if (key == ERR) {
            break;
        }
    }

    timeout(0);
    return key != ERR && selected == 0;
}

void about_screen(void) {
    int row = LINES / 2 - 4;

    erase();
    mvaddstr(row, (COLS - 10) / 2, "ABOUT GAME");
    mvaddstr(row + 2, 2, "Help the bat escape the cave and collect moths.");
    mvaddstr(row + 3, 2, "A/D or Left/Right: Move");
    mvaddstr(row + 4, 2, "Space: Flip gravity at a surface");
    mvaddstr(row + 5, 2, "P: Pause   R: Reset   Q/Esc: Quit during play");
    mvaddstr(row + 7, 2, "Esc: Back to menu");
    refresh();

    int key = getch();
    while (key != 27 && key != ERR) {
        key = getch();
    }
}

void setup_controls(void){
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    timeout(0);
}

void start_game(void){
    
    int level = 1;
    
    Levels current_level = read_level(level);

    int x = current_level.start_x;
    int y = current_level.start_y;

    int gravity = 1;
    
    char bat = 'M';

    Moth moth[3];
    int collected = 0;
    int total_collected = 0;

    moth[0].x = COLS / 4;
    moth[0].y = LINES - 2;
    moth[0].collected = false;

    moth[1].x = COLS / 2;
    moth[1].y = 1;
    moth[1].collected = false;

    moth[2].x = COLS * 3 / 4;
    moth[2].y = 1;
    moth[2].collected = false;

    while (true){
        GameInput input = read_input_key();
        
        if(input.quit){
            break;
        }

        if(input.pause){
            if (!pause_screen()){
                break;
            }
            continue;
        }

        if(input.reset){
            x = current_level.start_x;
            y = current_level.start_y;
            if(y == 1){
                gravity = -1;
            } else {
                gravity = 1;
            }
            if(level == 1){
                collected = 0;
                for(int i = 0; i < 3; i++){
                    moth[i].collected = false;
                }
            }
        }

        x = move_player_x(x, input.move_key);
        gravity = flip_gravity(gravity, input.flip_key, y);
        y = move_player_y(y, gravity);

        if(level == 1){
            collected += moth_collected(moth, x, y);
        }

        if(x == current_level.exit_x && y == current_level.exit_y){
            total_collected += collected;
            bool has_next_level = level < 3 || secret_level(level, total_collected);
            win_screen(level, has_next_level);
            if (!has_next_level) {
                break;
            }

            level++;
            collected = 0;
            current_level = read_level(level);
            x = current_level.start_x;
            y = current_level.start_y;

            if(y == 1){
                gravity = -1;
            } else {
                gravity = 1;
            }
        }

        if(y == 1){
            bat = 'W';
        } else if(y == LINES - 2){
            bat = 'M';
        }
        draw_game(x, y, bat, level, current_level.exit_x, current_level.exit_y, moth, collected);
        napms(50);
    }
}    

void draw_game(int x, int y, char bat, int level, int exit_x, int exit_y, const Moth moth[], int collected){
    erase();
    box(stdscr, 0, 0);
    mvaddstr(0, 2, "A/D or </> : Move   Space: Flip   Q/Esc: Quit  R: Reset  P: Pause");
    if (level == 4) {
        mvaddstr(LINES - 1, 2, "Secret Level");
    } else {
        mvprintw(LINES - 1, 2, "Level %d/3", level);
    }
    if(level == 1){
        printw(" | Moths %d/3", collected);
        for(int i = 0; i < 3; i++){
            if(!moth[i].collected){
                mvaddch(moth[i].y, moth[i].x, '*');
            }
        }
    }
    mvaddstr(exit_y, exit_x - 1, "[>]");
    mvaddch(y, x, bat);
    refresh();
}

bool pause_screen(void) {
    int selected = 0;
    int key;

    timeout(-1);
    while (true) {
        int menu_y = LINES / 2 - 2;
        int menu_x = (COLS - 10) / 2;

        erase();
        mvaddstr(menu_y, (COLS - 6) / 2, "PAUSED");
        mvaddstr(menu_y + 2, menu_x + 2, "CONTINUE");
        mvaddstr(menu_y + 3, menu_x + 2, "EXIT");
        mvaddch(menu_y + 2 + selected, menu_x, '>');
        refresh();

        key = getch();
        if (key == KEY_UP) {
            selected = 0;
        } else if (key == KEY_DOWN) {
            selected = 1;
        } else if (key == '\n' || key == '\r' || key == KEY_ENTER || key == ERR) {
            break;
        }
    }

    timeout(0);
    return key != ERR && selected == 0;
}

void win_screen(int level, bool has_next_level) {
    const char *message = "Level Complete!";
    const char *prompt = "Press Enter";

    if (!has_next_level) {
        message = "You Win!";
        prompt = "Press Enter to exit";
    } else if (level == 3) {
        message = "Secret Level Unlocked!";
        prompt = "Press Enter to play";
    }

    int message_x = (COLS - (int)strlen(message)) / 2;
    int prompt_x = (COLS - (int)strlen(prompt)) / 2;

    erase();
    mvaddstr(LINES / 2, message_x, message);
    mvaddstr(LINES / 2 + 1, prompt_x, prompt);
    refresh();

    timeout(-1);

    int key = getch();

    while (key != '\n' && key != '\r' && key != KEY_ENTER && key != ERR){
        key = getch();
    }

    timeout(0);
}

GameInput read_input_key(void){
    GameInput input;

    input.move_key = ERR;
    input.flip_key = ERR;
    input.reset = false;
    input.quit = false;
    input.pause = false;

    int key = getch();

    while(key != ERR){
        int direction = read_move_key(key);

        if(direction != ERR){
            input.move_key = direction;
        }
        if(key == ' '){
            input.flip_key = key;
        }
        if(key == 'r' || key == 'R'){
            input.reset = key;
        }
        if(key == 'q' || key == 'Q' || key == 27){
            input.quit = true;
        }

        if(key == 'p' || key == 'P'){
            input.pause = true;
        }

        key = getch();
    }
    
    return input;
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

int move_player_x(int x, int key){
    if ((key == 'a' || key == 'A' || key == KEY_LEFT) && x > 1){
        x--;
    } else if((key == 'd' || key == 'D' || key == KEY_RIGHT) && x < COLS - 2){
        x++;
    }   
    return x;
}

int flip_gravity(int gravity, int key, int y){
    if(y == LINES - 2 || y == 1){
        if(key == ' '){
            gravity = -gravity;           
        }
    }
    return gravity;
}

int move_player_y(int y, int gravity){
    int jump = y + gravity;
    if(jump >= 1 && jump <= LINES - 2){
        y = jump;
    }
    return y;
}

Levels read_level(int number){
    Levels level;

    level.start_x = 1;
    level.start_y = LINES - 2;
    level.exit_x = COLS / 2;
    level.exit_y = LINES - 2;

    if(number == 1){
        level.exit_x = COLS - 3;
    }

    if(number == 2){
        level.exit_x = COLS - 3;
        level.exit_y = 1;
    }

    if(number == 3){
        level.start_y = 1;
        level.exit_x = COLS - 3;
    }

    if(number == 4){
        level.start_y = 1;
        level.exit_x = COLS - 3;
    }

    return level;
}

int moth_collected(Moth moth[], int x, int y){
    int picked_up = 0;

    for(int i = 0; i < 3; i++){
        if(!moth[i].collected && moth[i].x == x && moth[i].y == y){
            moth[i].collected = true;
            picked_up++;
        }
    }

    return picked_up;
}

bool secret_level(int number, int total_collected) {
    return number == 3 && total_collected >= 30;
}
