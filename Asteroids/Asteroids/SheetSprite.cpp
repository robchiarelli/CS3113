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

void SheetSprite::DrawSheet(float x, float y) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glMatrixMode(GL_MODELVIEW);

	GLfloat quad[] = { height * scale, width * scale, -height * scale, width * scale, -height * scale, -width * scale, height * scale, -width * scale };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
}