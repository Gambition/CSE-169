#include "Water.h"

Water::Water()	
{
}

Water::Water(int n)
{
	this->numParticles = n;
	this->density = 0.0f;
	this->pressure = 0.0f;
	this->grid = new Grid(20.0f, this->support);
	this->k = 1.0f;
	this->restDensity = 100.0f;
	this->v = 3.0f;
}

void Water::initialize()
{
	int numY = 7;
	int numX = sqrt(numParticles / numY);
	int numZ = numX;
	float xStart = (0 - (numX) / 2.0f) * space;
	float yStart = 5.0f;
	float zStart = (0 - (numZ) / 2.0f) * space;
	cout << yStart << endl;
	float xEnd = -1.0f * xStart;
	float yEnd = yStart+numY * space;
	float zEnd = -1.0f * zStart;

	for (float x = xStart; x < xEnd; x += space)
	{
		for (float y = yStart; y < yEnd; y += space)
		{
			for (float z = zStart; z < zEnd; z += space)
			{
				SPHParticle *p = new SPHParticle();
				p->center = vec3(x, y, z);
				p->init_pos = vec3(x, y, z);
				p->prevPos = vec3(x, y, z);
				p->density = this->density;
				p->setupSphere(12, 24);
				p->model = translate(p->model,p->center);
				this->particles.push_back(p);
			}
		}
	}
}

void Water::update(float dt)
{
	this->updateGrid();

	for (SPHParticle *p : this->particles)
	{
		this->findNeightbors(p);
	}
	
	this->computeDensityAndPressure();
	this->computeForces();

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->update(dt);
	}

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->neighbors.clear();
	}
}

void Water::setup()
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->setupMesh();
	}

}

void Water::updateGrid()
{
	this->grid->empty();

	for (SPHParticle *p : this->particles)
	{
		vec3 pos = p->center;
		//map particle position to grid position
		int gridX = floor((pos.x + grid->halfDomain) / support);
		int gridY = floor((pos.y + grid->halfDomain) / support);
		int gridZ = floor((pos.z + grid->halfDomain) / support);
	
		if (gridX < 0)
		{
			gridX = 0;
		}
		else if (gridX > grid->xcells)
		{
			gridX = grid->xcells - 1;
		}
		if (gridY < 0)
		{
			gridY = 0;
		}
		else if (gridY > grid->xcells)
		{
			gridY = grid->ycells - 1;
		}
		if (gridZ < 0)
		{
			gridZ = 0;
		}
		else if (gridZ > grid->xcells)
		{
			gridZ = grid->zcells - 1;
		}
		
		grid->addParticles(p, gridX, gridY, gridZ);
	}
}

void Water::render(mat4 view, mat4 proj)
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->render(shader, view, proj);
	}
}

