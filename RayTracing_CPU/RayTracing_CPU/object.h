#pragma once

#include <memory>
#include <vector>
#include "math.h"

using texel = vec2;

struct AABB {
	AABB(vec4 low, vec4 high, int start, int end) : low_bound(low), high_bound(high), start_index(start), end_index(end) {}
	vec4 low_bound;
	vec4 high_bound;
	int start_index;
	int end_index;
	std::vector<AABB> child;
	inline bool hit(const vec4& dir, const vec4& src, double t_min = 0.01, double t_max = 10000.0) const {
		for (int a = 0; a < 3; a++) {
			auto invD = 1.0f / dir.v(a);
			auto t0 = (low_bound.v(a) - src.v(a)) * invD;
			auto t1 = (high_bound.v(a) - src.v(a)) * invD;
			if (invD < 0.0f)
				std::swap(t0, t1);
			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;
			if (t_max <= t_min)
				return false;
		}
		return true;
	}
};

struct Triangle {
	vec4 a, b, c, normal;
	texel ta, tb, tc;
	vec4 col;
	Triangle();
	Triangle(vec4 p1, vec4 p2, vec4 p3);
	Triangle(vec4 p1, vec4 p2, vec4 p3, vec4 col);
	Triangle(vec4 p1, vec4 p2, vec4 p3, texel t1, texel t2, texel t3);
};

using Mesh = std::vector<Triangle>;

struct Texture {
	std::string name;
	vec4 ambient_color;
	vec4 specular_color;
	vec4 diffuse_color;
	float refraction, specular_exp;
	int width, height;
	std::vector<vec4> texture_map;
};


struct Object {
	Mesh mesh;
	vec4 ambient_color;
	vec4 specular_color;
	vec4 diffuse_color;
	std::shared_ptr<Texture> texture;
	std::vector<AABB> collisionBox;
};

struct Scene {
	vec4 global_light_dir;
	std::vector<Object> objects;
	std::vector<std::shared_ptr<Texture>> textures;
};
