#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

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
TTF_Font* font = NULL;
TTF_Font* small_font = NULL;

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
    GAME_START,
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
        obs->width = 75;
        obs->height = 60 + rand() % 30; 
        obs->y = GROUND_Y - obs->height;
    } 
    else 
    {
        obs->width = 100 + rand() % 30; 
        obs->height = 80 + rand() % 10;
        obs->y = GROUND_Y - obs->height + 37;
    }

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
            obs->width = 75;
            obs->height = 60 + rand() % 30;
            obs->y = GROUND_Y - obs->height;
        } 
        else 
        {
            obs->width = 100 + rand() % 30; 
            obs->height = 80 + rand() % 10;
            obs->y = GROUND_Y - obs->height + 37; 
        }
        
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

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path); 
    if (!surface) {
        SDL_Log("Failed to load surface: %s", SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return texture;
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color color = {0, 0, 0, 255};  // Black text
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, strlen(text), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FRect dst = { (float)x, (float)y, (float)surface->w, (float)surface->h };

    SDL_RenderTexture(renderer, texture, NULL, &dst);

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}

void renderStartScreen(SDL_Renderer* renderer, TTF_Font* font, SDL_FRect* singleBtn, SDL_FRect* multiBtn) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 255, 255, 255}; // Black text

    // Draw Singleplayer Button
    singleBtn->x = 250; singleBtn->y = 120; singleBtn->w = 300; singleBtn->h = 60;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, singleBtn);
    SDL_Surface* singleSurf = TTF_RenderText_Solid(font, "SINGLEPLAYER", 12, textColor);
    SDL_Texture* singleTex = SDL_CreateTextureFromSurface(renderer, singleSurf);
    SDL_FRect singleTextRect = {
        singleBtn->x + (singleBtn->w - singleSurf->w) / 2.0f,
        singleBtn->y + (singleBtn->h - singleSurf->h) / 2.0f,
        singleSurf->w, singleSurf->h
    };
    SDL_RenderTexture(renderer, singleTex, NULL, &singleTextRect);
    SDL_DestroyTexture(singleTex);
    SDL_DestroySurface(singleSurf);

    // Draw Multiplayer Button
    multiBtn->x = 250; multiBtn->y = 200; multiBtn->w = 300; multiBtn->h = 60;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, multiBtn);
    SDL_Surface* multiSurf = TTF_RenderText_Solid(font, "MULTIPLAYER", 11, textColor);
    SDL_Texture* multiTex = SDL_CreateTextureFromSurface(renderer, multiSurf);
    SDL_FRect multiTextRect = {
        multiBtn->x + (multiBtn->w - multiSurf->w) / 2.0f,
        multiBtn->y + (multiBtn->h - multiSurf->h) / 2.0f,
        multiSurf->w, multiSurf->h
    };
    SDL_RenderTexture(renderer, multiTex, NULL, &multiTextRect);
    SDL_DestroyTexture(multiTex);
    SDL_DestroySurface(multiSurf);

    SDL_RenderPresent(renderer);
}

void renderGame(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs,
    SDL_Texture* dinoTexture, SDL_Texture* cactusTexture, SDL_Texture* rockTexture,
    SDL_Texture* backgroundTexture, TTF_Font* font, int score, int highScore) 
    {
    // Draw background
    SDL_RenderTexture(renderer, backgroundTexture, NULL, NULL);

    // Draw ground (optional if background has ground)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_FRect groundRect = {0, GROUND_Y, WINDOW_WIDTH, 50};
    SDL_RenderFillRect(renderer, &groundRect);

    // Draw dinosaur
    SDL_FRect dinoRect = {dino->x, dino->y-25, 100, 100};
    SDL_RenderTexture(renderer, dinoTexture, NULL, &dinoRect);

    // Draw obstacle
    if (obs->active) {
    SDL_FRect obsRect = {obs->x, obs->y, obs->width, obs->height};
    if (obs->type == OBSTACLE_CACTUS)
    SDL_RenderTexture(renderer, cactusTexture, NULL, &obsRect);
    else
    SDL_RenderTexture(renderer, rockTexture, NULL, &obsRect);
    }

    // Display score/highscore
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "SCORE: %05d", score);
    renderText(renderer, font, scoreText, WINDOW_WIDTH - 242, 10);

    char highText[32];
    snprintf(highText, sizeof(highText), "HI: %05d", highScore);
    renderText(renderer, font, highText, WINDOW_WIDTH - 180, 40);

    SDL_RenderPresent(renderer);
}

/*void renderGame(SDL_Renderer *renderer, Dinosaur *dino, Obstacle *obs) {
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
}*/
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

    // Display score/highscore
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "SCORE: %05d", score);
    renderText(renderer, font, scoreText, WINDOW_WIDTH - 180, 10);

    char highText[32];
    snprintf(highText, sizeof(highText), "HI: %05d", highScore);
    renderText(renderer, font, highText, WINDOW_WIDTH - 180, 40);

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

    // Display score/highscore
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "SCORE: %05d", score);
    renderText(renderer, font, scoreText, WINDOW_WIDTH - 180, 10);

    char highText[32];
    snprintf(highText, sizeof(highText), "HI: %05d", highScore);
    renderText(renderer, font, highText, WINDOW_WIDTH - 180, 40);

    SDL_RenderPresent(renderer);
}

void renderGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, TTF_Font* small_font, int score, int highScore) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White background
    SDL_RenderClear(renderer);

    // Draw black box
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_FRect box = {200, 100, 400, 200};
    SDL_RenderFillRect(renderer, &box);

    // Text inside the box
    SDL_Color textColor = {255, 255, 255, 255}; // White text
    char buffer[64];

    // Score
    snprintf(buffer, sizeof(buffer), "SCORE: %05d", score);
    SDL_Surface* scoreSurf = TTF_RenderText_Solid(font, buffer, strlen(buffer), textColor);
    SDL_Texture* scoreTex = SDL_CreateTextureFromSurface(renderer, scoreSurf);
    SDL_FRect scoreRect = {
        box.x + (box.w - scoreSurf->w) / 2.0f,
        box.y + 30,
        scoreSurf->w, scoreSurf->h
    };
    SDL_RenderTexture(renderer, scoreTex, NULL, &scoreRect);
    SDL_DestroyTexture(scoreTex);
    SDL_DestroySurface(scoreSurf);

    // High Score
    snprintf(buffer, sizeof(buffer), "HI: %05d", highScore);
    SDL_Surface* highSurf = TTF_RenderText_Solid(font, buffer, strlen(buffer), textColor);
    SDL_Texture* highTex = SDL_CreateTextureFromSurface(renderer, highSurf);
    SDL_FRect highRect = {
        box.x + (box.w - highSurf->w) / 2.0f,
        box.y + 70,
        highSurf->w, highSurf->h
    };
    SDL_RenderTexture(renderer, highTex, NULL, &highRect);
    SDL_DestroyTexture(highTex);
    SDL_DestroySurface(highSurf);

    // Restart message
    snprintf(buffer, sizeof(buffer), "Press any key to restart.");
    SDL_Surface* msgSurf = TTF_RenderText_Solid(small_font, buffer, strlen(buffer), textColor);
    SDL_Texture* msgTex = SDL_CreateTextureFromSurface(renderer, msgSurf);
    SDL_FRect msgRect = {
        box.x + (box.w - msgSurf->w) / 2.0f,
        box.y + 130,
        msgSurf->w, msgSurf->h
    };
    SDL_RenderTexture(renderer, msgTex, NULL, &msgRect);
    SDL_DestroyTexture(msgTex);
    SDL_DestroySurface(msgSurf);

    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[]) {
    printf("SDL version: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_MICRO_VERSION); 

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        SDL_Log("Unable to initialize SDL_ttf: %s", SDL_GetError());
        return 1;
    }

    font = TTF_OpenFont("arial.ttf", 24);
    small_font = TTF_OpenFont("arial.ttf", 16);
    if (!font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Dino Game", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

    if (!window || !renderer) {
        SDL_Log("Failed to create window or renderer: %s", SDL_GetError());
        return 1;
    }

    SDL_Texture* dinoTexture = loadTexture("dino.png", renderer);
    SDL_Texture* cactusTexture = loadTexture("cacti.png", renderer);
    SDL_Texture* rockTexture = loadTexture("rock.png", renderer);
    SDL_Texture* backgroundTexture = loadTexture("background.png", renderer);

    if (!dinoTexture || !cactusTexture || !rockTexture || !backgroundTexture) {
        SDL_Log("Failed to load one or more textures!");
        return 1;
    }

    GameState gameState = GAME_START;
    Dinosaur dino;
    Obstacle obs;
    SDL_Event event;
    SDL_FRect singleBtn, multiBtn;

    srand((unsigned int)time(NULL));
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (gameState == GAME_START) {
                if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    int mx = event.button.x;
                    int my = event.button.y;

                    if (mx >= singleBtn.x && mx <= singleBtn.x + singleBtn.w &&
                        my >= singleBtn.y && my <= singleBtn.y + singleBtn.h) {
                        initGame(&dino, &obs);
                        score = 0;
                        scoreTimer = 0;
                        obstacleSpeed = 5.0f;
                        gameState = GAME_RUNNING;
                    } else if (mx >= multiBtn.x && mx <= multiBtn.x + multiBtn.w &&
                               my >= multiBtn.y && my <= multiBtn.y + multiBtn.h) {
                        SDL_Log("Multiplayer mode not implemented.");
                    }
                }
            } else if (gameState == GAME_RUNNING) {
                handleInput(&event, &dino);
            } else if (gameState == GAME_OVER) {
                if (event.type == SDL_EVENT_KEY_DOWN) {
                    gameState = GAME_START;
                }
            }
        }

        if (gameState == GAME_START) {
            renderStartScreen(renderer, font, &singleBtn, &multiBtn);
            SDL_Delay(16);
            continue;
        }

        updateGame(&dino, &obs);

        if (checkCollision(&dino, &obs)) {
            if (score > highScore) {
                highScore = score;
            }
            gameState = GAME_OVER;
        }

        if (gameState == GAME_RUNNING) {
            scoreTimer++;

            if (scoreTimer >= 10) {
                score++;
                scoreTimer = 0;
            }

            obstacleSpeed = 5.0f + 2.0f * (score / 100.0f);
            if (obstacleSpeed > 100.0f) {
                obstacleSpeed = 100.0f;
            }

            if (score % 700 == 0) {
                tempscore = score;
            }

            if (score >= 10000 && score < 11000) {
                renderGameColored(renderer, &dino, &obs);
            } else {
                if (score < tempscore + 700) {
                    if (tempscore % 1400 == 0) {
                        renderGame(renderer, &dino, &obs, dinoTexture, cactusTexture, rockTexture, backgroundTexture, font, score, highScore);
                    } else {
                        renderGameReversed(renderer, &dino, &obs);
                    }
                }
            }
        } else if (gameState == GAME_OVER) {
            renderGameOverScreen(renderer, font, small_font, score, highScore);
        }

        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(dinoTexture);
    SDL_DestroyTexture(cactusTexture);
    SDL_DestroyTexture(rockTexture);
    SDL_DestroyTexture(backgroundTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
