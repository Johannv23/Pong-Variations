/**
* Author: Johann Varghese
* Assignment: Pong Clone
* Date due: 02/28/2025
*/

#include "CS3113/cs3113.h"
#include <cmath>

// Constants

constexpr int SCREEN_WIDTH  = 800;
constexpr int SCREEN_HEIGHT = 450;
constexpr int FPS = 60;

constexpr float PADDLE_SPEED = 400.0f;
constexpr float BALL_SPEED   = 350.0f;

constexpr char BACKGROUND_FP[] = "assets/landscape.jpg";
constexpr char P1_FP[] = "assets/player1.png";
constexpr char P2_FP[] = "assets/player2.png";
constexpr char BALL_FP[] = "assets/ball.png";
constexpr char WIN1_FP[] = "assets/P1.png";
constexpr char WIN2_FP[] = "assets/P2.png";

// Global Variables

AppStatus gAppStatus = RUNNING;

Texture2D gBackgroundTexture;
Texture2D gP1Texture;
Texture2D gP2Texture;
Texture2D gBallTexture;
Texture2D gWin1Texture;
Texture2D gWin2Texture;

Vector2 gP1Position = {50.0f, SCREEN_HEIGHT / 2.0f};
Vector2 gP2Position = {SCREEN_WIDTH - 50.0f, SCREEN_HEIGHT / 2.0f};

bool gSinglePlayer = false;

constexpr int MAX_BALLS = 3;
int gNumBalls = 1;

Vector2 gBallPositions[MAX_BALLS];
Vector2 gBallVelocities[MAX_BALLS];

float gPreviousTicks = 0.0f;

int gScore1 = 0;
int gScore2 = 0;

bool gGameOver = false;
int gWinner = 0;

// --------------------------------------------------

bool isColliding(float ax, float ay, float aw, float ah,
                 float bx, float by, float bw, float bh)
{
    float xDistance = fabs((ax + aw/2) - (bx + bw/2)) - ((aw + bw) / 2.0f);
    float yDistance = fabs((ay + ah/2) - (by + bh/2)) - ((ah + bh) / 2.0f);
    return (xDistance < 0.0f && yDistance < 0.0f);
}

// --------------------------------------------------

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Clone");

    gBackgroundTexture = LoadTexture(BACKGROUND_FP);
    gP1Texture = LoadTexture(P1_FP);
    gP2Texture = LoadTexture(P2_FP);
    gBallTexture = LoadTexture(BALL_FP);
    gWin1Texture = LoadTexture(WIN1_FP);
    gWin2Texture = LoadTexture(WIN2_FP);

    SetTargetFPS(FPS);

    for (int i = 0; i < MAX_BALLS; i++)
    {
        gBallPositions[i] = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        gBallVelocities[i] = {
            BALL_SPEED * (GetRandomValue(0,1) ? 1.0f : -1.0f),
            BALL_SPEED * (GetRandomValue(0,1) ? 1.0f : -1.0f)
        };
    }
}


void processInput(float deltaTime)
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
    if (gGameOver) return;

    // Toggle single player
    if (IsKeyPressed(KEY_T)) gSinglePlayer = !gSinglePlayer;

    // Ball count switching
    if (IsKeyPressed(KEY_ONE))   gNumBalls = 1;
    if (IsKeyPressed(KEY_TWO))   gNumBalls = 2;
    if (IsKeyPressed(KEY_THREE)) gNumBalls = 3;

    float paddleScale  = 0.25f;
    float paddleHeight = gP1Texture.height * paddleScale;

    // Player 1 (always keyboard)
    if (IsKeyDown(KEY_W))
        gP1Position.y -= PADDLE_SPEED * deltaTime;
    if (IsKeyDown(KEY_S))
        gP1Position.y += PADDLE_SPEED * deltaTime;

    gP1Position.y = fmaxf(paddleHeight / 2.0f,
                          fminf(gP1Position.y, SCREEN_HEIGHT - paddleHeight / 2.0f));

    // Player 2 (only if NOT single player)
    if (!gSinglePlayer)
    {
        if (IsKeyDown(KEY_UP))
            gP2Position.y -= PADDLE_SPEED * deltaTime;
        if (IsKeyDown(KEY_DOWN))
            gP2Position.y += PADDLE_SPEED * deltaTime;

        gP2Position.y = fmaxf(paddleHeight / 2.0f,
                              fminf(gP2Position.y, SCREEN_HEIGHT - paddleHeight / 2.0f));
    }
}

// --------------------------------------------------

