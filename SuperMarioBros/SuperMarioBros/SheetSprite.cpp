#include "SheetSprite.h"

void SheetSprite::DrawOther(float x, float y){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);

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

void SheetSprite::Draw(float x, float y){
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x, y, 0.0);

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);

	glTranslatef(-x, -y, 0.0);

	glDisable(GL_TEXTURE_2D);
}

void SheetSprite::DrawPlayer(float x, float y, int spriteCountX, int spriteCountY, int index) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);

	glTranslatef(x, y, 0.0);

	GLfloat quad[] = { width * -0.5, height * 0.5, width * -0.5, height * -0.5, width * 0.5, height * -0.5, width * 0.5, height * 0.5 };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;
	
	GLfloat quadUVs[] = { u, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight,
		u + spriteWidth, v
	};
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);

	glTranslatef(-x, -y, 0.0);

	glDisable(GL_TEXTURE_2D);
}