#include "Cloth.h"

Cloth::Cloth()
{
	//defaulty cloth size is 10
	this->width = 10;
	this->height = 10;
	this->Vair = vec3(0.0f, 0.0f, 0.0f);
	//initialize particle sizes
	this->particles.resize(10);
	this->model = mat4(1.0f);
	this->ratio = 0.25f;
	this->ground = new Plane(vec3(0.0f,-2.0f,0.0f));
	for (int i = 0; i < 10; i++)
	{
		particles[i].resize(10);
	}
}

Cloth::Cloth(int w, int h)
{
	this->width = w;
	this->height = h;
	this->Vair = vec3(0.0f, 0.0f, 0.0f);
	//initialize particle sizes
	this->particles.resize(h);
	this->model = mat4(1.0f);
	this->ratio = 0.25f;
	this->ground = new Plane(vec3(0.0f, -1.5f, 0.0f));
	for (int i = 0; i < h; i++)
	{
		particles[i].resize(w);
	}
}


void Cloth::initialize()
{
	//initialize all the particles:
	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			Particle *p = new Particle();
			float x = (j - (width / 2.0f))*ratio;
			float y = ((height/2.0f)-i)*ratio;
	
			p->pos = vec3(x, y, 0.01f);
			p->mass = 0.5f;
			p->index = j + i * width;
			p->prevPos = vec3(x, y, 0.01f);
			this->particles[i][j] = p;
			
			this->vertices.push_back(p->pos);
 		}
	}


	this->normals.resize(this->vertices.size());

	//initialize all the triangles:
	for (int i = 0; i < this->height-1; i++)
	{
		for (int j = 0; j < this->width-1; j++)
		{
			Particle* p0 = this->particles[i][j];
			Particle* p1 = this->particles[i][j+1];
			Particle* p2 = this->particles[i+1][j];
			Particle* p3 = this->particles[i+1][j+1];

			//triangles for calculating forces, faces for rendering
			this->triangles.push_back(new triangle(p0, p1, p2));
			this->indices.push_back(p0->index);
			this->indices.push_back(p1->index);
			this->indices.push_back(p2->index);

			this->triangles.push_back(new triangle(p3, p2, p1));
			this->indices.push_back(p3->index);
			this->indices.push_back(p2->index);
			this->indices.push_back(p1->index);
			
			this->triangles.push_back(new triangle(p0, p1, p3));
			this->indices.push_back(p0->index);
			this->indices.push_back(p1->index);
			this->indices.push_back(p3->index);
			
			this->triangles.push_back(new triangle(p0, p3, p2));
			this->indices.push_back(p0->index);
			this->indices.push_back(p3->index);
			this->indices.push_back(p2->index);
			
		}
	}

	//initialize spring-dampers
	float diag = sqrt(ratio*ratio + ratio * ratio);
	for (int i = 0; i < height - 1; i++)
	{
		for (int j = 0; j < width - 1; j++)
		{
			Particle* p0 = this->particles[i][j];
			Particle* p1 = this->particles[i][j + 1];
			Particle* p2 = this->particles[i + 1][j];
			Particle* p3 = this->particles[i + 1][j + 1];

			SpringDamper* s0 = new SpringDamper(p0, p1);
			s0->restLength = ratio;
			SpringDamper* s1 = new SpringDamper(p0, p2);
			s1->restLength = ratio;
			SpringDamper* s2 = new SpringDamper(p1, p2);
			s2->restLength = diag;
			SpringDamper* s3 = new SpringDamper(p0, p3);
			s3->restLength = diag;

			this->springDampers.push_back(s0);
			this->springDampers.push_back(s1);
			this->springDampers.push_back(s2);
			this->springDampers.push_back(s3);
		}
	}

	//last row
	for (int i = 0; i < width - 1; i++)
	{
		Particle* p0 = particles[height - 1][i];
		Particle* p1 = particles[height - 1][i + 1];
		SpringDamper* s = new SpringDamper(p0, p1);
		s->restLength = ratio;
		this->springDampers.push_back(s);
	}

	//last column
	for (int i = 0; i < height - 1; i++)
	{
		Particle* p0 = particles[i][width - 1];
		Particle* p1 = particles[i + 1][width - 1];
		SpringDamper* s = new SpringDamper(p0, p1);
		s->restLength = ratio;
		this->springDampers.push_back(s);
	}

	numParticles = vertices.size();
	numIndices = indices.size();
}

void Cloth::fixRow(int rowIndex)
{
	for (int i = 0; i < width; i++)
	{
		particles[rowIndex][i]->fixed = true;
	}
}

void Cloth::fixColumn(int columnIndex)
{
	for (int i = 0; i < height; i++)
	{
		particles[i][columnIndex]->fixed = true;
	}
}

void Cloth::calculateNormals()
{
	/**update vertices and normals**/
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			vertices[i*width + j] = particles[i][j]->pos;
			particles[i][j]->normal = vec3(0.0f, 0.0f, 0.0f);
		}
	}

	for (int i = 0; i < triangles.size(); i++)
	{
		triangle* curr = triangles[i];
		curr->computeNormal();
		curr->p1->normal += curr->normal;
		curr->p2->normal += curr->normal;
		curr->p3->normal += curr->normal;
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			particles[i][j]->normal = normalize(particles[i][j]->normal);
			normals[i*width + j] = particles[i][j]->normal;
		}
	}
}

void Cloth::update()
{
	vec3 gravity = vec3(0, -9.8f, 0);

	//calculate gravity
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			vec3 f = particles[i][j]->mass*gravity;
			particles[i][j]->applyForce(f);
		}
	}

	//spring forces
	for (int i = 0; i < springDampers.size(); i++)
	{
		springDampers[i]->computeForce();
	}

	//aerodynamic forces
	for (int i = 0; i < triangles.size(); i++)
	{
		triangles[i]->calculateForce(Vair);
	}

	//collision detection
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			segment s;
			intersection inter;
			s.A = particles[i][j]->prevPos;
			s.B = particles[i][j]->pos;
			if (ground->TestSegment(s, inter)) {
				vec3 v = particles[i][j]->velocity;
				vec3 p = particles[i][j]->pos;
				p.y = 2.0f*ground->point.y - p.y;
				particles[i][j]->velocity = -v;
				particles[i][j]->pos = p;
			}
			particles[i][j]->prevPos = particles[i][j]->pos;
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			particles[i][j]->update(0.002);
		}
	}
	this->calculateNormals();


}

void Cloth::setupMesh()
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

void Cloth::adjust(vec3 move)
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (particles[i][j]->fixed == true)
			{
				vec3 prev = particles[i][j]->pos;
				particles[i][j]->prevPos = prev;
				particles[i][j]->pos = prev + move;
			}
		}
	}
	this->calculateNormals();
}

void Cloth::reset()
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			particles[i][j]->reset();
		}
	}
}

void Cloth::render(Shader shader,glm::mat4 view, glm::mat4 proj)
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


Cloth::~Cloth()
{
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			delete particles[i][j];
		}
	}

	for (int i = 0; i < springDampers.size(); i++)
	{
		delete springDampers[i];
	}

	for (int i = 0; i < triangles.size(); i++)
	{
		delete triangles[i];
	}
}
