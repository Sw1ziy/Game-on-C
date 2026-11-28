#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_MOTHS 64

typedef struct {
    int move_key;
    int flip_key;
    bool reset;
    bool quit;
    bool pause;
}GameInput;

typedef struct{
    int x;
    int y;
    bool collected;
}Moth;

typedef struct {
    int start_x;
    int start_y;
    int exit_x;
    int exit_y;

    char map[64][128];
    int map_rows;
    int map_cols;

    Moth moth[MAX_MOTHS];
    int moth_count;
}Levels;

void start_game(void);
void draw_game(int x, int y, char bat, int level, int collected, const Levels *data);
void win_screen(int level, bool has_next_level);
void about_screen(void);
void setup_controls(void);

int move_player_x(int x, int y, int key, const Levels *data);
int move_player_y(int x, int y, int gravity, const Levels *data);
int flip_gravity(int gravity, int key, int x, int y, const Levels *data);
int read_move_key(int key);

bool is_wall(const Levels *data, int x, int y);
bool load_start_screen(void);
bool pause_screen(void);
bool secret_level(int number, int total_collected);

int moth_collected(Moth moth[], int moth_count, int x, int y);
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

bool load_start_screen(void) {
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
        mvaddstr(LINES - 1, 0, "Version: Beta 1.8.1");
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
    const char *description = "Help the bat escape the cave and collect moths.";
    int row = LINES / 2 - 4;
    int col = (COLS - (int)strlen(description)) / 2;
    if (col < 0) {
        col = 0;
    }

    erase();
    mvaddstr(row, (COLS - 10) / 2, "ABOUT GAME");
    mvaddstr(row + 2, col, description);
    mvaddstr(row + 3, col, "A/D or Left/Right: Move");
    mvaddstr(row + 4, col, "Space: Flip gravity at a surface");
    mvaddstr(row + 5, col, "P: Pause   R: Reset   Q/Esc: Quit during play");
    mvaddstr(row + 7, col, "Esc: Back to menu");
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

    int collected = 0;
    int total_collected = 0;

    while (true){
        if (current_level.map_rows == 0) {
            erase();
            mvprintw(0, 0, "Cannot load level%d.txt. Press any key to exit.", level);
            refresh();
            timeout(-1);
            getch();
            return;
        }

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
            collected = 0;
            for (int i = 0; i < current_level.moth_count; i++) {
                current_level.moth[i].collected = false;
            }
        }

        x = move_player_x(x, y, input.move_key, &current_level);
        gravity = flip_gravity(gravity, input.flip_key, x, y, &current_level);
        y = move_player_y(x, y, gravity, &current_level);

        collected += moth_collected(current_level.moth, current_level.moth_count, x, y);

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
                bat = 'W';
            } else {
                gravity = 1;
                bat = 'M';
            }
            continue;
        }

        if (is_wall(&current_level, x, y + gravity)) {
            if (gravity < 0) {
                bat = 'W';
            } else {
                bat = 'M';
            }
        }
        draw_game(x, y, bat, level, collected, &current_level);
        napms(50);
    }
}    

void draw_game(int x, int y, char bat, int level, int collected, const Levels *data) {
    erase();
    if (LINES < 2 || COLS < 2) {
        refresh();
        return;
    }

    int rows = data->map_rows;
    if (rows > LINES) {
        rows = LINES - 1;
    }
    int cols = data->map_cols;
    if (cols > COLS) {
        cols = COLS;
    }

    int top = y - rows / 2;
    if (top < 0) {
        top = 0;
    }
    if (top > data->map_rows - rows) {
        top = data->map_rows - rows;
    }
    int left = x - cols / 2;
    if (left < 0) {
        left = 0;
    }
    if (left > data->map_cols - cols) {
        left = data->map_cols - cols;
    }

    int screen_top = (LINES - rows) / 2;
    int screen_left = (COLS - cols) / 2;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            char cell = data->map[top + row][left + col];
            if (cell == '#' || cell == '|' || cell == '+') {
                mvaddch(screen_top + row, screen_left + col, cell);
            }
        }
    }

    int status_row = screen_top + rows;
    if (status_row >= LINES) {
        status_row = LINES - 1;
    }
    if (level == 4) {
        mvaddstr(status_row, screen_left + 2, "Secret Level");
    } else {
        mvprintw(status_row, screen_left + 2, "Level %d/3", level);
    }
    if (data->moth_count > 0) {
        printw(" | Moths %d/%d", collected, data->moth_count);
        for (int i = 0; i < data->moth_count; i++) {
            int row = data->moth[i].y - top;
            int col = data->moth[i].x - left;
            if (!data->moth[i].collected && row >= 0 && row < rows && col >= 0 && col < cols) {
                mvaddch(screen_top + row, screen_left + col, '*');
            }
        }
    }
    int exit_row = data->exit_y - top;
    int exit_col = data->exit_x - left;
    if (exit_row >= 0 && exit_row < rows && exit_col >= 0 && exit_col < cols) {
        if (exit_col == 0 || exit_col == cols - 1
            || is_wall(data, data->exit_x - 1, data->exit_y)
            || is_wall(data, data->exit_x + 1, data->exit_y)) {
            mvaddch(screen_top + exit_row, screen_left + exit_col, '>');
        } else {
            mvaddstr(screen_top + exit_row, screen_left + exit_col - 1, "[>]");
        }
    }
    mvaddch(screen_top + y - top, screen_left + x - left, bat);
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

