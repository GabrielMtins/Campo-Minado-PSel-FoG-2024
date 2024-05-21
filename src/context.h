#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define INTERNAL_WIDTH 640
#define INTERNAL_HEIGHT 360
#define FPS_MAX 60

/* Contexto para renderização do jogo
 * Estarei usando o renderer do SDL2 */

typedef struct{
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;

	double delta_time;
	int first_time;
	int quit;

	int window_width, window_height;
} Context;

/* Cria um objeto para contexto */
Context * Context_Create(const char *title);

/* Lê a posição atual do mouse na tela */
void Context_GetMousePosition(Context *context, int *x, int *y);

/* Atualiza a leitura de eventos */
void Context_PollEvent(Context *context);

/* Atualiza o delta time interno */
void Context_UpdateTime(Context *context);

/* Libera a memória utilizada pelo contexto */
void Context_Destroy(Context *context);

#endif
