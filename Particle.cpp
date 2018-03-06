#include "Particle.h"

Particle::Particle()
{
	mass = 0.5f;
	pos = vec3(0.0f);
	velocity = vec3(0.0f);
	normal = vec3(0.0f);
	force = vec3(0.0f);
	fixed = false;
}

void Particle::update(float deltaTime)
{
	if (fixed == true)
	{
		return;
	}
	vec3 acc = (1.0f / mass)*force;

	velocity += acc * deltaTime;
	pos += velocity * deltaTime;

	force = vec3(0.0f);
}

void Particle::reset()
{
	this->force = vec3(0.0f);
	this->velocity = vec3(0.0f);
}



void Particle::applyForce(vec3 f)
{
	this->force += f;
}


Particle::~Particle()
{
}
