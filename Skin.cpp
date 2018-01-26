#include "Skin.h"



Skin::Skin()
{
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
			t->FindToken("{");
			int numVertices = t->GetInt();
			for (unsigned int i = 0; i < numVertices; i++)
			{
				vec3 pos = vec3(t->GetFloat(), t->GetFloat(), t->GetFloat());
				this->vertices.push_back(new Vertex(pos));
			}
		}
		else if (strcmp(temp, "normal") == 0) {
			t->FindToken("{");
			int numFaces = t->GetInt();
			for (unsigned int i = 0; i < numFaces; i++)
			{
				vec3 norm = vec3(t->GetFloat(), t->GetFloat(), t->GetFloat());
				//set the normal to corresponding vertex
				this->vertices[i]->normal = norm;
			}
		}
		else if (strcmp(temp, "skinweight") == 0) {
			t->FindToken("{");
			int numWeight = t->GetInt();
			for (unsigned int i = 0; i < numWeight; i++)
			{
				int numAttach = t->GetInt();
				this->vertices[i]->numAttachments = numAttach;
				for (unsigned int j = 0; j < numAttach;j++)
				{
					int jointNum = t->GetInt();
					float weight = t->GetFloat();
					vertices[i]->weights[j].first = jointNum;
					vertices[i]->weights[j].second = weight;
				}
			}
		}
		else if (strcmp(temp, "triangles") == 0) {
			t->FindToken("{");
			int numFaces = t->GetInt();
			for (unsigned int i = 0; i < numFaces; i++)
			{
				Face* face = new Face();
				//push corresponding vertices into the face
				for (unsigned int j = 0; j < 3; j++)
				{
					face->vertices.push_back(this->vertices[t->GetInt()]);
				}
				this->faces.push_back(face);
			}
		}
		else if (strcmp(temp, "bindings") == 0) {
			t->FindToken("{");
			int numBindings = t->GetInt();
			for (unsigned int i = 0; i < numBindings; i++)
			{
				glm::mat4 bindingMatrix = mat4(0.0f);
				t->FindToken("matrix {");
				for (unsigned int j = 0; j < 4; j++)
				{
					for (unsigned int k = 0; k < 4; k++)
					{
						if (k == 3) {
							bindingMatrix[j][k] = 0.0f;
						}
						else {
							bindingMatrix[j][k] = t->GetFloat();
						}
					}
				}
				this->Bindings.push_back(bindingMatrix);
			}
		}
		else
		{
			break;
		}
	}
}

void Skin::update()
{
	

}
