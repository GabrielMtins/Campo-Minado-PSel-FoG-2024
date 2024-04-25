#include "board.h"

#define TILE_SIZE 16

static int Board_CheckBounds(Board *board, int x, int y);
static int Board_BombCount(Board *board, int x, int y);
static void Board_Generate(Board *board);
static void Board_GetOffset(Board *board, int *offset_x, int *offset_y);
static void Board_GetMouseTile(Game *game, Board *board, int *tile_x, int *tile_y);
static void Board_FloodFill(Board *board, int x, int y);
static void Board_ShowTile(Board *board, int x, int y);

static int Board_CheckBounds(Board *board, int x, int y){
	if(x < 0 || y < 0 || x >= board->width || y >= board->height) return 0;

	return 1;
}

static int Board_BombCount(Board *board, int x, int y){
	int dx, dy, num_bombs;

	num_bombs = 0;

	if(!Board_CheckBounds(board, x, y)) return -1;

	for(int i = -1; i <= 1; i++){
		for(int j = -1; j <= 1; j++){
			dx = x + i;
			dy = y + j;

			if(i == 0 && j == 0) continue;

			if(!Board_CheckBounds(board, dx, dy)) continue;

			if(board->tile[dx + dy * board->width] == TILE_HAS_BOMB)
				num_bombs++;
		}
	}

	return num_bombs;
}

static void Board_Generate(Board *board){
	int id;

	for(int i = 0; i < board->width * board->height; i++){
		board->shown[i] = 0;
		board->tile[i] = 0;
	}

	for(int i = 0; i < board->bombs; i++){
		id = rand() % (board->width * board->height);

		if(board->tile[id] == TILE_HAS_BOMB){
			i--;
			continue;
		}

		board->tile[id] = TILE_HAS_BOMB;
	}
}

static void Board_GetOffset(Board *board, int *offset_x, int *offset_y){
	*offset_x = (INTERNAL_WIDTH - board->texture_width) / 2;
	*offset_y = (INTERNAL_HEIGHT - board->texture_height) / 2;
}

static void Board_GetMouseTile(Game *game, Board *board, int *tile_x, int *tile_y){
	int mouse_x, mouse_y, offset_x, offset_y;

	Context_GetMousePosition(game->context, &mouse_x, &mouse_y);
	Board_GetOffset(board, &offset_x, &offset_y);

	*tile_x = (mouse_x - offset_x) / TILE_SIZE;
	*tile_y = (mouse_y - offset_y) / TILE_SIZE;

	if(mouse_x <= offset_x) *tile_x = -1;
	if(mouse_y <= offset_y) *tile_y = -1;
}

static void Board_FloodFill(Board *board, int x, int y){
	int id;

	id = x + y * board->width;

	if(!Board_CheckBounds(board, x, y)) return;
	if(board->shown[id] == 1) return;
	if(board->tile[id] == TILE_HAS_BOMB) return;

	board->shown[id] = 1;

	if(Board_BombCount(board, x, y) == 0){
		Board_FloodFill(board, x - 1, y);
		Board_FloodFill(board, x + 1, y);
		Board_FloodFill(board, x, y - 1);
		Board_FloodFill(board, x, y + 1);
		Board_FloodFill(board, x - 1, y - 1);
		Board_FloodFill(board, x - 1, y + 1);
		Board_FloodFill(board, x + 1, y - 1);
		Board_FloodFill(board, x + 1, y + 1);
	}
}

static void Board_ShowTile(Board *board, int x, int y){
	int id;

	if(!Board_CheckBounds(board, x, y)) return;

	id = x + y * board->width;

	Board_FloodFill(board, x, y);

	board->shown[id] = 1;
}

Board * Board_Create(int width, int height, int bombs){
	Board *board;

	board = (Board *) malloc(sizeof(Board));

	board->width = width;
	board->height = height;
	board->bombs = bombs;

	board->tile = (int *) malloc(sizeof(int) * width * height);
	board->shown = (int *) malloc(sizeof(int) * width * height);

	board->texture_width = board->width * TILE_SIZE;
	board->texture_height = board->height * TILE_SIZE;
	board->mouse_down = 0;
	board->mouse_up = 1;
	
	Board_Generate(board);

	return board;
}

void Board_Update(Game *game, Board *board){
	int tile_mouse_x, tile_mouse_y;

	if(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(1)){
		board->mouse_down = 1;
		board->mouse_up = 0;
	}
	else if(board->mouse_down == 1 && board->mouse_up == 1){
		board->mouse_down = 0;

		Board_GetMouseTile(game, board, &tile_mouse_x, &tile_mouse_y);
		Board_ShowTile(board, tile_mouse_x, tile_mouse_y);
	}
	else{
		board->mouse_up = 1;
	}
}

void Board_Render(Game *game, Board *board){
	int id, offset_x, offset_y;
	int tile_mouse_x, tile_mouse_y;
	int texture_id;

	Board_GetOffset(board, &offset_x, &offset_y);
	Board_GetMouseTile(game, board, &tile_mouse_x, &tile_mouse_y);

	for(int i = 0; i < board->width; i++){
		for(int j = 0; j < board->height; j++){
			id = i + j * board->width;

			texture_id = Board_BombCount(board, i, j);

			if(board->tile[id] == TILE_HAS_BOMB) texture_id = 10;

			if(!board->shown[id]) texture_id = 11;

			if(board->mouse_down && i == tile_mouse_x && j == tile_mouse_y)
				texture_id = 0;

			Texture_RenderCell(
					game->context, 
					game->board_texture,
					i * TILE_SIZE + offset_x,
					j * TILE_SIZE + offset_y,
					texture_id
					);
		}
	}
}

void Board_Destroy(Board *board){
	free(board->tile);
	free(board->shown);
}
