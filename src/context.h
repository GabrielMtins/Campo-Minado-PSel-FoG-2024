#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define INTERNAL_WIDTH 480
#define INTERNAL_HEIGHT 272

typedef struct{
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;

	double delta_time;
	int first_time;
	int quit;

	int window_width, window_height;
} Context;

Context * Context_Create(const char *title);

void Context_GetMousePosition(Context *context, int *x, int *y);

void Context_PollEvent(Context *context);

void Context_UpdateTime(Context *context);

void Context_Destroy(Context *context);

#endif
