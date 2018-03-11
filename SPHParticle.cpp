#include "SPHParticle.h"

SPHParticle::SPHParticle()
{
	mass = 0.5f;
	support = 0.02f;
	smooth = 0.01;
	velocity = vec3(0.0f);
	force = vec3(0.0f);
	pressure = 0.0f;
	density = 0.0f;
	cellIndex = 0;
}

void SPHParticle::update(float deltaTime)
{
	vec3 accel = (1.0f / mass)*force;
	velocity += accel * deltaTime;
	center += velocity * deltaTime;

	force = vec3(0.0f);
}

void SPHParticle::applyForce(vec3 f)
{
	force += f;
}

void SPHParticle::draw()
{
}

float SPHParticle::kernel(float q)
{
	if (q >= 0 && q < 1)
	{
		return 0.6f - pow(q, 2) + 0.5f*pow(q, 3);
	}
	else if (q >= 1 && q < 2)
	{
		return pow((2 - q), 3) / 6.0f;
	}
	return 0.0f;
}


SPHParticle::~SPHParticle()
{

}
