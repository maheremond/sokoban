#include "sokoban.h"

// Variables globales pour la position de la cible
int target_x_pos = -1, target_y_pos = -1;

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

char **get_board()
{
    char **board;
    int player_x, player_y, box_x, box_y, target_x, target_y;
    int attempts = 0;
    int level_ok = 0;
    
    // Initialiser le generateur de nombres aleatoires
    srand(time(NULL));
    
    while (!level_ok && attempts < 100) // Essayer maximum 100 fois
    {
        board = init_board();
        
        // Generer quelques murs aleatoires
        generate_random_walls(board);
        
        // Generer position aleatoire pour le joueur (pas sur les bords ni sur un mur)
        do {
            player_x = 1 + rand() % 8;
            player_y = 1 + rand() % 8;
        } while (board[player_y][player_x] != ' ');
        
        // Generer position aleatoire pour la caisse
        do {
            box_x = 1 + rand() % 8;
            box_y = 1 + rand() % 8;
        } while (board[box_y][box_x] != ' ' || (box_x == player_x && box_y == player_y));
        
        // Generer position aleatoire pour la cible
        do {
            target_x = 1 + rand() % 8;
            target_y = 1 + rand() % 8;
        } while (board[target_y][target_x] != ' ' || 
                 (target_x == player_x && target_y == player_y) ||
                 (target_x == box_x && target_y == box_y));
        
        // Verifier que le niveau est solvable
        if (is_level_solvable(board, player_x, player_y, box_x, box_y, target_x, target_y))
        {
            level_ok = 1;
            
            // Sauvegarder la position de la cible
            target_x_pos = target_x;
            target_y_pos = target_y;
            
            // Placer les elements
            board[player_y][player_x] = 'O';  // Joueur
            board[box_y][box_x] = 'X';        // Caisse
            board[target_y][target_x] = '.';  // Cible
        }
        else
        {
            // Liberer le plateau et reessayer
            free_board(board);
            attempts++;
        }
    }
    
    // Si on n'a pas trouve de niveau valide, creer un niveau simple
    if (!level_ok)
    {
        board = init_board();
        board[5][5] = 'O';  // Joueur au centre
        board[5][6] = 'X';  // Caisse a droite
        board[5][7] = '.';  // Cible plus a droite
        
        // Sauvegarder la position de la cible
        target_x_pos = 7;
        target_y_pos = 5;
    }
    
    return board;
}

void free_board(char **board)
{
    int i;
    for (i = 0; i < 10; i++)
    {
        free(board[i]);
    }
    free(board);
}

int check_player_move(char **board, int move)
{
    int player_x = -1, player_y = -1;
    int new_x, new_y;
    int i, j;
    
    // Trouver la position du joueur
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (board[i][j] == 'O')
            {
                player_x = j;
                player_y = i;
                break;
            }
        }
        if (player_x != -1) break;
    }
    
    if (player_x == -1) return 0; // Joueur non trouvé
    
    // Calculer la nouvelle position
    new_x = player_x;
    new_y = player_y;
    
    switch(move)
    {
        case 1: new_y--; break; // Haut
        case 2: new_y++; break; // Bas
        case 3: new_x--; break; // Gauche
        case 4: new_x++; break; // Droite
        default: return 0;
    }
    
    // Vérifier les limites du plateau
    if (new_x < 0 || new_x >= 10 || new_y < 0 || new_y >= 10)
        return 0;
    
    // Si c'est un espace vide ou une cible
    if (board[new_y][new_x] == ' ' || board[new_y][new_x] == '.')
        return 1;
    
    // Si c'est une caisse
    if (board[new_y][new_x] == 'X')
    {
        int box_new_x = new_x;
        int box_new_y = new_y;
        
        // Calculer où la caisse irait
        switch(move)
        {
            case 1: box_new_y--; break; // Haut
            case 2: box_new_y++; break; // Bas
            case 3: box_new_x--; break; // Gauche
            case 4: box_new_x++; break; // Droite
        }
        
        // Vérifier les limites pour la caisse
        if (box_new_x < 0 || box_new_x >= 10 || box_new_y < 0 || box_new_y >= 10)
            return 0;
        
        // La caisse peut être poussée si la destination est vide ou une cible
        if (board[box_new_y][box_new_x] == ' ' || board[box_new_y][box_new_x] == '.')
            return 2; // Mouvement avec poussée de caisse
    }
    
    return 0; // Mouvement impossible
}

