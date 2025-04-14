#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400
#define GROUND_Y (WINDOW_HEIGHT - 50)
#define GRAVITY 0.5f
#define JUMP_STRENGTH -12.0f
#define OBSTACLE_SPEED 5

int score = 0;
int highScore = 0;
int scoreTimer = 0; 

typedef struct {
    float x, y;
    float velocity;
    bool isJumping;
} Dinosaur;

typedef enum {
    OBSTACLE_CACTUS,
    OBSTACLE_ROCK
} ObstacleType;

typedef struct {
    float x, y;
    bool active;
    int width, height;
    ObstacleType type;
} Obstacle;

void initGame(Dinosaur *dino, Obstacle *obs) {
    dino->x = 50;
    dino->y = GROUND_Y;
    dino->velocity = 0.0f;
    dino->isJumping = false;
    obs->x = WINDOW_WIDTH;
    int type = rand() % 2; // 0 or 1
    obs->type = (ObstacleType)type;
    if (obs->type == OBSTACLE_CACTUS) 
    {
        obs->width = 20;
        obs->height = 50 + rand() % 30; // 50–80 tall
    } 
    else 
    {
        obs->width = 40 + rand() % 30; // 40–70 wide
        obs->height = 20 + rand() % 10; // 20–30 tall
    }

    obs->y = GROUND_Y - obs->height;
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
    // Updates dinosaur position
    dino->velocity += GRAVITY;
    dino->y += dino->velocity;
    if (dino->y >= GROUND_Y) {
        dino->y = GROUND_Y;
        dino->isJumping = false;
        dino->velocity = 0.0f;
    }

    // Updates obstacle position
    if (obs->x + obs->width < 0) {
        obs->x = WINDOW_WIDTH;
        int type = rand() % 2;
        obs->type = (ObstacleType)type;
        if (obs->type == OBSTACLE_CACTUS) 
        {
            obs->width = 20;
            obs->height = 50 + rand() % 30;
        } 
        else 
        {
            obs->width = 40 + rand() % 30;
            obs->height = 20 + rand() % 10;
        }
        obs->y = GROUND_Y - obs->height;
    }
}

bool checkCollision(Dinosaur *dino, Obstacle *obs) {
    if (obs->active &&
        dino->x + 50 > obs->x &&
        dino->x < obs->x + obs->width &&
        dino->y + 50 > obs->y &&
        dino->y < obs->y + obs->height) {
        return true;
    }
    return false;
}

void renderGame(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    // Draw ground
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Black ground
    SDL_Rect groundRect = {0, GROUND_Y, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &groundRect);

    // Draw dinosaur
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Back dinosaur
    SDL_Rect dinoRect = {(int)dino->x, (int)dino->y, 50, 50};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Draw obstacle
    if (obs->active) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Black obstacle
        SDL_Rect obsRect = {(int)obs->x, (int)obs->y, obs->width, obs->height};
        SDL_RenderFillRect(renderer, &obsRect);
    }

    SDL_RenderPresent(renderer);
}
void renderGameReversed(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs) {
    SDL_SetRenderDrawColor(renderer, 0, 0,0, 255); // Black background
    SDL_RenderClear(renderer);

    // Draw ground
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //White ground
    SDL_Rect groundRect = {0, GROUND_Y, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &groundRect);

    // Draw dinosaur
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //White dinosaur
    SDL_Rect dinoRect = {(int)dino->x, (int)dino->y, 50, 50};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Draw obstacle
    if (obs->active) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //White obstacle
        SDL_Rect obsRect = {(int)obs->x, (int)obs->y, obs->width, obs->height};
        SDL_RenderFillRect(renderer, &obsRect);
    }

    SDL_RenderPresent(renderer);
}
void renderGameColored(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blue background
    SDL_RenderClear(renderer);

    // Draw ground
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Green ground
    SDL_Rect groundRect = {0, GROUND_Y, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &groundRect);

    // Draw dinosaur
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Brown dinosaur
    SDL_Rect dinoRect = {(int)dino->x, (int)dino->y, 50, 50};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Draw obstacle
    if (obs->active) 
    {
        if (obs->type == OBSTACLE_CACTUS)
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); //Green cacti
        else
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); //Gray stone
        SDL_Rect obsRect = {(int)obs->x, (int)obs->y, obs->width, obs->height};
        SDL_RenderFillRect(renderer, &obsRect);
    }

    SDL_RenderPresent(renderer);
}

int main(int agrc, char *argv[])
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
    srand((unsigned int)time(NULL));
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
            if (score > highScore) {
                highScore = score;
            }
            score = 0;
            scoreTimer = 0;
            initGame(&dino, &obs);
        }

        scoreTimer++;
        if (scoreTimer >= 10) {  // Adjust this for faster/slower score rate
        score++;
        scoreTimer = 0;
        }

        renderGame(renderer, &dino, &obs);

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}