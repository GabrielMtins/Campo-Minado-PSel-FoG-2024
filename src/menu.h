#ifndef MENU_H
#define MENU_H

#include "game.h"

/* Arquivo que cuida do menu do jogo */

double Menu_Smooth(double i);

void Menu_Init(Game *game);

void Menu_Update(Game *game);

void Menu_Render(Game *game);

void Menu_Quit(void);

#endif