void update(float deltaTime)
{
    if (gGameOver) return;

    float ballScale    = 0.15f;
    float ballSize     = gBallTexture.width * ballScale;
    float paddleScale  = 0.25f;
    float paddleWidth  = gP1Texture.width  * paddleScale;
    float paddleHeight = gP1Texture.height * paddleScale;

    if (gSinglePlayer)
    {
        float ballCenter = gBallPositions[0].y;
        if (ballCenter < gP2Position.y)
            gP2Position.y -= PADDLE_SPEED * deltaTime;
        else
            gP2Position.y += PADDLE_SPEED * deltaTime;

        gP2Position.y = fmaxf(paddleHeight / 2.0f,
                              fminf(gP2Position.y, SCREEN_HEIGHT - paddleHeight / 2.0f));
    }

    float shrink = 0.5f;

    float p1x = gP1Position.x + (paddleWidth * (1 - shrink)) / 2.0f;
    float p1y = gP1Position.y - (paddleHeight * shrink) / 2.0f;
    float p1w = paddleWidth  * shrink;
    float p1h = paddleHeight * shrink;

    float p2x = (gP2Position.x - paddleWidth) + (paddleWidth * (1 - shrink)) / 2.0f;
    float p2y = gP2Position.y - (paddleHeight * shrink) / 2.0f;
    float p2w = paddleWidth  * shrink;
    float p2h = paddleHeight * shrink;

    // -------- BALL LOOP (Requirement 3) --------
    for (int i = 0; i < gNumBalls; i++)
    {
        gBallPositions[i].x += gBallVelocities[i].x * deltaTime;
        gBallPositions[i].y += gBallVelocities[i].y * deltaTime;

        // Top/bottom bounce
        if (gBallPositions[i].y <= 0 ||
            gBallPositions[i].y + ballSize >= SCREEN_HEIGHT)
            gBallVelocities[i].y *= -1;

        // Paddle collisions
        if (isColliding(p1x,p1y,p1w,p1h,
                        gBallPositions[i].x,gBallPositions[i].y,
                        ballSize,ballSize)
            && gBallVelocities[i].x < 0)
            gBallVelocities[i].x *= -1;

        if (isColliding(p2x,p2y,p2w,p2h,
                        gBallPositions[i].x,gBallPositions[i].y,
                        ballSize,ballSize)
            && gBallVelocities[i].x > 0)
            gBallVelocities[i].x *= -1;

        // Scoring
        if (gBallPositions[i].x < 0)
        {
            gScore2++;
            gBallPositions[i] = {SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
        }

        if (gBallPositions[i].x > SCREEN_WIDTH)
        {
            gScore1++;
            gBallPositions[i] = {SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};
        }
    }

    if (gScore1 >= 3) { gGameOver = true; gWinner = 1; }
    if (gScore2 >= 3) { gGameOver = true; gWinner = 2; }
}

// --------------------------------------------------

void render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(gBackgroundTexture,
                   {0,0,(float)gBackgroundTexture.width,(float)gBackgroundTexture.height},
                   {0,0,(float)SCREEN_WIDTH,(float)SCREEN_HEIGHT},
                   {0,0},0.0f,WHITE);

    if (!gGameOver)
    {
        float paddleScale = 0.25f;
        float ballScale   = 0.15f;

        // P1
        DrawTexturePro(gP1Texture,
                       {0,0,(float)gP1Texture.width,(float)gP1Texture.height},
                       {gP1Position.x,
                        gP1Position.y - (gP1Texture.height*paddleScale)/2,
                        gP1Texture.width*paddleScale,
                        gP1Texture.height*paddleScale},
                       {0,0},0.0f,WHITE);

        // P2
        DrawTexturePro(gP2Texture,
                       {0,0,(float)gP2Texture.width,(float)gP2Texture.height},
                       {gP2Position.x - gP2Texture.width*paddleScale,
                        gP2Position.y - (gP2Texture.height*paddleScale)/2,
                        gP2Texture.width*paddleScale,
                        gP2Texture.height*paddleScale},
                       {0,0},0.0f,WHITE);

        // Balls
        for (int i = 0; i < gNumBalls; i++)
        {
            DrawTexturePro(gBallTexture,
                           {0,0,(float)gBallTexture.width,(float)gBallTexture.height},
                           {gBallPositions[i].x,
                            gBallPositions[i].y,
                            gBallTexture.width*ballScale,
                            gBallTexture.height*ballScale},
                           {0,0},0.0f,WHITE);
        }
    }
    else
    {
        Texture2D winTexture = (gWinner == 1) ? gWin1Texture : gWin2Texture;

        DrawTexturePro(winTexture,
                       {0,0,(float)winTexture.width,(float)winTexture.height},
                       {(SCREEN_WIDTH - winTexture.width*0.5f)/2,
                        (SCREEN_HEIGHT - winTexture.height*0.5f)/2,
                        winTexture.width*0.5f,
                        winTexture.height*0.5f},
                       {0,0},0.0f,WHITE);
    }

    EndDrawing();
}

// --------------------------------------------------

void shutdown()
{
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gP1Texture);
    UnloadTexture(gP2Texture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gWin1Texture);
    UnloadTexture(gWin2Texture);

    CloseWindow();
}

// --------------------------------------------------

int main()
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        float ticks = (float)GetTime();
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks = ticks;

        processInput(deltaTime);
        update(deltaTime);
        render();
    }

    shutdown();
    return 0;
}