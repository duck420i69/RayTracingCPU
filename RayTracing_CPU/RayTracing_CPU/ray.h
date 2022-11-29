#pragma once

#include "object.h"

struct Packet {
	float u, v;
	std::shared_ptr<Texture> texture;
	vec4 color;
	vec4 normal;
	vec4 last_hit;
	vec4 next_dir;
};


bool find_hit(const Scene& objects, const vec4& src, const vec4& dir, Packet& result);

vec4 trace(const Scene& objects, const vec4& src, const vec4& dir);
