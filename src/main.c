#include "context.h"
#include "game.h"

int main(void){
	Game *game;

	game = Game_Create();
	
	Game_Run(game);

	Game_Destroy(game);

	return 0;
}
