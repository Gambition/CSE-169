#include "Grid.h"



void Grid::addParticles(SPHParticle *particle,float x,float y,float z)
{
	int cellNumber = x + y * xcells + z * xcells*ycells;
	entries[cellNumber].push_back(particle);
	
}

void Grid::empty()
{
	for (int i = 0; i < entries.size(); i++)
	{
		this->entries[i].clear();
	}
	
}

Grid::Grid()
{
	xcells = 0;
	ycells = 0;
	zcells = 0;
}

Grid::Grid(float size,float sR)
{
	this->domain = size;
	this->halfDomain = domain * 0.5f;
	this->numcell = size / sR;
	this->xcells = numcell;
	this->ycells = numcell;
	this->zcells = numcell;
	
	entries.resize(xcells*ycells*zcells);
	
}

Grid::~Grid()
{
}
