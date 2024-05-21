#include "menu.h"
#include "board.h"
#include "draw.h"

#define BUTTON_OFFSET_X ((INTERNAL_WIDTH - BUTTON_WIDTH) / 2)
#define BUTTON_WIDTH 160
#define BUTTON_HEIGHT 32

static const int timer_fg[4] = {0xff, 0xff, 0xff, 0xff};
static const int timer_bg[4] = {0x60, 0x60, 0x60, 0xff};

enum BUTTON_ID{
	BUTTON_ID_PLAY = 0,
	BUTTON_ID_YOUWIN,
	BUTTON_ID_GAMEOVER,
	BUTTON_ID_EASY,
	BUTTON_ID_MEDIUM,
	BUTTON_ID_HARD,
	BUTTON_ID_CUSTOM,
	BUTTON_ID_AGAIN,
	BUTTON_ID_BACK,
	BUTTON_ID_TITLE,
	BUTTON_ID_REPEAT,
	BACKRETRY_ID_BACK = 0,
	BACKRETRY_ID_RETRY = 1,
	BACKRETRY_ID_MINUS = 2,
	BACKRETRY_ID_PLUS = 3
};

enum MENU_STATES{
	MENU_MAIN = 0,
	MENU_PLAY,
	MENU_DIFFICULTY,
	MENU_WIN,
	MENU_GAMEOVER,
};

typedef struct{
	int x;
	int y;
	int ini;
	int end;
	Texture *texture;
	int id;
	double timer;
	int is_label;
} Button;

struct{
	int state;
	Board *board;

	int mouse_down;
	int mouse_up;

	Button back;
	Button retry;

	double timer;

	struct{
		Button title;
		Button play;
	} mainmenu;

	struct{
		Button easy;
		Button medium;
		Button hard;
	} difficulty;

	struct{
		Button gameover_button;
		Button again;
		Button back;
		Button repeat;
	} gameover;

	struct{
		Button youwin_button;
		Button again;
		Button back;
		Button repeat;
	} youwin;

	struct{
		int w, h;
		int bombs;
	} board_options;
} menu;

/* Retorna 1 caso o mouse esteja dentro do retângulo */
static int Menu_IsInsideButton(Game *game, int *rect);

/* Cria um botão */
static Button Button_Create(int x, int y, int ini, int end, Texture *texture, int id);

/* Reseta os membros do botão */
static void Button_Reset(Button *button);

/* Atualiza o botão */
static void Button_Update(Game *game, Button *button);

/* Renderiza o botão */
static void Button_Render(Game *game, Button *button);

/* Retorna 1 caso o botão foi pressionado, 0 caso contrário */
static int Button_Pressed(Game *game, Button *button);

static const int color_button_overlay[4] = {255, 255, 255, 50};
static const int color_button_click[4] = {255, 255, 255, 100};

static int Menu_IsInsideButton(Game *game, int *rect){
	int mouse_x, mouse_y;
	int x, y, w, h;

	Context_GetMousePosition(game->context, &mouse_x, &mouse_y);

	x = rect[0];
	y = rect[1];
	w = rect[2];
	h = rect[3];

	if(mouse_x < x) return 0;
	if(mouse_x > x + w) return 0;
	if(mouse_y < y) return 0;
	if(mouse_y > y + h) return 0;

	return 1;
}

static Button Button_Create(int x, int y, int ini, int end, Texture *texture, int id){
	Button button;

	button.x = x;
	button.y = ini;
	button.ini = ini;
	button.end = end;
	button.texture = texture;
	button.id = id;
	button.timer = 0;
	button.is_label = 0;

	return button;
}

static void Button_Reset(Button *button){
	button->timer = 0;
	button->y = button->ini;
}

static void Button_Update(Game *game, Button *button){
	button->timer += game->context->delta_time;
	button->y = button->ini + Menu_Smooth(button->timer) * (button->end - button->ini);
}

