#pragma once

#include "math.h"
#include <deque>


class Bound {
public:
	Bound() : min({ INFINITY, INFINITY, INFINITY, 1.0f }), max(-INFINITY, -INFINITY, -INFINITY, 1.0f) {};
	Bound(vec4 low_bound, vec4 high_bound) : min(low_bound), max(high_bound) {};
	inline bool hit(const vec4& origin, const vec4& invRayDir, const float& tMax) const {
		float t0 = 0.0f;
		float t1 = tMax;
		for (int i = 0; i < 3; i++) {
			float tNear = (min.v[i] - origin.v[i]) * invRayDir.v[i];
			float tFar = (max.v[i] - origin.v[i]) * invRayDir.v[i];
			if (tNear > tFar) std::swap(tNear, tFar);
			t0 = tNear > t0 ? tNear : t0;
			t1 = tFar < t1 ? tFar : t1;
			if (t0 > t1) return false;
		}
		return true;
	}

	inline float surfaceArea() const {
		float x = max.v[0] - min.v[0];
		float y = max.v[1] - min.v[1];
		float z = max.v[2] - min.v[2];
		return 2 * (x * y + y * z + x * z);
	}

	vec4 min;
	vec4 max;
};

inline Bound Union(const Bound& b1, const Bound& b2) {
	vec4 min, max;
	for (int i = 0; i < 3; i++) {
		min.v(i) = std::min(b1.min.v(i), b2.min.v(i));
		max.v(i) = std::max(b1.max.v(i), b2.max.v(i));
	}
	return Bound(min, max);
}

class BVHNode {
public:
	BVHNode() = default;
	BVHNode(Bound b, uint32_t index) : bound(b), obj_index(index) { centroid = (b.max + b.min) * 0.5f; }
	Bound bound;
	vec4 centroid;
	std::vector<std::unique_ptr<BVHNode>> child;
	float cost;
	uint32_t obj_index;
};

struct BVHLinearNode {
public:
	BVHLinearNode(Bound b, uint32_t obj) : bound(b), obj_index(obj), next_index(0) {}
	Bound bound;
	uint32_t next_index;
	uint32_t obj_index;
};

struct BVHLinear {
	std::vector<BVHLinearNode> bvh;
};

std::unique_ptr<BVHNode> constructBVH(std::deque<std::unique_ptr<BVHNode>> component);

void constructLinearBVH(const std::unique_ptr<BVHNode>& root, BVHLinear& arr, size_t& iter);