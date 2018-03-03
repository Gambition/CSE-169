#include "SpringDamper.h"




SpringDamper::SpringDamper()
{
}

SpringDamper::SpringDamper(Particle * p1, Particle * p2)
{
	this->p1 = p1;
	this->p2 = p2;
}

void SpringDamper::computeForce()
{
	vec3 e = p1->pos - p2->pos;
	float l = length(e);
	e = e / l;

	vec3 v1 = p1->velocity * e;
	vec3 v2 = p2->velocity * e;

	vec3 fsd = -1.0f*springConstant*(restLength - l) - dampingFactor * (v1 - v2);
	vec3 f1 = fsd * e;
	p1->applyForce(f1);
	p2->applyForce(-1.0f*f1);
	
}

SpringDamper::~SpringDamper()
{

}
