#include "board.h"
#include "draw.h"

#define TILE_SIZE 16

static const int color_mouse_over[4] = {255, 255, 255, 60};
static const int color_mouse_click[4] = {255, 255, 255, 200};

static int Board_CheckBounds(Board *board, int x, int y);
static int Board_BombCount(Board *board, int x, int y);
static void Board_Generate(Board *board, int og_x, int og_y);
static void Board_GetOffset(Board *board, int *offset_x, int *offset_y);
static void Board_GetMouseTile(Game *game, Board *board, int *tile_x, int *tile_y);
static void Board_FloodFill(Board *board, int x, int y);
static void Board_OpenTile(Board *board, int x, int y);
static void Board_PutFlag(Board *board, int x, int y);
static void Board_PutFlagOnAllBombs(Board *board);
static void Board_OpenAdjacentIfFlagged(Board *board, int x, int y);

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

static void Board_Generate(Board *board, int og_x, int og_y){
	int x, y, id;

	for(int i = 0; i < board->width * board->height; i++){
		board->shown[i] = 0;
		board->tile[i] = 0;
	}

	for(int i = 0; i < board->bombs; i++){
		x = rand() % board->width;
		y = rand() % board->height;

		id = x + y * board->width;

		if(board->tile[id] == TILE_HAS_BOMB || abs(x - og_x) <= 1 || abs(y - og_y) <= 1){
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
	if(board->shown[id] == SHOWN) return;
	if(board->tile[id] == TILE_HAS_BOMB) return;

	board->shown[id] = SHOWN;

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

static void Board_OpenTile(Board *board, int x, int y){
	int id;

	if(!Board_CheckBounds(board, x, y)) return;

	id = x + y * board->width;

	if(board->first_move){
		Board_Generate(board, x, y);
		board->first_move = 0;
	}

	if(board->shown[id] == SHOWN_FLAG){
		return;
	}

	if(board->shown[id] == SHOWN){
		Board_OpenAdjacentIfFlagged(board, x, y);
		return;
	}

	Board_FloodFill(board, x, y);

	board->shown[id] = SHOWN;
}

static void Board_PutFlag(Board *board, int x, int y){
	int id;

	if(!Board_CheckBounds(board, x, y)) return;

	id = x + y * board->width;

	if(board->shown[id] == SHOWN) return;

	if(board->shown[id] == SHOWN_FLAG)
		board->shown[id] = NOT_SHOWN;
	else
		board->shown[id] = SHOWN_FLAG;

}

static void Board_PutFlagOnAllBombs(Board *board){
	int id;

	for(int i = 0; i < board->width; i++){
		for(int j = 0; j < board->height; j++){
			id = i + j * board->width;

			if(board->tile[id] == TILE_HAS_BOMB)
				board->shown[id] = SHOWN_FLAG;
		}
	}
}

static void Board_OpenAdjacentIfFlagged(Board *board, int x, int y){
	int id, bomb_count, flag_count;

	bomb_count = Board_BombCount(board, x, y);
	flag_count = 0;

	for(int i = -1 + x; i <= 1 + x; i++){
		for(int j = -1 + y; j <= 1 + y; j++){
			if(!Board_CheckBounds(board, i, j)) continue;

			id = i + j * board->width;

			if(board->shown[id] == SHOWN_FLAG) flag_count++;
		}
	}

	if(flag_count == bomb_count){
		for(int i = -1 + x; i <= 1 + x; i++){
			for(int j = -1 + y; j <= 1 + y; j++){
				if(!Board_CheckBounds(board, i, j)) continue;
	
				id = i + j * board->width;

				if(board->shown[id] == NOT_SHOWN) board->shown[id] = SHOWN;
			}
		}
	}
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

	board->first_move = 1;
	
	Board_Generate(board, 0, 0);

	return board;
}

int Board_HasLost(Board *board){
	int id;

	for(int i = 0; i < board->width; i++){
		for(int j = 0; j < board->height; j++){
			id = i + j * board->width;

			/* Se há uma bomba que foi exposta */
			if(board->shown[id] == SHOWN && board->tile[id] == TILE_HAS_BOMB)
				return 1;
		}
	}

	return 0;
}

int Board_HasWon(Board *board){
	int id;
	/* Basicamente, o jogo é ganhado quando todos os blocos
	 * que não são bombas são expostos. */

	if(Board_HasLost(board)) return 0;

	for(int i = 0; i < board->width; i++){
		for(int j = 0; j < board->height; j++){
			id = i + j * board->width;

			/* Se há um tile que não é uma bomba e não foi exposta,
			 * o jogo não foi ganho ainda */
			if(board->shown[id] != SHOWN && board->tile[id] != TILE_HAS_BOMB)
				return 0;
		}
	}

	return 1;
}

void Board_Update(Game *game, Board *board){
	int tile_mouse_x, tile_mouse_y;
	uint32_t new_mouse_state;

	new_mouse_state = SDL_GetMouseState(NULL, NULL);

	if(new_mouse_state){
		board->mouse_down = 1;
		board->mouse_up = 0;
		board->mouse_state = new_mouse_state;
	}
	else if(board->mouse_down == 1 && board->mouse_up == 1){
		board->mouse_down = 0;
		Board_GetMouseTile(game, board, &tile_mouse_x, &tile_mouse_y);

		if(board->mouse_state & SDL_BUTTON(1)){
			Board_OpenTile(board, tile_mouse_x, tile_mouse_y);
		}
		else if(board->mouse_state & SDL_BUTTON(3)){
			Board_PutFlag(board, tile_mouse_x, tile_mouse_y);
		}
	}
	else{
		board->mouse_up = 1;
	}

	if(Board_HasWon(board))
		Board_PutFlagOnAllBombs(board);
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

			if(board->tile[id] == TILE_HAS_BOMB) texture_id = TILE_ID_BOMB;

			if(board->shown[id] == NOT_SHOWN) texture_id = TILE_ID_NOTOPEN;

			if(board->shown[id] == SHOWN_FLAG) texture_id = TILE_ID_FLAG;

			Texture_RenderCell(
					game->context, 
					game->board_texture,
					i * TILE_SIZE + offset_x,
					j * TILE_SIZE + offset_y,
					texture_id
					);

			if(i == tile_mouse_x && j == tile_mouse_y){
				int rect[4] = {
					i * TILE_SIZE + offset_x,
					j * TILE_SIZE + offset_y,
					TILE_SIZE, TILE_SIZE
				};

				if(board->mouse_down)
					Draw_DrawRect(game->context, rect, color_mouse_click);
				else
					Draw_DrawRect(game->context, rect, color_mouse_over);
			}
		}
	}
}

void Board_Destroy(Board *board){
	free(board->tile);
	free(board->shown);
}
