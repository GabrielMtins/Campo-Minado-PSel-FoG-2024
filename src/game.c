#include "game.h"
#include "draw.h"
#include "board.h"

#include <time.h>

static const int color_black[] = {0x00, 0x00, 0x00, 0xFF}; 
static const int color_grey[] = {0x10, 0x10, 0x10, 0xFF}; 
static const int color_white[] = {0xFF, 0xFF, 0xFF, 0xFF}; 

Game * Game_Create(void){
	Game *game;
	srand(4);
	//srand(time(NULL));

	game = (Game *) malloc(sizeof(Game));

	game->context = Context_Create("Campo Minado");

	game->board_texture = Texture_Load(game->context, "res/tile.png", 16, 16);

	game->board = (void *) Board_Create(9, 9, 10);
	
	return game;
}

void Game_Update(Game *game){
	Board_Update(game, game->board);
}

void Game_Render(Game *game){
	SDL_SetRenderDrawColor(game->context->renderer, 0x10, 0x10, 0x10, 0xff);
	SDL_RenderClear(game->context->renderer);
	//Draw_DrawRect(game->context, NULL, color_grey);
	Board_Render(game, game->board);
	SDL_RenderPresent(game->context->renderer);
}

void Game_Run(Game *game){
	while(!game->context->quit){
		Context_PollEvent(game->context);
		
		Game_Update(game);
		Game_Render(game);
		
		Context_UpdateTime(game->context);
	}
}

void Game_Destroy(Game *game){
	Board_Destroy(game->board);
	Texture_Destroy(game->board_texture);
	Context_Destroy(game->context);
}
