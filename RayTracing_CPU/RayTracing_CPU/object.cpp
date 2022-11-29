#include "object.h"

Triangle::Triangle() {}

Triangle::Triangle(vec4 p1, vec4 p2, vec4 p3) : a(p1), b(p2), c(p3) {
	col = { 1.0f, 1.0f, 1.0f, 1.0f };
	normal = (b - a).cross(c - a);
	normal.w = 1.0f;
	normal.normalize();
}

Triangle::Triangle(vec4 p1, vec4 p2, vec4 p3, vec4 col) : a(p1), b(p2), c(p3), col(col) {
	normal = (b - a).cross(c - a);
	normal.w = 1.0f;
	normal.normalize();
}

Triangle::Triangle(vec4 p1, vec4 p2, vec4 p3, texel t1, texel t2, texel t3) : a(p1), b(p2), c(p3), ta(t1), tb(t2), tc(t3) {
	col = { 1.0f, 1.0f, 1.0f, 1.0f };
	normal = (b - a).cross(c - a);
	normal.w = 1.0f;
	normal.normalize();
}