#pragma once
#include "Core.h"
using namespace std;

class Keyframe
{

public:
	float Time;
	float Value;
	float TangentIn, TangentOut;
	char* RuleIn;
	char* RuleOut;
	float A, B, C, D;
	Keyframe();
	~Keyframe();
	
};

