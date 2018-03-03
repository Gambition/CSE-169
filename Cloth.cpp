#include "Cloth.h"



Cloth::Cloth()
{
}

Cloth::Cloth(int w, int h)
{
	this->width = w;
	this->height = h;

	//initialize particle sizes
	this->particles.resize(h);
	for (int i = 0; i < h; i++)
	{
		particles[i].resize(w);
	}
}


void Cloth::initialize()
{
}


void Cloth::update()
{

}

void Cloth::setupMesh()
{

}

void Cloth::render(Shader & shader, glm::mat4 view, glm::mat4 proj)
{
}


Cloth::~Cloth()
{
}
