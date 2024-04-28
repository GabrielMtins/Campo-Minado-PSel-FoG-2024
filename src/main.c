#include "context.h"
#include "game.h"

int main(int argc, char **argv){
	Game *game;

	game = Game_Create();
	
	Game_Run(game);

	Game_Destroy(game);

	return 0;
}
