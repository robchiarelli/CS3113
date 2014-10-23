#include "ClassDemoApp.h"
#include "SheetSprite.h"
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
#define LEVEL_WIDTH 64
#define TILE_SIZE 16.0f / 256.0f
#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8
float timeLeftOver = 0.0f;

unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];

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

void ClassDemoApp::ParseLevelData(GLuint textureMap) {
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (levelData[y][x] != 0) {
				Entity e;

				float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float)SPRITE_COUNT_X;
				float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float)SPRITE_COUNT_Y;
				float spriteWidth = 1.0f / (float)SPRITE_COUNT_X;
				float spriteHeight = 1.0f / (float)SPRITE_COUNT_Y;

				e.sprite.quad[0] = TILE_SIZE * x;
				e.sprite.quad[1] = -TILE_SIZE * y;
				e.sprite.quad[2] = TILE_SIZE * x;
				e.sprite.quad[3] = (-TILE_SIZE * y) - TILE_SIZE * 2;
				e.sprite.quad[4] = (TILE_SIZE * x) + TILE_SIZE * 2;
				e.sprite.quad[5] = (-TILE_SIZE * y) - TILE_SIZE * 2;
				e.sprite.quad[6] = (TILE_SIZE * x) + TILE_SIZE * 2;
				e.sprite.quad[7] = -TILE_SIZE * y;

				e.sprite.quadUVs[0] = u;
				e.sprite.quadUVs[1] = v;
				e.sprite.quadUVs[2] = u;
				e.sprite.quadUVs[3] = v + spriteHeight;
				e.sprite.quadUVs[4] = u + spriteWidth;
				e.sprite.quadUVs[5] = v + spriteHeight;
				e.sprite.quadUVs[6] = u + spriteWidth;
				e.sprite.quadUVs[7] = v;

				e.sprite.width = TILE_SIZE * 2;
				e.sprite.height = TILE_SIZE * 2;
				e.sprite.textureID = textureMap;
				e.x = (TILE_SIZE * x);
				e.y = (-TILE_SIZE * y);
				entities.push_back(e);
			}
		}
	}
}

ClassDemoApp::ClassDemoApp() {
	Init();
	score = 0;
	clock = 0;
	gameState = 0;
	done = false;
	lastFrameTicks = 0.0f;
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
						levelData[y][x] = val - 1;
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
		player.type = type;
		player.x = placeX;
		player.y = placeY;
		player.sprite.width = TILE_SIZE * 2.0;
		player.sprite.height = TILE_SIZE * 2.0;
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

			float placeX = atoi(xPosition.c_str()) / 16 * TILE_SIZE * 2.0f;
			float placeY = atoi(yPosition.c_str()) / 16 * -TILE_SIZE * 2.0f;

			PlaceEntity(type, placeX, placeY);
		}
	}
	return true;
}

void ClassDemoApp::BuildLevelData() {
	ifstream stream("newmap.txt");
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

void ClassDemoApp::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Hell.", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	GLuint textureMap = LoadTexture("arne_sprites.png");
	GLuint texture2 = LoadTexture("green_panel.png");

	BuildLevelData();

	ParseLevelData(textureMap);
	player.sprite.textureID = textureMap;

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
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);

	if (gameState == 0) {
		writeScore.size = 0.1;
		writeScore.Draw("Press Enter to begin.", -1.0, -0.7);
	}
	if (gameState == 1) {
		glTranslatef(-player.x, -player.y - 0.25, 0.0f);
		player.Render();
		for (int i = 0; i < entities.size(); i++)
			entities[i].Render();
		//Testing positions
		string spriteSize = "(" + to_string(entities[0].sprite.width) + ", " + to_string(entities[0].sprite.height) + ")";
		writeScore.Draw(spriteSize, player.x + 0.5, player.y + 0.7);
		string spriteSize2 = "(" + to_string(player.sprite.width) + ", " + to_string(player.sprite.height) + ")";
		writeScore.Draw(spriteSize2, player.x + 0.5, player.y + 0.9);
		string first = "(" + to_string(entities[0].x) + ", " + to_string(entities[0].y) + ")";
		writeScore.Draw(first, player.x + 0.5, player.y + 0.5);
		string playerXY = "(" + to_string(player.x) + ", " + to_string(player.y) + ")";
		writeScore.Draw(playerXY, player.x + 0.5, player.y + 0.3);
		string collided = to_string(player.collidedBottom);
		writeScore.Draw(collided, player.x, player.y);
		glTranslatef(player.x, player.y + 0.25, 0.0f);
	}

	if (gameState == 2) {
		string scoreString = "Your final score is: " + to_string(score);
		writeScore.size = 0.1;
		writeScore.Draw(scoreString, 0.0, 0.5);
	}

	glPopMatrix();

	SDL_GL_SwapWindow(displayWindow);
}

void ClassDemoApp::FixedUpdate() {
	if (gameState == 1) {
		player.FixedUpdate();
		for (Entity e : entities) {
			if (player.isCollision(e))
				break;
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
		for (Entity e : entities)
			e.Update(elapsed, keys);
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