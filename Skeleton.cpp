#include "Skeleton.h"

void Skeleton::Update()
{
	this->root->Update();
}

int Skeleton::Load(const char* file)
{
	//open the token and find the root point
	t.Open(file);
	root = new Joint();
	root->parentJoint = NULL;
	//load the skeleton
	root->Load(t);
	//close token 
	t.Close();
	this->setJoints(root);
	return 1;
}

void Skeleton::setJoints(Joint * curr)
{
	this->joints.push_back(curr);
	this->jointNames.push_back(curr->jointName);
	for (unsigned int i = 0; i < curr->children.size(); i++)
	{
		setJoints(curr->children[i]);
	}
}

Joint * Skeleton::getRoot()
{
	return this->root;
}



void Skeleton::Draw(const glm::mat4 &viewProjMtx, uint shader)
{
	this->root->Draw(viewProjMtx,shader);
}

Skeleton::Skeleton()
{
	
}






Skeleton::~Skeleton()
{
}
