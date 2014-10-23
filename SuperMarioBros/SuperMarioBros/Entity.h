#ifndef ENTITY_H
#define ENTITY_H

#include "SheetSprite.h"
#include <string>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

class Entity {
public:
	Entity();

	void Update(float elapsed, const Uint8 *keys);
	void Render();

	bool isCollision(Entity entity);

	void FixedUpdate();

	SheetSprite sprite;
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

	std::string index;
	std::string type;

	bool enableCollisions;
	bool isVisible;

	bool collidedBottom;
	bool isCollisionBool;

	float rightEdge;
	float leftEdge;
	float topEdge;
	float bottomEdge;
};

#endif