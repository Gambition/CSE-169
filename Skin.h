#pragma once
#include "Core.h"
#include "Tokenizer.h"
#include "Skeleton.h"
#include "Shader.h"
#include "BMPImage.h"
using namespace glm;
using namespace std;

struct Vertex {
	vec3 position;
	vec3 normal;
	int numAttachments;

	//store the joint number and corresponding weight
	vector<pair<int,float>> weights;
	
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
	vector<mat4> inverseBindings;
	Tokenizer* t;
	GLuint VAO, VBO, NBO, EBO,TBO;
	vector<vec3> vtPos;
	vector<vec3> vtNorm;
	vector<GLuint> indices;
	vector<vec2> texCoords;


public:
	Skin();
	~Skin();
	void load(const char* filename);
	void update(Skeleton* skel);
	void setupMesh();
	void Draw(Shader &shader, glm::mat4 view, glm::mat4 proj);
	void LoadGlTextures();
	glm::mat4 model = glm::mat4(1.0f);
	GLuint texture;
	BMPImage image1;
	int hasTexture = 0;
};

