#include "ClassDemoApp.h"
#include "Entity.h"
#include <time.h>
#include <vector>

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
#define LEVEL_HEIGHT 16
#define LEVEL_WIDTH 128
#define TILE_SIZE 0.125f
#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8
#define WORLD_OFFSET_X 0.0f
#define WORLD_OFFSET_Y 0.0f

float timeLeftOver = 0.0f;

GLuint LoadTexture(const char *image_path) {
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

ClassDemoApp::ClassDemoApp() {
	InitOpenGL();
	InitRest();
	score = 0;
	bestScoreLevel0 = 0;
	bestScoreLevel1 = 0;
	bestScoreLevel2 = 0;
	clock = 0;
	gameState = 0;
	done = false;
	lastFrameTicks = 0.0f;

	particles = ParticleEmitter(100);

	screenShakeValue = 0.0f;
	screenShakeValue = 0.0f;
	screenShakeSpeed = 50.0f;

	animationAValue = 0.0f;
	animationBValue = 0.0f;
	animationCValue = 0.0f;
}

ClassDemoApp::~ClassDemoApp() {
	Mix_FreeMusic(maintheme);
	SDL_Quit();
}

bool ClassDemoApp::ReadHeader(std::ifstream &stream) {
	string line;
	mapWidth = -1;
	mapHeight = -1;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "width") {
			mapWidth = atoi(value.c_str());
		}
		else if (key == "height"){
			mapHeight = atoi(value.c_str());
		}
	}
	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else {
		levelData = new unsigned char*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			levelData[i] = new unsigned char[mapWidth];
		}
		return true;
	}
}

bool ClassDemoApp::ReadLayerData(std::ifstream &stream) {
	string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				istringstream lineStream(line);
				string tile;

				for (int x = 0; x < mapWidth; x++) {
					getline(lineStream, tile, ',');
					unsigned char val = (unsigned char)atoi(tile.c_str());
					if (val > 0) {
						levelData[y][x] = val;
					}
					else {
						levelData[y][x] = 0;
					}
				}
			}
		}
	}
	return true;
}

void ClassDemoApp::PlaceEntity(string type, float placeX, float placeY) {
	if (type == "player") {
		player = new Entity();
		player->type = type;
		player->x = placeX;
		player->y = placeY;
		player->width = TILE_SIZE;
		player->height = TILE_SIZE;
		player->textureID = texture;
	}

	if (type == "spikes") {
		Entity *e = new Entity();
		e->type = type;
		e->x = placeX + TILE_SIZE / 2;
		e->y = placeY;
		e->width = TILE_SIZE;
		e->height = TILE_SIZE;
		e->textureID = texture;
		e->spriteNum = 100;
		spikes.push_back(e);
	}

	if (type == "platform") {
		Entity *p = new Entity();
		p->type = type;
		p->x = placeX + TILE_SIZE / 2;
		p->y = placeY;
		p->velocity_y = 0.35;
		p->acceleration_y = 0.0f;
		p->width = TILE_SIZE;
		p->height = TILE_SIZE;
		p->textureID = texture;
		p->spriteNum = 5;
		platforms.push_back(p);
	}

	if (type == "turret") {
		Entity *t = new Entity();
		t->type = type;
		t->x = placeX + TILE_SIZE / 2;
		t->y = placeY;
		t->width = TILE_SIZE;
		t->height = TILE_SIZE;
		t->textureID = texture;
		t->spriteNum = 49;
		turrets.push_back(t);
	}
}

bool ClassDemoApp::ReadEntityData(std::ifstream &stream) {
	string line;
	string type;
		
	while (getline(stream, line)) {
		if (line == "") { break; }
		istringstream sStream(line);
		string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") {

			istringstream lineStream(value);
			string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');

			float placeX = atoi(xPosition.c_str()) / 16 * TILE_SIZE;
			float placeY = atoi(yPosition.c_str()) / 16 * -TILE_SIZE;

			PlaceEntity(type, placeX, placeY);
		}
	}
	return true;
}

