#ifndef SHEETSPRITE_H
#define SHEETSPRITE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

class SheetSprite {
public:
	SheetSprite() {}

	void DrawOther(float x, float y);
	void Draw(float x, float y);
	void DrawPlayer(float x, float y, int spriteCountX, int spriteCountY, int index);

	float scale;
	unsigned int textureID;
	//float u;
	//float v;
	float height;
	float width;
	GLfloat quad[8];
	GLfloat quadUVs[8];
};

#endif