#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400
#define GROUND_Y (WINDOW_HEIGHT - 50)
#define GRAVITY 0.5f
#define JUMP_STRENGTH -12.0f
#define OBSTACLE_WIDTH 20
#define OBSTACLE_HEIGHT 50
#define OBSTACLE_SPEED 5

typedef struct {
    float x, y;
    float velocity;
    bool isJumping;
} Dinosaur;

typedef struct {
    float x, y;
    bool active;
} Obstacle;

void initGame(Dinosaur *dino, Obstacle *obs) {
    dino->x = 50;
    dino->y = GROUND_Y;
    dino->velocity = 0.0f;
    dino->isJumping = false;

    obs->x = WINDOW_WIDTH;
    obs->y = GROUND_Y - OBSTACLE_HEIGHT;
    obs->active = true;
}

void handleInput(SDL_Event *event, Dinosaur *dino) 
{
    if (event->type == SDL_EVENT_KEY_DOWN) {
        if ((event->key.key == SDLK_SPACE || event->key.key == SDLK_W || event->key.key == SDLK_UP)  && !dino->isJumping) 
        {
            dino->velocity = JUMP_STRENGTH;
            dino->isJumping = true;
        }
    }
}

void updateGame(Dinosaur *dino, Obstacle *obs) {
    // Update dinosaur position
    dino->velocity += GRAVITY;
    dino->y += dino->velocity;
    if (dino->y >= GROUND_Y) {
        dino->y = GROUND_Y;
        dino->isJumping = false;
        dino->velocity = 0.0f;
    }

    // Update obstacle position
    if (obs->active) {
        obs->x -= OBSTACLE_SPEED;
        if (obs->x + OBSTACLE_WIDTH < 0) {
            obs->x = WINDOW_WIDTH;
            obs->active = true;
        }
    }
}

bool checkCollision(Dinosaur *dino, Obstacle *obs) {
    if (obs->active &&
        dino->x + 50 > obs->x &&
        dino->x < obs->x + OBSTACLE_WIDTH &&
        dino->y + 50 > obs->y &&
        dino->y < obs->y + OBSTACLE_HEIGHT) {
        return true;
    }
    return false;
}

void renderGame(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    // Draw ground
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //ground
    SDL_Rect groundRect = {0, GROUND_Y, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &groundRect);

    // Draw dinosaur
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //dinosaur
    SDL_Rect dinoRect = {(int)dino->x, (int)dino->y, 50, 50};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Draw obstacle
    if (obs->active) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //obstacle
        SDL_Rect obsRect = {(int)obs->x, (int)obs->y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT};
        SDL_RenderFillRect(renderer, &obsRect);
    }

    SDL_RenderPresent(renderer);
}

int main(int agrc, char *argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Dino Game", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if (!window || !renderer) {
        SDL_Log("Failed to create window or renderer: %s", SDL_GetError());
        return 1;
    }

    Dinosaur dino;
    Obstacle obs;
    initGame(&dino, &obs);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            handleInput(&event, &dino);
        }

        updateGame(&dino, &obs);

        if (checkCollision(&dino, &obs)) {
            // Handle collision (e.g., reset game)
            initGame(&dino, &obs);
        }

        renderGame(renderer, &dino, &obs);

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}