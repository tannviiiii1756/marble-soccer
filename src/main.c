#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "render.h"
#include "game.h"

int run_render();
int run_no_render();
int run_baseline_agent();   // <-- NEW

int main(int argc, char *argv[])
{
    bool render = false;
    bool baseline_agent = false;   // <-- NEW

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--render") == 0)
        {
            render = true;
        }
        else if (strcmp(argv[i], "--agent") == 0 && i + 1 < argc)
        {
            if (strcmp(argv[i + 1], "baseline") == 0)
            {
                baseline_agent = true;
            }
        }
    }

    srand((unsigned int)time(NULL));

    if (baseline_agent)
        return run_baseline_agent();

    return render ? run_render() : run_no_render();
}

int run_render()
{
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error initializing SDL2: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Marble soccer simulation",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          PLAYGROUND_WIDTH, PLAYGROUND_HEIGHT,
                                          SDL_WINDOW_SHOWN);

    if (!window)
    {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    game_play(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int run_no_render()
{
    game_play_no_render();
    return 0;
}

// NEW FUNCTION
int run_baseline_agent()
{
    printf("Running baseline agent simulation...\n");
    game_play_baseline_agent();   // <-- You will implement this in game.c
    return 0;
}
