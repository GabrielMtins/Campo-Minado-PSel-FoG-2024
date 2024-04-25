#include "menu.h"
#include "board.h"
#include "draw.h"

#define BUTTON_OFFSET_X 160
#define BUTTON_WIDTH 160
#define BUTTON_HEIGHT 32

enum BUTTON_ID{
	BUTTON_ID_PLAY = 0,
	BUTTON_ID_YOUWIN,
	BUTTON_ID_GAMEOVER,
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
		Button gameover_button;
	} gameover;

	struct{
		Button youwin_button;
	} youwin;
} menu;

static double Menu_Smooth(double i);
static int Menu_IsInsideButton(Game *game, int *rect);
static int Menu_PressedButton(Game *game, int *rect);

static Button Button_Create(int x, int y, int ini, int end, Texture *texture, int id);
static void Button_Reset(Button *button);
static void Button_Update(Game *game, Button *button);
static void Button_Render(Game *game, Button *button);
static int Button_Pressed(Game *game, Button *button);

static const int color_button_overlay[4] = {255, 255, 255, 50};
static const int color_button_click[4] = {255, 255, 255, 100};

static double Menu_Smooth(double i){
	if(i > 1) return 1;
	if(i < 0) return 0;
	return ((6*i - 15)*i + 10)*i*i*i;
}

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

static int Menu_PressedButton(Game *game, int *rect){
	if(!Menu_IsInsideButton(game, rect)) return 0;

	if(menu.mouse_down && menu.mouse_up == 1) return 1;

	return 0;
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
			300,
			150,
			game->button_texture,
			BUTTON_ID_GAMEOVER
			);

	menu.youwin.youwin_button = Button_Create(
			BUTTON_OFFSET_X,
			0,
			300,
			150,
			game->button_texture,
			BUTTON_ID_YOUWIN
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
				menu.state = MENU_PLAY;
				menu.board = Board_Create(9, 9, 10);
			}

			break;

		case MENU_PLAY:
			Board_Update(game, menu.board);

			if(Board_HasWon(menu.board)){
				menu.state = MENU_WIN;
				Button_Reset(&menu.youwin.youwin_button);
			}

			if(Board_HasLost(menu.board)){
				menu.state = MENU_GAMEOVER;
				Button_Reset(&menu.gameover.gameover_button);
			}

			break;

		case MENU_GAMEOVER:
			Button_Update(game, &menu.gameover.gameover_button);

			if(Button_Pressed(game, &menu.gameover.gameover_button)){
				menu.state = MENU_MAIN;
				Board_Destroy(menu.board);
				menu.board = NULL;
				Button_Reset(&menu.mainmenu.play);
			}

			break;

		case MENU_WIN:
			Button_Update(game, &menu.youwin.youwin_button);

			if(Button_Pressed(game, &menu.youwin.youwin_button)){
				menu.state = MENU_MAIN;
				Board_Destroy(menu.board);
				menu.board = NULL;
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

		case MENU_PLAY:
			Board_Render(game, menu.board);
			break;

		case MENU_GAMEOVER:
			Board_Render(game, menu.board);
			Button_Render(game, &menu.gameover.gameover_button);
			break;

		case MENU_WIN:
			Board_Render(game, menu.board);
			Button_Render(game, &menu.youwin.youwin_button);
			break;
	}
}

void Menu_Quit(void){
	if(menu.board != NULL) Board_Destroy(menu.board);
}
