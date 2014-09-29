#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

class Entity {
public:
	void Draw() {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glTranslatef(x, y, 0.0);
		glRotatef(rotation, 0.0, 0.0, 1.0);

		GLfloat quad[] = { width * -0.5, height * 0.5, width * -0.5, height * -0.5, width * 0.5, height * -0.5, width * 0.5, height * 0.5 };
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);

		GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
	}

	void DrawSpriteSheetSprite(int spriteTexture, int index, int spriteCountX, int spriteCountY) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glTranslatef(x, y, 0.0);
		glRotatef(rotation, 0.0, 0.0, 1.0);

		GLfloat quad[] = { width * -0.5, height * 0.5, width * -0.5, height * -0.5, width * 0.5, height * -0.5, width * 0.5, height * 0.5 };
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);

		float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
		float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
		float spriteWidth = 1.0 / (float)spriteCountX;
		float spriteHeight = 1.0 / (float)spriteCountY;

		GLfloat quadUVs[] = { u, v, u, v + spriteHeight, u + spriteWidth, v + spriteHeight, u + spriteWidth, v };
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
	
	}

	void setSize(float w, float h) {
		width = w;
		height = h;
	}

	void setPosition(float xTemp, float yTemp, float r) {
		x = xTemp;
		y = yTemp;
		rotation = r;
	}

	void setMovement(float s, float a) {
		speed = s;
		angle = a;
	}

	float x;
	float y;
	float rotation;

	int textureID;

	float width;
	float height;

	float speed;
	float angle;
};

#endif