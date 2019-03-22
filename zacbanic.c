/******************************************************************************
*   Zacbanic
*   Sord M5 computer came with a game in tape named Zacbanic.
*   This a Zacbanic clone.
******************************************************************************/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

#define FACTOR 2    // Must be 1, 2 or 4.
#define ASTER_NUM 8
#define ALIEN_NUM 4

// User-defined types.
typedef enum GameScreen { TITLE=0, GAMEPLAY, ENDING } GameScreen;
// typedef enum Direction { UP = 0, DOWN } Direction;

// Global variables.
Rectangle ship;
Rectangle shipShot;
bool shipShotReady;
Rectangle asteroids[ASTER_NUM];
Rectangle aliens[ALIEN_NUM];
Rectangle alienShot[ALIEN_NUM];

Texture2D sprites;

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
void AlienShoot(void);


// Initialize window and primary game elements.
// --------------------------------------------
void InitializeElements(void)
{
    InitWindow(screenWidth, screenHeight, "Zacbanic");
    // ToggleFullscreen();

    char spriteFile[128];
    sprintf(spriteFile, "SpritesZacbanicx%1d.png", FACTOR);
    sprites = LoadTexture(spriteFile);

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

    // Initialize asteroids' positions.
    for (int i=0; i<ASTER_NUM; ++i)
        asteroids[i] = (Rectangle) {
            GetRandomValue(0, (int)screenWidth>>4)<<4,
            GetRandomValue(0, screenHeight),
            asteroidTex.width,
            asteroidTex.height
        };

    // Initialize aliens' positions.
    for (int i=0; i<ALIEN_NUM; ++i)
        aliens[i] = (Rectangle) {
            GetRandomValue(0, (int)screenWidth>>4)<<4,
            GetRandomValue(0, screenHeight),
            alienTex.width,
            alienTex.height
        };

    SetTargetFPS(60);  // Set desired framerate (frames-per-second)
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
        // Advance shot.
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
                }
            } break;
            case GAMEPLAY:
            {
                // TODO: Update GAMEPLAY screen variables here!
                MoveAsteroids();
                MoveAliens();

                // Check keys.
                if (IsKeyDown(KEY_O) || IsKeyDown(KEY_LEFT))
                    ship.x -= FACTOR;
                if (IsKeyDown(KEY_P) || IsKeyDown(KEY_RIGHT))
                    ship.x += FACTOR;
                if (IsKeyDown(KEY_SPACE))
                    ShipShoot();
                UpdateShipShoot();
            } break;
            case ENDING:
            {
                // TODO: Update ENDING screen variables here!

                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = TITLE;
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
                    DrawText("ZACBANIC", 20, 20, 40*FACTOR, MAROON);
                    DrawText("Programmed with Raylib by Angel G. Cuartero", 20, 50*FACTOR, 5*FACTOR, GRAY);
                    DrawText("Player Keys: O, P, Space Bar", 20, 70*FACTOR, 10*FACTOR, GRAY);
                    DrawText("PRESS ENTER to PLAY", 20, 90*FACTOR, 10*FACTOR, GRAY);
                    DrawText("PRESS ESC to QUIT", 20, 100*FACTOR, 10*FACTOR, GRAY);
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
                } break;
                case ENDING:
                {
                    // TODO: Draw ENDING screen here!
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLUE);
                    // DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
                    DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
                } break;
                default: break;
            }

        EndDrawing();
        //---------------------------------------------------------------------
    }

    //-------------------------------------------------------------------------
    // De-Initialization
    //-------------------------------------------------------------------------

    // TODO: Unload all loaded data (textures, fonts, audio) here!

    CloseWindow();        // Close window and OpenGL context
    //-------------------------------------------------------------------------

    return 0;
}
