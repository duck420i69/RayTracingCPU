#include "ray.h"
#include "object.h"
#include <iostream>

constexpr int DEPTH = 8;
constexpr float INVERT_PI = 1 / 3.14159265359;

float BRDF(const vec4& dir, const vec4& outdir, const Triangle& tri) {
	vec4 reflected_ray = reflect(dir, tri.normal);
	return INVERT_PI * outdir.dot(reflected_ray);
}

bool find_hit(const Mesh& testTri, const vec4& src, const vec4& dir, Packet& result) {
	float min = (float)1e+300;
	const Triangle* hitted_tri = nullptr;
	for (auto& tri : testTri) {
		if (tri.normal.dot(dir) > 0) continue;
		vec4 E1 = tri.b - tri.a;
		vec4 E2 = tri.c - tri.a;
		vec4 N = E1.cross(E2);
		float det = -dir.dot(N);
		if (-1e-6 <= det && det <= 1e-6) continue;
		float invdet = 1.0 / det;
		vec4 AO = src - tri.a;
		vec4 DAO = AO.cross(dir);
		float u = E2.dot(DAO) * invdet;
		float v = -E1.dot(DAO) * invdet;
		float t = AO.dot(N) * invdet;
		if (t < min && t >= 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f) {
			result.u = u;
			result.v = v;
			result.next_dir = reflect(dir, tri.normal);
			result.last_hit = tri.a + E1 * result.u + E2 * result.v;
			result.col = tri.col * 1.5f * BRDF(dir, result.next_dir, tri);
			min = t;
			hitted_tri = &tri;
		}
	}
	return hitted_tri != nullptr;
}

vec4 trace(const Mesh& testTri, const vec4& src, const vec4& dir) {
	vec4 col = { 1.0f, 1.0f, 1.0f, 1.0f };
	vec4 iter_src = src;
	vec4 iter_dir = dir;
	for (int i = 0; i < DEPTH; i++) {
		Packet packet{};
		if (!find_hit(testTri, iter_src, iter_dir, packet))
			if (i != 0)
				if (iter_dir.y > 0) return { col.x * iter_dir.y, col.y * iter_dir.y, col.z * iter_dir.y, col.w };
				else return { col.x * 0.1f, col.y * 0.1f, col.z * 0.1f, col.w };
			else return { col.x * 0.52734375f, col.y * 0.8046875f, col.z * 0.91796875f, col.w };
		else {
			col = { col.x * packet.col.x, col.y * packet.col.y, col.z * packet.col.z, col.w };
			if (iter_dir.dot({ 0.0f, 1.0f, 0.0f, 1.0f }) < 0)
			if (find_hit(testTri, iter_src, { 0.0f, 1.0f, 0.0f, 1.0f }, packet))
			{
				col.x *= 1 + INVERT_PI;
				col.y *= 1 + INVERT_PI;
				col.z *= 1 + INVERT_PI;
			}

			iter_src = packet.last_hit;
			iter_dir = packet.next_dir;
		}
	}
	return col;
}

