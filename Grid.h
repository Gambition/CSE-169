#pragma once
#include "Core.h"
#include "SPHParticle.h"

class Grid
{
public:
	int xcells;
	int ycells;
	int zcells;
	int size;
	vector<vector<SPHParticle*>> entries;
	void addParticles(SPHParticle*, float x,float y,float z);
	void empty();
	Grid();
	Grid(int size, float sR);
	~Grid();
};

