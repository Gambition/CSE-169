#pragma once
#include "Core.h"
#include "Shader.h"

using namespace glm;
using namespace std;
class SPHParticle
{
public:
	float mass;
	float support;
	float smooth;
	float radius;
	vec3 init_pos;
	vec3 center;
	vec3 prevPos;
	vec3 velocity;
	vec3 force;

	float pressure;
	float density;
	int cellIndex;
	mat4 model;
	GLuint VAO, VBO, NBO,EBO;

	std::vector<SPHParticle*> neighbors;
	SPHParticle();
	void update(float deltaTime);
	void applyForce(vec3 f);
	void reset();
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<GLuint> indices;
	void setupSphere(int rings, int sectors);
	void setupMesh();
	void render(Shader shader,mat4 view, glm::mat4 proj);
	~SPHParticle();
	
};

