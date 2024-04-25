#ifndef GAME_H
#define GAME_H

#include "context.h"
#include "texture.h"

typedef struct{
	Context *context;
	Texture *board_texture;
	void *board;
} Game;

Game * Game_Create(void);

void Game_Update(Game *game);

void Game_Render(Game *game);

void Game_Run(Game *game);

void Game_Destroy(Game *game);

#endif
