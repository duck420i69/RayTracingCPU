#pragma once

#include <memory>
#include <vector>
#include "math.h"

struct texel {
	float u, v;
};

struct Triangle {
	vec4 a, b, c, normal;
	texel ta, tb, tc;
	vec4 col;
	Triangle();
	Triangle(vec4 p1, vec4 p2, vec4 p3);
	Triangle(vec4 p1, vec4 p2, vec4 p3, vec4 color);
};

using Mesh = std::vector<Triangle>;

using Texture = std::vector<std::vector<vec4>>;

struct Object {
	Mesh mesh;
	std::shared_ptr<Texture> texture;
};
