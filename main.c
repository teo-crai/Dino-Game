#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400
#define GROUND_Y (WINDOW_HEIGHT - 50)
#define GRAVITY 0.5f
#define JUMP_STRENGTH -13.5f

int score = 0;
int highScore = 0;
int scoreTimer = 0;
long long tempscore=0;
float obstacleSpeed = 5.0f; 

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

typedef enum {
    GAME_RUNNING,
    GAME_OVER
} GameState;

void initGame(Dinosaur *dino, Obstacle *obs) {
    dino->x = 50;
    dino->y = GROUND_Y - 50;
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
    if (dino->isJumping) {
        dino->velocity += GRAVITY;
        dino->y += dino->velocity;
    
        if (dino->y >= GROUND_Y) {
            dino->y = GROUND_Y - 50;
            dino->isJumping = false;
            dino->velocity = 0.0f;
        }
    }

    obs->x -= obstacleSpeed; //moves obs to the left

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
/*SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_image(path); // use SDL_image if using PNG
    if (!surface) {
        SDL_Log("Failed to load surface: %s", SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return texture;
}*/
void renderGame(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    // Draw ground
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Black ground
    SDL_FRect groundRect = {0, GROUND_Y, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &groundRect);

    // Draw dinosaur
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Back dinosaur
    SDL_FRect dinoRect = {(int)dino->x, (int)dino->y, 50, 50};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Draw obstacle
    if (obs->active) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Black obstacle
        SDL_FRect obsRect = {(int)obs->x, (int)obs->y, obs->width, obs->height};
        SDL_RenderFillRect(renderer, &obsRect);
    }

    SDL_RenderPresent(renderer);
}
void renderGameReversed(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs) {
    SDL_SetRenderDrawColor(renderer, 0, 0,0, 255); // Black background
    SDL_RenderClear(renderer);

    // Draw ground
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //White ground
    SDL_FRect groundRect = {0, GROUND_Y, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &groundRect);

    // Draw dinosaur
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //White dinosaur
    SDL_FRect dinoRect = {(int)dino->x, (int)dino->y, 50, 50};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Draw obstacle
    if (obs->active) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //White obstacle
        SDL_FRect obsRect = {(int)obs->x, (int)obs->y, obs->width, obs->height};
        SDL_RenderFillRect(renderer, &obsRect);
    }

    SDL_RenderPresent(renderer);
}
void renderGameColored(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Blue background
    SDL_RenderClear(renderer);

    // Draw ground
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Green ground
    SDL_FRect groundRect = {0, GROUND_Y, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &groundRect);

    // Draw dinosaur
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); //Brown dinosaur
    SDL_FRect dinoRect = {(int)dino->x, (int)dino->y, 50, 50};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Draw obstacle
    if (obs->active) 
    {
        if (obs->type == OBSTACLE_CACTUS)
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); //Green cacti
        else
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); //Gray stone
        SDL_FRect obsRect = {(int)obs->x, (int)obs->y, obs->width, obs->height};
        SDL_RenderFillRect(renderer, &obsRect);
    }

    SDL_RenderPresent(renderer);
}

void renderGameOverScreen(SDL_Renderer* renderer, int score, int highScore) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    // Draw a simple "game over" rectangle
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black box
    SDL_FRect box = {200, 100, 400, 200};
    SDL_RenderFillRect(renderer, &box);

    //placeholder sdl_ttf stuff

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    printf("SDL version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION); 
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Dino Game", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if (!window || !renderer) {
        SDL_Log("Failed to create window or renderer: %s", SDL_GetError());
        return 1;
    }

    GameState gameState = GAME_RUNNING;
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
            if (gameState == GAME_RUNNING) {
                handleInput(&event, &dino);
            } else if (gameState == GAME_OVER) {
                if (event.type == SDL_EVENT_KEY_DOWN) 
                {
                    // Reset game
                    initGame(&dino, &obs);
                    score = 0;
                    scoreTimer = 0;
                    obstacleSpeed = 5.0f;
                    gameState = GAME_RUNNING;
                }
            }
        }

        updateGame(&dino, &obs);

        if (checkCollision(&dino, &obs)) {
            // Handle collision (e.g., reset game)
            if (score > highScore) {
                highScore = score;
            }
            gameState=GAME_OVER;
            /*score = 0;
            scoreTimer = 0;
            initGame(&dino, &obs);*/
        }

        scoreTimer++;
        if (scoreTimer >= 10) {  // Adjust this for faster/slower score rate
        score++;
        scoreTimer = 0;
        }
        
        obstacleSpeed = 5.0f + 2.0f*(score / 100.0f); //increases speed for every 100 pts, adjust 100.0f to modify speed
        if (obstacleSpeed > 100.0f)
                obstacleSpeed = 100.0f;

        if (gameState == GAME_RUNNING)
        {
            if (score%700==0)
                tempscore=score;
            if (score>=10000 && score<11000)
            {
                renderGameColored(renderer, &dino, &obs);
            }
            else
            {
            if (score<tempscore+700)
            {
                if(tempscore%1400==0)
                    renderGame(renderer, &dino, &obs);
                else
                    renderGameReversed(renderer, &dino, &obs);
            }
        }
        }
        else if (gameState == GAME_OVER) {
        renderGameOverScreen(renderer, score, highScore); }

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}