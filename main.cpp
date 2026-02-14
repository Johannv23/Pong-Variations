#include "CS3113/cs3113.h"
#include <cmath>

constexpr int SCREEN_WIDTH  = 1600 / 2;
constexpr int SCREEN_HEIGHT = 900 / 2;
constexpr int FPS = 60;

constexpr float MAX_AMP = 10.0f;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
constexpr Vector2 BASE_SIZE = { 200.0f, 200.0f };

/**

* Author: Johann Varghese

* Assignment: Pong Clone

* Date due: [02/14/2026]

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/

constexpr char STAGE_FP[] = "assets/stage.png";
constexpr char MUSIC_NOTE_FP[] = "assets/musicnote.png";
constexpr char SPOTLIGHT_FP[] = "assets/spotlight.png";
constexpr char ARTIST_FP[] = "assets/xavier.png";

AppStatus gAppStatus = RUNNING;

Vector2 gArtistPosition = ORIGIN;
Vector2 gArtistScale = BASE_SIZE;
float gPulseTime = 0.0f;
float gFlipTimer = 0.0f;
bool gFacingRight = true;

Vector2 gSpotlightPosition;
Vector2 gSpotlightScale = {400.0f, 400.0f};
float   gSpotlightTime  = 0.0f;

Vector2 gNotePosition = {100.0f, 300.0f};
float   gNoteTime     = 0.0f;

Texture2D gStageTexture;
Texture2D gArtistTexture;
Texture2D gSpotlightTexture;
Texture2D gMusicNoteTexture;

float gPreviousTicks = 0.0f;

void initialise();
void processInput();
void update();
void render();
void shutdown();


void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Concert Scene");
    gStageTexture = LoadTexture(STAGE_FP);
    gArtistTexture = LoadTexture(ARTIST_FP);
    gSpotlightTexture = LoadTexture(SPOTLIGHT_FP);
    gMusicNoteTexture = LoadTexture(MUSIC_NOTE_FP);
    SetTargetFPS(FPS);
}

void processInput()
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks = (float)GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    gPulseTime += deltaTime;

    gArtistPosition.x = ORIGIN.x + sin(gPulseTime * 2.0f) * 20.0f;

    gFlipTimer += deltaTime;
    if (gFlipTimer >= 2.0f)
    {
        gFacingRight = !gFacingRight;
        gFlipTimer = 0.0f;
    }

    float pulse = BASE_SIZE.x + MAX_AMP * cos(gPulseTime);

    if (gFacingRight)
        gArtistScale.x = pulse;
    else
        gArtistScale.x = -pulse;

    gArtistScale.y = BASE_SIZE.y + MAX_AMP * cos(gPulseTime);

    gSpotlightTime += deltaTime;

    gSpotlightPosition.x = gArtistPosition.x + cos(gSpotlightTime) * 150.0f;
    gSpotlightPosition.y = gArtistPosition.y - 250.0f;

    float spotlightPulse = 350.0f + 50.0f * sin(gSpotlightTime * 2.0f);
    gSpotlightScale = {spotlightPulse, spotlightPulse};

    gNoteTime += deltaTime;

    gNotePosition.x = gArtistPosition.x + cos(gNoteTime * 3.0f) * 250.0f;
    gNotePosition.y = gArtistPosition.y + sin(gNoteTime * 3.0f) * 100.0f;
}

void render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle stageSource = {0, 0,
        (float)gStageTexture.width,
        (float)gStageTexture.height};

    Rectangle stageDest = {
        SCREEN_WIDTH / 2.0f,
        SCREEN_HEIGHT / 2.0f,
        (float)SCREEN_WIDTH,
        (float)SCREEN_HEIGHT};

    Vector2 stageOrigin = {
        SCREEN_WIDTH / 2.0f,
        SCREEN_HEIGHT / 2.0f};

    DrawTexturePro(gStageTexture, stageSource, stageDest, stageOrigin, 0.0f, WHITE);

    Rectangle spotlightSource = {0, 0,
        (float)gSpotlightTexture.width,
        (float)gSpotlightTexture.height};

    Rectangle spotlightDest = {
        gSpotlightPosition.x,
        gSpotlightPosition.y,
        gSpotlightScale.x,
        gSpotlightScale.y};

    Vector2 spotlightOrigin = {
        gSpotlightScale.x / 2.0f,
        0.0f
    };

    DrawTexturePro(gSpotlightTexture, spotlightSource, spotlightDest,
                   spotlightOrigin, 0.0f, {255,255,255,200});

    Rectangle performerSource = {0, 0,
        (float)gArtistTexture.width,
        (float)gArtistTexture.height};

    Rectangle performerDest = {
        gArtistPosition.x,
        gArtistPosition.y,
        fabs(gArtistScale.x),
        gArtistScale.y};

    Vector2 performerOrigin = {
        fabs(gArtistScale.x) / 2.0f,
        gArtistScale.y / 2.0f};

    DrawTexturePro(gArtistTexture, performerSource, performerDest,
                   performerOrigin, 0.0f, WHITE);

    Rectangle noteSource = {0, 0,
        (float)gMusicNoteTexture.width,
        (float)gMusicNoteTexture.height};

    Rectangle noteDest = {
        gNotePosition.x,
        gNotePosition.y,
        100.0f,
        100.0f};

    Vector2 noteOrigin = {50.0f, 50.0f};

    DrawTexturePro(gMusicNoteTexture, noteSource, noteDest, noteOrigin, 0.0f, WHITE);

    EndDrawing();
}

void shutdown()
{
    UnloadTexture(gStageTexture);
    UnloadTexture(gArtistTexture);
    UnloadTexture(gSpotlightTexture);
    UnloadTexture(gMusicNoteTexture);
    CloseWindow();
}

int main()
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();
    return 0;
}
