#include "Scene.h"
#include "Shader.h"
Scene::Scene()
{
	

}

Scene::Scene(const char * skeletonFile, const char * skinFile)
{
	mySkeleton->Load(skeletonFile);
	mySkeleton->Update();
	mySkin->load(skinFile);
	if (strcmp(skinFile, "head.skin" )== 0)
	{
		mySkin->hasTexture = 1;
	}

	mySkin->update(this->mySkeleton);
	mySkin->setupMesh();
}

void Scene::Draw(const glm::mat4 &viewProjMtx, uint shader)
{
	this->mySkeleton->getRoot()->Draw(viewProjMtx, shader);
}

void Scene::render(const glm::mat4 view, glm::mat4 proj)
{
	this->mySkin->Draw(shader, view, proj);	
	mySkeleton->Update();
	mySkin->update(this->mySkeleton);
	mySkin->setupMesh();
}

Scene::~Scene()
{

}
