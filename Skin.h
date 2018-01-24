#pragma once
#include "Core.h"
using namespace glm;
using namespace std;

struct Vertex {
	vec3 position;
	vec3 normal;
	int numAttachments;
	vector<mat4*> bindings;
	Vertex(vec3 pos)
	{
		position = pos;
	}

};

struct Face {
	vector<Vertex*> vertices;
	vec3 normal;
	
};
class Skin
{
private:
	vector<Vertex*> vertices;
	vector<Face*> faces;
	vector<mat4> Bindings;


public:
	Skin();
	~Skin();
	void load(const char* filename);

};

