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
	MENU_WIN,
	MENU_GAMEOVER,
};

struct{
	int state;
	Board *board;

	int mouse_down;
	int mouse_up;

	struct{
		double timer_play;
		int play_y;
	} mainmenu;

	struct{
		double timer_button;
		int button_y;
	} gameover;

	struct{
		double timer_button;
		int button_y;
	} youwin;
} menu;

static double Menu_Smooth(double i);
static int Menu_IsInsideButton(Game *game, int *rect);
static int Menu_PressedButton(Game *game, int *rect);

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

void Menu_Init(void){
	menu.state = MENU_MAIN;
	menu.board = NULL;
	menu.mainmenu.timer_play = 0;
}

void Menu_Update(Game *game){
	int rect[4];

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
			menu.mainmenu.timer_play += game->context->delta_time;
			menu.mainmenu.play_y = 300 - Menu_Smooth(1 * menu.mainmenu.timer_play) * 150;

			rect[0] = BUTTON_OFFSET_X;
			rect[1] = menu.mainmenu.play_y;
			rect[2] = BUTTON_WIDTH;
			rect[3] = BUTTON_HEIGHT;

			if(Menu_PressedButton(game, rect)){
				menu.state = MENU_PLAY;
				menu.board = Board_Create(9, 9, 10);
			}

			break;

		case MENU_PLAY:
			Board_Update(game, menu.board);

			if(Board_HasWon(menu.board)){
				menu.state = MENU_WIN;
			}

			if(Board_HasLost(menu.board)){
				menu.state = MENU_GAMEOVER;
				menu.gameover.timer_button = 0;
			}

			break;

		case MENU_GAMEOVER:
			menu.gameover.timer_button += game->context->delta_time;
			menu.gameover.button_y = 300 - Menu_Smooth(1 * menu.gameover.timer_button) * 150;

			rect[0] = BUTTON_OFFSET_X;
			rect[1] = menu.gameover.button_y;
			rect[2] = BUTTON_WIDTH;
			rect[3] = BUTTON_HEIGHT;

			if(Menu_PressedButton(game, rect)){
				menu.state = MENU_MAIN;
				Board_Destroy(menu.board);
				menu.board = NULL;
				menu.mainmenu.timer_play = 0;
			}

			break;

		case MENU_WIN:
			menu.youwin.timer_button += game->context->delta_time;
			menu.youwin.button_y = 300 - Menu_Smooth(1 * menu.youwin.timer_button) * 150;

			rect[0] = BUTTON_OFFSET_X;
			rect[1] = menu.youwin.button_y;
			rect[2] = BUTTON_WIDTH;
			rect[3] = BUTTON_HEIGHT;

			if(Menu_PressedButton(game, rect)){
				menu.state = MENU_MAIN;
				Board_Destroy(menu.board);
				menu.board = NULL;
				menu.mainmenu.timer_play = 0;
			}

			break;
	}
}

void Menu_Render(Game *game){
	int rect[4];

	switch(menu.state){
		case MENU_MAIN:
			Texture_RenderCell(
					game->context,
					game->button_texture,
					BUTTON_OFFSET_X,
					menu.mainmenu.play_y,
					BUTTON_ID_PLAY
					);

			rect[0] = BUTTON_OFFSET_X;
			rect[1] = menu.mainmenu.play_y;
			rect[2] = BUTTON_WIDTH;
			rect[3] = BUTTON_HEIGHT;

			if(Menu_IsInsideButton(game, rect)){
				if(menu.mouse_down)
					Draw_DrawRect(game->context, rect, color_button_click);
				else
					Draw_DrawRect(game->context, rect, color_button_overlay);
			}

			break;

		case MENU_PLAY:
			Board_Render(game, menu.board);
			break;

		case MENU_GAMEOVER:
			Board_Render(game, menu.board);

			Texture_RenderCell(
					game->context,
					game->button_texture,
					BUTTON_OFFSET_X,
					menu.gameover.button_y,
					BUTTON_ID_GAMEOVER
					);

			rect[0] = BUTTON_OFFSET_X;
			rect[1] = menu.gameover.button_y;
			rect[2] = BUTTON_WIDTH;
			rect[3] = BUTTON_HEIGHT;

			if(Menu_IsInsideButton(game, rect)){
				if(menu.mouse_down)
					Draw_DrawRect(game->context, rect, color_button_click);
				else
					Draw_DrawRect(game->context, rect, color_button_overlay);
			}

			break;

		case MENU_WIN:
			Board_Render(game, menu.board);

			Texture_RenderCell(
					game->context,
					game->button_texture,
					BUTTON_OFFSET_X,
					menu.youwin.button_y,
					BUTTON_ID_YOUWIN
					);

			rect[0] = BUTTON_OFFSET_X;
			rect[1] = menu.youwin.button_y;
			rect[2] = BUTTON_WIDTH;
			rect[3] = BUTTON_HEIGHT;

			if(Menu_IsInsideButton(game, rect)){
				if(menu.mouse_down)
					Draw_DrawRect(game->context, rect, color_button_click);
				else
					Draw_DrawRect(game->context, rect, color_button_overlay);
			}

			break;
	}
}

void Menu_Quit(void){
	if(menu.board != NULL) Board_Destroy(menu.board);
}
