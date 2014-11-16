#ifndef SHEETSPRITE_H
#define SHEETSPRITE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

class SheetSprite {
public:
	SheetSprite() {}
	SheetSprite(GLuint texture, float h, float w, float u, float v, float scale);

	void DrawSheet(float x, float y);

	float scale;
	unsigned int textureID;
	float u;
	float v;
	float height;
	float width;
};

#endif