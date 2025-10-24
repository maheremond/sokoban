#ifndef __SOKOBAN_H__
#define __SOKOBAN_H__

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* global variables for targets */
extern int target_x_pos, target_y_pos;

/* board.c */
char **init_board();
char **get_board();
void free_board(char **board);

/* movement functions */
void player_move(char **board, int move);
int check_player_move(char **board, int move);
int is_game_won(char **board, int target_x, int target_y);
int is_game_lost(char **board);
void save_game_state(char **board);

/* level generation */
void generate_random_walls(char **board);
int is_position_reachable(char **board, int start_x, int start_y, int target_x, int target_y);
int is_level_solvable(char **board, int player_x, int player_y, int box_x, int box_y, int target_x, int target_y);

#endif