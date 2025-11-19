#include <stdbool.h>
#include <time.h>
#include <SDL.h>
#include <stdlib.h>

#include "playground.h"
#include "ball.h"
#include "game.h"
#include "render.h"
#include "constants.h"
#include "simulation.h"
#include "game.h"

static void initial_placing(Game *game)
{
    Playground *playground = &(game->playground);
    Ball *ball = &(game->ball);

    Vector2D locations1[NUM_TEAM_PLAYERS] = {{3 * BALL_RADIUS / 2, playground->height / 2},
                                             {4 * BALL_RADIUS, playground->height / 2 - 3 * BALL_RADIUS},
                                             {4 * BALL_RADIUS, playground->height / 2 + 3 * BALL_RADIUS},
                                             {playground->width / 2 - 100, 150},
                                             {playground->width / 2 - 100, playground->height - 150}};
    Vector2D locations2[NUM_TEAM_PLAYERS] = {
        {playground->width - 3 * BALL_RADIUS / 2, playground->height / 2},
        {playground->width - 4 * BALL_RADIUS, playground->height / 2 - 3 * BALL_RADIUS},
        {playground->width - 4 * BALL_RADIUS, playground->height / 2 + 3 * BALL_RADIUS},
        {playground->width / 2 + 100, 150},
        {playground->width / 2 + 100, playground->height - 150}};

    for (int i = 0; i < NUM_TEAM_PLAYERS; i++)
    {
        game->team1[i].location = locations1[i];
        game->team1[i].velocity = (Vector2D){0, 0};
        game->team2[i].location = locations2[i];
        game->team2[i].velocity = (Vector2D){0, 0};
    }

    ball->radius = BALL_RADIUS;
    ball->mass = BALL_MASS;
    ball->location = (Vector2D){playground->width / 2, playground->height / 2};
    ball->velocity = (Vector2D){0, 0};
}

static void setup_game(Game *game)
{

    Playground *playground = &(game->playground);

    playground->height = PLAYGROUND_HEIGHT;
    playground->width = PLAYGROUND_WIDTH;
    playground->net_size = NET_SIZE;
    playground->deceleration = DECELERATION;

    for (int i = 0; i < NUM_TEAM_PLAYERS; i++)
    {
        game->team1[i].radius = PLAYER_RADIUS;
        game->team1[i].mass = PLAYER_MASS;
        game->team1[i].velocity = (Vector2D){0, 0};

        game->team2[i].radius = PLAYER_RADIUS;
        game->team2[i].mass = PLAYER_MASS;
        game->team2[i].velocity = (Vector2D){0, 0};
    }

    int num_balls = 1 + 2 * NUM_TEAM_PLAYERS;
    for (int i = 0; i < NUM_TEAM_PLAYERS; i++)
    {
        game->balls[i] = game->team1 + i;
        game->balls[i + NUM_TEAM_PLAYERS] = game->team2 + i;
    }
    game->balls[num_balls - 1] = &(game->ball);
    game->state = IDLE;
    game->turn = rand() % 2;

    initial_placing(game);
}

static short check_who_scored(Ball *ball, Playground *playground)
{
    float net_top = playground->height / 2 - playground->net_size / 2;
    float net_bottom = playground->height / 2 + playground->net_size / 2;
    float ball_y = ball->location.y;
    float ball_left = ball->location.x - ball->radius;
    float ball_right = ball->location.x + ball->radius;

    if (ball_y > net_top && ball_y < net_bottom)
    {
        if (ball_left < 0)
            return 2;
        if (ball_right > playground->width)
            return 1;
    }
    return 0;
}

static void update_game(Game *game, bool verbose)
{
    if (game->state == IDLE)
    {
        int player_index = rand() % NUM_TEAM_PLAYERS;
        Ball *player;

        if (game->turn)
        {
            player = game->team1 + player_index;
        }
        else
        {
            player = game->team2 + player_index;
        }
        int x = rand() % (MAX_AXIS_VELOCITY - MIN_AXIS_VELOCITY + 1) + MIN_AXIS_VELOCITY;
        int y = rand() % (MAX_AXIS_VELOCITY - MIN_AXIS_VELOCITY + 1) + MIN_AXIS_VELOCITY;
        player->velocity = (Vector2D){x, y};

        game->turn = (game->turn + 1) % 2;

        game->state = RUNNING;
    }
    else
    {
        short goal = check_who_scored(&(game->ball), &(game->playground));
        if (goal)
        {
            if (goal == 1)
            {
                game->score.team1++;
                game->turn = 0;
            }
            else
            {
                game->score.team2++;
                game->turn = 1;
            }

            if (verbose)
            {
                printf("Score: %d - %d\n", game->score.team1, game->score.team2);
            }

            initial_placing(game);
        }

        if (is_simulation_idle(game->balls, NUM_TEAM_PLAYERS * 2 + 1))
            game->state = IDLE;
    }
}

void game_play(SDL_Renderer *renderer)
{
    SDL_Event event;
    int running = 1;

    const int frame_delay = 1000 / FPS;
    Uint32 frame_start;
    int frame_time;

    Game game;
    setup_game(&game);

    while (running)
    {
        frame_start = SDL_GetTicks();

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }

        update_game(&game, false);

        simulate(game.balls, &(game.playground), NUM_TEAM_PLAYERS * 2 + 1, TIME_DELTA);

        render(renderer, game.team1, game.team2, &(game.ball), NUM_TEAM_PLAYERS);

        frame_time = SDL_GetTicks() - frame_start;
        if (frame_delay > frame_time)
        {
            SDL_Delay(frame_delay - frame_time);
        }
    }
}

void game_play_no_render()
{
    srand((unsigned int)time(NULL));
    Game game;
    setup_game(&game);

    while (true)
    {
        update_game(&game, true);
        simulate(game.balls, &(game.playground), NUM_TEAM_PLAYERS * 2 + 1, TIME_DELTA);
    }
}

//
// --------------------------------------------------------------
// NEW FUNCTION: BASELINE AGENT
// --------------------------------------------------------------
//

static void baseline_choose_velocity(Ball *player, Ball *ball)
{
    float dx = ball->location.x - player->location.x;
    float dy = ball->location.y - player->location.y;

    // Normalize direction (simple way)
    float length = sqrt(dx * dx + dy * dy);
    if (length == 0)
        length = 1;

    dx /= length;
    dy /= length;

    // Scale it to allowed speed
    player->velocity.x = dx * MAX_AXIS_VELOCITY;
    player->velocity.y = dy * MAX_AXIS_VELOCITY;
}

void game_play_baseline_agent()
{
    printf("[Baseline Agent] Starting simulation...\n");

    Game game;
    setup_game(&game);

    for (int step = 0; step < 3000; step++)
    {
        // Always control team1 player 0
        Ball *agent = &game.team1[0];
        Ball *ball = &game.ball;

        baseline_choose_velocity(agent, ball);

        update_game(&game, false);
        simulate(game.balls, &(game.playground), NUM_TEAM_PLAYERS * 2 + 1, TIME_DELTA);
    }

    printf("[Baseline Agent] Finished.\n");
}
