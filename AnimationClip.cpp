#include "AnimationClip.h"



AnimationClip::AnimationClip()
{
	t = new Tokenizer();
}


AnimationClip::~AnimationClip()
{
}

void AnimationClip::Load(const char * filename)
{
	t->Open(filename);
	while (1)
	{
		char temp[256];
		t->GetToken(temp);
		if (strcmp(temp, "range")==0) {
			this->startTime = t->GetFloat();
			this->endTime = t->GetFloat();
		}
		else if (strcmp(temp, "numchannels") == 0) {
			this->numChannels = t->GetInt();
		}
		else if (strcmp(temp, "channel") == 0) {
			Channel* c = new Channel();
			c->load(t);
			c->Precommupte();
			this->channels.push_back(c);
			
		}

		else if (strcmp(temp, "}") == 0)
		{
			t->Close();
			return;
		}
		else {
			t->SkipLine();
		}
	}
}

void AnimationClip::Evaluate(float Time, Skeleton * mySkeleton)
{

	int numJoints = mySkeleton->joints.size();
	//first 3 channels are the root translation
	float rootX = this->channels[0]->Evaluate(Time);
	float rootY = this->channels[1]->Evaluate(Time);
	float rootZ = this->channels[2]->Evaluate(Time);

	glm::mat4 rootMatrix = mySkeleton->getRoot()->getWorldMatrix();
	rootMatrix = glm::translate(rootMatrix, glm::vec3(rootX, rootY, rootZ));

	mySkeleton->getRoot()->setWorldMatrix(rootMatrix);

	for (int i = 3; i < this->numChannels; i++) {
		int jointIndex = ceil((i - 3) / 3);
		if (i % 3 == 0) {
			float rotX = channels[i]->Evaluate(Time);
			mySkeleton->joints[jointIndex]->DOFs[0]->setValue(rotX);
		}
		else if (i % 3 == 1) {
			float rotY = channels[i]->Evaluate(Time);
			mySkeleton->joints[jointIndex]->DOFs[1]->setValue(rotY);
		}
		else {
			float rotZ = channels[i]->Evaluate(Time);
			mySkeleton->joints[jointIndex]->DOFs[2]->setValue(rotZ);
		}
	}
}

