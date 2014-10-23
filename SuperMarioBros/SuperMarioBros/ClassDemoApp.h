#ifndef CLASSDEMOAPP_H
#define CLASSDEMOAPP_H

#define LEVEL_HEIGHT 16
#define LEVEL_WIDTH 22

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

	void BuildLevelData();
	bool ReadHeader(ifstream &stream);
	bool ReadLayerData(ifstream &stream);
	bool ReadEntityData(ifstream &stream);
	void ParseLevelData(GLuint textureMap);
	void PlaceEntity(string type, float placeX, float placeY);
	void Init();
	bool UpdateAndRender();

	void FixedUpdate();

	void Render();
	void Update(float elapsed);

private:
	Entity player;
	vector<Entity> entities;

	int score;
	Text writeScore;
	int itemLoc;
	int clock;

	int mapHeight;
	int mapWidth;

	int gameState;
	bool done;
	float lastFrameTicks;
	SDL_Window* displayWindow;
};

#endif