#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Entity.h"
#include "SheetSprite.h"
#include "ClassDemoApp.h"

SDL_Window* displayWindow;

Entity ship;

GLuint LoadTexture(const char *image_path) {
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

void Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Spaceman Spiff", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	ship.textureID = LoadTexture("characters_3.png");

	ship.setPosition(0.0, 0.0, 0.0);
	ship.setSize(0.5, 0.5);
}

void ProcessEvents(bool &done, float &elapsed, bool &gameOn) {
	SDL_Event event;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}
}

void Update() {

}

void Render(const int* runAnimationTemp, int& index) {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	ship.DrawSpriteSheetSprite(ship.textureID, runAnimationTemp[index], 8, 4);

	SDL_GL_SwapWindow(displayWindow);
}

int main(int argc, char *argv[]) {

	bool done = false;
	bool gameOn = false;

	float lastFrameTicks = 0.0f;
	float ballAngle = 0.0f;

	const int runAnimation[] = { 8, 9, 10, 11, 12, 13, 14, 15 };
	const int numFrames = 8;
	float animationElapsed = 0.0f;
	float framesPerSecond = 30.0f;
	int currentIndex = 0;

	Setup();

	while (!done) {

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		animationElapsed += elapsed;

		if (animationElapsed > 1.0 / framesPerSecond) {
			currentIndex++;
			animationElapsed = 0.0;

			if (currentIndex > numFrames - 1) {
				currentIndex = 0;
			}
		}
		
		ProcessEvents(done, elapsed, gameOn);

		Update();

		Render(runAnimation, currentIndex);
	}

	ship.DrawSpriteSheetSprite(ship.textureID, runAnimation[currentIndex], 8, 4);

	SDL_Quit();
	return 0;
}