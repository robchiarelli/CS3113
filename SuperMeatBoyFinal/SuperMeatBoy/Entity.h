#ifndef ENTITY_H
#define ENTITY_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include <string>
#include <vector>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

class Entity {
public:
	Entity();

	void Update(float elapsed, const Uint8 *keys);
	void Render();

	bool isCollision(Entity entity);

	void FixedUpdateX();
	void FixedUpdateY();

	float x;
	float y;

	float width;
	float height;
	float velocity_x;
	float velocity_y;
	float acceleration_x;
	float acceleration_y;
	float friction_x;
	float friction_y;
	float gravity_y;
	float mass;
	bool isStatic;

	bool movingRight;
	bool movingLeft;

	bool facingRight;

	std::string index;
	std::string type;

	bool enableCollisions;
	bool isVisible;
	int timeAlive;

	void Jump();

	bool collidedBottom;
	bool collidedLeft;
	bool collidedRight;
	bool collidedTop;
	bool isCollisionBool;

	float rightEdge;
	float leftEdge;
	float bottomEdge;
	float topEdge;

	void Draw(int spriteCountX, int spriteCountY, int index, bool facingRight);

	unsigned int textureID;
	GLfloat quad[8];
	GLfloat quadUVs[8];

	int *runAnimation;
	int numFrames;
	int spriteNum;
	float animationElapsed;
	float framesPerSecond;
	int spriteIndex;
};

#endif