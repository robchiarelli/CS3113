#include "Asteroids.h"
#include "SheetSprite.h"
#include "Entity.h"
#include <time.h>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
float timeLeftOver = 0.0f;

Asteroids::Asteroids() {
	InitOpenGL();
	InitRest();
	gameState = 0;
	gameStateClock = 0;
	clock = 0;
	done = false;
	hit = false;
	lastFrameTicks = 0.0f;

	exhaust = ParticleEmitter(100);

	screenShakeValue = 0.0f;
	screenShakeValue = 0.0f;
	screenShakeSpeed = 50.0f;

	animationAValue = 0.0f;
	animationBValue = 0.0f;
	animationCValue = 0.0f;
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

float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax) {
	float retVal = dstMin + ((value - srcMin) / (srcMax - srcMin) * (dstMax - dstMin));
	if (retVal < dstMin) {
		retVal = dstMin;
	}
	if (retVal > dstMax) {
		retVal = dstMax;
	}
	return retVal;
}

float easeIn(float from, float to, float time) {
	float tVal = time*time*time*time*time;
	return (1.0f - tVal)*from + tVal*to;
}

float easeOut(float from, float to, float time) {
	float oneMinusT = 1.0f - time;
	float tVal = 1.0f - (oneMinusT * oneMinusT * oneMinusT *
		oneMinusT * oneMinusT);
	return (1.0f - tVal)*from + tVal*to;
}

float easeInOut(float from, float to, float time) {
	float tVal;
	if (time > 0.5) {
		float oneMinusT = 1.0f - ((0.5f - time)*-2.0f);
		tVal = 1.0f - ((oneMinusT * oneMinusT * oneMinusT * oneMinusT *
			oneMinusT) * 0.5f);
	}
	else {
		time *= 2.0;
		tVal = (time*time*time*time*time) / 2.0;
	}
	return (1.0f - tVal)*from + tVal*to;
}

float easeOutElastic(float from, float to, float time) {
	float p = 0.3f;
	float s = p / 4.0f;
	float diff = (to - from);
	return from + diff + (diff*pow(2.0f, -10.0f*time) * sin((time - s)*(2 * M_PI) / p));
}

void Asteroids::InitOpenGL() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Asteroids with MATH!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	maintheme = Mix_LoadMUS("maintheme.mp3");

	shoot = Mix_LoadWAV("shoot.wav");
	explode = Mix_LoadWAV("explode.wav");
}

