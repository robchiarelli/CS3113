#include "Entity.h"
#include <math.h>
#include <algorithm>

Entity::Entity() {
	x = 0.0f;
	y = 0.0f;
	r = 0.0f;
	velocity_xy = 0.0f;
	velocity_r = 0.0f;
	acceleration_xy = 0.0f;
	acceleration_r = 0.0f;
	friction_xy = 1.0f;
	friction_r = 5.0f;
	rightEdge = 0.0f;
	leftEdge = 0.0f;
	topEdge = 0.0f;
	bottomEdge = 0.0f;
	isVisible = true;
	clock = 0;
}

Entity::Entity(float x, float y) {
	this->x = x;
	this->y = y;
	r = M_PI / 2;
	velocity_xy = 0.0f;
	velocity_r = 0.0f;
	acceleration_xy = 0.0f;
	acceleration_r = 0.0f;
	friction_xy = 2.0f;
	friction_r = 2.0f;
	rightEdge = 0.0f;
	leftEdge = 0.0f;
	topEdge = 0.0f;
	bottomEdge = 0.0f;
	clock = 0;
}

float lerp(float v0, float v1, float t) {
	return (1.0 - t) * v0 + t * v1;
}

void Entity::BuildMatrix() {
	Matrix scale;
	Matrix translate;
	Matrix rotate;

	scale.identity();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			scale.m[i][j] *= sprite.scale;
		}
	}

	translate.identity();
	translate.m[0][3] = x;
	translate.m[1][3] = y;

	rotate.identity();
	rotate.m[0][0] = cos(r);
	rotate.m[0][1] = -sin(r);
	rotate.m[1][0] = sin(r);
	rotate.m[1][1] = cos(r);
	
	matrix.identity();
	matrix = scale * rotate * translate;
}

void Entity::Update(float elapsed, const Uint8 *keys) {
	rightEdge = sprite.width;
	leftEdge = -sprite.width;
	topEdge = sprite.height;
	bottomEdge = -sprite.height;
	topLeft = Vector(leftEdge, topEdge, 1.0f);
	topRight = Vector(rightEdge, topEdge, 1.0f);
	bottomLeft = Vector(leftEdge, bottomEdge, 1.0f);
	bottomRight = Vector(rightEdge, bottomEdge, 1.0f);
	
	if (type == "player") {
		if (keys[SDL_SCANCODE_RIGHT]) {
			velocity_r = -5.0f;
		}
		if (keys[SDL_SCANCODE_LEFT]) {
			velocity_r = 5.0f;
		}
		if (keys[SDL_SCANCODE_UP]) {
			velocity_xy = 0.6f;
		}
		//else engineOn = false;
	}

	if (type == "bullet") {
		velocity_xy = 4.0f;
		if (x > 1.33 || x < -1.33 || y > 1 || y < -1)
			isVisible = false;
	}

	if (type == "rock") {
		if (isVisible) {
			if (clock < 1000)
				velocity_xy = 0.25f;
			if (clock > 1000 && clock < 2000)
				velocity_xy = 0.5f;
			if (clock > 2000 && clock < 3000)
				velocity_xy = 0.75f;
			if (clock > 3000)
				velocity_xy = 1.0f;
		}
		if (x > 1.8 || x < -1.8 || y > 1.5 || y < -1.5) {
			isVisible = false;
		}

		if (!isVisible) {
			x = startX;
			y = startY;
			r = startR;
		}
	}
}

bool Entity::isCollision(Entity &e){
	if (isVisible && e.isVisible) {
		BuildMatrix();

		Vector tl1 = e.matrix * e.topLeft;
		tl1 = matrix.inverse() * tl1;
		Vector tr1 = e.matrix * e.topRight;
		tr1 = matrix.inverse() * tr1;
		Vector bl1 = e.matrix * e.bottomLeft;
		bl1 = matrix.inverse() * bl1;
		Vector br1 = e.matrix * e.bottomRight;
		br1 = matrix.inverse() * br1;

		Vector tl2 = topLeft;
		Vector tr2 = topRight;
		Vector bl2 = bottomLeft;
		Vector br2 = bottomRight;

		float minX1 = std::min(std::min(tl1.x, tr1.x), std::min(bl1.x, br1.x));
		float minY1 = std::min(std::min(tl1.y, tr1.y), std::min(bl1.y, br1.y));
		float maxX1 = std::max(std::max(tl1.x, tr1.x), std::max(bl1.x, br1.x));
		float maxY1 = std::max(std::max(tl1.y, tr1.y), std::max(bl1.y, br1.y));

		float minX2 = std::min(std::min(tl2.x, tr2.x), std::min(bl2.x, br2.x));
		float minY2 = std::min(std::min(tl2.y, tr2.y), std::min(bl2.y, br2.y));
		float maxX2 = std::max(std::max(tl2.x, tr2.x), std::max(bl2.x, br2.x));
		float maxY2 = std::max(std::max(tl2.y, tr2.y), std::max(bl2.y, br2.y));

		if (minX1 < e.rightEdge && maxX1 > e.leftEdge
			&& minY1 < e.topEdge && maxY1 > e.bottomEdge
			&& minX2 < rightEdge && maxX2 > leftEdge
			&& minY2 < topEdge && maxY2 > bottomEdge) {

			float rot = r;
			if (x < e.x) {
				x -= 0.02f;
				e.x += 0.02f;
			}
			else if (x > e.x) {
				x += 0.02f;
				e.x -= 0.02f;
			}
			if (y < e.y) {
				y -= 0.02f;
				e.y += 0.02f;
			}
			else if (y > e.y) {
				y += 0.02f;
				e.y -= 0.02f;
			}
			r = e.r;
			e.r = rot;
			return true;
		}
		return false;
	}
	else
		return false;
}

void Entity::FixedUpdate() {
	velocity_xy = lerp(velocity_xy, 0.0f, FIXED_TIMESTEP * friction_xy);
	velocity_r = lerp(velocity_r, 0.0f, FIXED_TIMESTEP * friction_r);

	velocity_xy += FIXED_TIMESTEP * acceleration_xy;
	velocity_r += FIXED_TIMESTEP * acceleration_r;

	x += FIXED_TIMESTEP * velocity_xy * cos(r);
	y += FIXED_TIMESTEP * velocity_xy * sin(r);
	r += FIXED_TIMESTEP * velocity_r;

	BuildMatrix();
}

void Entity::Render() {
	BuildMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(matrix.ml);

	sprite.DrawSheet(x, y);

	glPopMatrix();
}

