#include <stdio.h>   // Pour printf() 
#include <time.h>    // Pour time() 
#include <stdlib.h>  // Pour srand() et rand() 
#include <stdbool.h> // Pour utiliser les booleens true/false
#include <wchar.h>   // Pour les caractères larges

int main()
{
    char **board = init_board();
    display_board(board);
    return 0;
}