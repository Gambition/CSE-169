#pragma once
#include "Core.h"
using namespace glm;

class Particle
{
public:
	float mass;
	vec3 pos;
	vec3 velocity;
	vec3 force;
	vec3 color;
	Particle();
	~Particle();
	void update(float deltaTime);
	void Draw();
	void applyForce(vec3 force);
};
