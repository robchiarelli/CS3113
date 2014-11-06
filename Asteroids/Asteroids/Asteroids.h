#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <vector>
#include <string>
#include <time.h>
#include "SheetSprite.h"
#include "Entity.h"
#include "Text.h"
#include <vector>

class Asteroids {
public:

	Asteroids();
	~Asteroids();

	GLuint LoadTexture(const char *image_path);
	void Init();
	bool UpdateAndRender();

	void FixedUpdate();

	void ShootBullet();

	void Render();
	void Update(float elapsed);

private:
	Entity player;
	Text text;

	std::vector<Entity> bullets;
	std::vector<Entity> rocks;

	int clock;
	int gameState;
	bool done;
	float lastFrameTicks;
	SDL_Window* displayWindow;
};

#endif