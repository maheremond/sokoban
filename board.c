#include "sokoban.h"

char **init_board()
{
    char **board;
    int i, j;

    board = malloc(11 * sizeof(*board));
    board[10] = NULL;

    i = 0;
    while (i < 10)
    {
        board[i] = malloc(11 * sizeof(char));
        board[i][10] = '\0';
        
        j = 0;
        while (j < 10)
        {
            board[i][j] = ' ';
            j++;
        }
        i++;
    }
    
    return board;
}

void display_board(char **board)
{
    int i, j;

    printf("\n");
    
    // Bordure supérieure
    printf("+----------+\n");
    
    // Affichage du contenu
    i = 0;
    while (i < 10)
    {
        printf("|");
        j = 0;
        while (j < 10)
        {
            printf("%c", board[i][j]);
            j++;
        }
        printf("|\n");
        i++;
    }
    
    // Bordure inférieure
    printf("+----------+\n");
    printf("\n");
}

void free_board(char **board)
{
    int i;
    
    i = 0;
    while (i < 10)
    {
        free(board[i]);
        i++;
    }
    free(board);
}