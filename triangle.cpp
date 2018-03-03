#include "triangle.h"


triangle::triangle()
{
}

triangle::triangle(Particle * p1, Particle * p2, Particle * p3)
{
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	computeNormal();
}

void triangle::computeNormal()
{
	vec3 n = cross((p1->pos - p2->pos), (p2->pos-p3->pos));
	this->normal = normalize(n);
}

void triangle::calculateForce(vec3 airVelocity)
{
	//calculate velocity
	vec3 velocity = (p1->velocity + p2->velocity + p3->velocity) / 3.0f;
	vec3 v = velocity - airVelocity;

	//calcuate area
	vec3 n = cross((p1->pos - p2->pos), (p2->pos - p3->pos));
	float a = 0.5f*length(n);
	a = a * dot(v, normal) / length(v);

	//calulate areodynamic force
	vec3 force = -0.5f*density*pow(length(v), 2)*drag*a*normal;
	vec3 forceSplit = force / 3.0f;

	//apply force for each particle
	p1->applyForce(forceSplit);
	p2->applyForce(forceSplit);
	p3->applyForce(forceSplit);
}

triangle::~triangle()
{

}

