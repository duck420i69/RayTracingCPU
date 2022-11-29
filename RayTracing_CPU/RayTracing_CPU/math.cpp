#include "math.h"
#include <cmath>
#include <random>

vec2 vec2::operator+(const vec2& other) const
{
	return { x + other.x, y + other.y};
}

vec2 vec2::operator*(const float& other) const
{
	return { x * other, y * other };
}

vec2 vec2::operator-(const vec2& other) const
{
	return { x - other.x, y - other.y };
}

vec4::vec4() {
	v << 0.0f, 0.0f, 0.0f;
	w = 1.0f;
}

vec4::vec4(Eigen::Vector3f vector3, float w) {
	v = vector3;
	this->w = 1.0f;
}

vec4::vec4(float x, float y, float z, float w) {
	v << x, y, z;
	this->w = w;
}

vec4 vec4::operator-(const vec4& other) const {
	return { v - other.v, 1.0f };
}

vec4 vec4::operator+(const vec4& other) const {
	return { v + other.v, 1.0f };
}

float vec4::norm() {
	return v.norm();
}

vec4 vec4::cross(const vec4& other) const {
	return { v.cross(other.v), 1.0f };
}

float vec4::dot(const vec4& other) const {
	return v.dot(other.v);
}

void vec4::normalize() {
	v.normalize();
}

vec4 operator*(const vec4& vec, const float& other) {
	return { vec.v * other, vec.w };
}

void vec2::normalize() {
	float lenght = 1/sqrt(x * x + y * y);
	x *= lenght;
	y *= lenght;
}
