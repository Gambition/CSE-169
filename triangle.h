#pragma once
#include "Core.h"
#include "Particle.h"

class triangle
{
public:
	Particle* p1;
	Particle* p2;
	Particle* p3;
	vec3 normal;

	float density;
	float drag;
	


	triangle(Particle* p1, Particle* p2, Particle* p3);
	triangle();

	void computeNormal();
	void calculateForce(vec3 airVelocity);

	~triangle();
};

