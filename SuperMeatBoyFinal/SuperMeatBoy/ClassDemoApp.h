#ifndef CLASSDEMOAPP_H
#define CLASSDEMOAPP_H

#define LEVEL_HEIGHT 16
#define LEVEL_WIDTH 22

#include <vector>
#include <string>
#include <time.h>
#include "Entity.h"
#include "Text.h"
#include "ParticleStuff.h"

class ClassDemoApp {
public:

	ClassDemoApp();
	~ClassDemoApp();

	void BuildLevelData(int level);
	bool ReadHeader(ifstream &stream);
	bool ReadLayerData(ifstream &stream);
	bool ReadEntityData(ifstream &stream);
	void RenderMap();
	void PlaceEntity(string type, float placeX, float placeY);
	
	void InitOpenGL();
	void InitRest();
	bool UpdateAndRender();

	void ShootMissile(Entity* e);

	bool IsCollisionWithMapX(Entity* e);
	bool IsCollisionWithMapY(Entity* e);
	void FixedUpdate();

	void Render();
	void Update(float elapsed);

	void Reset();

private:
	Entity* player;
	vector<Entity*> spikes;
	vector<Entity*> platforms;
	vector<Entity*> turrets;
	vector<Entity*> missiles;

	ParticleEmitter particles;

	float screenShakeValue;
	float screenShakeSpeed;
	float screenShakeIntensity;

	float textAnimationValue;

	float animationAValue;
	float animationBValue;
	float animationCValue;

	int score;
	int bestScoreLevel0;
	int bestScoreLevel1;
	int bestScoreLevel2;
	int bestScoreLevel3;
	Text text;
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

	Mix_Music* maintheme;

	Mix_Chunk* shoot;
	Mix_Chunk* explode;
};

#endif