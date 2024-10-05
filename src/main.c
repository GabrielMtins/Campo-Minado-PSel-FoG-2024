#include "context.h"
#include "game.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

static Game *game;

/* Cria uma instância do objeto game e inicializa o jogo
 * olhar o arquivo game.h para implementações */

#ifdef __EMSCRIPTEN__
static void main_loop(void){
	Game_Loop(game);
}
#endif

int main(int argc, char **argv){
	game = Game_Create();
	#ifdef __EMSCRIPTEN__	
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	Game_Run(game);
	#endif

	Game_Destroy(game);

	return 0;
}
