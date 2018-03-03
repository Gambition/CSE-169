#pragma once
#include "Core.h"
#include "Particle.h"

class SpringDamper
{
public:
	Particle* p1;
	Particle* p2;
	float springConstant;
	float dampingFactor;
	float restLength;


	SpringDamper();
	SpringDamper(Particle* p1, Particle* p2);
	void computeForce();
	~SpringDamper();
};