void Water::findNeightbors(SPHParticle *p)
{
	vec3 pos = p->center;
	int gridX = floor((pos.x + grid->halfDomain) / support);
	int gridY = floor((pos.y + grid->halfDomain) / support);
	int gridZ = floor((pos.z + grid->halfDomain) / support);
	//loop through 27 cells
	for (int i = gridX - 1; i <= gridX + 1; i++)
	{
		if (i < 0)
			continue;
		if (i >= grid->xcells)
			break;
		for (int j = gridY - 1; j <= gridY + 1; j++)
		{
			if (j < 0)
				continue;
			if (j >= grid->ycells)
				break;
			for (int k = gridZ - 1; k <= gridZ + 1; k++)
			{
					
				if (k < 0)
					continue;
				if (k >= grid->zcells)
					break;
				int entry = i + j * (grid->xcells) + k * (grid->xcells * grid->ycells);
				p->neighbors.insert(p->neighbors.end(), grid->entries[entry].begin(), grid->entries[entry].end());
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
	if (q >= 0 && q < 1)
	{
		result = 0.6f - pow(q, 2) + 0.5 * pow(q, 3);
	}
	else if (q >= 1 && q < 2)
	{
		result = pow((2.0f - q), 3.0f) / 6.0f;
	}

	return (1.5f / pi<float>()) * result;
}

float Water::Wd(float q)
{
	return this->fd(q) / pow(smooth, 3);
}

float Water::fd(float q)
{
	float result = 0.0f;
	if (q >= 0 && q < 1)
	{
		result = -2.0f * q + 1.5f * pow(q, 2);
	}
	else if (q >= 1 && q < 2)
	{
		result = -0.5f * pow(2.0f - q, 2);
	}
	return (1.5f / pi<float>()) * result;
}

void Water::computeDensityAndPressure()
{
	for (SPHParticle *p : this->particles)
	{
		p->density = 0.0f;
		p->pressure = 0.0f;
		float d = restDensity;
	
		for (SPHParticle *n : p->neighbors)
		{
			float q = (length(n->center - p->center)) / smooth;
			d += (n->mass) * W(q);	
		}
		p->density = d;
	
		p->pressure = k * (pow((p->density / restDensity), 7.0f) - 1.0f);
		
		//cout << p->pressure << " " << p->density << endl;
	}
}

void Water::computeForces()
{
	for (SPHParticle *p : this->particles)
	{
		vec3 pressure = vec3(0.0f);
		vec3 viscosity = vec3(0.0f);
		for (SPHParticle *n : p->neighbors)
		{
			vec3 dist = p->center - n->center;
			vec3 vDiff = p->velocity - n->velocity;
			float d = length(dist)/smooth;
		
			vec3 Wgrad = this->Wd(d)*2.0f*Normalize(dist);
			//cout << Wgrad.x << " " << Wgrad.y<<" " << Wgrad.z << endl;
			//calculate pressure
			//cout << ((p->pressure / (pow(p->density, 2.0f))) + (n->pressure / (pow((n->density), 2.0f))))<< endl;
			pressure += (n->mass) * ((p->pressure / (pow(p->density, 2.0f))) + (n->pressure / (pow((n->density), 2.0f)))) * Wgrad;
			
			//calculate viscosity
			float temp = dot(dist, Wgrad) / (pow(length(dist),2.0f) + 0.01f * pow(smooth, 2.0f));

			//cout << (n->mass / n->density)*temp << endl;
			viscosity += ((n->mass / n->density) * temp)* vDiff;
			
		}
		
		pressure = -100000.0f * (p->mass) * pressure;
	
		//cout << pressure.x << " " << pressure.y << " " << pressure.z << endl;
		viscosity = p->mass * this->v * 2.0f * viscosity;
		//cout << viscosity.x << " " << viscosity.y << " " << viscosity.z << endl;

		//apply all three forces
		p->applyForce(pressure);
		p->applyForce(viscosity);
		vec3 gravity = p->mass * vec3(0.0f, -9.8f, 0.0f);
		p->applyForce(gravity);
	}
}

void Water::collisionDetection()
{
	float elasticity = 0.8f;
	float friction = 0.95f;
	for (SPHParticle *p : particles)
	{
		for (Plane *wall : walls)
		{
			segment s;
			intersection inter;
			s.A = p->prevPos;
			s.B = p->center;
			if (wall->TestSegment(s, inter))
			{
				vec3 v = p->velocity;
				vec3 position = p->center;
				//ground plane
				if (wall->normal == vec3(0.0f, 1.0f, 0.0f))
				{
					
					position.y = 2.0f * wall->point.y - position.y;
					p->center = position;
					p->velocity.x = (1.0f - friction) * p->velocity.x;
					p->velocity.z = (1.0f - friction) * p->velocity.z;
				}
				//left wall
				else if (wall->normal == vec3(1.0f, 0.0f, 0.0f))
				{
					position.x = 2.0f * wall->point.x - position.x;
					p->center = position;
					p->velocity.y = (1.0f - friction) * p->velocity.y;
					p->velocity.z = (1.0f - friction) * p->velocity.z;
				}
				//right wall
				else if (wall->normal == vec3(-1.0f, 0.0f, 0.0f))
				{
					position.x = 2.0f * wall->point.x - position.x;
					p->center = position;
					p->velocity.y = (1.0f - friction) * p->velocity.y;
					p->velocity.z = (1.0f - friction) * p->velocity.z;
				}
				//front wall
				else if (wall->normal == vec3(0.0f, 0.0f, -1.0f))
				{
					position.z = 2.0f * wall->point.z - position.z;
					p->center = position;
					p->velocity.x = (1.0f - friction) * p->velocity.x;
					p->velocity.y = (1.0f - friction) * p->velocity.y;
				}
				//back wall
				else if (wall->normal == vec3(-1.0f, 0.0f, 0.0f))
				{
					position.z = 2.0f * wall->point.z - position.z;
					p->center = position;
					p->velocity.x = (1.0f - friction) * p->velocity.x;
					p->velocity.y = (1.0f - friction) * p->velocity.y;
				}
			}
			//vec3 diff = p->center - p->prevPos;
			//p->model = translate(p->model, diff);
			p->prevPos = p->center;
		}
	}
}

void Water::reset()
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->reset();
	}
}

vec3 Water::Normalize(vec3 in)
{
	if (in.x == 0.0f && in.y == 0.0f &&in.z == 0.0f)
	{
		return vec3(0.0f);
	}
	else {
		return glm::normalize(in);
	}
}

Water::~Water()
{
}