void ClassDemoApp::BuildLevelData(int level) {
	if (level == 0) {
		ifstream stream("introlevel.txt");
		string line;
		while (getline(stream, line)) {
			if (line == "[header]") {
				if (!ReadHeader(stream)) {
					return;
				}
			}
			else if (line == "[layer]") {
				ReadLayerData(stream);
			}
			else if (line == "[ObjectsLayer]") {
				ReadEntityData(stream);
			}
		}
	}
	if (level == 1) {
		ifstream stream("firstlevel.txt");
		string line;
		while (getline(stream, line)) {
			if (line == "[header]") {
				if (!ReadHeader(stream)) {
					return;
				}
			}
			else if (line == "[layer]") {
				ReadLayerData(stream);
			}
			else if (line == "[ObjectsLayer]") {
				ReadEntityData(stream);
			}
		}
	}
	if (level == 2) {
		ifstream stream("secondlevel.txt");
		string line;
		while (getline(stream, line)) {
			if (line == "[header]") {
				if (!ReadHeader(stream)) {
					return;
				}
			}
			else if (line == "[layer]") {
				ReadLayerData(stream);
			}
			else if (line == "[ObjectsLayer]") {
				ReadEntityData(stream);
			}
		}
	}
}

void ClassDemoApp::RenderMap() {
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	vector<float> vertexData;
	vector<float> texCoordData;
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			if (levelData[y][x]) {
				float u = (float)(((int)levelData[y][x] - 1) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
				float v = (float)(((int)levelData[y][x] - 1) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;
				float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
				float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;
				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, (-TILE_SIZE * y) - TILE_SIZE,
					(TILE_SIZE * x) + TILE_SIZE, -TILE_SIZE * y
				});
				texCoordData.insert(texCoordData.end(), { u, v,
					u, v + (spriteHeight),
					u + spriteWidth, v + (spriteHeight),
					u + spriteWidth, v
				});
			}
		}
	}
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, vertexData.size() / 2);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ClassDemoApp::InitOpenGL() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("This is a game.", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
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

void ClassDemoApp::InitRest() {
	Mix_PlayMusic(maintheme, -1);
	texture = LoadTexture("arne_sprites.png");

	text.font = LoadTexture("font1.png");
	text.size = 0.125;
	text.spacing = -0.060;
	text.r = 0.0;
	text.g = 1.0;
	text.b = 0.0;
	text.a = 1.0;

	for (int i = 0; i < 50; i++) {
		Entity *m = new Entity();
		m->type = "missile";
		m->x = -10;
		m->y = -10;
		m->width = TILE_SIZE;
		m->height = TILE_SIZE;
		m->textureID = texture;
		m->spriteNum = 8;
		missiles.push_back(m);
	}
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

void ClassDemoApp::Render() {
	glClearColor(0.0f, 0.59f, 0.74f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);

	if (gameState == 0) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (clock > 10) {
			text.size = 0.35;
			text.g = 1.0f;
			text.b = 1.0f;
			text.r = 0.0f;
			text.spacing = -0.15f;

			text.Draw("Totally Not", -1.0f, animationAValue + 0.3);
			text.Draw("A Super", -0.65f, animationAValue);
			text.Draw("Meat Boy", -0.675f, animationAValue - 0.3);
			text.Draw("Ripoff", -0.5f, animationAValue - 0.6);

			text.size = 0.2;
			text.r = 0.5f;
			text.g = 0.0f;
			text.b = 1.0f;
			text.spacing = -0.125;
			text.Draw("(Poorly) Designed by", -0.75f, animationAValue - 0.9);
			text.Draw("Rob Chiarelli", -0.55f, animationAValue - 1.15);
		}
		if (clock > 100) {
			text.size = 0.1;
			text.r = 0.5f;
			text.g = 1.0f;
			text.b = 0.0f;
			text.spacing = -0.060;
			text.Draw("Press Space to Continue", -0.55f, animationAValue - 1.4);
		}
	}
	if (gameState == 1) {
		text.size = 0.1;
		text.r = 0.0f;
		text.g = 1.0f;
		text.b = 0.0f;
		text.spacing = -0.055;
		text.Draw("Press '1' to play the first level", -1.2, 0.7);
		text.Draw("Press '2' to play the second level", -1.2, 0.5);
		text.Draw("Press '3' to play the third level", -1.2, 0.3);
		text.Draw("Press escape to exit the game", -1.2, -0.5);
	}
	if (gameState == 2 || gameState == 6 || gameState == 9) {
		glTranslatef(0.0f, sin(screenShakeValue * screenShakeSpeed) * screenShakeIntensity, sin(screenShakeValue * screenShakeSpeed) * screenShakeIntensity);
		float translateX = 0.0f;
		if (player->x > 1.33)
			translateX = -player->x;
		else
			translateX = -1.33f;
		if (player->x > LEVEL_WIDTH / 8 - 1.33)
			translateX = -LEVEL_WIDTH / 8 + 1.33;
		glTranslatef(translateX, 1.0, 0.0f);
		RenderMap();
		player->Render();
		for (Entity *e : spikes)
			e->Render();
		for (Entity *p : platforms) {
			p->Render();
		}
		particles.Render();
		for (Entity *t : turrets) {
			t->Render();
		}
		for (Entity *m : missiles) {
			if (m->isVisible)
				m->Render();
		}
		text.Draw(to_string((int)floor(clock / 60)), -translateX + 1.125, -0.25f);
	}
	if (gameState == 3 || gameState == 7 || gameState == 10) {
		float translateX = 0.0f;
		if (player->x > 1.33)
			translateX = -player->x;
		else
			translateX = -1.33f;
		if (player->x > LEVEL_WIDTH / 8 - 1.33)
			translateX = -LEVEL_WIDTH / 8 + 1.33;
		glTranslatef(translateX, 1.0, 0.0f);
		RenderMap();
		for (Entity *e : spikes)
			e->Render();
		for (Entity *p : platforms) {
			p->Render();
		}
		particles.Render();
		for (Entity *t : turrets) {
			t->Render();
		}
		for (Entity *m : missiles) {
			if (m->isVisible)
				m->Render();
		}
		text.Draw(to_string((int)floor(clock / 60)), -translateX + 1.125, -0.25f);
	}
	if (gameState == 4) {
		text.Draw("Congratulations!  You beat level 0", -1.2, 0.5);
		text.Draw("	with a score of " + to_string(score), -1.2, 0.3);
		text.Draw("High Score: " + to_string(bestScoreLevel0), -1.2, 0.1);
		text.Draw("Press the space bar to return to the menu", -1.2, -0.3);
	}
	if (gameState == 8) {
		text.Draw("Congratulations!  You beat level 1", -1.2, 0.5);
		text.Draw("	with a score of " + to_string(score), -1.2, 0.3);
		text.Draw("High Score: " + to_string(bestScoreLevel1), -1.2, 0.1);
		text.Draw("Press the space bar to return to the menu", -1.2, -0.3);
	}
	if (gameState == 11) {
		text.Draw("Congratulations!  You beat level 2", -1.2, 0.5);
		text.Draw("	with a score of " + to_string(score), -1.2, 0.3);
		text.Draw("High Score: " + to_string(bestScoreLevel2), -1.2, 0.1);
		text.Draw("Press the space bar to return to the menu", -1.2, -0.3);
	}
	if (gameState == 5) {
		text.Draw("You lose", -1.2, 0.1);
		text.Draw("Press the space bar to play again", -1.2, -0.1);
	}

	glPopMatrix();

	SDL_GL_SwapWindow(displayWindow);
}

int worldToTileCoordX(float x) {
	int xx = (int)((x + (WORLD_OFFSET_X)) / TILE_SIZE);
	if (xx > LEVEL_WIDTH)
		xx = LEVEL_WIDTH;
	if (xx < 0)
		xx = 0;
	return xx;
}

int worldToTileCoordY(float y) {
	int yy = (int)((-y + (WORLD_OFFSET_Y)) / TILE_SIZE);
	if (yy > LEVEL_HEIGHT)
		yy = LEVEL_HEIGHT;
	if (yy < 0)
		yy = 0;
	return yy;
}

bool ClassDemoApp::IsCollisionWithMapX(Entity *e) {
	float tileBottom = e->y - e->height * TILE_SIZE;
	float tileTop = e->y + e->height * TILE_SIZE;
	float tileLeft = e->x - e->width * TILE_SIZE * 4;
	float tileRight = e->x + e->width * TILE_SIZE * 4;

	if (e->velocity_x < 0) {
		if ((levelData[worldToTileCoordY(tileBottom)][worldToTileCoordX(tileLeft)]) ||
			(levelData[worldToTileCoordY(tileTop)][worldToTileCoordX(tileLeft)])) {
			e->collidedLeft = true;
			e->x -= FIXED_TIMESTEP * e->velocity_x;
			e->velocity_x = 0.0f;
			e->acceleration_x = 0.0f;
		}
	}
	else if (e->velocity_x > 0) {
		if ((levelData[worldToTileCoordY(tileBottom)][worldToTileCoordX(tileRight)]) ||
			(levelData[worldToTileCoordY(tileTop)][worldToTileCoordX(tileRight)])) {
			e->collidedRight = true;
			e->x -= FIXED_TIMESTEP * e->velocity_x;
			e->velocity_x = 0.0f;
			e->acceleration_x = 0.0f;
		}
	}
	else {
		e->collidedLeft = false;
		e->collidedRight = false;
	}
	return (e->collidedLeft || e->collidedRight);
}

bool ClassDemoApp::IsCollisionWithMapY(Entity *e) {
	float tileBottom = e->y - e->height * TILE_SIZE * 4;
	float tileTop = e->y + e->height * TILE_SIZE * 4;
	float tileLeft = e->x - e->width * TILE_SIZE;
	float tileRight = e->x + e->width * TILE_SIZE;

	if (e->velocity_y < 0){
		if ((levelData[worldToTileCoordY(tileBottom)][worldToTileCoordX(tileRight)]) ||
			(levelData[worldToTileCoordY(tileBottom)][worldToTileCoordX(tileLeft)])) {
			e->collidedBottom = true;
			e->y -= FIXED_TIMESTEP * e->velocity_y;
			e->velocity_y = 0.0f; 
			e->acceleration_y = 0.0f;
		}
	}
	else if (e->velocity_y > 0 && !e->collidedTop){
		if ((levelData[worldToTileCoordY(tileTop)][worldToTileCoordX(tileRight)]) ||
			(levelData[worldToTileCoordY(tileTop)][worldToTileCoordX(tileLeft)])) {
			e->collidedTop = true;
			e->y -= FIXED_TIMESTEP * e->velocity_y;
			e->velocity_y = 0.0f; 
			e->acceleration_y = -4.0f;
		}
	}
	else {
		e->collidedTop = false;
		e->collidedBottom = false;
	}
	return (e->collidedTop || e->collidedBottom);
}

void ClassDemoApp::ShootMissile(Entity* e) {
	Mix_PlayChannel(-1, shoot, 0);
	bool check = false;
	for (int i = 0; i < missiles.size(); i++) {
		if (!missiles[i]->isVisible && !check) {
			missiles[i]->isVisible = true;
			missiles[i]->x = e->x;
			missiles[i]->y = e->y;
			missiles[i]->velocity_x = (player->x - e->x);
			missiles[i]->velocity_y = (player->y - e->y);
			check = true;
		}
	}
}

float shootClock = 0.0f;

float lerp2(float v0, float v1, float t) {
	return (1.0 - t) * v0 + t * v1;
}

void ClassDemoApp::FixedUpdate() {
	screenShakeIntensity = lerp2(screenShakeIntensity, 0.0f, FIXED_TIMESTEP);
	if (screenShakeIntensity < 0.01f)
		screenShakeIntensity = 0.0f;

	if (gameState == 2 || gameState == 6 || gameState == 9) {
		player->FixedUpdateY();
		IsCollisionWithMapY(player);
		player->FixedUpdateX();
		IsCollisionWithMapX(player);
		
		for (Entity *e : spikes) {
			if (player->isCollision(*e)) {
				particles.position.x = player->x;
				particles.position.y = player->y;
				particles.Reset();
				Mix_PlayChannel(-1, explode, 0);
				gameState = 3;
			}
		}

		for (Entity *p : platforms) {
			player->isCollision(*p);
			p->FixedUpdateY();
			IsCollisionWithMapY(p);
		}

		for (Entity *m : missiles) {
			m->FixedUpdateX();
			m->FixedUpdateY();
			if (IsCollisionWithMapX(m) || IsCollisionWithMapY(m)) {
				particles.position.x = m->x;
				particles.position.y = m->y;
				particles.Reset();
				m->isVisible = false;
				m->timeAlive = 0;
				screenShakeIntensity = 0.05f;
				Mix_PlayChannel(-1, explode, 0);
			}
			if (m->isVisible) {
				if (m->isCollision(*player)) {
					particles.position.x = player->x;
					particles.position.y = player->y;
					particles.Reset();
					gameState = 3;
					screenShakeIntensity = 0.05f;
					Mix_PlayChannel(-1, explode, 0);
				}
			}
		}

		for (Entity *t : turrets) {
			if (gameState == 6) {
				if (abs(t->x - player->x) < 1.33 && shootClock > 200) {
					ShootMissile(t);
					shootClock = 0;
				}
			}
			else {
				if (abs(t->x - player->x) < 1.33 && shootClock > 75) {
					ShootMissile(t);
					shootClock = 0;
				}
			}
		}
	}
}

float GS3 = 0.0f;

void ClassDemoApp::Update(float elapsed) {

	SDL_Event event;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}

	if (gameState == 0) {
		if (keys[SDL_SCANCODE_SPACE])
			gameState = 1;
	}

	if (gameState == 1) {
		if (keys[SDL_SCANCODE_1]) {
			BuildLevelData(0);
			gameState = 2;
		}
		if (keys[SDL_SCANCODE_2]) {
			BuildLevelData(1);
			gameState = 6;
		}
		if (keys[SDL_SCANCODE_3]) {
			BuildLevelData(2);
			gameState = 9;
		}
		if (keys[SDL_SCANCODE_ESCAPE])
			done = true;
	}
	if (gameState == 2 || gameState == 6 || gameState == 9) {
		player->Update(elapsed, keys);
		if (keys[SDL_SCANCODE_SPACE])
			player->Jump();

		if (player->x > LEVEL_WIDTH / 8 - 0.25) {
			score = floor(clock / 60);
			if (gameState == 2) {
				if (score < bestScoreLevel0 || bestScoreLevel0 == 0)
					bestScoreLevel0 = score;
			}
			if (gameState == 6) {
				if (score < bestScoreLevel1 || bestScoreLevel1 == 0)
					bestScoreLevel1 = score;
			}
			if (gameState == 9) {
				if (score < bestScoreLevel2 || bestScoreLevel2 == 0)
					bestScoreLevel2 = score;
			}
			gameState += 2;

		}

		for (Entity *e : spikes)
			e->Update(elapsed, keys);

		for (Entity *p : platforms) {
			p->Update(elapsed, keys);
			if (p->y > LEVEL_HEIGHT / 8 - 2.25)
				p->velocity_y = -0.35f;
			else if (p->velocity_y == 0)
				p->velocity_y = 0.35f;				
		}
		particles.Update(elapsed);

		for (Entity *m : missiles) {
			m->Update(elapsed, keys);
		}
	}

	if (gameState == 3 || gameState == 7 || gameState == 10) {
		GS3 += elapsed;
		if (GS3 > 3.0f)
			gameState = 5;
		particles.Update(elapsed);
	}

	if (gameState == 4 || gameState == 5 || gameState == 8 || gameState == 11) {
		if (keys[SDL_SCANCODE_SPACE])
			Reset();
	}
}

bool ClassDemoApp::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	screenShakeValue += elapsed;

	if (clock > 10) {
		textAnimationValue += elapsed;
		animationAValue += mapValue(textAnimationValue, 5.0f, 7.0f, 0.0f, 1.0f);
		if (animationAValue < 0.5)
			animationAValue = easeInOut(-1.33, 0.5, textAnimationValue);
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

		shootClock++;
		clock++;
	}
	timeLeftOver = fixedElapsed;

	for (Entity *m : missiles) {
		if (m->isVisible) {
			m->timeAlive++;
		}
		if (m->timeAlive > 400) {
			m->isVisible = false;
			m->timeAlive = 0;
		}
	}

	Update(elapsed);
	Render();
	return done;
}

void ClassDemoApp::Reset() {
	gameState = 0;
	clock = 0;
	done = false;
	lastFrameTicks = 0.0f;
	score = 0;
	GS3 = 0;

	delete player;
	delete levelData;
	platforms.clear();
	spikes.clear();
	turrets.clear();
	missiles.clear();

	InitRest();
}

int main(int argc, char *argv[]) {
	ClassDemoApp app;
	while (!app.UpdateAndRender()) {}
	return 0;
}