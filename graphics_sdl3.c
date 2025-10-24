#include "graphics.h"
#include <stdio.h>

// Fonction pour créer une texture colorée simple
SDL_Texture* create_colored_texture(SDL_Renderer* renderer, int r, int g, int b, int size) {
    SDL_Surface* surface = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA8888);
    if (!surface) return NULL;
    
    // SDL3 change: utiliser SDL_MapSurfaceRGBA au lieu de SDL_MapRGBA
    Uint32 color = SDL_MapSurfaceRGBA(surface, r, g, b, 255);
    SDL_FillSurfaceRect(surface, NULL, color);
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    
    return texture;
}

int graphics_init(Graphics* graphics) {
    // Initialiser SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("Erreur SDL_Init: %s\n", SDL_GetError());
        return 0;
    }

    // Créer la fenêtre
    graphics->window = SDL_CreateWindow(
        "Sokoban - Version Graphique SDL3",
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (!graphics->window) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    // Créer le renderer
    graphics->renderer = SDL_CreateRenderer(graphics->window, NULL);

    if (!graphics->renderer) {
        printf("Erreur création renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(graphics->window);
        SDL_Quit();
        return 0;
    }

    graphics->is_running = true;

    // Charger les textures
    if (!load_textures(graphics)) {
        graphics_cleanup(graphics);
        return 0;
    }

    return 1;
}

void graphics_cleanup(Graphics* graphics) {
    // Libérer les textures
    if (graphics->textures.grass) SDL_DestroyTexture(graphics->textures.grass);
    if (graphics->textures.wall) SDL_DestroyTexture(graphics->textures.wall);
    if (graphics->textures.player) SDL_DestroyTexture(graphics->textures.player);
    if (graphics->textures.box) SDL_DestroyTexture(graphics->textures.box);
    if (graphics->textures.target) SDL_DestroyTexture(graphics->textures.target);
    if (graphics->textures.box_on_target) SDL_DestroyTexture(graphics->textures.box_on_target);

    // Libérer SDL
    if (graphics->renderer) SDL_DestroyRenderer(graphics->renderer);
    if (graphics->window) SDL_DestroyWindow(graphics->window);
    SDL_Quit();
}

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path) {
    // Essayer de charger depuis un fichier BMP
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        printf("Impossible de charger %s, utilisation d'une texture colorée\n", path);
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (!texture) {
        printf("Erreur création texture %s: %s\n", path, SDL_GetError());
        return NULL;
    }

    return texture;
}

int load_textures(Graphics* graphics) {
    // Créer des textures colorées avec de meilleures couleurs
    
    graphics->textures.grass = create_colored_texture(graphics->renderer, 34, 139, 34, TILE_SIZE); // Vert herbe
    graphics->textures.wall = create_colored_texture(graphics->renderer, 101, 67, 33, TILE_SIZE);  // Marron foncé
    graphics->textures.player = create_colored_texture(graphics->renderer, 30, 144, 255, TILE_SIZE); // Bleu vif
    graphics->textures.box = create_colored_texture(graphics->renderer, 255, 165, 0, TILE_SIZE);   // Orange (différent du mur)
    graphics->textures.target = create_colored_texture(graphics->renderer, 255, 215, 0, TILE_SIZE); // Or
    graphics->textures.box_on_target = create_colored_texture(graphics->renderer, 255, 140, 0, TILE_SIZE); // Orange foncé

    return 1;
}

