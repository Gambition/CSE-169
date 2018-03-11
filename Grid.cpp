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

Grid::Grid(int size,float sR)
{
	this->xcells = size;
	this->ycells = size;
	this->zcells = size;

	entries.reserve(xcells*ycells*zcells);
}



Grid::~Grid()
{
}