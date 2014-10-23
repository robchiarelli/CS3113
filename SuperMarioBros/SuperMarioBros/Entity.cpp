#include "Entity.h"

Entity::Entity() {
	sprite = SheetSprite();
	friction_x = 5.0f;
	gravity_y = -4.0f;
	enableCollisions = true;
	collidedBottom = false;
	isStatic = true;
	isCollisionBool = false;
}

float lerp(float v0, float v1, float t) {
	return (1.0 - t) * v0 + t * v1;
}

void Entity::Update(float elapsed, const Uint8 *keys) {
	rightEdge = x + sprite.width / 2;
	leftEdge = x - sprite.width / 2;
	topEdge = y + sprite.height / 2;
	bottomEdge = y - sprite.height / 2;

	height = sprite.height;
	width = sprite.width;

	if (type == "player") {
		if (keys[SDL_SCANCODE_RIGHT]) {
			velocity_x = 2.0f;
			movingRight = true;
		}
		else
			movingRight = false;
		if (keys[SDL_SCANCODE_LEFT]) {
			velocity_x = -2.0f;
			movingLeft = true;
		}
		else
			movingLeft = false;
		if (keys[SDL_SCANCODE_UP]) {
			if (collidedBottom && (velocity_y == 0.0)) {
				velocity_y = 2.5f;
			}
		}
	}
}

bool Entity::isCollision(Entity other) {
	//general collision
	if ((bottomEdge < other.topEdge) && (rightEdge > other.leftEdge) && (leftEdge < other.rightEdge) && (topEdge > other.bottomEdge)) {
		isCollisionBool = true;
		//collidedBottom
		if ((velocity_y <= 0) && bottomEdge > other.topEdge - 0.05) {
			if (other.isStatic) {
				collidedBottom = true;
				velocity_y = 0.0f;
				acceleration_y = 0.0f;
				y = sprite.height / 2 + other.topEdge;
			}
		}
		//collidedTop
		else if ((velocity_y > 0) && topEdge < other.bottomEdge + 0.03) {
			velocity_y = 0.0f;
			y = -sprite.height / 2 + other.bottomEdge;
		}
		//collidedRight
		else if (velocity_x > 0) {
			if (other.isStatic) {
				velocity_x = 0.0f;
				x = -sprite.width / 2 + other.leftEdge;
			}
		}
		//collidedLeft
		else if (velocity_x < 0) {
			if (other.isStatic) {
				velocity_x = 0.0f;
				x = sprite.width / 2 + other.rightEdge;
			}
		}
	}
	else {
		isCollisionBool = false;
		collidedBottom = false;
		acceleration_y = gravity_y;
	}

	return isCollisionBool;
}

void Entity::FixedUpdate() {
	velocity_x = lerp(velocity_x, 0.0f, FIXED_TIMESTEP * friction_x);
	velocity_y = lerp(velocity_y, 0.0f, FIXED_TIMESTEP * friction_y);

	velocity_x += FIXED_TIMESTEP * acceleration_x;
	velocity_y += FIXED_TIMESTEP * acceleration_y;

	x += FIXED_TIMESTEP * velocity_x;
	y += FIXED_TIMESTEP * velocity_y;
}

void Entity::Render() {
	if (type == "player")
		sprite.DrawPlayer(x, y, 16, 8, 81);
	else
		sprite.Draw(x, y);
}