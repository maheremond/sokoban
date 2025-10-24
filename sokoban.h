#ifndef __SOKOBAN_H__
#define __SOKOBAN_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/* board.c */
char ** init_board();
void display_board(char **board);
char ** get_board();
/* moves.c */
int ask_player_move(char **board);
void player_move(char **board, int move);
int check_player_move(char **board, int move);

/* object and player */
typedef struct {
    int x;
    int y;
} Player; 

typedef struct {
    int x;
    int y;
} Box;
typedef struct {
    int x;
    int y;
} Target;