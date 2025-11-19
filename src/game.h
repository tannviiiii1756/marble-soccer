#ifndef GAME_H_
#define GAME_H_
#include <SDL.h>

#include "playground.h"
#include "constants.h"
#include "ball.h"

typedef enum
{
    IDLE,
    RUNNING
} GameState;

typedef struct
{
    unsigned int team1;
    unsigned int team2;
} Score;

typedef struct
{
    Playground playground;
    Ball team1[NUM_TEAM_PLAYERS];
    Ball team2[NUM_TEAM_PLAYERS];
    Ball ball;
    GameState state;
    Score score;
    Ball *balls[NUM_TEAM_PLAYERS * 2 + 1];
    short turn;
} Game;

void game_play(SDL_Renderer *renderer);
void game_play_no_render();

// --------------------------------------------------
// NEW: Baseline Agent Simulator
// --------------------------------------------------
void game_play_baseline_agent();

#endif
