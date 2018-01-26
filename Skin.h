#pragma once
#include "Core.h"
#include "Tokenizer.h"
#include "Skeleton.h"

using namespace glm;
using namespace std;

struct Vertex {
	vec3 position;
	vec3 normal;
	int numAttachments;

	//store the joint number and corresponding weight
	vector<pair<int,float>> weights;
	
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
	Tokenizer* t;
	Skeleton* skel;
	

public:
	Skin();
	~Skin();
	void load(const char* filename);
	void update();
};

