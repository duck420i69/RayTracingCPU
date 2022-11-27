#pragma once
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
	float x, y, z, w;
	inline vec4 operator-(const vec4& other) const {
		return { x - other.x, y - other.y, z - other.z, w };
	}
	inline vec4 operator+(const vec4& other) const { return { x + other.x, y + other.y, z + other.z, w }; }
	inline vec4 operator*(const float& other) const { return { x * other, y * other, z * other, 1 }; }
	inline float dot(const vec4& other) const { return x * other.x + y * other.y + z * other.z; }
	inline vec4 cross(const vec4& other) const { 
		return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x };
	}
	inline void normalize() {
		float lenght = 1 / sqrt(x * x + y * y + z * z);
		x *= lenght;
		y *= lenght;
		z *= lenght;
	}
};

struct mat4 {
	float mat[16];
	mat4 operator+(const mat4& other);
	mat4 operator-(const mat4& other);
	mat4 operator*(const mat4& other);
	vec4 operator*(const vec4& other);
};

inline float random() {
	static std::uniform_real_distribution<float> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline float random1() {
	static std::uniform_real_distribution<float> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline vec4 random_sphere() {
	vec4 vec{};
	do {
		vec.x = random1();
		vec.y = random1();
		vec.z = random1();
	} while (vec.x * vec.x + vec.y * vec.y + vec.z * vec.z > 1);
	vec.w = 1;
	return vec;
}

// Expect normalize vector
inline vec4 reflect(const vec4& src, const vec4& nor) {
	float temp = -2 * src.dot(nor);
	return { src.x + temp * nor.x, src.y + temp * nor.y, src.z + temp * nor.z, 1.0f };
}