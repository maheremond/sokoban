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

char **get_board()
{
    char **board;
    int i;
    
    board = init_board();
    
    // Créer les murs extérieurs uniquement
    i = 0;
    while (i < 10)
    {
        board[0][i] = '#';  // Mur haut
        board[9][i] = '#';  // Mur bas
        board[i][0] = '#';  // Mur gauche
        board[i][9] = '#';  // Mur droite
        i++;
    }
    
    // Placer le joueur
    board[5][5] = '@';
    
    // Placer 1 caisse
    board[4][4] = '$';
    
    // Placer 1 cible
    board[2][2] = '.';
    
    return board;
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