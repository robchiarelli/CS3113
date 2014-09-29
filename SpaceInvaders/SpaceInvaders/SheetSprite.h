#ifndef SHEETSPRITE_H
#define SHEETSPRITE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

class SheetSprite {
public:
	SheetSprite() {
		timeAlive = 0.0;
	}
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float scale, bool isVisible) { }

	void setAttributes(unsigned int t, float u_, float v_, float w, float h, float s, bool vis, float x_, float y_);
	void Draw();
	void setPosition(float xTemp, float yTemp);

	float scale;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;

	float x;
	float y;
	float direction;

	float timeAlive;
	bool isVisible;
};

void SheetSprite::setAttributes(unsigned int t, float u_, float v_, float w, float h, float s, bool vis, float x_, float y_) {
	textureID = t;
	u = u_;
	v = v_;
	width = w;
	height = h;
	scale = s;
	isVisible = vis;
	x = x_;
	y = y_;
}

void SheetSprite::Draw() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTranslatef(x, y, 0.0);

	GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale, width * scale, -height * scale, width * scale, height * scale };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glTranslatef(-x, -y, 0.0);
	glDisable(GL_TEXTURE_2D);
}

void SheetSprite::setPosition(float xTemp, float yTemp) {
	x = xTemp;
	y = yTemp;
}

#endif