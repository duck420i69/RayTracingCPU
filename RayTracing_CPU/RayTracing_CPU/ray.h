#pragma once

#include "object.h"

struct Packet {
	float u, v;
	vec4 col;
	vec4 nor;
	vec4 last_hit;
	vec4 next_dir;
};

bool find_hit(const Mesh& testTri, const vec4& src, const vec4& dir, Packet& result);

vec4 trace(const Mesh& testTri, const vec4& src, const vec4& dir);
