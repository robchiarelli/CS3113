#ifndef CLASSDEMOAPP_H
#define CLASSDEMOAPP_H

#define LEVEL_HEIGHT 16
#define LEVEL_WIDTH 22

#include <vector>
#include <string>
#include <time.h>
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
	void RenderMap();
	void PlaceEntity(string type, float placeX, float placeY);
	
	void InitOpenGL();
	void InitRest();
	bool UpdateAndRender();

	void IsCollisionWithMapX(Entity* e);
	void IsCollisionWithMapY(Entity* e);
	void FixedUpdate();

	void Render();
	void Update(float elapsed);

	void Reset();

private:
	Entity* player;
	vector<Entity*> spikes;
	vector<Entity> entities;

	int score;
	int bestScore;
	Text writeScore;
	int itemLoc;
	int clock;

	int mapHeight;
	int mapWidth;
	unsigned char** levelData;
	GLuint texture;

	float titleAnimationValue;

	int gameState;
	bool done;
	float lastFrameTicks;
	SDL_Window* displayWindow;
};

#endif