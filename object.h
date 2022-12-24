#pragma once

#include <memory>
#include <vector>
#include "accelerator.h"

using texel = vec2;

class Ray {
public:
	Ray(vec4 origin, vec4 direction, float tMaximum = INFINITY) : o(origin), d(direction), tMax(tMaximum) {};

	vec4 o;
	vec4 d;
	mutable float tMax;
};

class Material {
public:
	std::string name;
	vec4 ambient_color;
	vec4 specular_color;
	vec4 diffuse_color;
	float refraction, specular_exp;
	int width, height;
	std::vector<vec4> texture_map;
};

struct Packet {
	float u, v;
	std::shared_ptr<Material> material;
	vec4 color;
	vec4 normal;
	vec4 last_hit;
	vec4 next_dir;
};

class Hittable {
public:
	virtual bool hit(const Ray& ray, Packet& result) const = 0;
	virtual Bound getBound() const = 0;
};