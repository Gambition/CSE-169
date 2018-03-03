#pragma once
#include "Core.h"
#include "SpringDamper.h"
#include "triangle.h"
#include "Shader.h"

class Cloth
{
public:
	int width, height;
	float ratio;
	std::vector<std::vector<Particle*>> particles;
	std::vector<SpringDamper*> springDampers;
	std::vector<triangle*> triangles;
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<vec3> faces;

	Cloth();
	Cloth(int w, int h);
	void initialize();
	void update();
	void setupMesh();
	void render(Shader & shader, glm::mat4 view, glm::mat4 proj);
	~Cloth();
};

