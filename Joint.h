#pragma once

#include "DOF.h"
#include "Tokenizer.h"
#include "Model.h"
class Joint
{
private:
	glm::mat4 worldMatrix;
	glm::mat4 localMatrix;

	
public:
	std::vector<DOF*> DOFs;
	Joint();
	~Joint();
	Model boxModel;
	Joint * parentJoint;
	std::vector<Joint*> children;
	glm::vec3 offset, boxmin, boxmax, pose;
	DOF* rotxlimit;
	DOF* rotylimit;
	DOF* rotzlimit;
	std::string jointName;
	void setWorldMatrix(glm::mat4 w);
	void setLocalMatrix(glm::mat4 l);
	glm::mat4 getWorldMatrix();
	glm::mat4 getLocalMatrix();
	void addDOF(DOF* dof);
	void addChild(Joint* child);
	void Load(Tokenizer &t);
	void Update();
	void Draw(const glm::mat4 &viewProjMtx, uint shader);
	void Clamp();
};

