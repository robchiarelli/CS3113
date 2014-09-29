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

	void shootLaser();
	void shootEnemyLaser();

	void Render();
	void Update(float elapsed);

private:
	SheetSprite ship;
	SheetSprite enemy1;
	std::vector<SheetSprite> lasers;
	std::vector<SheetSprite> enemyLasers;
	std::vector<SheetSprite> enemies;

	int numLasers;
	int numEnemyLasers;
	int lastShot;
	int score;
	Text writeScore;

	int gameState;
	bool done;
	float lastFrameTicks;
	SDL_Window* displayWindow;
};

ClassDemoApp::ClassDemoApp() {
	Init();
	score = 0;
	gameState = 0;
	done = false;
	lastShot = 5;
	numLasers = 9;
	numEnemyLasers = 23;
	lastFrameTicks = 0.0f;
}

GLuint ClassDemoApp::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);
	return textureID;
}

void ClassDemoApp::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Spaceman Spiff", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	GLuint spriteSheetTexture = LoadTexture("sheet.png");

	ship.setAttributes(spriteSheetTexture, 346.0f / 1024.0f, 75.0f / 1024.0f, 98.0f / 1024.0f, 75.0f / 1024.0f, 0.7f, true, 0.0f, -0.85f);

	float xOrigin = -1.2f;
	float yOrigin = 0.7f;
	int count = 0;
	for (int i = 0; i < 24; i++) {
		SheetSprite enemy;
		enemy.setAttributes(spriteSheetTexture, 425.0f / 1024.0f, 384.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.7f, true, xOrigin, yOrigin);
		enemy.direction = 1;
		xOrigin += 0.25f;
		count++;
		if (count % 6 == 0) {
			xOrigin = -1.2;
			yOrigin -= 0.2;
		}
		enemies.push_back(enemy);
	}

	for (int i = 0; i < 10; i++) {
		SheetSprite laser;
		laser.setAttributes(spriteSheetTexture, 858.0f / 1024.0f, 230.0f / 1024.0f, 9.0f / 1024.0f, 54.0f / 1024.0f, 1.0f, false, -2.0f, -2.0f);
		lasers.push_back(laser);
	}

	for (int i = 0; i < 24; i++) {
		SheetSprite enemyLaser;
		enemyLaser.setAttributes(spriteSheetTexture, 809.0f / 1024.0f, 437.0f / 1024.0f, 19.0f / 1024.0f, 30.0f / 1024.0f, 1.0f, false, -2.0f, -2.0f);
		enemyLasers.push_back(enemyLaser);
	}

	writeScore.font = LoadTexture("font1.png");
	writeScore.size = 0.1;
	writeScore.spacing = -0.055;
	writeScore.r = 0.0;
	writeScore.g = 1.0;
	writeScore.b = 0.0;
	writeScore.a = 1.0;
}

ClassDemoApp::~ClassDemoApp() {
	SDL_Quit();
}

void ClassDemoApp::shootLaser() {
	if (numLasers >= 9) {
		numLasers = 0;
	}
	lasers[numLasers].isVisible = true;
	lasers[numLasers].x = ship.x;
	lasers[numLasers].y = ship.y + 0.1f;
	numLasers++;
}

void ClassDemoApp::shootEnemyLaser() {
	numEnemyLasers = rand() % 24;
	if (enemies[numEnemyLasers].isVisible) {
		enemyLasers[numEnemyLasers].isVisible = true;
		enemyLasers[numEnemyLasers].x = enemies[numEnemyLasers].x;
		enemyLasers[numEnemyLasers].y = enemies[numEnemyLasers].y - 0.1f;
	}
	srand(time(NULL));
}

