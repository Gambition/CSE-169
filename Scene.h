#pragma once
#include "Cloth.h"
#include "Shader.h"

class Scene
{
	
public:
	float clothWidth;
	float clothHeight;
	Cloth * myCloth;
	Scene();
	Scene(int width,int height);
	void render(const glm::mat4 view, glm::mat4 proj);
	~Scene();
	Shader shader = Shader("shader.vs.glsl", "shader.frag.glsl");
	Shader planeShader = Shader("shader.vs.glsl", "plane.frag.glsl");
	GLuint planeVAO, planeVBO;
	vector<GLfloat> planeVertices = {
		// positions            // normals         // texcoords
		10.0f, -1.5f,  10.0f,  0.0f, 1.0f, 0.0f,  20.0f,  0.0f,
		-10.0f, -1.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -1.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 20.0f,

		10.0f, -1.5f,  10.0f,  0.0f, 1.0f, 0.0f,  20.0f,  0.0f,
		-10.0f, -1.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		10.0f, -1.5f, -10.0f,  0.0f, 1.0f, 0.0f,  20.0f, 20.0f
	};
	mat4 planeModel = mat4(1.0f);
	void setupPlane();
	void drawPlane(const glm::mat4 view, glm::mat4 proj);
};

