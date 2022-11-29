#pragma once

#include <Eigen/Dense>
#include <random>

constexpr int epsilon = 0.0001f;


struct vec2 {
	float x, y;
	vec2 operator-(const vec2& other) const;
	vec2 operator+(const vec2& other) const;
	vec2 operator*(const float& other) const;
	void normalize();
};

struct vec4 {
	Eigen::Vector3f v;
	float w;

	vec4();
	vec4(Eigen::Vector3f vector3, float w);
	vec4(float x, float y, float z, float w);

	vec4 operator-(const vec4& other) const;
	vec4 operator+(const vec4& other) const;
	vec4 cross(const vec4& other) const;
	float dot(const vec4& other) const;
	float norm();
	void normalize();
};

vec4 operator*(const vec4& vec, const float& other);


inline float random() {
	static std::uniform_real_distribution<float> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline vec4 random_sphere() {
	vec4 vec;

	do {
		vec.v.Random();
	} while (vec.norm() > 1);
	vec.w = 1.0f;
	return vec;
}

// Expect normalize vector
inline vec4 reflect(const vec4& src, const vec4& nor) {
	float temp = -2 * src.dot(nor);;
	return src + nor * temp;
}