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
typedef struct
{
    Rectangle ship;
    Rectangle shipShot;
    bool shipShotReady;
} ship_t;

typedef struct
{
    Rectangle alien;
    Rectangle alienShot;
    bool alienShotReady;
    int shotDir;
} alien_t;

// Global variables.
ship_t starShip;

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
alien_t aliens[ALIEN_NUM];

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
void InitShip(void);
void InitAdversaries(void);
void MoveAsteroids(void);
void MoveAliens(void);
void ShipShoot(void);
void UpdateShipShoot(void);
void AlienShoot(alien_t *alien);

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
    starShip.ship = (Rectangle) {
        screenWidth/2 - shipTex.width/2,
        screenHeight - shipTex.height - 16*FACTOR,
        shipTex.width,
        shipTex.height
    };

    starShip.shipShot = (Rectangle) {
        screenWidth,
        screenHeight,
        sshotTex.width,
        sshotTex.height
    };

    starShip.shipShotReady = true;
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
    {
        aliens[i].alien = (Rectangle) {
            GetRandomValue(0, (int)screenWidth>>4)<<4,
            GetRandomValue(0, screenHeight/3),
            alienTex.width,
            alienTex.height
        };
        aliens[i].alienShotReady = true;
        aliens[i].shotDir = 0;
        aliens[i].alienShot.x = screenWidth;
        aliens[i].alienShot.y = screenHeight;
    }
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

       if (CheckCollisionRecs(asteroids[i], starShip.shipShot))
       {
            asteroids[i].x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            asteroids[i].y = -asteroids[i].height;
            if (!starShip.shipShotReady)
            {
                // Hide sprite and get it available for shooting again.
                starShip.shipShot.x = starShip.shipShot.y = screenWidth;
                starShip.shipShotReady = true;
            }
       }

       if (CheckCollisionRecs(asteroids[i], starShip.ship))
       {
            --lives;
            currentScreen = DEATH;
            starShip.ship.x = screenWidth/2 - shipTex.width/2;
            InitAdversaries();
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
        // Move aliens.
        aliens[i].alien.y += FACTOR*1.1;
        if (aliens[i].alien.y > screenHeight)
        {
            aliens[i].alien.x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            aliens[i].alien.y = -aliens[i].alien.height;
            continue;
        }

        AlienShoot(&aliens[i]);

        // Alien shot by ship?
        if (CheckCollisionRecs(aliens[i].alien, starShip.shipShot))
        {
            aliens[i].alien.x = GetRandomValue(0, (int)screenWidth>>4)<<4;
            aliens[i].alien.y = -aliens[i].alien.height;
            if (!starShip.shipShotReady)
            {
                // Hide sprite and get it available for shooting again.
                starShip.shipShot.x = starShip.shipShot.y = screenWidth;
                starShip.shipShotReady = true;
                score += 100;
                if (score > highScore)
                    highScore = score;
            }
        }

        // Alien crashed into ship?
        if (CheckCollisionRecs(aliens[i].alien, starShip.ship))
        {
            score += 100;
            if (score > highScore)
                highScore = score;
            --lives;
            currentScreen = DEATH;
            starShip.ship.x = screenWidth/2 - shipTex.width/2;
            InitAdversaries();
            if (lives < 0)
            {
                currentScreen = ENDING;
                score = 0;
                lives = 2;
            }
        }

        // Ship shot by alien?
        if (CheckCollisionRecs(aliens[i].alienShot, starShip.ship))
        {
            --lives;
            currentScreen = DEATH;
            starShip.ship.x = screenWidth/2 - shipTex.width/2;
            InitAdversaries();
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
    if (starShip.shipShotReady)
    {
        starShip.shipShot.x = starShip.ship.x + (int)starShip.ship.width/2 - FACTOR;
        starShip.shipShot.y = starShip.ship.y - starShip.shipShot.height - 1;
        starShip.shipShotReady = false;
    }
}

void UpdateShipShoot(void)
{
    if (!starShip.shipShotReady)
    {
        starShip.shipShot.y -= (2*FACTOR);
        starShip.shipShotReady = starShip.shipShot.y + starShip.shipShot.height < 0? true: false;
    }
}

void AlienShoot(alien_t *alien)
{
    if (alien->alienShotReady)
    {
        // Ready to shoot.
        if (GetRandomValue(0, 10) > 5)
        {
            alien->alienShotReady = false;
            alien->alienShot.x = alien->alien.x + alien->alien.width/2 - FACTOR;
            alien->alienShot.y = alien->alien.y + alien->alien.height + FACTOR;
        }
    }
    else
    {
        // Move shot.
        int probable = GetRandomValue(0, 10);
        alien->shotDir = probable < 3? -2 : probable > 7? 2 : 0;
        alien->alienShot.x += FACTOR*alien->shotDir;
        alien->alienShot.y += FACTOR*1.3;
        alien->alienShotReady = alien->alienShot.y + alien->alienShot.height > screenHeight? true: false;
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
                    starShip.ship.x -= FACTOR;
                    if (starShip.ship.x < 0)
                        starShip.ship.x = 0;
                }

                if (IsKeyDown(KEY_P) || IsKeyDown(KEY_RIGHT))
                {
                    starShip.ship.x += FACTOR;
                    if (starShip.ship.x > screenWidth - starShip.ship.width)
                        starShip.ship.x = screenWidth - starShip.ship.width;
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
                    DrawTextEx(font, "Programmed with Raylib by Angel G. Cuartero", (Vector2){5*8*FACTOR, 7*8*FACTOR}, 6*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, "Player Keys: O, P, Space Bar", (Vector2){6*8*FACTOR, 10*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
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
                    {
                        DrawTextureRec(sprites, alienTex, (Vector2) {aliens[i].alien.x, aliens[i].alien.y}, WHITE);
                        DrawTextureRec(sprites, ashotTex, (Vector2) {aliens[i].alienShot.x, aliens[i].alienShot.y}, WHITE);
                    }
                    // starShip.
                    DrawTextureRec(sprites, shipTex, (Vector2) {starShip.ship.x, starShip.ship.y}, WHITE);
                    // Ship Shot.
                    DrawTextureRec(sprites, sshotTex, (Vector2) {starShip.shipShot.x, starShip.shipShot.y}, WHITE);
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
                    DrawTextEx(font, "--> PRESS ENTER TO START", (Vector2){6*8*FACTOR, 12*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
                    DrawTextureRec(sprites, shipTex, (Vector2) {starShip.ship.x, starShip.ship.y}, WHITE);

                } break;
                case ENDING:
                {
                    sprintf(strScore, "HighScore: %04d", highScore);
                    DrawTextEx(font, strScore, highScorePosition, 9*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, "== GAME OVER ==", (Vector2){9*8*FACTOR, 9*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, "--> PRESS ENTER TO START", (Vector2){6*8*FACTOR, 12*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
                    DrawTextEx(font, "--> PRESS ESC TO QUIT", (Vector2){6*8*FACTOR, 13*8*FACTOR}, 9*FACTOR, FACTOR, GRAY);
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
