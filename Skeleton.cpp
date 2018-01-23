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
	return 1;
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
