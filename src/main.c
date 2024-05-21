#include "context.h"
#include "game.h"

/* Cria uma instância do objeto game e inicializa o jogo
 * olhar o arquivo game.h para implementações */

int main(int argc, char **argv){
	Game *game;

	game = Game_Create();
	
	Game_Run(game);

	Game_Destroy(game);

	return 0;
}
