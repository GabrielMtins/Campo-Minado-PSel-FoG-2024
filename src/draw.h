#ifndef DRAW_H
#define DRAW_H

#include "context.h"

/*
 * Esse arquivo cuida da parte de desenhar retangulos e texto na tela.
 * rect: {x, y, w, h};
 * color: {r, g, b, a} */

/* se rect == null, a tela será preenchida com a cor dada */
void Draw_DrawRect(Context *context, const int *rect, const int *color);
void Draw_DrawText(Context *context, int x, int y, const int *color, const char *text);
void Draw_DrawTextCentered(Context *context, int x, int y, const int *color, const char *text);
void Draw_SmallBorderedText(Context *context, int x, int y, const int *bg, const int *fg, const char *text);
void Draw_DrawTextWithBox(Context *context, int x, int y, int border, const int *bg, const int *fg, const char *text);
void Draw_DrawTextWithBoxCentered(Context *context, int x, int y, int border, const int *bg, const int *fg, const char *text);

#endif
