#ifndef BOARD_H
#define BOARD_H

#define TILE_HAS_BOMB 1

#include "game.h"

typedef struct{
	int *tile;
	int *shown;

	int width;
	int height;
	int bombs;

	int texture_width;
	int texture_height;

	int mouse_down;
	int mouse_up;
} Board;

Board * Board_Create(int width, int height, int bombs);

void Board_Update(Game *game, Board *board);

void Board_Render(Game *game, Board *board);

void Board_Destroy(Board *board);

#endif
