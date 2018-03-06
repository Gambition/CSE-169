#pragma once
#include "Core.h"
using namespace glm;
using namespace std;
class Particle
{
public:
	float mass;
	vec3 pos;
	vec3 prevPos;
	vec3 velocity;
	vec3 force;
	vec3 color;
	vec3 normal;
	int index;
	bool fixed;

	Particle();
	~Particle();
	void update(float deltaTime);
	void reset();
	void applyForce(vec3 f);
};
