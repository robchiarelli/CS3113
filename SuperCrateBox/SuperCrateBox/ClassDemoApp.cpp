#include "ClassDemoApp.h"
#include "SheetSprite.h"
#include "Entity.h"
#include <time.h>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
float timeLeftOver = 0.0f;

ClassDemoApp::ClassDemoApp() {
	Init();
	score = 0;
	clock = 0;
	gameState = 0;
	done = false;
	lastFrameTicks = 0.0f;
}

GLuint ClassDemoApp::LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

	GLuint spriteSheetTexture = LoadTexture("green_panel.png");
	GLuint itemTexture = LoadTexture("flat_medal1.png");

	player.x = 0.0;
	player.y = 1.0;
	player.sprite.textureID = spriteSheetTexture;
	player.sprite.width = 0.125;
	player.sprite.height = 0.125;

	midPlatform.x = 0.0;
	midPlatform.y = -0.8;
	midPlatform.sprite.textureID = spriteSheetTexture;
	midPlatform.sprite.width = 1.0;
	midPlatform.sprite.height = 0.125f;
	midPlatform.isStatic = true;
	entities.push_back(midPlatform);
	
	leftPlatform.x = -1.0;
	leftPlatform.y = -0.3;
	leftPlatform.isStatic = true;
	leftPlatform.sprite.textureID = spriteSheetTexture;
	leftPlatform.sprite.width = 1.0;
	leftPlatform.sprite.height = 0.125f;
	entities.push_back(leftPlatform);

	rightPlatform.x = 1.0;
	rightPlatform.y = -0.3;
	rightPlatform.isStatic = true;
	rightPlatform.sprite.textureID = spriteSheetTexture;
	rightPlatform.sprite.width = 1.0;
	rightPlatform.sprite.height = 0.125f;
	entities.push_back(rightPlatform);

	topPlatform.x = 0.0;
	topPlatform.y = 0.2;
	topPlatform.isStatic = true;
	topPlatform.sprite.textureID = spriteSheetTexture;
	topPlatform.sprite.width = 1.0;
	topPlatform.sprite.height = 0.125f;
	entities.push_back(topPlatform);

	itemLoc = 2;

	item.sprite.textureID = itemTexture;
	item.sprite.width = 0.125f;
	item.sprite.height = 0.125f;

	writeScore.font = LoadTexture("font1.png");
	writeScore.size = 0.1;
	writeScore.spacing = -0.055;
	writeScore.r = 1.0;
	writeScore.g = 0.0;
	writeScore.b = 0.0;
	writeScore.a = 1.0;
}

ClassDemoApp::~ClassDemoApp() {
	SDL_Quit();
}

void ClassDemoApp::Render() {
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	if (gameState == 0) {
		writeScore.size = 0.1;
		writeScore.Draw("Collect as many medals as you can in 30 seconds.", -1.0, 0.7);
		writeScore.Draw("Use the arrow keys to move.", -1.0, 0.5);
		writeScore.Draw("", -1.0, 0.3);
		writeScore.Draw("", -1.0, 0.1);
		writeScore.Draw("Press Enter to begin.", -1.0, -0.7);
	}

	if (gameState == 1) {
		for (Entity e : entities)
			e.Render();
		item.Render();
		player.Render();
		string scoreString = "Score: " + to_string(score);
		string clockString = "Clock: " + to_string(clock);
		writeScore.size = 0.1;
		writeScore.Draw(scoreString, 0.8, 0.8);
		writeScore.size = 0.1;
		writeScore.Draw(clockString, -1.0, 0.8);
	}

	if (gameState == 2) {
		string scoreString = "Your final score is: " + to_string(score);
		writeScore.size = 0.1;
		writeScore.Draw(scoreString, 0.0, 0.5);
	}

	SDL_GL_SwapWindow(displayWindow);
}

void ClassDemoApp::FixedUpdate() {
	srand(time(NULL));
	if (gameState == 1) {
		if (!player.FixedUpdate(midPlatform)) {
			if (!player.FixedUpdate(leftPlatform)) {
				if (!player.FixedUpdate(rightPlatform)) {
					player.FixedUpdate(topPlatform);
				}
			}
		}
		srand(time(NULL));
		if ((item.x > player.leftEdge) && (item.x < player.rightEdge)
			&& (item.y < player.topEdge) && (item.y > player.bottomEdge)) {
			itemLoc = (rand() % 4) + 1;
			score += 1;
		}
	}
}

void ClassDemoApp::Update(float elapsed) {

	SDL_Event event;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}

	if (gameState == 0) {
		if (keys[SDL_SCANCODE_RETURN]) {
			gameState = 1;
		}
	}
	if (gameState == 1) {
		player.Update(elapsed, keys);
		rightPlatform.Update(elapsed, keys);
		midPlatform.Update(elapsed, keys);
		leftPlatform.Update(elapsed, keys);
		topPlatform.Update(elapsed, keys);

		if (itemLoc == 1) {
			item.x = 0.0;
			item.y = 0.7;
		}
		if (itemLoc == 2) {
			item.x = 0.8;
			item.y = 0.0;
		}
		if (itemLoc == 3) {
			item.x = 0.0;
			item.y = -0.5;
		}
		if (itemLoc == 4) {
			item.x = -0.8;
			item.y = 0.0;
		}
		if (clock > 30)
			gameState = 2;
	}
}

int clockCount = 0;

bool ClassDemoApp::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}

	while (fixedElapsed >= FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		FixedUpdate();
		if (clockCount >= 60) {
			clock++;
			clockCount = 0;
		}
		clockCount++;
	}
	timeLeftOver = fixedElapsed;

	Update(elapsed);
	Render();
	return done;
}

int main(int argc, char *argv[]) {
	ClassDemoApp app;
	while (!app.UpdateAndRender()) {}
	return 0;
}