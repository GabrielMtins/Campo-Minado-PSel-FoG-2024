#include "game.h"
#include "draw.h"
#include "board.h"
#include "menu.h"

#include <time.h>

static const int color_black[] = {0x00, 0x00, 0x00, 0xFF}; 
static const int color_grey[] = {0x10, 0x10, 0x10, 0xFF}; 
static const int color_white[] = {0xFF, 0xFF, 0xFF, 0xFF}; 

Game * Game_Create(void){
	Game *game;
	srand(time(NULL));

	game = (Game *) malloc(sizeof(Game));

	game->context = Context_Create("Campo Minado");

	game->board_texture = Texture_Load(game->context, "res/tile.png", 16, 16);
	game->button_texture = Texture_Load(game->context, "res/button.png", 160, 32);
	game->backretry_texture = Texture_Load(game->context, "res/button2.png", 32, 32);

	Menu_Init(game);
	
	return game;
}

void Game_Update(Game *game){
	Menu_Update(game);
}

void Game_Render(Game *game){
	SDL_SetRenderDrawColor(game->context->renderer, 0x10, 0x10, 0x10, 0xff);
	SDL_RenderClear(game->context->renderer);
	Menu_Render(game);
	SDL_RenderPresent(game->context->renderer);
}

void Game_Loop(Game *game){
	Context_PollEvent(game->context);
		
	Game_Update(game);
	Game_Render(game);
		
	Context_UpdateTime(game->context);
}

void Game_Run(Game *game){
	while(!game->context->quit){
		Game_Loop(game);
	}
}

void Game_Destroy(Game *game){
	Menu_Quit();
	Texture_Destroy(game->board_texture);
	Texture_Destroy(game->button_texture);
	Texture_Destroy(game->backretry_texture);
	Context_Destroy(game->context);

	free(game);
}