static void Button_Render(Game *game, Button *button){
	int rect[4];

	Texture_RenderCell(
			game->context,
			button->texture,
			button->x,
			button->y,
			button->id
			);

	rect[0] = button->x;
	rect[1] = button->y;
	rect[2] = button->texture->cell_width;
	rect[3] = button->texture->cell_height;

	if(button->is_label) return;

	if(Menu_IsInsideButton(game, rect)){
		if(menu.mouse_down)
			Draw_DrawRect(game->context, rect, color_button_click);
		else
			Draw_DrawRect(game->context, rect, color_button_overlay);
	}
}

static int Button_Pressed(Game *game, Button *button){
	int mouse_x, mouse_y;
	int x, y, w, h;

	Context_GetMousePosition(game->context, &mouse_x, &mouse_y);

	x = button->x;
	y = button->y;
	w = button->texture->cell_width;
	h = button->texture->cell_height;

	if(mouse_x < x) return 0;
	if(mouse_x > x + w) return 0;
	if(mouse_y < y) return 0;
	if(mouse_y > y + h) return 0;

	if(menu.mouse_down == 1 && menu.mouse_up == 1)
		return 1;

	return 0;
}

double Menu_Smooth(double i){
	if(i > 1) return 1;
	if(i < 0) return 0;
	return ((6*i - 15)*i + 10)*i*i*i;
}

void Menu_Init(Game *game){
	menu.state = MENU_MAIN;
	menu.board = NULL;

	/* Aqui vou criar cada botão do menu */

	menu.mainmenu.title = Button_Create(
			BUTTON_OFFSET_X,
			0,
			-60,
			60,
			game->button_texture,
			BUTTON_ID_TITLE
			);

	menu.mainmenu.title.is_label = 1;

	menu.mainmenu.play = Button_Create(
			BUTTON_OFFSET_X,
			0,
			360,
			160,
			game->button_texture,
			BUTTON_ID_PLAY
			);

	menu.gameover.gameover_button = Button_Create(
			BUTTON_OFFSET_X,
			0,
			-40,
			100,
			game->button_texture,
			BUTTON_ID_GAMEOVER
			);

	menu.gameover.gameover_button.is_label = 1;

	menu.youwin.youwin_button = Button_Create(
			BUTTON_OFFSET_X,
			0,
			-40,
			100,
			game->button_texture,
			BUTTON_ID_YOUWIN
			);

	menu.youwin.youwin_button.is_label = 1;

	menu.youwin.again = Button_Create(
			BUTTON_OFFSET_X,
			0,
			360,
			160,
			game->button_texture,
			BUTTON_ID_AGAIN
			);

	menu.youwin.back = Button_Create(
			BUTTON_OFFSET_X,
			0,
			360,
			200,
			game->button_texture,
			BUTTON_ID_BACK
			);

	menu.youwin.repeat = Button_Create(
			BUTTON_OFFSET_X,
			0,
			360,
			240,
			game->button_texture,
			BUTTON_ID_REPEAT
			);

	menu.gameover.back = menu.youwin.back;
	menu.gameover.again = menu.youwin.again;
	menu.gameover.repeat = menu.youwin.repeat;

	menu.difficulty.easy = Button_Create(
			BUTTON_OFFSET_X,
			0,
			360,
			120,
			game->button_texture,
			BUTTON_ID_EASY
			);

	menu.difficulty.medium = Button_Create(
			BUTTON_OFFSET_X,
			0,
			360,
			160,
			game->button_texture,
			BUTTON_ID_MEDIUM
			);

	menu.difficulty.hard = Button_Create(
			BUTTON_OFFSET_X,
			0,
			360,
			200,
			game->button_texture,
			BUTTON_ID_HARD
			);

	menu.back = Button_Create(
			20,
			0,
			20,
			20,
			game->backretry_texture,
			BACKRETRY_ID_BACK
			);

	menu.retry = Button_Create(
			60,
			0,
			20,
			20,
			game->backretry_texture,
			BACKRETRY_ID_RETRY
			);
}

