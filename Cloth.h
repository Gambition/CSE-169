#pragma once
#include "Core.h"
#include "SpringDamper.h"
#include "triangle.h"
#include "Shader.h"
#include "Plane.h"

class Cloth
{
public:
	int width, height;
	float ratio;
	float numParticles;
	float numIndices;
	vec3 Vair;
	std::vector<std::vector<Particle*>> particles;
	std::vector<SpringDamper*> springDampers;
	std::vector<triangle*> triangles;
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<GLuint> indices;
	GLuint VAO, VBO, NBO,EBO;
	mat4 model;
	Plane* ground;

	Cloth();
	Cloth(int w, int h);
	void initialize();
	void fixRow(int rowIndex);
	void fixColumn(int columnIndex);
	void calculateNormals();
	void update();
	void setupMesh();
	void adjust(vec3 move);
	void reset();
	void render(Shader shader,glm::mat4 view, glm::mat4 proj);
	~Cloth();
};