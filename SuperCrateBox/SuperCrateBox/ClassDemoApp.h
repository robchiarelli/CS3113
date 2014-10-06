#ifndef CLASSDEMOAPP_H
#define CLASSDEMOAPP_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <vector>
#include <string>
#include <time.h>
#include "SheetSprite.h"
#include "Entity.h"
#include "Text.h"

class ClassDemoApp {
public:

	ClassDemoApp();
	~ClassDemoApp();

	GLuint LoadTexture(const char *image_path);
	void Init();
	bool UpdateAndRender();

	void FixedUpdate();

	void Render();
	void Update(float elapsed);

private:
	Entity player;
	Entity midPlatform;
	Entity leftPlatform;
	Entity rightPlatform;
	Entity topPlatform;
	Entity item;
	vector<Entity> entities;

	int score;
	Text writeScore;
	int itemLoc;
	int clock;

	int gameState;
	bool done;
	float lastFrameTicks;
	SDL_Window* displayWindow;
};

#endif