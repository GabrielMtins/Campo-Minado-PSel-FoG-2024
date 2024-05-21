#ifndef BOARD_H
#define BOARD_H

#define TILE_HAS_BOMB 1
#define TILE_ID_FLAG 12
#define TILE_ID_NOTOPEN 11
#define TILE_ID_BOMB 10

#include "game.h"

/* A maior parte da lógica do jogo de fato está nesse arquivo e na
 * sua implementação */

enum SHOWN{
	NOT_SHOWN = 0,
	SHOWN,
	SHOWN_FLAG
};

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

	int first_move;

	int num_flags;
} Board;

Board * Board_Create(int width, int height, int bombs);

/* Essa função é utilizada para repetir a tentativa de um mapa */
void Board_RepeatMap(Board *board);

int Board_HasLost(Board *board);

int Board_HasWon(Board *board);

void Board_Update(Game *game, Board *board);

void Board_Render(Game *game, Board *board);

void Board_Destroy(Board *board);

#endif
