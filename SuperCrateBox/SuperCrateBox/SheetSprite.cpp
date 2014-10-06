#include "SheetSprite.h"

/*
void SheetSprite::setAttributes(unsigned int t, float u_, float v_, float h, float w, float s) {
	textureID = t;
	u = u_;
	v = v_;
	height = h;
	width = w * s;
	scale = s;
}
*/
/*
void SheetSprite::setAttributesUniform(unsigned int t, float u_, float v_, float h, float w, float s, int i, int sCX, int sCY) {
	textureID = t;
	u = u_;
	v = v_;
	height = h;
	width = w * s;
	scale = s;
	index = i;
	spriteCountX = sCX;
	spriteCountY = sCY;
}
*/

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