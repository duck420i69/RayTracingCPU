#include "ray.h"
#include "object.h"
#include <iostream>

constexpr int DEPTH = 3;
constexpr float INVERT_PI = 1 / 3.14159265359;

vec4 BRDF(const vec4& dir, const vec4& outdir, const vec4& normal, std::shared_ptr<Texture> tex) {
	vec4 reflected_ray = reflect(dir, normal);
	return tex->diffuse_color * normal.dot(outdir) + vec4(0.3f, 0.3f, 0.3f, 1.0f) * pow(reflected_ray.dot(outdir), tex->specular_exp);
}

bool find_hit(const Scene& objects, const vec4& src, const vec4& dir, Packet& result) {
	float min = (float)1e+300;
	const Triangle* hitted_tri = nullptr;
	for (auto& object : objects.objects) {
		for (auto& box : object.collisionBox) {
			if (box.hit(dir, src)) {
				for (int i = box.start_index; i <= box.end_index; i++) {
					const auto& tri = object.mesh[i];
					if (tri.normal.dot(dir) > 0) continue;
					vec4 E1 = tri.b - tri.a;
					vec4 E2 = tri.c - tri.a;
					vec4 N;
					N = E1.cross(E2);
					float det = -dir.dot(N);
					if (-1e-6 <= det && det <= 1e-6) continue;
					float invdet = 1.0 / det;
					vec4 AO = src - tri.a;
					vec4 DAO;
					DAO = AO.cross(dir);
					float u = E2.dot(DAO) * invdet;
					float v = -E1.dot(DAO) * invdet;
					float t = AO.dot(N) * invdet;
					if (t < min && t > 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f) {
						result.u = u;
						result.v = v;
						if (object.texture == nullptr) result.color = tri.col;
						else if (object.texture->texture_map.size() == 0) 
							result.color = object.texture->diffuse_color;
						else {
							texel texcord = tri.ta + (tri.tb - tri.ta) * u + (tri.tc - tri.ta) * v;
							while (texcord.x > 1.0f) texcord.x -= 1.0f;
							while (texcord.x < 0.0f) texcord.x += 1.0f;
							while (texcord.y > 1.0f) texcord.y -= 1.0f;
							while (texcord.y < 0.0f) texcord.y += 1.0f;
							texcord.x = (texcord.x * object.texture->width) - 0.5f;
							texcord.y = (1.0f - texcord.y) * object.texture->height - 0.5f;
							result.color = object.texture->texture_map[static_cast<int>(texcord.x) + static_cast<int>(texcord.y) * object.texture->width];
						}
						result.texture = object.texture;
						result.normal = tri.normal;
						result.next_dir = reflect(dir, tri.normal);
						result.last_hit = tri.a + E1 * result.u + E2 * result.v;
						min = t;
						hitted_tri = &tri;
					}
				}
			}
		}
	}

	return hitted_tri != nullptr;
}

vec4 trace(const Scene& objects, const vec4& src, const vec4& dir) {
	vec4 col = { 1.0f, 1.0f, 1.0f, 1.0f };
	vec4 iter_src = src;
	vec4 iter_dir = dir;
	for (int i = 0; i < DEPTH; i++) {
		Packet packet{};
		if (!find_hit(objects, iter_src, iter_dir, packet))
			if (i > 2)
				if (iter_dir.v(1) > 0) return { 2.0f * col.v(0) * iter_dir.v(1), 2.0f * col.v(1) * iter_dir.v(1), 2.0f * col.v(2) * iter_dir.v(1), col.w};
				else return { col.v(0) * 0.1f, col.v(1) * 0.1f, col.v(2) * 0.1f, col.w };
			else return { col.v(0) * 0.52734375f, col.v(1) * 0.8046875f, col.v(2) * 0.94796875f, col.w };
		else {
			vec4 global_light;
			for (int i = 0; i < 3; i++) global_light.v(i) = packet.color.v(i) * BRDF(iter_dir, packet.next_dir, packet.normal, packet.texture).v(i);
			if (packet.normal.dot(vec4(0.0f, 1.0f, 0.0f, 1.0f)) > 0)
			if (!find_hit(objects, iter_src, { 0.0f, 1.0f, 0.0f, 1.0f }, packet))
			{
				for (int i = 0; i < 3; i++) global_light.v(i) += packet.color.v(i) * BRDF(iter_dir, {0.0f, 1.0f, 0.0f, 1.0f}, packet.normal, packet.texture).v(i) * 0.5f;
			}
			for (int i = 0; i < 3; i++) col.v(i) *= global_light.v(i);

			iter_src = packet.last_hit;
			iter_dir = packet.next_dir;
		}
	}
	return col * 0.1f;
}

