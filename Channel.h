#pragma once
#include "Keyframe.h"
#include "Tokenizer.h"
#include <iostream>
class Channel
{
public:
	Channel();
	~Channel();
	vector<Keyframe*> keyFrames;

	//extrapolation type
	char* extraIn;
	char* extraOut;
	int numKeyframes;
	void load(Tokenizer* t);
	float Evaluate(float time);
	void Precommupte();
	float calculateTan(Keyframe* first, Keyframe* second);
	void calculateCoeff(Keyframe* first, Keyframe* second);
	float extrapolate(float t);	

};

