#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL3/SDL.h>
#include <stdbool.h>

// Constantes
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TILE_SIZE 32
#define BOARD_SIZE 10

// Structure pour les textures
typedef struct {
    SDL_Texture* grass;
    SDL_Texture* wall;
    SDL_Texture* player;
    SDL_Texture* box;
    SDL_Texture* target;
    SDL_Texture* box_on_target;
} GameTextures;

// Structure principale pour le système graphique
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    GameTextures textures;
    bool is_running;
} Graphics;

// Fonctions principales
int graphics_init(Graphics* graphics);
void graphics_cleanup(Graphics* graphics);
int load_textures(Graphics* graphics);
void render_board(Graphics* graphics, char** board);
void render_game(Graphics* graphics, char** board, int game_state);
bool handle_events(Graphics* graphics, int* move);
void render_text_overlay(Graphics* graphics, int game_state);
SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path);

#endif