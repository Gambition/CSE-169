#define _USE_MATH_DEFINES
#include "SPHParticle.h"

SPHParticle::SPHParticle()
{
	mass = 0.5f;
	support = 2.0f;
	//smooth = 1.0f;
	radius = 0.5f;
	velocity = vec3(0.0f);
	force = vec3(0.0f);
	pressure = 0.0f;
	density = 0.0f;
	cellIndex = 0;
	model = mat4(1.0f);
}

void SPHParticle::update(float deltaTime)
{
	vec3 accel = (1.0f / mass)*force;
	velocity += accel * deltaTime;
	center += velocity * deltaTime;
	
	//collision detection
	if (center.y <= -6.0f)
	{
	
		center.y = 2.0f*-6.0f - center.y;
	
		velocity.x = (1.0f - 0.5f) * velocity.x;
		velocity.y = -0.8f*velocity.y;
		velocity.z = (1.0f - 0.5f) * velocity.z;
	
	}
	if (center.x >= 3.5f)
	{
		center.x = 2.0f*3.5f - center.x;
		velocity.x = -0.8f*velocity.x;
		velocity.y = (1.0f - 0.5f) * velocity.z;
		velocity.z = (1.0f - 0.5f) * velocity.z;
		center += velocity * 0.04f;
		
	}
	if (center.x <= -3.5f)
	{
		center.x = 2.0f*-3.5f - center.x;
		velocity.x = -0.8f*velocity.x;
		velocity.y = (1.0f - 0.5f) * velocity.z;
		velocity.z = (1.0f - 0.5f) * velocity.z;
		center += velocity * 0.04f;
	
	}
	if (center.z <= -3.5f)
	{
		center.z = 2.0f*-3.5f - center.z;
		velocity.x = (1.0f - 0.5f) * velocity.x;
		velocity.y = (1.0f - 0.5f) * velocity.y;
		velocity.z = -0.8f*velocity.z;
		center += velocity * 0.04f;
	
	}
	if (center.z >= 3.5f)
	{
		center.z = 2.0f*3.5f - center.z;
		velocity.x = (1.0f - 0.5f) * velocity.x;
		velocity.y = (1.0f - 0.5f) * velocity.y;
		velocity.z = -0.8f*velocity.z;
		center += velocity * 0.04f;
	}
	
	
	vec3 diff = center - prevPos;
	model = translate(model, diff);
	prevPos = center;
	force = vec3(0.0f);
}

void SPHParticle::applyForce(vec3 f)
{
	force += f;
}

void SPHParticle::reset()
{
	
	this->center = this->init_pos;
	this->prevPos = center;
	this->velocity = vec3(0.0f);
	this->force = vec3(0.0f);
	this->pressure = 0.0f;
	this-> density = 0.0f;
	this->neighbors.clear();
	this->model = mat4(1.0f);
	model = translate(model, center);
}

void SPHParticle::setupSphere(int rings, int sectors)
{
   float const R = 1./(float)(rings-1);
    float const S = 1./(float)(sectors-1);

    for(int r = 0; r < rings; ++r) {
        for(int s = 0; s < sectors; ++s) {
            float const y = sin( -M_PI_2 + M_PI * r * R );
            float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
            float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

			vec3 p = vec3(x,y,z)*radius;
			vec3 n = normalize(p);
            this->vertices.push_back(p);
			this->normals.push_back(n);
            int curRow = r * sectors;
    		int nextRow = (r+1) * sectors;

			indices.push_back(curRow + s);
			indices.push_back(nextRow + s);
			indices.push_back(nextRow + (s+1));

			indices.push_back(curRow + s);
			indices.push_back(nextRow + (s+1));
			indices.push_back(curRow + (s+1));
        }
    }
}

void SPHParticle::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Vertex Positions to layout location 0
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*this->vertices.size(),
		&this->vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
		(GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, this->NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*this->normals.size(),
		&this->normals[0], GL_STATIC_DRAW);

	// Vertex Normals to layout location 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
		(GLvoid*)0);

	//indices array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(),
		&this->indices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void SPHParticle::render(Shader shader, mat4 view, mat4 proj)
{

	glm::mat4 modelView = view * this->model;

	glUseProgram(shader.Program);
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(this->model));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "modelview"), 1, GL_FALSE, glm::value_ptr(modelView));
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
	////set up material properties
	glUniform4f(glGetUniformLocation(shader.Program, "mtl.ambient"), 0.6f, 0.6f, 0.6f, 1.0f);
	glUniform4f(glGetUniformLocation(shader.Program, "mtl.diffuse"), 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(glGetUniformLocation(shader.Program, "mtl.specular"), 1.0f, 1.0f, 1.0f, 1.0f);
	glUniform1f(glGetUniformLocation(shader.Program, "mtl.shininess"), 100.0f);


	//set up lights
	GLfloat positions[8];
	GLfloat colors[8];

	glm::vec4 light1Pos = vec4(0.0f, 3.0f, 5.0f, 0.0f);
	glm::vec4 light2Pos = vec4(0.0f, 3.0f, -5.0f, 0.0f);

	glm::vec4 light1Col = vec4(1.0f, 0.0f, 0.1f, 1.0f);
	glm::vec4 light2Col = vec4(0.1f, 0.0f, 1.0f, 1.0f);
	for (int i = 0; i < 4; i++)
	{
		positions[i] = light1Pos[i];
		colors[i] = light1Col[i];
	}
	for (int i = 4; i < 8; i++)
	{
		positions[i] = light2Pos[i - 4];
		colors[i] = light2Col[i - 4];
	}

	glUniform1i(glGetUniformLocation(shader.Program, "numLight"), 2);
	glUniform4fv(glGetUniformLocation(shader.Program, "lightPos"), 2, positions);
	glUniform4fv(glGetUniformLocation(shader.Program, "lightColor"), 2, colors);

	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

SPHParticle::~SPHParticle()
{

}