void render_board(Graphics* graphics, char** board) {
    // Effacer l'écran
    SDL_SetRenderDrawColor(graphics->renderer, 50, 50, 50, 255);
    SDL_RenderClear(graphics->renderer);

    // Calculer la position de départ pour centrer le plateau
    int start_x = (WINDOW_WIDTH - (BOARD_SIZE * TILE_SIZE)) / 2;
    int start_y = (WINDOW_HEIGHT - (BOARD_SIZE * TILE_SIZE)) / 2;

    // Dessiner le plateau
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            SDL_FRect dest_rect = {
                (float)(start_x + x * TILE_SIZE),
                (float)(start_y + y * TILE_SIZE),
                (float)TILE_SIZE,
                (float)TILE_SIZE
            };

            char cell = board[y][x];

            // Dessiner d'abord le fond (herbe)
            if (graphics->textures.grass) {
                SDL_RenderTexture(graphics->renderer, graphics->textures.grass, NULL, &dest_rect);
            }

            // Dessiner les éléments selon le contenu
            switch (cell) {
                case '#': // Mur
                    if (graphics->textures.wall) {
                        SDL_RenderTexture(graphics->renderer, graphics->textures.wall, NULL, &dest_rect);
                    }
                    break;
                
                case 'O': // Joueur
                    if (graphics->textures.player) {
                        SDL_RenderTexture(graphics->renderer, graphics->textures.player, NULL, &dest_rect);
                    }
                    break;
                
                case 'X': // Caisse
                    if (graphics->textures.box) {
                        SDL_RenderTexture(graphics->renderer, graphics->textures.box, NULL, &dest_rect);
                    }
                    break;
                
                case '.': // Cible
                    if (graphics->textures.target) {
                        SDL_RenderTexture(graphics->renderer, graphics->textures.target, NULL, &dest_rect);
                    }
                    break;
                
                default: // Espace vide (herbe déjà dessinée)
                    break;
            }

            // Dessiner les bordures des cases (optionnel)
            SDL_SetRenderDrawColor(graphics->renderer, 0, 0, 0, 50);
            SDL_RenderRect(graphics->renderer, &dest_rect);
        }
    }

    // Dessiner les bordures du plateau
    SDL_FRect border = {
        (float)(start_x - 2),
        (float)(start_y - 2),
        (float)(BOARD_SIZE * TILE_SIZE + 4),
        (float)(BOARD_SIZE * TILE_SIZE + 4)
    };
    SDL_SetRenderDrawColor(graphics->renderer, 0, 0, 0, 255);
    SDL_RenderRect(graphics->renderer, &border);
}

