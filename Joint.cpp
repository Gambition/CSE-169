#include "Joint.h"
using namespace std;
Joint::Joint()
{
	//initialize values
	worldMatrix = glm::mat4(1.0f);
	localMatrix = glm::mat4(1.0f);
	offset = glm::vec3(0);
	boxmin = glm::vec3(-0.1, -0.1, -0.1);
	boxmax = glm::vec3(0.1, 0.1, 0.1);

	//initialize DOFs
	this->rotxlimit = new DOF();
	this->rotylimit = new DOF();
	this->rotzlimit = new DOF();
	this->DOFs.push_back(rotxlimit);
	this->DOFs.push_back(rotylimit);
	this->DOFs.push_back(rotzlimit);
}

void Joint::setLocalMatrix(glm::mat4 l)
{
	this->localMatrix = l;
}

void Joint::setWorldMatrix(glm::mat4 w)
{
	this->worldMatrix = w;
}
glm::mat4 Joint::getWorldMatrix()
{
	return this->worldMatrix;
}
glm::mat4 Joint::getLocalMatrix()
{
	return this->localMatrix;
}

void Joint::addDOF(DOF* dof)
{
	this->DOFs.push_back(dof);
}

void Joint::addChild(Joint * child)
{
	this->children.push_back(child);
}

void Joint::Load(Tokenizer & t)
{
	char name[256];
	t.GetToken(name);
	this->jointName = strdup(name);

	t.FindToken("{");
	while (1)
	{
		char temp[256];
		t.GetToken(temp);
		if (strcmp(temp, "offset") == 0) {
			this->offset.x = t.GetFloat();
			this->offset.y = t.GetFloat();
			this->offset.z = t.GetFloat();
		}
		else if (strcmp(temp, "boxmin") == 0) {
			this->boxmin.x = t.GetFloat();
			this->boxmin.y = t.GetFloat();
			this->boxmin.z = t.GetFloat();
		}
		else if (strcmp(temp, "boxmax") == 0) {
			this->boxmax.x = t.GetFloat();
			this->boxmax.y = t.GetFloat();
			this->boxmax.z = t.GetFloat();
		}
		else if (strcmp(temp, "pose") == 0)
		{
			float valueX = t.GetFloat();
			float valueY = t.GetFloat();
			float valueZ = t.GetFloat();
			rotxlimit->setValue(valueX);
			rotxlimit->setInitialValue(valueX);
			rotylimit->setValue(valueY);
			rotylimit->setInitialValue(valueY);
			rotzlimit->setValue(valueZ);
			rotzlimit->setInitialValue(valueZ);
		}
		else if (strcmp(temp, "rotxlimit") == 0)
		{
			this->rotxlimit->setMin(t.GetFloat());
			this->rotxlimit->setMax(t.GetFloat());
		}
		else if (strcmp(temp, "rotylimit") == 0)
		{
			this->rotylimit->setMin(t.GetFloat());
			this->rotylimit->setMax(t.GetFloat());
		}
		else if (strcmp(temp, "rotzlimit") == 0)
		{
			this->rotzlimit->setMin(t.GetFloat());
			this->rotzlimit->setMax(t.GetFloat());
		}
		else if (strcmp(temp, "balljoint") == 0)
		{			
			Joint* jnt = new Joint();
			jnt->parentJoint = this;
			jnt->Load(t);
			this->addChild(jnt);
		
		}
		else if (strcmp(temp, "}") == 0)
		{
			return;
		}
		else {
			t.SkipLine();
		}
	}
}

//update the skeleton by calculating the matrices
void Joint::Update()
{
	this->Clamp();
	float x = rotxlimit->getValue();
	float y = rotylimit->getValue();
	float z = rotzlimit->getValue();

	//local matrix of a 3-DOF rotational joint
	localMatrix = glm::mat4(cos(y)*cos(z), cos(y)*sin(z), -1.0f*sin(y), 0.0f,
		sin(x)*sin(y)*cos(z)-cos(x)*sin(z),sin(x)*sin(y)*sin(z)+cos(x)*cos(z), sin(x)*cos(y), 0.0f,
		cos(x)*sin(y)*cos(z)+sin(x)*sin(z),cos(x)*sin(y)*sin(z)-sin(x)*cos(z), cos(x)*cos(y), 0.0f, 
		offset.x, offset.y, offset.z, 1.0f);

	//multiply by parent's world matrix to get current world matrix
	if (this->parentJoint != NULL)
	{
		this->worldMatrix = (this->parentJoint->worldMatrix)*(this->localMatrix);
	}
	else
	{
		this->worldMatrix = glm::mat4(1.0f)*this->localMatrix;
	}

	//set up cube box

	//this->boxModel.MakeBox(this->boxmin, this->boxmax);	
	
	//Perform same procedure for all the children current joint
	for (unsigned int i = 0; i < this->children.size(); i++)
	{
		children[i]->Update();
	}
}

void Joint::Draw(const glm::mat4 &viewProjMtx, uint shader)
{
	//draw current joint
	this->boxModel.Draw(this->worldMatrix, viewProjMtx, shader);

	//draw all of its children
	for (unsigned int i = 0; i < this->children.size(); i++)
	{
		children[i]->Draw(viewProjMtx, shader);
	}
}

void Joint::Clamp()
{
	if (rotxlimit->getValue() > rotxlimit->getMax()) {
		rotxlimit->setValue(rotxlimit->getMax());
	}
	else if (rotxlimit->getValue() < rotxlimit->getMin()) {
		rotxlimit->setValue(rotxlimit->getMin());
	}

	if (rotylimit->getValue() > rotylimit->getMax()) {
		rotylimit->setValue(rotylimit->getMax());
	}
	else if (rotylimit->getValue() < rotylimit->getMin()) {
		rotylimit->setValue(rotylimit->getMin());
	}

	if (rotzlimit->getValue() > rotzlimit->getMax()) {
		rotzlimit->setValue(rotzlimit->getMax());
	}
	else if (rotzlimit->getValue() < rotzlimit->getMin()) {
		rotzlimit->setValue(rotzlimit->getMin());
	}

}

Joint::~Joint()
{
}

