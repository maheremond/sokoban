#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "sokoban.h"
#include "graphics.h"

int main()
{
    char **board;
    int move;
    Graphics graphics = {0};
    int game_state = 0; // 0=en cours, 1=victoire, 2=défaite
    
    printf("=== SOKOBAN GRAPHIQUE ===\n");
    
    // Initialiser SDL3
    if (!graphics_init(&graphics)) {
        printf("Erreur: Impossible d'initialiser SDL3\n");
        return 1;
    }
    
    board = get_board();
    
    // Boucle de jeu principale
    while (graphics.is_running)
    {
        // Gérer les événements
        if (!handle_events(&graphics, &move)) {
            break; // Quitter
        }
        
        // Traiter les mouvements seulement si le jeu est en cours
        if (move != -1 && game_state == 0) {
            if (move == 0) // Quitter
            {
                break;
            }
            else if (move == 5) // Reset
            {
                free_board(board);
                board = get_board();
                game_state = 0;
            }
            else if (move >= 1 && move <= 4) // Mouvements
            {
                if (check_player_move(board, move))
                {
                    player_move(board, move);
                    
                    // Vérifier victoire/défaite
                    if (is_game_won(board, target_x_pos, target_y_pos))
                    {
                        game_state = 1; // Victoire
                        save_game_state(board);
                    }
                    else if (is_game_lost(board))
                    {
                        game_state = 2; // Défaite
                        save_game_state(board);
                    }
                }
            }
        }
        else if (move != -1 && game_state != 0) {
            // Gérer reset ou quitter après victoire/défaite
            if (move == 5) { // Reset
                free_board(board);
                board = get_board();
                game_state = 0;
            } else if (move == 0) { // Quitter
                break;
            }
        }
        
        // Afficher le jeu
        render_game(&graphics, board, game_state);
        
        SDL_Delay(16); // ~60 FPS
    }
    
    // Nettoyage
    free_board(board);
    graphics_cleanup(&graphics);
    
    return 0;
}