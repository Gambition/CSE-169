#pragma once
#include "Joint.h"


class Skeleton
{
	private:
		Joint* root;
		Tokenizer t;
	
	public:
		int Load(const char* file);
		void Update();
		void Draw(const glm::mat4 &viewProjMtx, uint shader);
		Skeleton();
	
		
		~Skeleton();
};
