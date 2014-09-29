#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

SDL_Window* displayWindow;

class Entity {
public:
	void Draw(){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glTranslatef(x, y, 0.0);
		glRotatef(rotation, 0.0, 0.0, 1.0);

		GLfloat quad[] = { width * -0.5, height * 0.5, width * -0.5, height * -0.5, width * 0.5, height * -0.5, width * 0.5, height * 0.5 };
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);

		GLfloat quadUVs[] = { 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0 };
		glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_QUADS, 0, 4);
		glDisable(GL_TEXTURE_2D);
	}

	void setSize(float w, float h) {
		width = w;
		height = h;
	}

	void setPosition(float xTemp, float yTemp, float r){
		x = xTemp;
		y = yTemp;
		rotation = r;
	}

	void setMovement(float s, float a){
		speed = s;
		angle = a;
	}

	float x;
	float y;
	float rotation;

	int textureID;

	float width;
	float height;

	float speed;
	float angle;
};

Entity ball;
Entity paddle1;
Entity paddle2;
Entity top;
Entity bottom;
Entity space;

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
	displayWindow = SDL_CreateWindow("Rob Chiarelli's Pong V", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	ball.textureID = LoadTexture("green_panel.png");
	paddle1.textureID = LoadTexture("green_panel.png");
	paddle2.textureID = LoadTexture("green_panel.png");
	top.textureID = LoadTexture("green_panel.png");
	bottom.textureID = LoadTexture("green_panel.png");
	space.textureID = LoadTexture("grey_button02.png");

	srand(time(NULL));

	ball.setSize(0.1, 0.1);
	ball.setPosition(0.0, 0.0, 0.0);
	ball.setMovement(2.0, rand() % 50 - 25);
	//The ball is set to move right at a random angle between 
	//-25 and 25 degrees for maximum playability

	paddle1.setSize(0.1, 0.5);
	paddle1.setPosition(-1.2, 0.0, 0.0);
	paddle1.setMovement(2.0, 0);

	paddle2.setSize(0.1, 0.5);
	paddle2.setPosition(1.2, 0.0, 0.0);
	paddle2.setMovement(2.0, 0);

	top.setSize(2.66, 0.1);
	top.setPosition(0.0, 1.0, 0.0);

	bottom.setSize(2.66, 0.1);
	bottom.setPosition(0.0, -1.0, 0.0);

	space.setSize(1.0, 0.3);
	space.setPosition(0.0, 0.8, 0.0);
}

void ProcessEvents(bool &done, float &elapsed, bool &gameOn) {

	SDL_Event event;

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}

	//Paddle1 Movement
	if (keys[SDL_SCANCODE_S]) {
		paddle1.y -= elapsed * paddle2.speed;
	}
	else if (keys[SDL_SCANCODE_W]) {
		paddle1.y += elapsed * paddle2.speed;
	}
	//https://wiki.libsdl.org/SDL_Scancode

	//Paddle2 Movement
	if (keys[SDL_SCANCODE_DOWN]) {
		paddle2.y -= elapsed * paddle2.speed;
	}
	else if (keys[SDL_SCANCODE_UP]) {
		paddle2.y += elapsed * paddle2.speed;
	}

	//Activate Game
	//I had to do this because the ball moves off the screen before the GUI loads
	if (keys[SDL_SCANCODE_SPACE]){
		gameOn = true;
	}

	//Ball Movement
	if (gameOn) {
		ball.x += elapsed * ball.speed * cos(ball.angle * 3.14 / 180);
		ball.y += elapsed * ball.speed * sin(ball.angle * 3.14 / 180);
	}
}

void Update(bool &done, float &elapsed, float &lastValidX, float &lastValidY) {
	//Collision Detection
	//Ball and paddles
	if (((ball.x - paddle1.x) < (paddle1.width / 2 + ball.width / 2) 
		&& (paddle1.y - paddle1.height / 2) < ball.y 
		&& (paddle1.y + paddle1.height / 2) > ball.y)
		|| ((paddle2.x - ball.x) < (paddle2.width / 2 + ball.width / 2) 
		&& (paddle2.y - paddle2.height / 2) < ball.y 
		&& (paddle2.y + paddle2.height / 2) > ball.y)) {
		ball.x = lastValidX;
		ball.y = lastValidY;
		if (ball.angle <= 180) ball.angle = 90.0 - (ball.angle - 90.0);
		else ball.angle = 270.0 - (ball.angle - 270.0);
	}
	else {
		lastValidX = ball.x;
		lastValidY = ball.y;
	}

	//Ball ball and walls
	if (ball.y - bottom.y < (bottom.height / 2 + ball.height / 2) 
		|| top.y - ball.y < (top.height / 2 + ball.height / 2)) {
		ball.x = lastValidX;
		ball.y = lastValidY;
		if (ball.angle <= 90.0) ball.angle = -(ball.angle);
		else ball.angle = 180.0 - (ball.angle - 180.0);
	}
	else {
		lastValidX = ball.x;
		lastValidY = ball.y;
	}

	//Paddle1 and walls
	if (top.y - paddle1.y < (top.height / 2 + paddle1.height / 2)) {
		paddle1.y -= elapsed * paddle1.speed;
	}
	if (paddle1.y - bottom.y < (bottom.height / 2 + paddle1.height / 2)) {
		paddle1.y += elapsed * paddle1.speed;
	}

	//Paddle2 and walls
	if (top.y - paddle2.y < (top.height / 2 + paddle2.height / 2)) {
		paddle2.y -= elapsed * paddle2.speed;
	}
	if (paddle2.y - bottom.y < (bottom.height / 2 + paddle2.height / 2)) {
		paddle2.y += elapsed * paddle2.speed;
	}

	//Ball and window edge
	if (ball.x < -1.33 || ball.x > 1.33) {
		done = true;
	}
}

void Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	space.Draw();
	ball.Draw();
	paddle1.Draw();
	paddle2.Draw();
	top.Draw();
	bottom.Draw();

	SDL_GL_SwapWindow(displayWindow);
}

int main(int argc, char *argv[])
{
	//I would put these in Setup(), but then I have to return them
	bool done = false;
	bool gameOn = false;

	float lastFrameTicks = 0.0f;
	float ballAngle = 0.0f;

	//This "last valid location" method seems to have fixed my collision issues,
	//played a 1-2 minute game without any issues
	float lastValidX = 0.0;
	float lastValidY = 0.0;

	Setup();

	while (!done) {

		//Couldn't think of a better place for this either
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		ProcessEvents(done, elapsed, gameOn);

		Update(done, elapsed, lastValidX, lastValidY);

		Render();
	}

	SDL_Quit();
	return 0;
}