GameInput read_input_key(void) {
    GameInput input;

    input.move_key = ERR;
    input.flip_key = ERR;
    input.reset = false;
    input.quit = false;
    input.pause = false;

    int key = getch();

    while (key != ERR) {
        int direction = read_move_key(key);

        if(direction != ERR){
            input.move_key = direction;
        }
        if(key == ' '){
            input.flip_key = key;
        }
        if (key == 'r' || key == 'R') {
            input.reset = true;
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

bool is_wall(const Levels *data, int x, int y) {
    if (x < 1 || x >= data->map_cols - 1 || y < 1 || y >= data->map_rows - 1) {
        return true;
    }
    return data->map[y][x] == '#';
}

int move_player_x(int x, int y, int key, const Levels *data) {
    int next = x;
    if (key == 'a' || key == 'A' || key == KEY_LEFT) {
        next--;
    } else if (key == 'd' || key == 'D' || key == KEY_RIGHT) {
        next++;
    }
    if (!is_wall(data, next, y)) {
        return next;
    }
    return x;
}

int flip_gravity(int gravity, int key, int x, int y, const Levels *data) {
    if (is_wall(data, x, y + gravity)) {
        if (key == ' ') {
            gravity = -gravity;
        }
    }
    return gravity;
}

int move_player_y(int x, int y, int gravity, const Levels *data) {
    int jump = y + gravity;
    if (!is_wall(data, x, jump)) {
        y = jump;
    }
    return y;
}

Levels read_level(int number) {
    Levels level = {0};
    char filename[16];

    if (number < 1 || number > 4) {
        return level;
    }

    snprintf(filename, sizeof filename, "level%d.txt", number);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return level;
    }

    while (level.map_rows < 64) {
        int row = level.map_rows;
        if (fgets(level.map[row], sizeof level.map[0], file) == NULL) {
            break;
        }

        int cols = (int)strcspn(level.map[row], "\r\n");
        if (row == 0) {
            level.map_cols = cols;
        }
        if (cols != level.map_cols) {
            level.map_rows = 0;
            break;
        }
        level.map_rows++;
    }
    fclose(file);

    if (level.map_rows < 3 || level.map_cols < 3 || level.map[0][0] != '+') {
        level.map_rows = 0;
        return level;
    }

    int starts = 0;
    int exits = 0;
    for (int row = 1; row < level.map_rows - 1; row++) {
        for (int col = 1; col < level.map_cols - 1; col++) {
            char cell = level.map[row][col];
            if (cell == 'S') {
                level.start_x = col;
                level.start_y = row;
                starts++;
            } else if (cell == 'E') {
                level.exit_x = col;
                level.exit_y = row;
                exits++;
            } else if (cell == '*') {
                if (level.moth_count == MAX_MOTHS) {
                    level.map_rows = 0;
                    return level;
                }

                int i = level.moth_count;
                level.moth[i].x = col;
                level.moth[i].y = row;
                level.moth_count++;
            }
        }
    }

    if (starts != 1 || exits != 1) {
        level.map_rows = 0;
    }
    return level;
}

int moth_collected(Moth moth[], int moth_count, int x, int y) {
    int picked_up = 0;

    for(int i = 0; i < moth_count; i++){
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
