#include "menu.h"
#include "board.h"
#include "draw.h"

#define BUTTON_OFFSET_X ((INTERNAL_WIDTH - BUTTON_WIDTH) / 2)
#define BUTTON_WIDTH 160
#define BUTTON_HEIGHT 32

enum BUTTON_ID{
	BUTTON_ID_PLAY = 0,
	BUTTON_ID_YOUWIN,
	BUTTON_ID_GAMEOVER,
	BUTTON_ID_EASY,
	BUTTON_ID_MEDIUM,
	BUTTON_ID_HARD,
	BUTTON_ID_CUSTOM,
	BUTTON_ID_AGAIN,
	BUTTON_ID_BACK
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

	struct{
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
	} gameover;

	struct{
		Button youwin_button;
		Button again;
		Button back;
	} youwin;

	struct{
		int w, h;
		int bombs;
	} board_options;
} menu;

static int Menu_IsInsideButton(Game *game, int *rect);

static Button Button_Create(int x, int y, int ini, int end, Texture *texture, int id);
static void Button_Reset(Button *button);
static void Button_Update(Game *game, Button *button);
static void Button_Render(Game *game, Button *button);
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
	button.y = y;
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

	menu.mainmenu.play = Button_Create(
			BUTTON_OFFSET_X,
			0,
			300,
			150,
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

	menu.gameover.back = menu.youwin.back;
	menu.gameover.again = menu.youwin.again;

	menu.difficulty.easy = Button_Create(
			BUTTON_OFFSET_X,
			0,
			300,
			120,
			game->button_texture,
			BUTTON_ID_EASY
			);

	menu.difficulty.medium = Button_Create(
			BUTTON_OFFSET_X,
			0,
			300,
			160,
			game->button_texture,
			BUTTON_ID_MEDIUM
			);

	menu.difficulty.hard = Button_Create(
			BUTTON_OFFSET_X,
			0,
			300,
			200,
			game->button_texture,
			BUTTON_ID_HARD
			);
}

void Menu_Update(Game *game){
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

	switch(menu.state){
		case MENU_MAIN:
			Button_Update(game, &menu.mainmenu.play);

			if(Button_Pressed(game, &menu.mainmenu.play)){
				Button_Reset(&menu.difficulty.easy);
				Button_Reset(&menu.difficulty.medium);
				Button_Reset(&menu.difficulty.hard);
				menu.state = MENU_DIFFICULTY;
			}

			break;

		case MENU_DIFFICULTY:
			Button_Update(game, &menu.difficulty.easy);
			Button_Update(game, &menu.difficulty.medium);
			Button_Update(game, &menu.difficulty.hard);

			if(Button_Pressed(game, &menu.difficulty.easy)){
				menu.state = MENU_PLAY;
				menu.board = Board_Create(9, 9, 10);

				menu.board_options.w = menu.board_options.h = 9;
				menu.board_options.bombs = 10;
			}

			if(Button_Pressed(game, &menu.difficulty.medium)){
				menu.state = MENU_PLAY;
				menu.board = Board_Create(16, 16, 40);

				menu.board_options.w = menu.board_options.h = 16;
				menu.board_options.bombs = 40;
			}

			if(Button_Pressed(game, &menu.difficulty.hard)){
				menu.state = MENU_PLAY;
				menu.board = Board_Create(24, 20, 99);

				menu.board_options.w = 24;
				menu.board_options.h = 20;
				menu.board_options.bombs = 99;
			}

			break;

		case MENU_PLAY:
			Board_Update(game, menu.board);

			if(Board_HasWon(menu.board)){
				menu.state = MENU_WIN;
				Button_Reset(&menu.youwin.youwin_button);
				Button_Reset(&menu.youwin.again);
				Button_Reset(&menu.youwin.back);
			}

			if(Board_HasLost(menu.board)){
				menu.state = MENU_GAMEOVER;
				Button_Reset(&menu.gameover.gameover_button);
				Button_Reset(&menu.gameover.again);
				Button_Reset(&menu.gameover.back);
			}

			break;

		case MENU_GAMEOVER:
			Button_Update(game, &menu.gameover.gameover_button);
			Button_Update(game, &menu.gameover.back);
			Button_Update(game, &menu.gameover.again);

			if(Button_Pressed(game, &menu.gameover.back)){
				menu.state = MENU_MAIN;
				Board_Destroy(menu.board);
				menu.board = NULL;
				Button_Reset(&menu.mainmenu.play);
			}

			if(Button_Pressed(game, &menu.gameover.again)){
				menu.state = MENU_PLAY;
				Board_Destroy(menu.board);

				menu.board = Board_Create(
						menu.board_options.w,
						menu.board_options.h,
						menu.board_options.bombs
						);

				Button_Reset(&menu.mainmenu.play);
			}
			break;

		case MENU_WIN:
			Button_Update(game, &menu.youwin.youwin_button);
			Button_Update(game, &menu.youwin.back);
			Button_Update(game, &menu.youwin.again);

			if(Button_Pressed(game, &menu.youwin.back)){
				menu.state = MENU_MAIN;
				Board_Destroy(menu.board);
				menu.board = NULL;
				Button_Reset(&menu.mainmenu.play);
			}

			if(Button_Pressed(game, &menu.youwin.again)){
				menu.state = MENU_PLAY;
				Board_Destroy(menu.board);

				menu.board = Board_Create(
						menu.board_options.w,
						menu.board_options.h,
						menu.board_options.bombs
						);

				Button_Reset(&menu.mainmenu.play);
			}

			break;
	}
}

void Menu_Render(Game *game){
	switch(menu.state){
		case MENU_MAIN:
			Button_Render(game, &menu.mainmenu.play);
			break;

		case MENU_DIFFICULTY:
			Button_Render(game, &menu.difficulty.easy);
			Button_Render(game, &menu.difficulty.medium);
			Button_Render(game, &menu.difficulty.hard);
			break;

		case MENU_PLAY:
			Board_Render(game, menu.board);
			break;

		case MENU_GAMEOVER:
			Board_Render(game, menu.board);
			Button_Render(game, &menu.gameover.gameover_button);
			Button_Render(game, &menu.gameover.back);
			Button_Render(game, &menu.gameover.again);
			break;

		case MENU_WIN:
			Board_Render(game, menu.board);
			Button_Render(game, &menu.youwin.youwin_button);
			Button_Render(game, &menu.youwin.back);
			Button_Render(game, &menu.youwin.again);
			break;
	}
}

void Menu_Quit(void){
	if(menu.board != NULL) Board_Destroy(menu.board);
}
