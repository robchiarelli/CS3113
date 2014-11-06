#include "Asteroids.h"
#include "SheetSprite.h"
#include "Entity.h"
#include <time.h>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
float timeLeftOver = 0.0f;

Asteroids::Asteroids() {
	Init();
	gameState = 0;
	done = false;
	lastFrameTicks = 0.0f;
}

GLuint Asteroids::LoadTexture(const char *image_path) {
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

int randomInt(int i) {
	return rand() % i;
	srand(time(NULL));
}

void Asteroids::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Asteroids with MATH!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	GLuint texture = LoadTexture("sheet.png");
	GLuint fontTexture = LoadTexture("font1.png");

	SheetSprite s(texture, 75.0f / 1024.0f, 99.0f / 1024.0f, 237.0f / 1024.0f, 377.0f / 1024.0f, 1.0f);
	player.sprite = s;
	player.type = "player";

	SheetSprite bS(texture, 54.0f / 1024.0f, 13.0f / 1024.0f, 843.0f / 1024.0f, 62.0f / 1024.0f, 1.0f);
	for (int i = 0; i < 10; i++) {
		Entity b;
		b.x = 2.0f;
		b.y = 2.0f;
		b.sprite = bS;
		b.type = "bullet";
		b.isVisible = false;
		bullets.push_back(b);
	}

	SheetSprite rS(texture, 84.0f / 1024.0f, 101.0f / 1024.0f, 224.0f / 1024.0f, 664.0f / 1024.0f, 2.5f);
	for (int i = 0; i < 20; i++) {
		Entity rock;
		int loc = randomInt(4);
		if (loc == 0 || loc == 1)
			rock.startY = 1.2f;
		else
			rock.startY = -1.2f;
		if (loc == 1 || loc == 2)
			rock.startX = 1.5f;
		else
			rock.startX = -1.5f;

		rock.x = rock.startX;
		rock.y = rock.startY;

		int angle = randomInt(60);
		if (angle < 30) angle += 30;
		if (loc == 0) rock.r = (angle - 90) * M_PI / 180;
		if (loc == 1) rock.r = (angle - 180) * M_PI / 180;
		if (loc == 2) rock.r = (angle + 90) * M_PI / 180;
		if (loc == 3) rock.r = (angle)* M_PI / 180;
		rock.sprite = rS;
		rock.type = "rock";
		rock.isVisible = false;
		rocks.push_back(rock);
	}

	text.font = fontTexture;
	text.spacing = -0.055;
	text.r = 0.0;
	text.g = 1.0;
	text.b = 0.0;
	text.a = 1.0;
}

Asteroids::~Asteroids() {
	SDL_Quit();
}

void Asteroids::Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (gameState == 0) {
		text.size = 0.1;
		text.Draw("Arrow keys to move, space to shoot.", -1.0, 0.0);
	}

	if (gameState == 1) {
		for (Entity r : rocks) {
			if (r.isVisible)
				r.Render();
		}
		for (Entity b : bullets) {
			if (b.isVisible)
				b.Render();
		}
		player.Render();
	}

	if (gameState == 2) {
		text.Draw("You lose.", 0.0, 0.0);
	}

	glPopMatrix();
	SDL_GL_SwapWindow(displayWindow);
}

void Asteroids::ShootBullet() {
	bool check = false;
	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i].isVisible && !check) {
			bullets[i].isVisible = true;
			bullets[i].x = player.x;
			bullets[i].y = player.y;
			bullets[i].r = player.r;
			check = true;
		}
	}
}

void Asteroids::FixedUpdate() {
	for (Entity &r : rocks) {
		if (player.isCollision(r))
			gameState = 2;
	}
	for (Entity &r : rocks) {
		for (Entity &r2 : rocks){
			if (r.isCollision(r2))
				break;
		}
	}

	player.FixedUpdate();
	for (Entity &b : bullets)
		b.FixedUpdate();
	for (Entity &r : rocks)
		r.FixedUpdate();
}

void Asteroids::Update(float elapsed) {

	SDL_Event event;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
			if (clock > 40) {
				ShootBullet();
				clock = 0;
			}
		}
	}

	if (gameState == 0) {
		if (keys[SDL_SCANCODE_RETURN]) {
			gameState = 1;
		}
	}
	if (gameState == 1) {
		player.Update(elapsed, keys);
		for (Entity &b : bullets)
			b.Update(elapsed, keys);
		for (Entity &r : rocks)
			r.Update(elapsed, keys);
		for (Entity &r : rocks) {
			if (rand() % 50 == 5)
				r.isVisible = true;
		}
		srand(time(NULL));
	}
}

int clockCount = 0;

bool Asteroids::UpdateAndRender() {
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
			clockCount = 0;
		}
		clock++;
		clockCount++;
	}
	timeLeftOver = fixedElapsed;

	Update(elapsed);
	Render();
	return done;
}

int main(int argc, char *argv[]) {
	Asteroids a;
	while (!a.UpdateAndRender()) {}
	return 0;
}