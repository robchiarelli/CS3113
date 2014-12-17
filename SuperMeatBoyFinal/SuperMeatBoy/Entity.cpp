#include "Entity.h"

Entity::Entity() {
	friction_x = 5.0f;
	friction_y = 0.0f;
	velocity_x = 0.0f;
	velocity_y = 0.0f;
	acceleration_x = 0.0f;
	acceleration_y = -4.0f;
	enableCollisions = true;
	collidedBottom = false;
	collidedRight = false;
	collidedLeft = false;
	collidedTop = false;
	isStatic = true;
	isCollisionBool = false;
	textureID = NULL;

	rightEdge = 0.0f;
	leftEdge = 0.0f;
	bottomEdge = 0.0f;
	topEdge = 0.0f;

	runAnimation = new int[2];
	runAnimation[0] = 80;
	runAnimation[1] = 81;
	numFrames = 2;

	animationElapsed = 0.0f;
	framesPerSecond = 20.0f;
	spriteIndex = 0;

	facingRight = true;

	isVisible = false;
	timeAlive = 0;
}

float lerp(float v0, float v1, float t) {
	return (1.0 - t) * v0 + t * v1;
}

void Entity::Update(float elapsed, const Uint8 *keys) {

	rightEdge = x + width / 2;
	leftEdge = x - width / 2;
	topEdge = y + height / 2;
	bottomEdge = y - height / 2;

	if (type == "player") {
		if (acceleration_x != 0) {
			animationElapsed += elapsed;

			if (animationElapsed > 1.0 / framesPerSecond) {
				spriteIndex++;
				animationElapsed = 0.0;
				if (spriteIndex > numFrames - 1) {
					spriteIndex = 0;
				}
			}
			spriteNum = runAnimation[spriteIndex];
		}
		else
			spriteNum = 81;

		if (acceleration_y != 0.0)
			spriteNum = 80;

		if (velocity_x > 0)
			facingRight = true;
		if (velocity_x < 0)
			facingRight = false;

		if (!keys[SDL_SCANCODE_RIGHT])
			movingRight = false;
		if (!keys[SDL_SCANCODE_LEFT])
			movingLeft = false;

		if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_LEFT])
		{
			if (keys[SDL_SCANCODE_RIGHT]) {
				if (!collidedRight)
					acceleration_x = 5.0f;
				movingRight = true;
				if (collidedRight && velocity_y < 0) {
					velocity_y = -0.35f;
					acceleration_y = 0.0f;
				}
			}
			else
				movingRight = false;

			if (keys[SDL_SCANCODE_LEFT]) {
				if (!collidedLeft)
					acceleration_x = -5.0f;
				movingLeft = true;
				if (collidedLeft && velocity_y < 0) {
					velocity_y = -0.35f;
					acceleration_y = 0.0f;
				}
			}
			else
				movingLeft = false;
		}
		else {
			acceleration_x = 0.0f;
			movingRight = false;
			movingLeft = false;
		}
	}
}

void Entity::Jump() {
	if (collidedBottom) {
		if (velocity_y > 0)
			y += 0.05;
		velocity_y = 1.2f;
		acceleration_y = 0.0f;
	}
	else
		acceleration_y = -4.0f;
	if (collidedRight && movingRight && velocity_y != 0.0f) {
		velocity_x = -2.0f;
		acceleration_x = 0.0f;
		velocity_y = 1.2f;
		collidedRight = false;
	}
	if (collidedLeft && movingLeft && velocity_y != 0.0f) {
		velocity_x = 2.0f;
		acceleration_x = 0.0f;
		velocity_y = 1.2f;
		collidedLeft = false;
	}
	collidedBottom = false;
}

bool Entity::isCollision(Entity other) {
	if ((bottomEdge < other.topEdge) && (rightEdge > other.leftEdge) && (leftEdge < other.rightEdge) && (topEdge > other.bottomEdge)) {
		if (y > other.y) {
			if (other.velocity_y < 0) {
				y = other.y + height;
				velocity_y = other.velocity_y;
				acceleration_y = 0.0f;
				collidedBottom = true;
			}
			else if (other.velocity_y > 0) {
				y = other.y + height;
				velocity_y = other.velocity_y;
				acceleration_y = 0.0f;
				collidedBottom = true;
			}
		}
		else {
			collidedBottom = false;
			acceleration_y = -4.0f;
		}
	}
	else {
		return false;
	}
}

void Entity::FixedUpdateX() {
	if (type != "missile" && type != "platform") {
		velocity_x = lerp(velocity_x, 0.0f, FIXED_TIMESTEP * friction_x);
		velocity_x += FIXED_TIMESTEP * acceleration_x;
	}
	x += FIXED_TIMESTEP * velocity_x;
}

void Entity::FixedUpdateY() {
	if (type != "missile" && type != "platform") {
		if (!collidedBottom) acceleration_y = -4.0f;

		velocity_y = lerp(velocity_y, 0.0f, FIXED_TIMESTEP * friction_y);
		velocity_y += FIXED_TIMESTEP * acceleration_y;
		if (velocity_y < -2.0)
			velocity_y = -2.0;
	}
	y += FIXED_TIMESTEP * velocity_y;
}

void Entity::Render() {
	Draw(16, 8, spriteNum, facingRight);
}

void Entity::Draw(int spriteCountX, int spriteCountY, int index, bool facingRight) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, 0.0);

	if (facingRight) {
		GLfloat quad[] = { width * -0.5, height * 0.5, width * -0.5, height * -0.5, width * 0.5, height * -0.5, width * 0.5, height * 0.5 };
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);
	}
	else {
		GLfloat quad[] = { width * 0.5, height * 0.5, width * 0.5, height * -0.5, width * -0.5, height * -0.5, width * -0.5, height * 0.5 };
		glVertexPointer(2, GL_FLOAT, 0, quad);
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0 / (float)spriteCountX;
	float spriteHeight = 1.0 / (float)spriteCountY;

	GLfloat quadUVs[] = { u, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight,
		u + spriteWidth, v
	};
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}