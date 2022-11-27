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

void vec2::normalize() {
	float lenght = 1/sqrt(x * x + y * y);
	x *= lenght;
	y *= lenght;
}

mat4 mat4::operator+(const mat4& other)
{
	mat4 temp{0.0f};
	for (int i = 0; i < 16; i++) {
		temp.mat[i] = mat[i] + other.mat[i];
	}
	return temp;
}

mat4 mat4::operator-(const mat4& other)
{
	mat4 temp{0.0f};
	for (int i = 0; i < 16; i++) {
		temp.mat[i] = mat[i] - other.mat[i];
	}
	return temp;
}

mat4 mat4::operator*(const mat4& other)
{
	mat4 result{0.0f};
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.mat[i + j * 4] = mat[i + k * 4] * other.mat[k + j * 4];
			}
		}
	}
	return result;
}

vec4 mat4::operator*(const vec4& other)
{
	vec4 result{0.0f};
	result.x = other.x * mat[0]  + other.y * mat[1]  + other.z * mat[2]  + other.w * mat[3];
	result.y = other.x * mat[4]  + other.y * mat[5]  + other.z * mat[6]  + other.w * mat[7];
	result.z = other.x * mat[8]  + other.y * mat[9]  + other.z * mat[10] + other.w * mat[11];
	result.w = other.x * mat[12] + other.y * mat[13] + other.z * mat[14] + other.w * mat[15];
	return result;
}
