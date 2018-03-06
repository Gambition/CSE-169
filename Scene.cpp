#include "Scene.h"
#include "Shader.h"
Scene::Scene()
{
	

}

Scene::Scene(int width,int height)
{
	myCloth = new Cloth(width, height);
	myCloth->initialize();
	myCloth->setupMesh();
	setupPlane();
}

void Scene::setupPlane()
{
	//set up plane for ground collision detection
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*planeVertices.size(), &planeVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

}

void Scene::drawPlane(const glm::mat4 view, glm::mat4 proj)
{
	mat4 modelView = view * this->planeModel;
	glUseProgram(planeShader.Program);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(this->planeModel));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelview"), 1, GL_FALSE, glm::value_ptr(modelView));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
	glBindVertexArray(this->planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Scene::render(const glm::mat4 view, glm::mat4 proj)
{
	myCloth->render(shader,view, proj);
	drawPlane(view, proj);
}



Scene::~Scene()
{
	delete myCloth;
}


