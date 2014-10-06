#include "Entity.h"

Entity::Entity() {
	sprite = SheetSprite();
	friction_x = 2.0f;
	gravity_y = -0.3f;
	enableCollisions = true;
	collidedBottom = false;
	isStatic = false;
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

	if (!isStatic) {
		if (keys[SDL_SCANCODE_RIGHT]) {
			velocity_x = 0.3f;
			movingRight = true;
		}
		else
			movingRight = false;
		if (keys[SDL_SCANCODE_LEFT]) {
			velocity_x = -0.3f;
			movingLeft = true;
		}
		else
			movingLeft = false;
		if (keys[SDL_SCANCODE_UP]) {
			if (collidedBottom && (velocity_y == 0.0)) {
				velocity_y = 0.6f;
			}
		}
	}
}

bool Entity::FixedUpdate(Entity other) {
	//collidedBottom
	if ((velocity_y <= 0) && (bottomEdge < other.topEdge) && (bottomEdge > other.bottomEdge)
		&& (rightEdge > other.leftEdge) && (leftEdge < other.rightEdge)) {
		if (other.isStatic) {
			collidedBottom = true;
			velocity_y = 0.0f;
			acceleration_y = 0.0f;
			y = sprite.height / 2 + other.topEdge;
		}
	}
	else {
		collidedBottom = false;
		acceleration_y = gravity_y;
	}
	//collidedRight
	if ((velocity_x > 0 || movingRight) && (rightEdge > other.leftEdge) && (rightEdge < other.leftEdge + 0.01)
		&& (topEdge > other.bottomEdge) && (bottomEdge < other.topEdge)) {
		if (other.isStatic) {
			velocity_x = 0.0f;
			x = -sprite.width / 2 + other.leftEdge + 0.02;
		}
	}
	//collidedLeft
	else if ((velocity_x < 0 || movingLeft) && (leftEdge > other.rightEdge) && (leftEdge < other.rightEdge + 0.01)
		&& (topEdge > other.bottomEdge) && (bottomEdge < other.topEdge)) {
		if (other.isStatic) {
			velocity_x = 0.0f;
			x = sprite.width / 2 + other.rightEdge + 0.02;
		}
	}
	//collidedTop
	else if ((topEdge > other.bottomEdge) && (topEdge < other.topEdge)
		&& (rightEdge > other.leftEdge) && (leftEdge < other.rightEdge)) {
		if (other.isStatic) {
			velocity_y = 0.0f;
			y = -sprite.height / 2 + other.bottomEdge;
		}
	}

	velocity_x = lerp(velocity_x, 0.0f, FIXED_TIMESTEP * friction_x);
	velocity_y = lerp(velocity_y, 0.0f, FIXED_TIMESTEP * friction_y);

	velocity_x += FIXED_TIMESTEP * acceleration_x;
	velocity_y += FIXED_TIMESTEP * acceleration_y;

	x += FIXED_TIMESTEP * velocity_x;
	y += FIXED_TIMESTEP * velocity_y;

	return collidedBottom;
}

void Entity::Render() {
	sprite.Draw(x, y);
}

