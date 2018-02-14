#pragma once
#include "Channel.h"
#include "Skeleton.h"


class AnimationClip
{
public:
	Tokenizer* t;
	AnimationClip();
	~AnimationClip();
	vector<Channel*> channels;
	void Load(const char* filename);
	void Evaluate(float Time, Skeleton* mySkeleton);

	float startTime, endTime;
	int numChannels;

};