void player_move(char **board, int move)
{
    int player_x = -1, player_y = -1;
    int new_x, new_y;
    int i, j;
    int move_type;
    
    move_type = check_player_move(board, move);
    if (move_type == 0) return; // Mouvement impossible
    
    // Trouver la position du joueur
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (board[i][j] == 'O')
            {
                player_x = j;
                player_y = i;
                break;
            }
        }
        if (player_x != -1) break;
    }
    
    // Calculer la nouvelle position
    new_x = player_x;
    new_y = player_y;
    
    switch(move)
    {
        case 1: new_y--; break; // Haut
        case 2: new_y++; break; // Bas
        case 3: new_x--; break; // Gauche
        case 4: new_x++; break; // Droite
    }
    
    // Si on pousse une caisse
    if (move_type == 2)
    {
        int box_new_x = new_x;
        int box_new_y = new_y;
        
        // Calculer où la caisse va
        switch(move)
        {
            case 1: box_new_y--; break; // Haut
            case 2: box_new_y++; break; // Bas
            case 3: box_new_x--; break; // Gauche
            case 4: box_new_x++; break; // Droite
        }
        
        // Déplacer la caisse
        board[box_new_y][box_new_x] = 'X';
    }
    
    // Restaurer la cible si le joueur était dessus
    if (player_x == target_x_pos && player_y == target_y_pos)
    {
        board[player_y][player_x] = '.'; // Remettre la cible
    }
    else
    {
        board[player_y][player_x] = ' '; // Effacer l'ancienne position
    }
    
    // Déplacer le joueur
    board[new_y][new_x] = 'O'; // Nouvelle position
}

int is_game_won(char **board, int target_x, int target_y)
{
    int i, j;
    int box_x = -1, box_y = -1;
    
    // Trouver la position de la caisse
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (board[i][j] == 'X')
            {
                box_x = j;
                box_y = i;
                break;
            }
        }
        if (box_x != -1) break;
    }
    
    // Vérifier si la caisse est sur la cible
    if (box_x == target_x && box_y == target_y)
    {
        return 1; // Victoire !
    }
    
    return 0; // Pas encore gagné
}

int is_game_lost(char **board)
{
    int i, j;
    int box_x = -1, box_y = -1;
    
    // Trouver la position de la caisse
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (board[i][j] == 'X')
            {
                box_x = j;
                box_y = i;
                break;
            }
        }
        if (box_x != -1) break;
    }
    
    if (box_x == -1) return 0; // Pas de caisse trouvée
    
    // Vérifier si la caisse est dans un coin ET qu'elle n'est pas sur la cible
    int on_target = 0;
    // Vérifier si la position de la caisse correspond à une cible
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (board[i][j] == '.' && i == box_y && j == box_x)
            {
                on_target = 1;
                break;
            }
        }
        if (on_target) break;
    }
    
    // Si la caisse est sur la cible, pas de défaite
    if (on_target) return 0;
    
    // Vérifier si la caisse est dans un coin ou contre un mur de façon irréversible
    int blocked_up = (box_y == 0) || (board[box_y-1][box_x] == '#');
    int blocked_down = (box_y == 9) || (board[box_y+1][box_x] == '#');
    int blocked_left = (box_x == 0) || (board[box_y][box_x-1] == '#');
    int blocked_right = (box_x == 9) || (board[box_y][box_x+1] == '#');
    
    // Si bloquée dans un coin
    if ((blocked_up && blocked_left) || (blocked_up && blocked_right) || 
        (blocked_down && blocked_left) || (blocked_down && blocked_right))
    {
        return 1; // Partie perdue
    }
    
    return 0; // Jeu continue
}

void save_game_state(char **board)
{
    FILE *file;
    int i, j;
    
    file = fopen("end.txt", "w");
    if (file == NULL)
    {
        printf("Erreur: impossible de creer le fichier end.txt\n");
        return;
    }
    
    // Ecrire la bordure superieure
    for (j = 0; j < 12; j++)
    {
        fprintf(file, "#");
    }
    fprintf(file, "\n");
    
    // Ecrire l'etat du plateau avec bordures
    for (i = 0; i < 10; i++)
    {
        fprintf(file, "#");
        for (j = 0; j < 10; j++)
        {
            fprintf(file, "%c", board[i][j]);
        }
        fprintf(file, "#\n");
    }
    
    // Ecrire la bordure inferieure
    for (j = 0; j < 12; j++)
    {
        fprintf(file, "#");
    }
    fprintf(file, "\n");
    
    fclose(file);
}

