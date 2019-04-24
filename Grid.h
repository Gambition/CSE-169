#pragma once
#include "Core.h"
#include "SPHParticle.h"

class Grid
{
public:
	int xcells;
	int ycells;
	int zcells;
	int numcell;
	float halfDomain;
	float domain;
	vector<vector<SPHParticle*>> entries;
	void addParticles(SPHParticle*, float x,float y,float z);
	void empty();
	Grid();
	Grid(float size, float sR);
	~Grid();
};

