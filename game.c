#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
int main(void)
{
    srand((unsigned) time(NULL));          // инициализация генератора случайных чисел
    int secret = rand() % 100 + 1;         // число от 1 до 100
    int guess = 0;
    int tries = 0;
 
    printf("Я загадал число от 1 до 100. Угадай!\n");
 
    while (guess != secret) {
        printf("Твой вариант: ");
 
        if (scanf("%d", &guess) != 1) {    // ввели не число
            printf("Это не число. Попробуй ещё раз.\n");
            while (getchar() != '\n');     // очистить ввод
            continue;
        }
 
        tries++;
 
        if (guess < secret) {
            printf("Больше!\n");
        } else if (guess > secret) {
            printf("Меньше!\n");
        } else {
            printf("Угадал за %d попыток!\n", tries);
        }
    }
 
    return 0;
}
