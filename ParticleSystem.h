#pragma once
#include "Particle.h"
using namespace std;

class ParticleSystem
{
public:
	vector<Particle*> particles;
	int numParticles;

	ParticleSystem();

	void initialize();

	void update(float dt);


	~ParticleSystem();
};
            
