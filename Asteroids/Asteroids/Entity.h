#ifndef ENTITY_H
#define ENTITY_H

#include "SheetSprite.h"
#include "Matrix.h"
#include <string>

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

class Entity {
public:
	Entity();
	Entity(float x, float y, float mass);

	void Update(float elapsed, const Uint8 *keys);
	void Render();

	void FixedUpdate();

	SheetSprite sprite;
	float x;
	float y;
	float r;

	float velocity_xy;
	float velocity_r;
	float acceleration_xy;
	float acceleration_r;
	float friction_xy;
	float friction_r;
	float mass;

	int loc;
	float startX;
	float startY;

	bool isCollision(Entity e);
	bool isVisible;

	std::string type;
	Matrix matrix;

	float rightEdge;
	float leftEdge;
	float bottomEdge;
	float topEdge;
	Vector topLeft;
	Vector topRight;
	Vector bottomLeft;
	Vector bottomRight;

	void BuildMatrix();
};

#endif