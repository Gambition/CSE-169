#include "Channel.h"
#include <math.h>

using namespace glm;
using namespace std;
Channel::Channel()
{
	
}


Channel::~Channel()
{
}

void Channel::load(Tokenizer * t)
{
	t->FindToken("{");
	while (1) {
		char temp[256];
		t->GetToken(temp);
		if (strcmp(temp, "extrapolate") == 0) {
			char tempExtraIn[256];
			t->GetToken(tempExtraIn);
			this->extraIn = strdup(tempExtraIn);

			char tempExtraOut[256];
			t->GetToken(tempExtraOut);
			this->extraOut = strdup(tempExtraOut);
		}
		else if (strcmp(temp, "keys") == 0) {
			this->numKeyframes = t->GetInt();
			t->FindToken("{");
			for (int i = 0; i < numKeyframes; i++) {
				Keyframe* k = new Keyframe();

				k->Time = t->GetFloat();
				k->Value = t->GetFloat();

				char tanIn[256];
				t->GetToken(tanIn);
				k->RuleIn = strdup(tanIn);

				char tanOut[256];
				t->GetToken(tanOut);
				k->RuleOut = strdup(tanOut);
				this->keyFrames.push_back(k);
			}
			t->FindToken("}");
		}
		else if (strcmp(temp, "}") == 0) {
			return;
		}
		else {
			t->SkipLine();
		}
	}
}

void Channel::Precommupte()
{
	//if there is only 1 key frame
	if (this->numKeyframes == 1)
	{
		return;
	}
	
	//special case for the first key frame
	Keyframe* curr = this->keyFrames[0];
	if (strcmp(curr->RuleOut, "flat") == 0) {
		curr->TangentOut = 0.0f;
	}
	else{
		curr->TangentOut = calculateTan(curr, this->keyFrames[1]);
	}

	float linear = 0.0f;
	float smooth = 0.0f;

	for (int i = 1; i < numKeyframes - 1; i++)
	{

		curr = this->keyFrames[i];
		if (strcmp(curr->RuleIn, "flat") == 0) {
			curr->TangentIn = 0.0f;
		}
		else if (strcmp(curr->RuleIn, "linear") == 0) {
			linear = calculateTan(keyFrames[i-1], curr);
			curr->TangentIn = linear;
		}
		else if (strcmp(curr->RuleIn, "smooth") == 0) {
			smooth = calculateTan(keyFrames[i - 1], keyFrames[i + 1]);
			curr->TangentIn = smooth;
		}
		else {
			curr->TangentIn = 0.0f;
		}

		if (strcmp(curr->RuleOut, "flat") == 0) {
			curr->TangentOut = 0.0f;
		}
		else if (strcmp(curr->RuleOut, "linear") == 0) {
			linear = calculateTan(curr,keyFrames[i+1]);
			curr->TangentOut = linear;
		}
		else if (strcmp(curr->RuleOut, "smooth") == 0) {
			smooth = calculateTan(keyFrames[i - 1], keyFrames[i + 1]);
			curr->TangentOut = smooth;
		}
		else {
			curr->TangentOut = 0.0f;
		}
	}
	//speical case for the last key frame
	curr = this->keyFrames[numKeyframes-1];
	if (strcmp(curr->RuleIn, "flat") == 0) {
		curr->TangentIn = 0.0f;
	}
	else {
		curr->TangentIn = calculateTan(keyFrames[numKeyframes-2], curr);
	}

	//calculate the A,B,C,D coefficients for every keyframe
	for (int i = 1; i < keyFrames.size(); i++)
	{
		calculateCoeff(keyFrames[i-1], keyFrames[i]);
	}
}

float Channel::calculateTan(Keyframe* first, Keyframe* second) {
	float p0 = first->Value;
	float p1 = second->Value;
	float t0 = first->Time;
	float t1 = second->Time;
	return (p1 - p0) / (t1 - t0);
}

void Channel::calculateCoeff(Keyframe* first, Keyframe* second) {
	float t0 = first->Time;
	float t1 = second->Time;
	float p0 = first->Value;
	float p1 = second->Value;
	float v0 = (t1 - t0)*first->TangentOut;
	float v1 = (t1 - t0)*second->TangentIn;

	first->A = (2 * p0) - (2 * p1) + v0 + v1;
	first->B = (-3 * p0) + (3 * p1) + (-2 * v0) - v1;
	first->C = v0;
	first->D = p0;

	
}

