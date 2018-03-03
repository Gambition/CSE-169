#include "ParticleSystem.h"



ParticleSystem::ParticleSystem()
{
}

void ParticleSystem::initialize()
{
}

void ParticleSystem::update(float dt)
{
	//apply gravity
	vec3 gravity = vec3(0.0f, -9.8f, 0.0f);
	for (int i = 0; i < numParticles; i++)
	{
		vec3 force = gravity * particles[i]->mass;
		particles[i]->applyForce(gravity);
	}


}


ParticleSystem::~ParticleSystem()
{
}
