#pragma once

#include "ray.h"

constexpr float DEGREE_TO_RADIAN = 3.14159265359 / 180;

struct Camera {
	vec4 pos;
	vec4 dir;
	vec2 resolution;

	float fov;
	float aspect_ratio;
	float rot;

	vec4 topleft;
	vec4 right;
	vec4 down;

	Camera() {}

	Camera(vec4 position, vec4 direction, vec2 resolution1, float fov1) {
		pos = position;
		dir = direction;
		resolution = resolution1;
		aspect_ratio = resolution.x / (float)resolution.y;
		fov = fov1;
		updateCamera(pos, dir);
	}

	void updateCamera(vec4 pos, vec4 dir) {
		vec4 left = { dir.v(2), 0.0f, -dir.v(0), 0.0f};
		left.normalize();
		left = left * tanf(fov * DEGREE_TO_RADIAN);
		vec4 top(0.0f, 0.0f, 0.0f, 1.0f);
		top = dir.cross(left);
		left = left * aspect_ratio;
		topleft = dir + top + left;
		right = left * (-2 / resolution.x);
		down = top * (-2 / resolution.y);
	}
};

vec4 shootRay(const Scene& object, const Camera& cam, std::vector<vec4>& buffer, int current);