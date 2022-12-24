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

	inline vec4() {
		v << 0.0f, 0.0f, 0.0f;
		w = 1.0f;
	}

	inline vec4(Eigen::Vector3f vector3, float w) {
		v = vector3;
		this->w = 1.0f;
	}

	inline vec4(float x, float y, float z, float w) {
		v << x, y, z;
		this->w = w;
	}

	inline vec4 operator-(const vec4& other) const {
		return { v - other.v, 1.0f };
	}

	inline vec4 operator+(const vec4& other) const {
		return { v + other.v, 1.0f };
	}

	inline float norm() {
		return v.norm();
	}

	inline vec4 cross(const vec4& other) const {
		return { v.cross(other.v), 1.0f };
	}

	inline float dot(const vec4& other) const {
		return v.dot(other.v);
	}

	inline void normalize() {
		v.normalize();
	}
};

inline vec4 operator*(const vec4& vec, const float& other) {
	return { vec.v * other, vec.w };
}

inline vec4 operator*(const vec4& vec, const vec4& other) {
	return { vec.v[0] * other.v[0], vec.v[1] * other.v[1], vec.v[2] * other.v[2], vec.w };
}


inline float random0to1() {
	static std::uniform_real_distribution<float> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline float randomrange1() {
	static std::uniform_real_distribution<float> distribution(-1.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline int random0to2() {
	static std::uniform_real_distribution<float> distribution(-0.5, 2.49999);
	static std::mt19937 generator;
	return (int)distribution(generator);
}

inline vec4 random_in_sphere() {
	float z = random0to1();
	float r = std::sqrt(1 - z * z);
	float phi = 2.0f * EIGEN_PI * random0to1();
	return vec4(r * std::cos(phi), r * std::sin(phi), z, 1.0f);
}

inline vec4 random_in_cone(const float& cosThetaMax, const vec4& direction) {
	float u = random0to1();
	float cosTheta = 1 - u + u * cosThetaMax;
	float sinTheta = std::sqrt(1 - cosTheta * cosTheta);
	float phi = 2.0f * EIGEN_PI * random0to1();
	vec4 result(sinTheta * std::cos(phi), cosTheta, sinTheta * std::sin(phi), 1.0f);
	if (direction.v(1) == 1.0f) return result;
	vec4 x = direction.cross(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	vec4 z = direction.cross(x);
	return x * result.v(0) + direction * result.v(1) + z * result.v(2);
}

// Expect normalize vector
inline vec4 reflect(const vec4& src, const vec4& nor) {
	float temp = -2 * src.dot(nor);;
	return src + nor * temp;
}