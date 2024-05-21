#ifndef TEXTURE_H
#define TEXTURE_H

#include "context.h"

/* Arquivo que cuida de texturas. Cada textura é composta
 * de células, assim é possível carregar apenas uma textura
 * em vez de várias */

typedef struct{
	SDL_Texture *texture;
	int cell_width, cell_height;
	int texture_width, texture_height;
} Texture;

Texture * Texture_Load(Context *context, const char *filename, int cell_width, int cell_height);

void Texture_RenderCell(Context *context, Texture *texture, int x, int y, int id);

void Texture_Destroy(Texture *texture);

#endif