void Menu_Update(Game *game){
	/* Atualizando as características do mouse */
	if(SDL_GetMouseState(NULL, NULL)){
		menu.mouse_down = 1;
		menu.mouse_up = 0;
	}
	else if(menu.mouse_down == 1 && menu.mouse_up == 1){
		menu.mouse_down = 0;
	}
	else{
		menu.mouse_up = 1;
	}

	/* Fazendo um switch para cada estado do menu */
	switch(menu.state){
		case MENU_MAIN:
			Button_Update(game, &menu.mainmenu.play);
			Button_Update(game, &menu.mainmenu.title);

			if(Button_Pressed(game, &menu.mainmenu.play)){
				/*
				Button_Reset(&menu.difficulty.easy);
				Button_Reset(&menu.difficulty.medium);
				Button_Reset(&menu.difficulty.hard);
				*/
				menu.state = MENU_DIFFICULTY;
			}

			break;

		case MENU_DIFFICULTY:
			Button_Update(game, &menu.difficulty.easy);
			Button_Update(game, &menu.difficulty.medium);
			Button_Update(game, &menu.difficulty.hard);
			Button_Update(game, &menu.back);

			if(Button_Pressed(game, &menu.back)){
				menu.state = MENU_MAIN;
			}

			if(Button_Pressed(game, &menu.difficulty.easy)){
				/* Se a dificuldade for fácil,
				 * criar um campo 9x9 com 10 bombas */
				menu.state = MENU_PLAY;
				menu.board = Board_Create(9, 9, 10);

				menu.board_options.w = menu.board_options.h = 9;
				menu.board_options.bombs = 10;
				menu.timer = 0;
			}

			if(Button_Pressed(game, &menu.difficulty.medium)){
				/* Se a dificuldade for média,
				 * criarr um campo 16x16 com 40 bombas */
				menu.state = MENU_PLAY;
				menu.board = Board_Create(16, 16, 40);

				menu.board_options.w = menu.board_options.h = 16;
				menu.board_options.bombs = 40;
				menu.timer = 0;
			}

			if(Button_Pressed(game, &menu.difficulty.hard)){
				/* Se a dificuldade for difícil,
				 * criar um campo 24x20 com 99 bombas */
				menu.state = MENU_PLAY;
				menu.board = Board_Create(24, 20, 99);

				menu.board_options.w = 24;
				menu.board_options.h = 20;
				menu.board_options.bombs = 99;
				menu.timer = 0;
			}

			break;

		case MENU_PLAY:
			Board_Update(game, menu.board);
			Button_Update(game, &menu.back);
			Button_Update(game, &menu.retry);

			menu.timer += game->context->delta_time;

			if(Board_HasWon(menu.board)){
				menu.state = MENU_WIN;
				Button_Reset(&menu.youwin.youwin_button);
				Button_Reset(&menu.youwin.again);
				Button_Reset(&menu.youwin.back);
				Button_Reset(&menu.youwin.repeat);
			}

			if(Board_HasLost(menu.board)){
				menu.state = MENU_GAMEOVER;
				Button_Reset(&menu.gameover.gameover_button);
				Button_Reset(&menu.gameover.again);
				Button_Reset(&menu.gameover.back);
				Button_Reset(&menu.gameover.repeat);
			}

			if(Button_Pressed(game, &menu.retry)){
				menu.state = MENU_PLAY;
				Board_Destroy(menu.board);

				menu.board = Board_Create(
						menu.board_options.w,
						menu.board_options.h,
						menu.board_options.bombs
						);
				menu.timer = 0;
			}

			if(Button_Pressed(game, &menu.back)){
				menu.state = MENU_DIFFICULTY;
				Board_Destroy(menu.board);
				menu.board = NULL;
				menu.timer = 0;
			}

			break;

		case MENU_GAMEOVER:
			Button_Update(game, &menu.gameover.gameover_button);
			Button_Update(game, &menu.gameover.back);
			Button_Update(game, &menu.gameover.again);
			Button_Update(game, &menu.gameover.repeat);

			if(Button_Pressed(game, &menu.gameover.back)){
				menu.state = MENU_MAIN;
				Board_Destroy(menu.board);
				menu.board = NULL;
				menu.timer = 0;
			}

			if(Button_Pressed(game, &menu.gameover.again)){
				menu.state = MENU_PLAY;
				Board_Destroy(menu.board);

				menu.board = Board_Create(
						menu.board_options.w,
						menu.board_options.h,
						menu.board_options.bombs
						);
				menu.timer = 0;
			}

			if(Button_Pressed(game, &menu.gameover.repeat)){
				menu.state = MENU_PLAY;
				Board_RepeatMap(menu.board);
				menu.timer = 0;
			}
			break;

		case MENU_WIN:
			Button_Update(game, &menu.youwin.youwin_button);
			Button_Update(game, &menu.youwin.back);
			Button_Update(game, &menu.youwin.again);
			Button_Update(game, &menu.youwin.repeat);

			if(Button_Pressed(game, &menu.youwin.back)){
				menu.state = MENU_MAIN;
				Board_Destroy(menu.board);
				menu.board = NULL;
				menu.timer = 0;
			}

			if(Button_Pressed(game, &menu.youwin.again)){
				menu.state = MENU_PLAY;
				Board_Destroy(menu.board);

				menu.board = Board_Create(
						menu.board_options.w,
						menu.board_options.h,
						menu.board_options.bombs
						);

				menu.timer = 0;
			}

			if(Button_Pressed(game, &menu.youwin.repeat)){
				menu.state = MENU_PLAY;
				Board_RepeatMap(menu.board);
				menu.timer = 0;
			}

			break;
	}
}

