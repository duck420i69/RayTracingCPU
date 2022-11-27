#include "Camera.h"
#include <iostream>


vec4 shootRay(const Mesh& mesh, const Camera& cam, std::vector<vec4>& buffer, int current) {
	vec4 current_ray = cam.topleft + (cam.down * random() + cam.right * random());
	current_ray = current_ray + cam.down * (cam.resolution.y - (current / (int)cam.resolution.x)) + cam.right * (current % (int)cam.resolution.x);
	current_ray.normalize();
	return trace(mesh, cam.pos, current_ray);
}