#pragma once

#include "Shape.h"
#include "Sampling.h"

bool find_hit(const Scene& objects, const Ray& ray, Packet& result);

vec4 trace(const Scene& objects, const Ray& ray, const int& depth, bool debug);