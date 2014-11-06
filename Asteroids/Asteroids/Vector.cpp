#include "Vector.h"
#include <math.h>

Vector::Vector() {}

Vector::Vector(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

float Vector::length() {
	return sqrt(x * x + y * y + z * z);
}

void Vector::normalize() {
	x /= length();
	y /= length();
	z /= length();
}