void ClassDemoApp::Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	if (gameState == 0) {
		writeScore.Draw("Attention, Spaceman Spiff.", -1.0, 0.7);
		writeScore.Draw("There is an alien force approaching your location.", -1.0, 0.5);
		writeScore.Draw("You must defeat them all to survive", -1.0, 0.3);
		writeScore.Draw("Good luck.", -1.0, 0.1);
		writeScore.Draw("Press Enter to begin.", -1.0, -0.7);
	}

	if (gameState == 1) {
		
		ship.Draw();

		for (SheetSprite e : enemies) {
			if (e.isVisible == true) {
				e.Draw();
			}
		}

		for (SheetSprite l : lasers) {
			if (l.isVisible == true) {
				l.Draw();
			}
		}

		for (SheetSprite el : enemyLasers) {
			if (el.isVisible == true) {
				el.Draw();
			}
		}
		
		std::string scoreString = to_string(score);
		writeScore.Draw(scoreString, -1.2, 0.9);

	}

	if (gameState == 2) {
		writeScore.Draw("Congratulations, Spiff.", -1.0f, 0.3f);
		writeScore.Draw("Now that the aliens are dead,", -1.0f, 0.1f);
		writeScore.Draw("the space station is safe.", -1.0f, -0.1f);
	}

	if (gameState == 3) {
		writeScore.Draw("You have failed.", -1.0f, 0.0f);
	}

	SDL_GL_SwapWindow(displayWindow);
}

bool isCollision(SheetSprite laser, SheetSprite vessel) {
	if (laser.isVisible == true && vessel.isVisible == true) {
		if (((vessel.y - laser.y) < (vessel.height / 1.75 + laser.height / 1.75) && (vessel.y - vessel.height / 1.75) < (laser.y + laser.height / 1.75)
			&& (vessel.y + vessel.height / 1.75) > (laser.y - laser.height / 1.75) && (vessel.x - laser.x) < (vessel.width / 1.75 + laser.width / 1.75)
			&& (vessel.x - vessel.height / 1.75) < (laser.x + laser.width / 1.75)
			&& (vessel.x + vessel.height / 1.75) > (laser.x - laser.width / 1.75))) {
				return true;
			}
		return false;
	}
}

void ClassDemoApp::Update(float elapsed) {

	SDL_Event event;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}

		if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
			if (lastShot >= 10) {
				shootLaser();
				lastShot = 0;
			}
		}
	}
	lastShot++;

	if (gameState == 0) {
		if (keys[SDL_SCANCODE_0]) {
			gameState = 1;
		}
	}

	if (gameState == 1) {
		if (keys[SDL_SCANCODE_LEFT]) {
			ship.x -= elapsed * 1.5;
		}
		if (keys[SDL_SCANCODE_RIGHT]) {
			ship.x += elapsed * 1.5;
		}

		for (SheetSprite &l : lasers) {
			if (l.isVisible == true) {
				l.y += elapsed;
				if (l.y >= 1.0f) {
					l.isVisible = false;
				}
			}
		}

		for (SheetSprite &el : enemyLasers) {
			el.y -= elapsed;
			if (el.y <= -1.0f) {
				el.isVisible = false;
			}
			
		}

		bool hasEnemies = false;
		for (int i = 0; i < enemies.size(); i++) {
			if (enemies[i].isVisible) {
				hasEnemies = true;
				if (rand() % 2 == 1) {
					shootEnemyLaser();
				}
			}
			srand(time(NULL));

			enemies[i].x += elapsed * 0.5 * enemies[i].direction;
			if (enemies[i].x > 1.2 && enemies[i].isVisible || enemies[i].x < -1.2 && enemies[i].isVisible) {
				for (int j = 0; j < enemies.size(); j++) {
					enemies[j].x += .01 * -1 * enemies[j].direction;
					enemies[j].direction *= -1;
					enemies[j].y -= 0.075f;
				}
			}
			for (int k = 0; k < lasers.size(); k++) {
				if (isCollision(lasers[k], enemies[i])) {
					enemies[i].isVisible = false;
					lasers[k].isVisible = false;
					score += 100;
				}
			}
			if (isCollision(enemies[i], ship)) {
				gameState = 3;
			}
		}
		for (int i = 0; i < enemyLasers.size(); i++) {
			if (isCollision(ship, enemyLasers[i])){
				gameState = 3;
			}
		}
		if (!hasEnemies) {
			gameState = 2;
		}
	}
}

bool ClassDemoApp::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	Update(elapsed);
	Render();
	return done;
}

#endif
