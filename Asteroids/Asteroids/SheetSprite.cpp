#include "SheetSprite.h"
#include <math.h>

SheetSprite::SheetSprite(GLuint texture, float h, float w, float u, float v, float scale) {
	textureID = texture;
	height = h;
	width = w;
	this->u = u;
	this->v = v;
	this->scale = scale;
}

void SheetSprite::Draw(float x, float y){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(x, y, 0.0);

	GLfloat quad[] = { width * -0.5, height * 0.5, width * -0.5, height * -0.5, width * 0.5, height * -0.5, width * 0.5, height * 0.5 };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glTranslatef(-x, -y, 0.0);
	glDisable(GL_TEXTURE_2D);
}

void SheetSprite::DrawSheet(float x, float y, float r) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTranslatef(x, y, 0.0);
	glRotatef((r - 89.5) * 180 / M_PI, 0.0f, 0.0f, 1.0f);

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