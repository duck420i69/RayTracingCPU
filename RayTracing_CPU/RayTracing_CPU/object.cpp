#include "object.h"

Triangle::Triangle() {}

Triangle::Triangle(vec4 p1, vec4 p2, vec4 p3) : a(p1), b(p2), c(p3) {
	col = { 1.0f, 1.0f, 1.0f, 1.0f };
	normal = (b - a).cross(c - a);
	normal.normalize();
}

Triangle::Triangle(vec4 p1, vec4 p2, vec4 p3, vec4 color) : a(p1), b(p2), c(p3), col(color) {
	normal = (b - a).cross(c - a);
	normal.normalize();
}