#include "Renderer.h"
#include <iostream>

constexpr float INVERT_PI = 1 / 3.14159265359;

constexpr float cosThetaMaxSun = 0.999f;

constexpr float LIGHT_INTENSITY = 0.15f / (1 - cosThetaMaxSun);

constexpr float NEXT_RAY_SAMPLING_PROPABILITY = 0.9f;

constexpr float CONE_PDF = 2 * EIGEN_PI * (1 - cosThetaMaxSun);

constexpr float HEMISPHERE_PDF = 2 * EIGEN_PI - CONE_PDF;

const vec4 SKY_BLUE = { 0.52734375f, 0.8046875f, 0.94796875f, 1.0f };


vec4 BRDF(const vec4& dir, const vec4& outdir, const vec4& normal, std::shared_ptr<Material> material) {
	vec4 reflected_ray = reflect(dir, normal);
	if (!material) return vec4(INVERT_PI, INVERT_PI, INVERT_PI, 1.0f);
	return material->diffuse_color * INVERT_PI + material->specular_color * pow(std::max(reflected_ray.dot(outdir), 0.0f), material->specular_exp);
}

bool find_hit(const Scene& objects, const Ray& ray, Packet& result) {
	vec4 invertRayDir = { 1 / ray.d.v(0), 1 / ray.d.v(1), 1 / ray.d.v(2), 1.0f };
	uint32_t iter = 0, stack_size = 0;
	uint32_t stack[64];

	auto& bvh = objects.tree.bvh;

	bool hitted = false;

	while (true) {
		if (bvh[iter].bound.hit(ray.o, invertRayDir, ray.tMax)) {
			if (bvh[iter].next_index == 0) {
				auto& obj = objects.objects[bvh[iter].obj_index];
				if (obj->hit(ray, result)) hitted = true;
				if (stack_size < 1) break;
				iter = stack[--stack_size];
			}
			else {
				if (invertRayDir.v(0) > 0) {
					stack[stack_size++] = bvh[iter++].next_index;
				}
				else {
					stack[stack_size++] = iter + 1;
					iter = bvh[iter].next_index;
				}
			}
		}
		else {
			if (stack_size < 1) break;
			iter = stack[--stack_size];
		}
	}
	if (result.normal.dot(objects.global_light_dir) > -1 + cosThetaMaxSun) {
		if (random0to1() < NEXT_RAY_SAMPLING_PROPABILITY) {
			do {
				result.next_dir = random_in_sphere();
				if (result.next_dir.dot(result.normal) < 0) result.next_dir = result.next_dir * -1.0f;
			} while (result.next_dir.dot(objects.global_light_dir) > cosThetaMaxSun);
			result.color = result.color * (HEMISPHERE_PDF / NEXT_RAY_SAMPLING_PROPABILITY);
		}
		else {
			result.next_dir = random_in_cone(cosThetaMaxSun, objects.global_light_dir);
			if (result.next_dir.dot(result.normal) < 0) result.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			result.color = result.color * (CONE_PDF / (1 - NEXT_RAY_SAMPLING_PROPABILITY));
		}
	}
	else {
		result.next_dir = random_in_sphere();
		if (result.next_dir.dot(result.normal) < 0) result.next_dir = result.next_dir * -1.0f;
		result.color = result.color * (2 * EIGEN_PI);
	}
	return hitted;
}

vec4 trace(const Scene& scene, const Ray& ray, const int& depth, bool debug) {
	vec4 col = { 1.0f, 1.0f, 1.0f, 1.0f };
	vec4 iter_src = ray.o;
	vec4 iter_dir = ray.d;

	for (int i = 0; i < depth; i++) {
		Packet packet{};
		if (debug) std::cout << "Bounce: " << i + 1 << "\n\n";
		if (!find_hit(scene, Ray(iter_src, iter_dir), packet)) {
			if (debug) std::cout << "Hit the sky \n\n";
			if (iter_dir.dot(scene.global_light_dir) < cosThetaMaxSun) {
				vec4 result = col;
				return result * SKY_BLUE;
			}
			vec4 result = col * iter_dir.dot(scene.global_light_dir) * LIGHT_INTENSITY;
			return result;
		}
		else {
			if (debug) {
				std::cout << "Hit pos: " << packet.last_hit.v(0) << ", " << packet.last_hit.v(1) << ", " << packet.last_hit.v(2) << "\n";
				std::cout << "Incoming ray: " << iter_dir.v(0) << ", " << iter_dir.v(1) << ", " << iter_dir.v(2) << "\n";
				std::cout << "Normal: " << packet.normal.v(0) << ", " << packet.normal.v(1) << ", " << packet.normal.v(2) << "\n";
				std::cout << "Next ray: " << packet.next_dir.v(0) << ", " << packet.next_dir.v(1) << ", " << packet.next_dir.v(2) << "\n";
				std::cout << "Reflect: " << std::max(reflect(iter_dir, packet.normal).dot(packet.next_dir), 0.0f) << "\n";
			}

			col = col * packet.color * BRDF(iter_dir, packet.next_dir, packet.normal, packet.material) * packet.next_dir.dot(packet.normal);

			if (debug) std::cout << "Current ray color: " << col.v(0) << " " << col.v(1) << " " << col.v(2) << "\n\n";

			iter_src = packet.last_hit + packet.normal * 0.0006f;
			iter_dir = packet.next_dir;
		}
	}
	//while (true) {
	//	float p = col.dot(col) * 0.3333333f;
	//	if (p < 0.8f || random0to1() > p) break;
	//	float invert_p = 1 / p;
	//	Packet packet{};
	//	if (!find_hit(scene, Ray(iter_src, iter_dir), packet)) {
	//		if (debug) std::cout << "Hit the sky \n\n";
	//		if (iter_dir.dot(scene.global_light_dir) < 0.9f) {
	//			vec4 result = col * invert_p;
	//			return result * SKY_BLUE;
	//		}
	//		vec4 result = col * invert_p * iter_dir.dot(scene.global_light_dir) * LIGHT_INTENSITY;
	//		return result;
	//	}
	//	else {
	//		if (debug) {
	//			std::cout << "Hit pos: " << packet.last_hit.v(0) << ", " << packet.last_hit.v(1) << ", " << packet.last_hit.v(2) << "\n";
	//			std::cout << "Incoming ray: " << iter_dir.v(0) << ", " << iter_dir.v(1) << ", " << iter_dir.v(2) << "\n";
	//			std::cout << "Normal: " << packet.normal.v(0) << ", " << packet.normal.v(1) << ", " << packet.normal.v(2) << "\n";
	//			std::cout << "Next ray: " << packet.next_dir.v(0) << ", " << packet.next_dir.v(1) << ", " << packet.next_dir.v(2) << "\n";
	//			std::cout << "Reflect: " << std::max(reflect(iter_dir, packet.normal).dot(packet.next_dir), 0.0f) << "\n";
	//		}

	//		col = col * invert_p * packet.color * BRDF(iter_dir, packet.next_dir, packet.normal, packet.material);

	//		if (debug) std::cout << "Current ray color: " << col.v(0) << " " << col.v(1) << " " << col.v(2) << "\n\n";

	//		iter_src = packet.last_hit + packet.normal * 0.0006f;
	//		iter_dir = packet.next_dir;
	//	}
	//}
	vec4 result = { 0.06f * col.v(0), 0.06f * col.v(1), 0.06f * col.v(2), col.w };
	return result;
}