void Menu_Render(Game *game){
	const uint8_t *keys = SDL_GetKeyboardState(NULL);

	char text_timer[200];
	char text_bombs[200];
	/* Formatação para printar o tempo passado */
	sprintf(text_timer, "Tempo: %02d:%05.2lf", (int) (menu.timer / 60), fmod(menu.timer, 60));

	switch(menu.state){
		case MENU_MAIN:
			Button_Render(game, &menu.mainmenu.play);
			Button_Render(game, &menu.mainmenu.title);
			break;

		case MENU_DIFFICULTY:
			Button_Render(game, &menu.difficulty.easy);
			Button_Render(game, &menu.difficulty.medium);
			Button_Render(game, &menu.difficulty.hard);
			Button_Render(game, &menu.back);
			break;

		case MENU_PLAY:
			Board_Render(game, menu.board);
			Button_Render(game, &menu.back);
			Button_Render(game, &menu.retry);

			Draw_DrawTextWithBox(
					game->context,
					20,
					100,
					4,
					timer_bg,
					timer_fg,
					text_timer
					);

			/* Formatação para printar o número de bombas */
			sprintf(text_bombs, "Bombas: %02d/%02d", menu.board->num_flags, menu.board_options.bombs);

			Draw_DrawTextWithBox(
					game->context,
					20,
					140,
					4,
					timer_bg,
					timer_fg,
					text_bombs
					);
			break;

		case MENU_GAMEOVER:
			Board_Render(game, menu.board);

			if(!keys[SDL_SCANCODE_TAB]){
				Button_Render(game, &menu.gameover.gameover_button);
				Button_Render(game, &menu.gameover.back);
				Button_Render(game, &menu.gameover.again);
				Button_Render(game, &menu.gameover.repeat);
			}

			Draw_DrawTextWithBox(
					game->context,
					20,
					100,
					4,
					timer_bg,
					timer_fg,
					text_timer
					);

			sprintf(text_bombs, "Bombas: %02d/%02d", menu.board->num_flags, menu.board_options.bombs);

			Draw_DrawTextWithBox(
					game->context,
					20,
					140,
					4,
					timer_bg,
					timer_fg,
					text_bombs
					);
			break;

		case MENU_WIN:
			Board_Render(game, menu.board);

			if(!keys[SDL_SCANCODE_TAB]){
				Button_Render(game, &menu.youwin.youwin_button);
				Button_Render(game, &menu.youwin.back);
				Button_Render(game, &menu.youwin.again);
				Button_Render(game, &menu.youwin.repeat);
			}

			Draw_DrawTextWithBox(
					game->context,
					20,
					100,
					4,
					timer_bg,
					timer_fg,
					text_timer
					);

			sprintf(text_bombs, "Bombas: %02d/%02d", menu.board->num_flags, menu.board_options.bombs);

			Draw_DrawTextWithBox(
					game->context,
					20,
					140,
					4,
					timer_bg,
					timer_fg,
					text_bombs
					);
			break;
	}
}

void Menu_Quit(void){
	if(menu.board != NULL) Board_Destroy(menu.board);
}
