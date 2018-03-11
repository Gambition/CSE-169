#include "Water.h"

Water::Water()
{
}


Water::Water(int n)
{
	this->numParticles = n;
	this->density = 0.0f;
	this->pressure = 0.0f;
	this->grid = new Grid(10, this->support);
	this->k = 0.0f;
	this->restDensity = 0.0f;

}

void Water::initialize()
{
	int numZ = 10;
	int numX = sqrt(numParticles / numZ);
	int numY = numX;
	float xStart = (0 - (numX) / 2.0f)*space;
	float yStart = (0 - (numY) / 2.0f)*space;
	float zStart = (0 - (numZ) / 2.0f)*space;

	float xEnd = -1.0f*xStart;
	float yEnd = -1.0f*yStart;
	float zEnd = -1.0f*zStart;

	for (float x = xStart; x < xEnd; x += space)
	{
		for (float y = yStart; y < yEnd; y += space)
		{
			for (float z = zStart; z < zEnd; z += space)
			{
				SPHParticle* p = new SPHParticle();
				p->center = vec3(x, y, z);
				p->density = this->density;
				this->particles.push_back(p);
			}
		}
	}
}

void Water::update(float dt)
{
}


void Water::updateGrid()
{
	this->grid->empty();

	for (SPHParticle* p : this->particles)
	{
		vec3 pos = p->center;
		//TODO: map particle position to grid position
		int gridX = floor((pos.x + (grid->size) / 2.0f) /support );
		int gridY = floor((pos.y + (grid->size) / 2.0f) / support);
		int gridZ = floor((pos.z + (grid->size) / 2.0f) / support);
		if (gridX < 0) {
			gridX = 0;
		}
		else if (gridX > grid->xcells) {
			gridX = grid->xcells - 1;
		}
		if (gridY < 0) {
			gridY = 0;
		}
		else if (gridY > grid->xcells) {
			gridY = grid->ycells - 1;
		}
		if (gridZ < 0) {
			gridZ = 0;
		}
		else if (gridZ > grid->xcells) {
			gridZ = grid->zcells - 1;
		}

		grid->addParticles(p, gridX, gridY, gridZ);
	}

}

void Water::render()
{
}

void Water::findNeightbors(SPHParticle * p)
{
	for (SPHParticle* p : this->particles)
	{
		vec3 pos = p->center;
		int gridX = floor((pos.x + (grid->size) / 2.0f) / support);
		int gridY = floor((pos.y + (grid->size) / 2.0f) / support);
		int gridZ = floor((pos.z + (grid->size) / 2.0f) / support);
		for (int i = gridX - 1; i < gridX + 1; i++)
		{
			if (i < 0) continue;
			if (i >= grid->xcells) break;
			for (int j = gridY - 1; j < gridY + 1; j++)
			{
				if (j < 0) continue;
				if (j >= grid->ycells) break;
				for (int k = gridZ - 1; k < gridZ + 1; k++) 
				{
					if (k < 0) continue;
					if (k >= grid->zcells) break;
					int entry = i + j * (grid->xcells) + k * (grid->xcells*grid->ycells);
					for (SPHParticle* n : grid->entries[entry]) {
						p->neighbors.push_back(n);
					}
				}
			}
		}
	}

}

float Water::W(float q)
{
	return this->f(q) / pow(smooth, 3);
}

float Water::f(float q)
{
	float result = 0.0f;
	if (q >= 0 && q < 1) {
		result = 0.6f - pow(q, 2) + 0.5*pow(q, 3);
	}
	else if (q >= 1 && q < 2) {
		result = pow((2 - q), 3) / 6.0f;
	}

	return (1.5f/pi<float>())*result;
}

void Water::computeDensityAndPressure()
{
	for (SPHParticle*p : this->particles) {
		p->density = 0.0f;
		p->pressure = 0.0f;
		float d = 0.0f;
		for (SPHParticle* n : p->neighbors) {
			float q = (length(p->center - n->center)) / smooth;
			d += (n->mass)*W(q);
		}
		p->density = d;
		p->pressure = k * (pow((p->density / restDensity), 7) - 1);
	}
}

void Water::computeForces()
{
	for (SPHParticle*p : this->particles) {
		for (SPHParticle*n : p->neighbors) {

		}
	}

}







Water::~Water()
{
}
