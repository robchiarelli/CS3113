#ifndef SHEETSPRITE_H
#define SHEETSPRITE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

class SheetSprite {
public:
	SheetSprite() {}

	void Draw(float x, float y);

	float scale;
	unsigned int textureID;
	float u;
	float v;
	float height;
	float width;
};

#endif