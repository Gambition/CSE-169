#pragma once
#include "Core.h"
using namespace glm;
struct segment {
	vec3 A, B;
};

struct intersection {
	vec3 pos;
	vec3 normal;
};
class Plane
{
public:
	vec3 point;
	vec3 normal;

	Plane();
	Plane(vec3 p);
	bool TestSegment(segment &s, intersection &i);
	~Plane();
};

