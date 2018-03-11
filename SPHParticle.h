#pragma once
#include "Core.h"
using namespace glm;
using namespace std;
class SPHParticle
{
public:
	float mass;
	float support;
	float smooth;
	vec3 center;
	vec3 prevPos;
	vec3 velocity;
	vec3 force;
	float pressure;
	float density;
	int cellIndex;
	std::vector<SPHParticle*> neighbors;
	SPHParticle();
	void update(float deltaTime);
	void applyForce(vec3 f);
	void draw();
	float kernel(float q);
	~SPHParticle();
};