float Channel::Evaluate(float time)
{
	if (numKeyframes == 1)
	{
		return this->keyFrames[0]->Value;
	}

	if (time<keyFrames[0]->Time || time>keyFrames[numKeyframes-1]->Time)
	{
		return extrapolate(time);
	}

	for (int i = 0; i < numKeyframes; i++) {
		if (keyFrames[i]->Time == time) {
	
			return keyFrames[i]->Value;
		}
		else if(time > keyFrames[i]->Time && time < keyFrames[i+1]->Time)
		{

			float u = (time - keyFrames[i]->Time) / (keyFrames[i + 1]->Time - keyFrames[i]->Time);
	
			float a = keyFrames[i]->A;
			float b = keyFrames[i]->B;
			float c = keyFrames[i]->C;
			float d = keyFrames[i]->D;
			return d + (u*(c + (u*(b + (u*a)))));
		}
	}
	return extrapolate(time);
}

float Channel::extrapolate(float t) {
	//before the first key
	if (t < keyFrames[0]->Time) {
		if (strcmp(extraIn, "constant") == 0) {
			
			return keyFrames[0]->Value;
		}
		else if(strcmp(extraIn, "linear") == 0)
		{
			float t1 = keyFrames[0]->Time;
			float p = keyFrames[0]->Value;
			return p - (keyFrames[0]->TangentOut)*(t1 - t);

		}
		else if (strcmp(extraIn, "cycle") == 0) {
			float t0 = keyFrames[0]->Time;
			float t1 = keyFrames[numKeyframes - 1]->Time;
			int numCycles = ceil((t0 - t) / (t1 - t0));
			float convertedTime = t + numCycles * (t1 - t0);
			return Evaluate(convertedTime);
		}
		else if (strcmp(extraIn, "cycle_offset") == 0) {
		
			float t0 = keyFrames[0]->Time;
			float t1 = keyFrames[numKeyframes - 1]->Time;
			int numCycles = ceil((t0 - t) / (t1 - t0));
			float convertedTime = t + numCycles * (t1 - t0);
			float valueDiff = keyFrames[numKeyframes - 1]->Value - keyFrames[0]->Value;
			return Evaluate(convertedTime) - valueDiff * numCycles;
		}
		else if (strcmp(extraIn, "bounce") == 0) {
			float t0 = keyFrames[0]->Time;
			float t1 = keyFrames[numKeyframes - 1]->Time;
			int numCycles = ceil((t0 - t) / (t1 - t0));
			float convertedTime = t + numCycles * (t1 - t0);
			if (numCycles % 2 == 0)
			{
				return Evaluate(convertedTime);
			}
			else {
				return Evaluate(t1 - convertedTime);
			}

		}
	}
	//after the last key
	else {
		if (strcmp(extraOut, "constant") == 0) {
			return keyFrames[numKeyframes - 1]->Value;

		}
		else if (strcmp(extraOut, "linear") == 0)
		{
			float t0 = keyFrames[numKeyframes - 1]->Time;
			float p = keyFrames[numKeyframes - 1]->Value;
			return p + (keyFrames[numKeyframes - 1]->TangentIn)*(t - t0);
		}
		else if (strcmp(extraOut, "cycle") == 0) {
			float t0 = keyFrames[0]->Time;
			float t1 = keyFrames[numKeyframes - 1]->Time;
			int numCycles = ceil((t - t1) / (t1 - t0));
			float convertedTime = t - numCycles * (t1 - t0);
			return Evaluate(convertedTime);
		}
		else if (strcmp(extraOut, "cycle_offset") == 0) {
			float t0 = keyFrames[0]->Time;
			float t1 = keyFrames[numKeyframes - 1]->Time;
			int numCycles = ceil((t - t1) / (t1 - t0));
			float convertedTime = t - numCycles * (t1 - t0);
			float valueDiff = keyFrames[numKeyframes - 1]->Value - keyFrames[0]->Value;
			return Evaluate(convertedTime) + valueDiff * numCycles;
		}
		else if (strcmp(extraOut, "bounce") == 0) {
			float t0 = keyFrames[0]->Time;
			float t1 = keyFrames[numKeyframes - 1]->Time;
			int numCycles = ceil((t - t1) / (t1 - t0));
			float convertedTime = t - numCycles * (t1 - t0);
			if (numCycles % 2 == 0)
			{
				return Evaluate(convertedTime);
			}
			else {
				return Evaluate(t1 - convertedTime);
			}
		}

	}

}
