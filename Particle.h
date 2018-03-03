#pragma once
#include "Core.h"

using namespace glm;
class Particle
{
public:
	float mass;
	vec3 position;
	vec3 velocity;
	vec3 force;
	vec3 color;

	Particle();
	~Particle();

	void update(float dt);
	void draw();
	void applyForce(vec3 &f);
};

