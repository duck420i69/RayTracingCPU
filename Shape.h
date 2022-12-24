#pragma once

#include "Sphere.h"
#include "Mesh.h"

struct Scene {
	vec4 global_light_dir;
	BVHLinear tree;
	std::vector<std::shared_ptr<Hittable>> objects;
	std::vector<std::shared_ptr<Material>> materials;
};