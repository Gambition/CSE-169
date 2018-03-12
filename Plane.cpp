#include "Plane.h"


using namespace std;
Plane::Plane()
{
	point = vec3(0.0f);
	normal = vec3(0.0f, 1.0f, 0.0f);

}

Plane::Plane(vec3 p)
{
	point = p;
	normal = vec3(0.0f, 1.0f, 0.0f);
}

Plane::Plane(vec3 p, vec3 n) 
{
	point = p;
	normal = n;
}

bool Plane::TestSegment(segment & s, intersection & i)
{
	float da = dot((s.A - point), normal);
	float db = dot((s.B - point), normal);
	if (db > 0)
	{
		return false;
	}
	vec3 interPoint = (da*s.B - db * s.A) / (da - db);
	i.pos = interPoint;
	i.normal = s.B - point;
}


Plane::~Plane()
{
}
