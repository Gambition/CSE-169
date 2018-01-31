#include "Skin.h"

const float PI = 3.14159265359;

Skin::Skin()
{
	t = new Tokenizer();
}


Skin::~Skin()
{
}

void Skin::load(const char * filename)
{
	t->Open(filename);

	//load .skin file
	while (1) {
		char temp[256];
		t->GetToken(temp);
		if (strcmp(temp, "positions") == 0) {

			int numVertices = t->GetInt();
			t->FindToken("{");
			for (int i = 0; i < numVertices; i++)
			{
				float x = t->GetFloat();
				float y = t->GetFloat();
				float z = t->GetFloat();
				vec3 pos = vec3(x, y, z);
				this->vtPos.push_back(pos);
				this->vertices.push_back(new Vertex(pos));
			}
		
		}
		else if (strcmp(temp, "normals") == 0) {

			int numFaces = t->GetInt();
			t->FindToken("{");

			for (unsigned int i = 0; i < numFaces; i++)
			{
				float x = t->GetFloat();
				float y = t->GetFloat();
				float z = t->GetFloat();
				vec3 norm = vec3(x, y, z);
				this->vtNorm.push_back(norm);
				//set the normal to corresponding vertex
				this->vertices[i]->normal = norm;
			}
	
		}
		else if (strcmp(temp, "skinweights") == 0) {
			int numWeight = t->GetInt();
			t->FindToken("{");
			
			for (unsigned int i = 0; i < numWeight; i++)
			{
				int numAttach = t->GetInt();
				this->vertices[i]->numAttachments = numAttach;
			
				this->vertices[i]->weights.reserve(numAttach);
				for (unsigned int j = 0; j < numAttach;j++)
				{
					int jointNum = t->GetInt();
					float weight = t->GetFloat();
	
					vertices[i]->weights.push_back(pair<int, float>(jointNum, weight));
				}
			}

		}
		else if (strcmp(temp, "triangles") == 0) {

			int numFaces = t->GetInt();
			t->FindToken("{");
		
			for (unsigned int i = 0; i < numFaces; i++)
			{
				Face* face = new Face();
				//push corresponding vertices into the face
				for (unsigned int j = 0; j < 3; j++)
				{
					int index = t->GetInt();
					this->indices.push_back(index);
					face->vertices.push_back(this->vertices[index]);
				}
				this->faces.push_back(face);
			}
		}
		else if (strcmp(temp, "texcoords") == 0) {
			int numTexs = t->GetInt();
			t->FindToken("{");
			for (int i = 0; i < numTexs; i++)
			{
				float x = t->GetFloat();
				float y = t->GetFloat();
				this->texCoords.push_back(glm::vec2(x,y));
			}

		}
		else if (strcmp(temp, "bindings") == 0) {

			int numBindings = t->GetInt();
			t->FindToken("{");
	
			for (unsigned int i = 0; i < numBindings; i++)
			{
				
				t->FindToken("matrix {");
				float ax = t->GetFloat();
				float ay = t->GetFloat();
				float az = t->GetFloat();
				float bx = t->GetFloat();
				float by = t->GetFloat();
				float bz = t->GetFloat();
				float cx = t->GetFloat();
				float cy = t->GetFloat();
				float cz = t->GetFloat();
				float dx = t->GetFloat();
				float dy = t->GetFloat();
				float dz = t->GetFloat();
				
				glm::mat4 bindingMatrix = mat4(ax, bx, cx, dx,
					ay, by, cy, dy,
					az, bz, cz, dz,
					0.0f, 0.0f, 0.0f,1.0f);
				this->Bindings.push_back(bindingMatrix);
				bindingMatrix = glm::transpose(bindingMatrix);
				this->inverseBindings.push_back(glm::inverse(bindingMatrix));
			}
			break;
		}
		else
		{
			t->SkipLine();
			
		}
	}
	t->Close();
}


void Skin::update(Skeleton * skel)
{
	//update all vertices 
	for (int i = 0; i < this->vertices.size(); i++)
	{
		glm::mat4 deform = glm::mat4(0.0f);
		vec3 updatedPos = vec3(0.0f);
		vec3 updatedNorm = vec3(0.0f);
		for (int j = 0; j < vertices[i]->numAttachments; j++)
		{
			int index = vertices[i]->weights[j].first;
			float weight = vertices[i]->weights[j].second;
			mat4 weightMatrix = skel->joints[index]->getWorldMatrix()*this->inverseBindings[index];
			updatedPos = updatedPos + vec3(weight*weightMatrix*vec4(vertices[i]->position, 1.0f));
			updatedNorm = updatedNorm + glm::normalize(vec3(weight*weightMatrix*vec4(vertices[i]->normal, 1.0f)));
		}
		//vertices[i]->position = updatedPos;
		vtPos[i] = updatedPos;
		//vertices[i]->normal = updatedNorm;
		vtNorm[i] = updatedNorm;
	}
}

void Skin::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Vertex Positions to layout location 0
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*this->vtPos.size(),
		&this->vtPos[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
		(GLvoid*)0);


	glBindBuffer(GL_ARRAY_BUFFER, this->NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*this->vtNorm.size(),
		&this->vtNorm[0], GL_STATIC_DRAW);

	// Vertex Normals to layout location 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3),
		(GLvoid*)0);

	//Texture coordinates to layout location 2
	if (this->hasTexture == 1)
	{

		glBindBuffer(GL_ARRAY_BUFFER, this->TBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*this->vtPos.size(),
			&this->texCoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec2),
			(GLvoid*)0);
		LoadGlTextures();
		cout << "texture loaded" << endl;
	}
	//indices array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(),
		&this->indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Skin::Draw(Shader & shader, glm::mat4 view, glm::mat4 proj)
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

	//set up texture
	if (this->hasTexture)
	{
		glUniform1i(glGetUniformLocation(shader.Program, "hasTexture"), 1);
		glUniform1i(glGetUniformLocation(shader.Program, "tex"), 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	
	//set up lights
	GLfloat positions[8];
	GLfloat colors[8];

	glm::vec4 light1Pos = vec4(-6.0f, 3.0f, 0.0f, 0.0f);
	glm::vec4 light2Pos = vec4(6.0f, 3.0f, 0.0f, 0.0f);

	glm::vec4 light1Col = vec4(1.0f, 0.1f, 0.1f, 1.0f);
	glm::vec4 light2Col = vec4(0.2f, 0.3f, 1.0f, 1.0f);
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
	glBindVertexArray(0);;

}

// Load Bitmaps And Convert To Textures
void Skin::LoadGlTextures()
{

	if (!image1.load("head.bmp"))
		exit(1);

	// Create Texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// note that BMP images store their data in BRG order, not RGB
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1.sizeX, image1.sizeY, 0, GL_BGR,
		GL_UNSIGNED_BYTE, image1.data);
};


