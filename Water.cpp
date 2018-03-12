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
	this->v = 0.0f;

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
				p->prevPos = vec3(x, y, z);
				p->density = this->density;
				this->particles.push_back(p);
			}
		}
	}

	//set up the box for collision detection and replusion forces
	Plane* ground = new Plane(vec3(0.0f,-2.0f,0.0f), vec3(0.0f, 1.0f, 0.0f));
	Plane* leftWall = new Plane(vec3(-2.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
	Plane* rightWall = new Plane(vec3(2.0f, 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f));
	Plane* frontWall = new Plane(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, -1.0f));
	Plane* backWall = new Plane(vec3(0.0f, 0.0f, -2.0f), vec3(1.0f, 0.0f, 1.0f));
	this->walls.push_back(ground);
	this->walls.push_back(leftWall);
	this->walls.push_back(rightWall);
	this->walls.push_back(frontWall);
	this->walls.push_back(backWall);

}

void Water::update(float dt)
{
	this->updateGrid();
	for (SPHParticle* p : this->particles) {
		this->findNeightbors(p);
	}
	this->computeDensityAndPressure();
	this->computeForces();

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
		
		//loop through 27 cells
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
		vec3 pressure = vec3(0.0f);
		vec3 viscosity = vec3(0.0f);
		for (SPHParticle*n : p->neighbors) {
			vec3 dist = n->center - p->center;
			vec3 vDiff = p->velocity - n->velocity;
			float d = length(dist);
			vec3 Wgrad = this->W(d)*normalize(dist);

			//calculate pressure
			pressure += (n->mass)*((p->pressure / (pow(p->density, 2))) + (n->pressure / (pow((n->density), 2))))*Wgrad;
			
			//calculate viscosity
			float temp = dot(dist,Wgrad) / (dot(dist, dist) + 0.01f*pow(smooth, 2));
			viscosity += (n->mass / n->density)*vDiff*temp;
		}
		pressure = -1.0f*p->mass*pressure;
		viscosity = p->mass*this->v*2.0f*viscosity;

		//apply all three forces
		p->applyForce(pressure);
		p->applyForce(viscosity);
		vec3 gravity = p->mass*vec3(0.0f, -9.8f, 0.0f);
		p->applyForce(gravity);
	}

}

void Water::collisionDetection()
{
	float elasticity = 0.0f;
	float friction = 0.0f;
	for (SPHParticle* p : particles)
	{
		for (Plane* wall : walls)
		{
			segment s;
			intersection inter;
			s.A = p->prevPos;
			s.B = p->center;
			if (wall->TestSegment(s, inter)) {
				vec3 v = p->velocity;
				vec3 position = p->center;
				//ground plane
				if (wall->normal == vec3(0.0f, 1.0f, 0.0f)) {
					position.y = 2.0f*wall->point.y - position.y;
					p->center = position;
					p->velocity.x = (1.0f - friction)*p->velocity.x;
					p->velocity.z = (1.0f - friction)*p->velocity.z;
				}
				//left wall
				else if (wall->normal == vec3(1.0f, 0.0f, 0.0f)) {
					position.x = 2.0f*wall->point.x - position.x;
					p->center = position;
					p->velocity.y = (1.0f - friction)*p->velocity.y;
					p->velocity.z = (1.0f - friction)*p->velocity.z;
				}
				//right wall
				else if (wall->normal == vec3(-1.0f, 0.0f, 0.0f)) {
					position.x = 2.0f*wall->point.x - position.x;
					p->center = position;
					p->velocity.y = (1.0f - friction)*p->velocity.y;
					p->velocity.z = (1.0f - friction)*p->velocity.z;
				}
				//front wall
				else if (wall->normal == vec3(0.0f, 0.0f, -1.0f)) {
					position.z = 2.0f*wall->point.z - position.z;
					p->center = position;
					p->velocity.x = (1.0f - friction)*p->velocity.x;
					p->velocity.y = (1.0f - friction)*p->velocity.y;
				}
				//back wall
				else if (wall->normal == vec3(-1.0f, 0.0f, 0.0f)) {
					position.z = 2.0f*wall->point.z - position.z;
					p->center = position;
					p->velocity.x = (1.0f - friction)*p->velocity.x;
					p->velocity.y = (1.0f - friction)*p->velocity.y;
				}
			}
			p->prevPos = p->center;
		}
	}

}


Water::~Water()
{
}