// Fonction pour dessiner une lettre simple en pixels
void draw_letter(SDL_Renderer* renderer, char letter, int x, int y, int size) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blanc
    
    int pixel_size = size / 8;
    
    switch(letter) {
        case 'Z':
            // Ligne du haut
            for(int i = 0; i < 6; i++) {
                SDL_FRect pixel = {x + i*pixel_size, y, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            // Diagonale
            for(int i = 0; i < 6; i++) {
                SDL_FRect pixel = {x + (5-i)*pixel_size, y + i*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            // Ligne du bas
            for(int i = 0; i < 6; i++) {
                SDL_FRect pixel = {x + i*pixel_size, y + 5*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            break;
            
        case 'Q':
            // Carré avec queue
            for(int i = 1; i < 5; i++) {
                SDL_FRect pixel1 = {x, y + i*pixel_size, pixel_size, pixel_size};
                SDL_FRect pixel2 = {x + 4*pixel_size, y + i*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel1);
                SDL_RenderFillRect(renderer, &pixel2);
            }
            for(int i = 1; i < 4; i++) {
                SDL_FRect pixel1 = {x + i*pixel_size, y, pixel_size, pixel_size};
                SDL_FRect pixel2 = {x + i*pixel_size, y + 4*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel1);
                SDL_RenderFillRect(renderer, &pixel2);
            }
            // Queue
            SDL_FRect pixel = {x + 3*pixel_size, y + 5*pixel_size, pixel_size, pixel_size};
            SDL_RenderFillRect(renderer, &pixel);
            break;
            
        case 'S':
            // Forme S
            for(int i = 0; i < 5; i++) {
                SDL_FRect pixel = {x + i*pixel_size, y, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            SDL_FRect p1 = {x, y + pixel_size, pixel_size, pixel_size};
            SDL_RenderFillRect(renderer, &p1);
            for(int i = 0; i < 4; i++) {
                SDL_FRect pixel = {x + i*pixel_size, y + 2*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            SDL_FRect p2 = {x + 4*pixel_size, y + 3*pixel_size, pixel_size, pixel_size};
            SDL_RenderFillRect(renderer, &p2);
            for(int i = 0; i < 5; i++) {
                SDL_FRect pixel = {x + i*pixel_size, y + 4*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            break;
            
        case 'D':
            // Forme D
            for(int i = 0; i < 5; i++) {
                SDL_FRect pixel = {x, y + i*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            for(int i = 1; i < 4; i++) {
                SDL_FRect pixel1 = {x + i*pixel_size, y, pixel_size, pixel_size};
                SDL_FRect pixel2 = {x + i*pixel_size, y + 4*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel1);
                SDL_RenderFillRect(renderer, &pixel2);
            }
            SDL_FRect pd1 = {x + 4*pixel_size, y + pixel_size, pixel_size, pixel_size};
            SDL_FRect pd2 = {x + 4*pixel_size, y + 3*pixel_size, pixel_size, pixel_size};
            SDL_RenderFillRect(renderer, &pd1);
            SDL_RenderFillRect(renderer, &pd2);
            break;
            
        case 'R':
            // Forme R
            for(int i = 0; i < 5; i++) {
                SDL_FRect pixel = {x, y + i*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            for(int i = 1; i < 4; i++) {
                SDL_FRect pixel = {x + i*pixel_size, y, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            SDL_FRect pr1 = {x + 4*pixel_size, y + pixel_size, pixel_size, pixel_size};
            SDL_RenderFillRect(renderer, &pr1);
            for(int i = 1; i < 4; i++) {
                SDL_FRect pixel = {x + i*pixel_size, y + 2*pixel_size, pixel_size, pixel_size};
                SDL_RenderFillRect(renderer, &pixel);
            }
            SDL_FRect pr2 = {x + 3*pixel_size, y + 3*pixel_size, pixel_size, pixel_size};
            SDL_FRect pr3 = {x + 4*pixel_size, y + 4*pixel_size, pixel_size, pixel_size};
            SDL_RenderFillRect(renderer, &pr2);
            SDL_RenderFillRect(renderer, &pr3);
            break;
    }
}

void render_text_overlay(Graphics* graphics, int game_state) {
    // Zone d'information en bas avec fond semi-transparent
    SDL_SetRenderDrawColor(graphics->renderer, 0, 0, 0, 220);
    SDL_FRect info_bg = {10, WINDOW_HEIGHT - 120, WINDOW_WIDTH - 20, 110};
    SDL_RenderFillRect(graphics->renderer, &info_bg);
    
    SDL_SetRenderDrawColor(graphics->renderer, 255, 255, 255, 255);
    SDL_RenderRect(graphics->renderer, &info_bg);
    
    // Afficher "ZQSD" en pixels
    draw_letter(graphics->renderer, 'Z', 20, WINDOW_HEIGHT - 110, 24);
    draw_letter(graphics->renderer, 'Q', 50, WINDOW_HEIGHT - 110, 24);
    draw_letter(graphics->renderer, 'S', 80, WINDOW_HEIGHT - 110, 24);
    draw_letter(graphics->renderer, 'D', 110, WINDOW_HEIGHT - 110, 24);
    
    // Petite flèche après ZQSD
    SDL_SetRenderDrawColor(graphics->renderer, 255, 255, 255, 255);
    SDL_FRect arrow1 = {150, WINDOW_HEIGHT - 100, 15, 3};
    SDL_RenderFillRect(graphics->renderer, &arrow1);
    SDL_FRect arrow_head1 = {160, WINDOW_HEIGHT - 103, 8, 3};
    SDL_FRect arrow_head2 = {160, WINDOW_HEIGHT - 94, 8, 3};
    SDL_RenderFillRect(graphics->renderer, &arrow_head1);
    SDL_RenderFillRect(graphics->renderer, &arrow_head2);
    
    // Zone "R = Reset" avec couleur verte
    SDL_SetRenderDrawColor(graphics->renderer, 50, 205, 50, 255);
    SDL_FRect reset_bg = {20, WINDOW_HEIGHT - 75, 60, 20};
    SDL_RenderFillRect(graphics->renderer, &reset_bg);
    draw_letter(graphics->renderer, 'R', 25, WINDOW_HEIGHT - 72, 16);
    
    // Zone "Échap = Quitter" avec couleur rouge
    SDL_SetRenderDrawColor(graphics->renderer, 255, 69, 0, 255);
    SDL_FRect quit_bg = {20, WINDOW_HEIGHT - 50, 100, 20};
    SDL_RenderFillRect(graphics->renderer, &quit_bg);
    
    // Afficher l'objectif avec des symboles colorés à droite
    // Texte "Objectif:" simulé avec une ligne blanche
    SDL_SetRenderDrawColor(graphics->renderer, 255, 255, 255, 255);
    SDL_FRect objective_line = {WINDOW_WIDTH - 200, WINDOW_HEIGHT - 105, 80, 2};
    SDL_RenderFillRect(graphics->renderer, &objective_line);
    
    // Caisse (orange) -> Cible (or)
    SDL_SetRenderDrawColor(graphics->renderer, 255, 165, 0, 255); // Orange (caisse)
    SDL_FRect box_symbol = {WINDOW_WIDTH - 120, WINDOW_HEIGHT - 90, 20, 20};
    SDL_RenderFillRect(graphics->renderer, &box_symbol);
    
    // Flèche "→"
    SDL_SetRenderDrawColor(graphics->renderer, 255, 255, 255, 255);
    SDL_FRect arrow = {WINDOW_WIDTH - 90, WINDOW_HEIGHT - 82, 15, 3};
    SDL_RenderFillRect(graphics->renderer, &arrow);
    SDL_FRect arrow_head_r1 = {WINDOW_WIDTH - 80, WINDOW_HEIGHT - 85, 8, 3};
    SDL_FRect arrow_head_r2 = {WINDOW_WIDTH - 80, WINDOW_HEIGHT - 76, 8, 3};
    SDL_RenderFillRect(graphics->renderer, &arrow_head_r1);
    SDL_RenderFillRect(graphics->renderer, &arrow_head_r2);
    
    SDL_SetRenderDrawColor(graphics->renderer, 255, 215, 0, 255); // Or (cible)
    SDL_FRect target_symbol = {WINDOW_WIDTH - 60, WINDOW_HEIGHT - 90, 20, 20};
    SDL_RenderFillRect(graphics->renderer, &target_symbol);
    
    // Messages selon l'état du jeu
    if (game_state == 1) { // Victoire
        SDL_SetRenderDrawColor(graphics->renderer, 0, 255, 0, 240);
        SDL_FRect win_bg = {WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 40, 200, 80};
        SDL_RenderFillRect(graphics->renderer, &win_bg);
        
        SDL_SetRenderDrawColor(graphics->renderer, 255, 255, 255, 255);
        SDL_RenderRect(graphics->renderer, &win_bg);
        
        // Grande étoile de victoire
        SDL_SetRenderDrawColor(graphics->renderer, 255, 215, 0, 255);
        SDL_FRect star = {WINDOW_WIDTH/2 - 15, WINDOW_HEIGHT/2 - 15, 30, 30};
        SDL_RenderFillRect(graphics->renderer, &star);
        
        // Texte "WIN!" simulé
        SDL_SetRenderDrawColor(graphics->renderer, 255, 255, 255, 255);
        SDL_FRect win_text = {WINDOW_WIDTH/2 - 20, WINDOW_HEIGHT/2 + 25, 40, 8};
        SDL_RenderFillRect(graphics->renderer, &win_text);
        
    } else if (game_state == 2) { // Défaite
        SDL_SetRenderDrawColor(graphics->renderer, 255, 0, 0, 240);
        SDL_FRect lose_bg = {WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 40, 200, 80};
        SDL_RenderFillRect(graphics->renderer, &lose_bg);
        
        SDL_SetRenderDrawColor(graphics->renderer, 255, 255, 255, 255);
        SDL_RenderRect(graphics->renderer, &lose_bg);
        
        // Grand X pour défaite
        SDL_SetRenderDrawColor(graphics->renderer, 139, 0, 0, 255);
        // Diagonal 1
        for(int i = 0; i < 20; i++) {
            SDL_FRect pixel = {WINDOW_WIDTH/2 - 10 + i, WINDOW_HEIGHT/2 - 10 + i, 2, 2};
            SDL_RenderFillRect(graphics->renderer, &pixel);
        }
        // Diagonal 2
        for(int i = 0; i < 20; i++) {
            SDL_FRect pixel = {WINDOW_WIDTH/2 + 10 - i, WINDOW_HEIGHT/2 - 10 + i, 2, 2};
            SDL_RenderFillRect(graphics->renderer, &pixel);
        }
        
        // Texte "LOST" simulé
        SDL_SetRenderDrawColor(graphics->renderer, 255, 255, 255, 255);
        SDL_FRect lose_text = {WINDOW_WIDTH/2 - 25, WINDOW_HEIGHT/2 + 25, 50, 8};
        SDL_RenderFillRect(graphics->renderer, &lose_text);
    }
}

void render_game(Graphics* graphics, char** board, int game_state) {
    render_board(graphics, board);
    render_text_overlay(graphics, game_state);
    SDL_RenderPresent(graphics->renderer);
}

bool handle_events(Graphics* graphics, int* move) {
    SDL_Event event;
    *move = -1; // Pas de mouvement par défaut

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                graphics->is_running = false;
                *move = 0; // Quitter
                return false;

            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key) {
                    case SDLK_Z:
                    case SDLK_UP:
                        *move = 1; // Haut
                        return true;
                    
                    case SDLK_S:
                    case SDLK_DOWN:
                        *move = 2; // Bas
                        return true;
                    
                    case SDLK_Q:
                    case SDLK_LEFT:
                        *move = 3; // Gauche
                        return true;
                    
                    case SDLK_D:
                    case SDLK_RIGHT:
                        *move = 4; // Droite
                        return true;
                    
                    case SDLK_R:
                        *move = 5; // Reset
                        return true;
                    
                    case SDLK_ESCAPE:
                        graphics->is_running = false;
                        *move = 0; // Quitter
                        return false;
                }
                break;
        }
    }

    return graphics->is_running;
}