/******************************************************************************
*   Zacbanic
*   Sord M5 computer came with a game in tape named Zacbanic.
*   This a clone of that game.
******************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define FACTOR 4    // Must be 1, 2 or 4.
#define ASTER_NUM 8
#define ALIEN_NUM 4

// User-defined types.
typedef enum GameScreen { TITLE=0, GAMEPLAY, DEATH, ENDING } GameScreen;
// typedef enum Direction { UP = 0, DOWN } Direction;

// Global variables.
Rectangle ship;
Rectangle shipShot;
bool shipShotReady;

int lives = 2;
int score = 0;
int highScore = 0;
char strLives[8];
char strScore[16];
Vector2 scorePosition;
Vector2 highScorePosition;
Vector2 livesPosition;
Vector2 livesSpritePosition;

Texture2D sprites;  // Contains all the sprites.
Font font;

Rectangle asteroids[ASTER_NUM];
Rectangle aliens[ALIEN_NUM];
Rectangle alienShot[ALIEN_NUM];

// Locate individual textures.
Rectangle shipTex = {0, 0, 14*FACTOR, 16*FACTOR};
Rectangle alienTex = {14*FACTOR, 0, 12*FACTOR, 16*FACTOR};
Rectangle asteroidTex = {(14+12)*FACTOR, 0, 8*FACTOR, 14*FACTOR};
Rectangle livesTex = {(14+12+8)*FACTOR, 0, 8*FACTOR, 8*FACTOR};
Rectangle ashotTex = {(14+12+8+8)*FACTOR, 0, 3*FACTOR, 3*FACTOR};
Rectangle sshotTex = {(14+12+8+8)*FACTOR, 3*FACTOR, 3*FACTOR, 10*FACTOR};

GameScreen currentScreen = TITLE;
int screenWidth = 256*FACTOR;
int screenHeight = 192*FACTOR;

// Prototypes
// ----------
void InitializeElements(void);
void MoveAsteroids(void);
void MoveAliens(void);
void ShipShoot(void);
void UpdateShipShoot(void);
void InitShip(void);
void InitAdversaries(void);


// Initialize window and primary game elements.
// --------------------------------------------
void InitializeElements(void)
{
    InitWindow(screenWidth, screenHeight, "Zacbanic");
    // ToggleFullscreen();

    char spriteFile[128];
    sprintf(spriteFile, "SpritesZacbanicx%1d.png", FACTOR);
    sprites = LoadTexture(spriteFile);
    font = LoadFont("romulus.png");

    sprintf(strScore, "Score = %06d", score);
    sprintf(strLives, " = %d", lives);

    // scorePosition = (Vector2) {screenWidth - , screenHeight - livesTex.height};
    scorePosition = (Vector2) {4*8*FACTOR, 0};
    highScorePosition = (Vector2) {11*8*FACTOR, 2*8*FACTOR};
    livesSpritePosition = (Vector2) {screenWidth - livesTex.width*4, screenHeight - livesTex.height};
    livesPosition = (Vector2) {screenWidth - livesTex.width*3, screenHeight - livesTex.height};
    InitShip();
    InitAdversaries();
    SetTargetFPS(60);  // Set desired framerate (frames-per-second)
}

void InitShip(void)
{
   // Init ship and shot.
    ship = (Rectangle) {
        screenWidth/2 - shipTex.width/2,
        screenHeight - shipTex.height - 16*FACTOR,
        shipTex.width,
        shipTex.height
    };

    shipShot = (Rectangle) {
        screenWidth,
        screenHeight,
        sshotTex.width,
        sshotTex.height
    };

    shipShotReady = true;
}

void InitAdversaries(void)
{
    // Initialize asteroids' positions.
    for (int i=0; i<ASTER_NUM; ++i)
        asteroids[i] = (Rectangle) {
            GetRandomValue(0, (int)screenWidth>>4)<<4,
            GetRandomValue(0, screenHeight/2),
            asteroidTex.width,
            asteroidTex.height
        };

    // Initialize aliens' positions.
    for (int i=0; i<ALIEN_NUM; ++i)
        aliens[i] = (Rectangle) {
            GetRandomValue(0, (int)screenWidth>>4)<<4,
            GetRandomValue(0, screenHeight/3),
            alienTex.width,
            alienTex.height
        };
}

void MoveAsteroids(void)
{
    for (int i=0; i<ASTER_NUM; ++i)
    {
        asteroids[i].y += FACTOR;
        if (asteroids[i].y > screenHeight)
        {
            asteroids[i].x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            asteroids[i].y = -asteroids[i].height;
            continue;
        }

       if (CheckCollisionRecs(asteroids[i], shipShot))
       {
            asteroids[i].x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            asteroids[i].y = -asteroids[i].height;
            if (!shipShotReady)
            {
                // Hide sprite and get it available for shooting again.
                shipShot.x = shipShot.y = screenWidth;
                shipShotReady = true;
            }
       }

       if (CheckCollisionRecs(asteroids[i], ship))
       {
            --lives;
            currentScreen = DEATH;
            ship.x = screenWidth/2 - shipTex.width/2;
            asteroids[i].x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            asteroids[i].y = -asteroids[i].height;
            if (lives < 0)
            {
                currentScreen = ENDING;
                score = 0;
                lives = 2;
            }
       }
    }
}

void MoveAliens(void)
{
    for (int i=0; i<ALIEN_NUM; ++i)
    {
        aliens[i].y += FACTOR*1.1;
        if (aliens[i].y > screenHeight)
        {
            aliens[i].x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            aliens[i].y = -aliens[i].height;;
            continue;
        }
        if (CheckCollisionRecs(aliens[i], shipShot))
        {
            aliens[i].x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            aliens[i].y = -aliens[i].height;;
            if (!shipShotReady)
            {
                // Hide sprite and get it available for shooting again.
                shipShot.x = shipShot.y = screenWidth;
                shipShotReady = true;
                score += 100;
                if (score > highScore)
                    highScore = score;
            }
        }

        if (CheckCollisionRecs(aliens[i], ship))
        {
            score += 100;
            if (score > highScore)
                highScore = score;
            --lives;
            currentScreen = DEATH;
            ship.x = screenWidth/2 - shipTex.width/2;
            aliens[i].x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            aliens[i].y = -aliens[i].height;
            if (lives < 0)
            {
                currentScreen = ENDING;
                score = 0;
                lives = 2;
            }
        }
    }
}

void ShipShoot(void)
{
    if (shipShotReady)
    {
        shipShot.x = ship.x + (int)ship.width/2 - FACTOR;
        shipShot.y = ship.y - shipShot.height - 1;
        shipShotReady = false;
    }
}

void UpdateShipShoot(void)
{
    if (!shipShotReady)
    {
        shipShot.y -= (2*FACTOR);
        shipShotReady = shipShot.y + shipShot.height < 0? true: false;
    }
}

//-----------------------------------------------------------------------------
// Main entry point
//-----------------------------------------------------------------------------
int main(void)
{
    InitializeElements();
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //---------------------------------------------------------------------
        // Update
        //---------------------------------------------------------------------
        switch(currentScreen)
        {
            case TITLE:
            {
                // TODO: Update TITLE screen variables here!

                // Press enter to change to GAMEPLAY screen
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                    InitAdversaries();
                    InitShip();
                }
            } break;
            case GAMEPLAY:
            {
                // TODO: Update GAMEPLAY screen variables here!
                MoveAsteroids();
                MoveAliens();

                // Check keys.
                if (IsKeyDown(KEY_O) || IsKeyDown(KEY_LEFT))
                {
                    ship.x -= FACTOR;
                    if (ship.x < 0)
                        ship.x = 0;
                }

                if (IsKeyDown(KEY_P) || IsKeyDown(KEY_RIGHT))
                {
                    ship.x += FACTOR;
                    if (ship.x > screenWidth - ship.width)
                        ship.x = screenWidth - ship.width;
                }

                if (IsKeyDown(KEY_SPACE))
                    ShipShoot();
                UpdateShipShoot();
            } break;
            case DEATH:
            {
                // Press enter to return to GAME.
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                    InitAdversaries();
                    InitShip();
                }
            } break;
            case ENDING:
            {
                // TODO: Update ENDING screen variables here!

                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentScreen = GAMEPLAY;
                    InitAdversaries();
                    InitShip();
                }
            } break;
            default: break;
        }
        //---------------------------------------------------------------------
        // Draw
        //---------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(BLACK);

            switch(currentScreen)
            {
                case TITLE:
                {
                    DrawText("ZACBANIC", 3*8*FACTOR, 2*8*FACTOR, 40*FACTOR, MAROON);
                    // DrawText("Programmed with Raylib by Angel G. Cuartero", 20, 50*FACTOR, 5*FACTOR, GRAY);
                    DrawTextEx(font, "Programmed with Raylib by Angel G. Cuartero", (Vector2){5*8*FACTOR, 7*8*FACTOR}, 6*FACTOR, FACTOR, GRAY);
                    // DrawText("Player Keys: O, P, Space Bar", 20, 70*FACTOR, 10*FACTOR, GRAY);
                    DrawTextEx(font, "Player Keys: O, P, Space Bar", (Vector2){6*8*FACTOR, 10*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
                    // DrawText("PRESS ENTER to PLAY", 20, 90*FACTOR, 10*FACTOR, GRAY);
                    // DrawText("PRESS ESC to QUIT", 20, 100*FACTOR, 10*FACTOR, GRAY);

                    DrawTextEx(font, "--> PRESS ENTER TO PLAY", (Vector2){6*8*FACTOR, 12*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, "--> PRESS ESC TO QUIT", (Vector2){6*8*FACTOR, 13*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);

                } break;
                case GAMEPLAY:
                {
                    // Asteroids.
                    for (int i=0; i<ASTER_NUM; ++i)
                        DrawTextureRec(sprites, asteroidTex, (Vector2) {asteroids[i].x, asteroids[i].y}, WHITE);
                    // Aliens.
                    for (int i=0; i<ALIEN_NUM; ++i)
                        DrawTextureRec(sprites, alienTex, (Vector2) {aliens[i].x, aliens[i].y}, WHITE);
                    // Ship.
                    DrawTextureRec(sprites, shipTex, (Vector2) {ship.x, ship.y}, WHITE);
                    // Ship Shot.
                    DrawTextureRec(sprites, sshotTex, (Vector2) {shipShot.x, shipShot.y}, WHITE);
                    // Lives.
                    sprintf(strLives, " = %d", lives);
                    sprintf(strScore, "Score: %04d", score);
                    DrawTextureRec(sprites, livesTex, livesSpritePosition, WHITE);
                    DrawTextEx(font, strLives, livesPosition, 9*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, strScore, scorePosition, 9*FACTOR, FACTOR, GRAY);
                } break;
                case DEATH:
                {
                    sprintf(strScore, "HighScore: %04d", highScore);
                    DrawTextEx(font, strScore, highScorePosition, 9*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, "--> PRESS ENTER TO START", (Vector2){6*8*FACTOR, 11*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
                    DrawTextureRec(sprites, shipTex, (Vector2) {ship.x, ship.y}, WHITE);

                } break;
                case ENDING:
                {
                    sprintf(strScore, "HighScore: %04d", highScore);
                    DrawTextEx(font, strScore, highScorePosition, 9*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, "== GAME OVER ==", (Vector2){9*8*FACTOR, 9*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, "--> PRESS ENTER TO START", (Vector2){6*8*FACTOR, 11*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
                } break;
                default: break;
            }

        EndDrawing();
        //---------------------------------------------------------------------
    }

    //-------------------------------------------------------------------------
    // De-Initialization
    //-------------------------------------------------------------------------

    UnloadTexture(sprites);
    UnloadFont(font);

    CloseWindow();        // Close window and OpenGL context
    //-------------------------------------------------------------------------

    return 0;
}
