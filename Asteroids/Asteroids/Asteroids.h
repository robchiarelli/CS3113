#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <time.h>
#include "SheetSprite.h"
#include "Entity.h"
#include "Text.h"
#include <vector>
#include "ParticleStuff.h"

class Asteroids {
public:

	Asteroids();
	~Asteroids();

	GLuint LoadTexture(const char *image_path);
	void InitOpenGL();
	void InitRest();
	bool UpdateAndRender();

	void FixedUpdate();

	void ShootBullet();

	void Render();
	void Update(float elapsed);

	void ResetGame();

private:
	Entity player;
	Text text;

	std::vector<Entity> bullets;
	std::vector<Entity> rocks;

	ParticleEmitter exhaust;

	float screenShakeValue;
	float screenShakeSpeed;
	float screenShakeIntensity;

	float textAnimationValue;

	float animationAValue;
	float animationBValue;
	float animationCValue;

	int score;

	int clock;
	int gameStateClock;
	int gameState;
	bool done;
	bool hit;
	float lastFrameTicks;
	SDL_Window* displayWindow;

	Mix_Music* maintheme;

	Mix_Chunk* shoot;
	Mix_Chunk* explode;
};

#endif