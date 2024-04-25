#ifndef BOARD_H
#define BOARD_H

#define TILE_HAS_BOMB 1
#define TILE_ID_FLAG 12
#define TILE_ID_NOTOPEN 11
#define TILE_ID_BOMB 10

enum SHOWN{
	NOT_SHOWN = 0,
	SHOWN,
	SHOWN_FLAG
};

#include "game.h"

typedef struct{
	int *tile;
	int *shown;

	int width;
	int height;
	int bombs;

	int texture_width;
	int texture_height;

	uint32_t mouse_state;
	int mouse_down;
	int mouse_up;
} Board;

Board * Board_Create(int width, int height, int bombs);

int Board_HasLost(Board *board);

int Board_HasWon(Board *board);

void Board_Update(Game *game, Board *board);

void Board_Render(Game *game, Board *board);

void Board_Destroy(Board *board);

#endif