void Asteroids::InitRest() {
	Mix_PlayMusic(maintheme, -1);

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

	SheetSprite rS(texture, 84.0f / 1024.0f, 101.0f / 1024.0f, 224.0f / 1024.0f, 664.0f / 1024.0f, 1.5f);
	for (int i = 0; i < 40; i++) {
		float sY = 0.0f;
		float sX = 0.0f;

		int loc = randomInt(4);
		if (loc == 0 || loc == 1)
			sY = 1.4f;
		else
			sY = -1.4f;
		if (loc == 1 || loc == 2)
			sX = 1.75f;
		else
			sX = -1.75f;

		Entity rock(sX, sY);
		rock.startX = sX;
		rock.startY = sY;

		int angle = randomInt(60);
		if (angle < 30) angle += 30;
		if (loc == 0) rock.r = (angle - 90) * M_PI / 180;
		if (loc == 1) rock.r = (angle - 180) * M_PI / 180;
		if (loc == 2) rock.r = (angle + 90) * M_PI / 180;
		if (loc == 3) rock.r = (angle)* M_PI / 180;
		rock.startR = rock.r;
		rock.sprite = rS;
		rock.type = "rock";
		rock.rightEdge = rock.sprite.width / 2;
		rock.leftEdge = -rock.sprite.width / 2;
		rock.topEdge = rock.sprite.height / 2;
		rock.bottomEdge = -rock.sprite.height / 2;
		rock.topLeft = Vector(rock.leftEdge, rock.topEdge, 1.0f);
		rock.topRight = Vector(rock.rightEdge, rock.topEdge, 1.0f);
		rock.bottomLeft = Vector(rock.leftEdge, rock.bottomEdge, 1.0f);
		rock.bottomRight = Vector(rock.rightEdge, rock.bottomEdge, 1.0f);
		rock.isVisible = false;
		rock.BuildMatrix();
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
	Mix_FreeMusic(maintheme);

	SDL_Quit();
}

void Asteroids::Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (gameState == 0) {
		if (clock > 10) {
			text.size = 0.35;
			text.g = 1.0f;
			text.b = 1.0f;
			text.spacing = -0.075f;

			text.Draw("Asteroids", -1.05f, animationAValue);

			text.size = 0.1;
			text.b = 0.0f;
			text.spacing = -0.055;

			text.Draw("Use the arrow keys to move.", -0.7f, animationAValue - 0.5);
			text.Draw("Press the space button to shoot.", -0.7f, animationAValue - 0.7);
			text.Draw("Press Enter to begin.", -0.7f, animationAValue - 1.1);
		}
	}

	if (gameState == 1 || gameState == 2 || gameState == 3 || gameState == 4 || gameState == 5 || gameState == 6 || gameState == 7 || gameState == 8) {

		glTranslatef(0.0f, sin(screenShakeValue * screenShakeSpeed) * screenShakeIntensity, sin(screenShakeValue * screenShakeSpeed) * screenShakeIntensity);
		
		exhaust.Render();
		for (Entity r : rocks) {
			if (r.isVisible)
				r.Render();
		}
		for (Entity b : bullets) {
			if (b.isVisible)
				b.Render();
		}
		if (player.isVisible) {
			player.Render();
		}

		text.Draw("Score:" + to_string(score), 0.8, 0.8);
	}

	if (gameState == 2 || gameState == 4 || gameState == 6) {
		text.size = 0.2f;
		text.Draw("Speed Up!", -0.5f, 0.0f);
		text.size = 0.1f;
	}

	if (gameState == 9) {
		text.Draw("Your final score is " + to_string(score), -0.4, 0.0);
		text.Draw("Press Space to play again", -0.4, -0.2);
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

float lerp2(float v0, float v1, float t) {
	return (1.0 - t) * v0 + t * v1;
}

void Asteroids::FixedUpdate() {	 
	screenShakeIntensity = lerp2(screenShakeIntensity, 0.0f, FIXED_TIMESTEP);
	if (screenShakeIntensity < 0.01f)
		screenShakeIntensity = 0.0f;

	for (Entity &r : rocks) {
		if (player.isCollision(r)) {
			gameState = 8;
			exhaust.position.x = player.x;
			exhaust.position.y = player.y;
			exhaust.Reset();
			player.isVisible = false;
			Mix_PlayChannel(-1, explode, 0);
			break;
		}
	}
	for (Entity &r : rocks) {
		for (Entity &r2 : rocks){
			if (r.isCollision(r2)) {
				break;
			}
		}
	}

	for (Entity &r : rocks) {
		r.clock = gameStateClock;
	}

	for (Entity &b : bullets) {
		for (Entity &r : rocks){
			if (b.isCollision(r)) {
				score += 100;
				exhaust.position.x = r.x;
				exhaust.position.y = r.y;
				exhaust.Reset();
				screenShakeIntensity = 0.05f;
				r.isVisible = false;
				b.isVisible = false;
				Mix_PlayChannel(-1, explode, 0);
				break;
			}
			else
				hit = false;
		}
	}

	player.FixedUpdate();
	for (Entity &b : bullets) {
		if (b.isVisible)
			b.FixedUpdate();
	}
	for (Entity &r : rocks)
		r.FixedUpdate();
}

float GS8 = 0.0f;

void Asteroids::Update(float elapsed) {

	SDL_Event event;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && gameState > 0 && gameState < 9) {
			if (clock > 20) {
				Mix_PlayChannel(-1, shoot, 0);
				ShootBullet();
				clock = 0;
			}
		}
	}


	if (gameState == 0) {
		if (keys[SDL_SCANCODE_RETURN])
			gameState = 1;
	}
	if (gameState == 1 || gameState == 2 || gameState == 3 || gameState == 4 || gameState == 5 || gameState == 6 || gameState == 7 || gameState == 8) {
		player.Update(elapsed, keys);
		for (Entity &b : bullets)
			b.Update(elapsed, keys);
		for (Entity &r : rocks)
			r.Update(elapsed, keys);
		for (Entity &r : rocks) {
			if (clock < 2000) {
				if (rand() % 60 == 0 && !r.isVisible)
					r.isVisible = true;
			}
			else {
				if (rand() % 25 == 0 && !r.isVisible)
					r.isVisible = true;
			}
		}
		srand(time(NULL));

		exhaust.Update(elapsed);
	}
	if (gameState == 1 && gameStateClock == 1000) {
		gameState = 2;
	}
	if (gameState == 2 && gameStateClock == 1075) {
		gameState = 3;
	}
	if (gameState == 3 && gameStateClock == 2000) {
		gameState = 4;
	}
	if (gameState == 4 && gameStateClock == 2075) {
		gameState = 5;
	}
	if (gameState == 5 && gameStateClock == 3000) {
		gameState = 6;
	}
	if (gameState == 6 && gameStateClock == 3075) {
		gameState = 7;
	}

	if (gameState == 8) {
		GS8 += elapsed;
		if (GS8 > 0.75f)
			gameState = 9;
	}

	if (gameState == 9) {
		if (keys[SDL_SCANCODE_SPACE]) {
			ResetGame();
		}
		else
			gameState = 9;
	}
}

int clockCount = 0;

bool Asteroids::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	screenShakeValue += elapsed;
	
	if (clock > 10) {
		textAnimationValue += elapsed;
		animationAValue += mapValue(textAnimationValue, 5.0f, 7.0f, 0.0f, 1.0f);
		if (animationAValue < 0.5)
			animationAValue = easeInOut(-1.2, 0.5, textAnimationValue);
		else
			textAnimationValue = 0.0f;
	}

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
		if (gameState > 0) gameStateClock++;
		clockCount++;
		animationBValue = mapValue(elapsed, 2.0f, 4.0f, 0.0f, 1.0f);
	}
	timeLeftOver = fixedElapsed;

	Update(elapsed);
	Render();
	return done;
}

void Asteroids::ResetGame() {
	bullets.clear();
	rocks.clear();
	
	InitRest();

	gameState = 0;
	gameStateClock = 0;
	clock = 0;
	done = false;
	hit = false;
	lastFrameTicks = 0.0f;
	score = 0;

	player.x = 0.0f;
	player.y = 0.0f;
	player.r = 0.0f;
	player.velocity_xy = 0.0f;
	player.velocity_r = 0.0f;
	player.acceleration_xy = 0.0f;
	player.acceleration_r = 0.0f;
	player.friction_xy = 1.0f;
	player.friction_r = 5.0f;
	player.matrix.identity();
	player.BuildMatrix();
	player.isVisible = true;

	GS8 = 0.0f;

	exhaust = ParticleEmitter(100);

	screenShakeValue = 0.0f;
	screenShakeValue = 0.0f;
	screenShakeSpeed = 50.0f;

	animationAValue = 0.0f;
	animationBValue = 0.0f;
	animationCValue = 0.0f;
}

int main(int argc, char *argv[]) {
	Asteroids a;
	while (!a.UpdateAndRender()) {}
	return 0;
}