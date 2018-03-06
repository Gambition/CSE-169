#include "SpringDamper.h"

SpringDamper::SpringDamper()
{
	p1 = NULL;
	p2 = NULL;
	springConstant = 200.f;
	dampingFactor = 40.0f;
	restLength = 0.25f;
}

SpringDamper::SpringDamper(Particle * p1, Particle * p2)
{
	this->p1 = p1;
	this->p2 = p2;
	springConstant = 200.f;
	dampingFactor = 40.0f;
	restLength = 0.25f;
}

void SpringDamper::computeForce()
{
	vec3 e = p2->pos - p1->pos;
	float l = length(e);
	if (l != 0.0f) {
		e = normalize(e);
	}
	float v1 = dot(p1->velocity,e);
	float v2 = dot(p2->velocity,e);

	float fsd = -1.0f*springConstant*(restLength - l) - dampingFactor * (v1 - v2);
	vec3 f1 = fsd * e;
	p1->applyForce(f1);
	p2->applyForce(-1.0f*f1);
	
}

SpringDamper::~SpringDamper()
{

}