void generate_random_walls(char **board)
{
    int i;
    int num_walls = 8 + rand() % 5; // Entre 8 et 12 murs
    
    // Ajouter quelques murs aleatoires (pas sur les bords)
    for (i = 0; i < num_walls; i++)
    {
        int wall_x, wall_y;
        do {
            wall_x = 1 + rand() % 8; // Entre 1 et 8
            wall_y = 1 + rand() % 8; // Entre 1 et 8
        } while (board[wall_y][wall_x] != ' ');
        
        board[wall_y][wall_x] = '#';
    }
}

int is_position_reachable(char **board, int start_x, int start_y, int target_x, int target_y)
{
    char **visited;
    int i, j;
    int queue_x[100], queue_y[100];
    int front = 0, rear = 0;
    
    // Allouer le tableau visite
    visited = malloc(10 * sizeof(*visited));
    for (i = 0; i < 10; i++)
    {
        visited[i] = malloc(10 * sizeof(char));
        for (j = 0; j < 10; j++)
        {
            visited[i][j] = 0;
        }
    }
    
    // BFS pour verifier l'accessibilite
    queue_x[rear] = start_x;
    queue_y[rear] = start_y;
    rear++;
    visited[start_y][start_x] = 1;
    
    while (front < rear)
    {
        int x = queue_x[front];
        int y = queue_y[front];
        front++;
        
        if (x == target_x && y == target_y)
        {
            // Liberer la memoire
            for (i = 0; i < 10; i++)
                free(visited[i]);
            free(visited);
            return 1; // Accessible
        }
        
        // Verifier les 4 directions
        int dx[] = {0, 0, -1, 1};
        int dy[] = {-1, 1, 0, 0};
        
        for (i = 0; i < 4; i++)
        {
            int new_x = x + dx[i];
            int new_y = y + dy[i];
            
            if (new_x >= 0 && new_x < 10 && new_y >= 0 && new_y < 10 &&
                !visited[new_y][new_x] && board[new_y][new_x] != '#')
            {
                visited[new_y][new_x] = 1;
                queue_x[rear] = new_x;
                queue_y[rear] = new_y;
                rear++;
            }
        }
    }
    
    // Liberer la memoire
    for (i = 0; i < 10; i++)
        free(visited[i]);
    free(visited);
    
    return 0; // Non accessible
}

int is_level_solvable(char **board, int player_x, int player_y, int box_x, int box_y, int target_x, int target_y)
{
    // 1. Le joueur doit pouvoir atteindre la caisse
    if (!is_position_reachable(board, player_x, player_y, box_x, box_y))
        return 0;
    
    // 2. La caisse ne doit pas etre dans un coin
    int blocked_up = (box_y == 0) || (board[box_y-1][box_x] == '#');
    int blocked_down = (box_y == 9) || (board[box_y+1][box_x] == '#');
    int blocked_left = (box_x == 0) || (board[box_y][box_x-1] == '#');
    int blocked_right = (box_x == 9) || (board[box_y][box_x+1] == '#');
    
    if ((blocked_up && blocked_left) || (blocked_up && blocked_right) || 
        (blocked_down && blocked_left) || (blocked_down && blocked_right))
        return 0; // Caisse dans un coin
    
    // 3. Il doit y avoir au moins une direction libre autour de la cible
    // pour pouvoir pousser la caisse dessus
    int target_accessible = 0;
    if (target_x > 0 && board[target_y][target_x-1] != '#') target_accessible = 1;
    if (target_x < 9 && board[target_y][target_x+1] != '#') target_accessible = 1;
    if (target_y > 0 && board[target_y-1][target_x] != '#') target_accessible = 1;
    if (target_y < 9 && board[target_y+1][target_x] != '#') target_accessible = 1;
    
    if (!target_accessible) return 0;
    
    // 4. Verification basique: la caisse doit pouvoir aller vers la cible
    // (simulation simplifiee - on verifie qu'il y a un chemin libre)
    return is_position_reachable(board, box_x, box_y, target_x, target_y);
}