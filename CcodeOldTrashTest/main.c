#include <SDL2/SDL.h>
#include <stdio.h>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Multijoueur carré", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, 0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Joueur 1 (blanc)
    int x1 = 100, y1 = 100;
    int size1 = 50;
    int speed1 = 5;

    // Joueur 2 (rouge)
    int x2 = 600, y2 = 400;
    int size2 = 50;
    int speed2 = 5;

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }

        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        // Joueur 1 (ZQSD)
        if (keystate[SDL_SCANCODE_Z]) y1 -= speed1;
        if (keystate[SDL_SCANCODE_S]) y1 += speed1;
        if (keystate[SDL_SCANCODE_Q]) x1 -= speed1;
        if (keystate[SDL_SCANCODE_D]) x1 += speed1;

        // Joueur 2 (flèches)
        if (keystate[SDL_SCANCODE_UP]) y2 -= speed2;
        if (keystate[SDL_SCANCODE_DOWN]) y2 += speed2;
        if (keystate[SDL_SCANCODE_LEFT]) x2 -= speed2;
        if (keystate[SDL_SCANCODE_RIGHT]) x2 += speed2;

        // Nettoyer l'écran
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // noir
        SDL_RenderClear(renderer);

        // Dessiner Joueur 1
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // blanc
        SDL_Rect rect1 = { x1, y1, size1, size1 };
        SDL_RenderFillRect(renderer, &rect1);

        // Dessiner Joueur 2
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // rouge
        SDL_Rect rect2 = { x2, y2, size2, size2 };
        SDL_RenderFillRect(renderer, &rect2);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
