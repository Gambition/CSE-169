#pragma once
#include "Skin.h"
#include "Skeleton.h"


class Scene
{
private:
	Skeleton * mySkeleton = new Skeleton();
	Skin* mySkin = new Skin();
	Shader shader = Shader("shader.vs.glsl", "shader.frag.glsl");
public:
	Scene();
	Scene(const char* skeletonFile, const char* skinFile);
	void Draw(const glm::mat4 &viewProjMtx, uint shader);
	void render(const glm::mat4 view, glm::mat4 proj);
	Skin* getSkin() { return mySkin; }
	Skeleton* getSkeleton() { return mySkeleton; }
	~Scene();

};

