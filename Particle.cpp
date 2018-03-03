#include "Particle.h"
Particle::Particle()
{
	mass = 1.0f;
	position = vec3(0.0f);
	velocity = vec3(0.0f);
	force = vec3(0.0f);
}


Particle::~Particle()
{

}

void Particle::update(float dt)
{
	vec3 acc = (1.0f / mass)*force;
	
	velocity += acc * dt;
	position += velocity * dt;

	force = vec3(0.0f);

}

void Particle::draw()
{
	
}

void Particle::applyForce(vec3 & f)
{
	force += f;
}
