#pragma once
#include "SPHParticle.h"
#include "Grid.h"
using namespace std;

class Water
{
public:

	Grid* grid;
	float support = 0.04f;
	float smooth = 0.02f;
	float space = 0.01f;
	float density;
	float pressure;
	float k;
	float restDensity;
	void initialize();
	void update(float dt);
	void render();
	void findNeightbors(SPHParticle* p);
	float W(float q);
	float f(float q);
	void computeDensityAndPressure();
	void computeForces();
	vector<SPHParticle*> particles;
	int numParticles;

	Water();
	Water(int n);
	void updateGrid();
	~Water();
};
