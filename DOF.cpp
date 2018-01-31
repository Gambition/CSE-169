#include "DOF.h"

void DOF::setValue(float value) { this->value = value; }

void DOF::setInitialValue(float value)
{
	this->initialValue = value;
}


void DOF::setMin(float min) { this->min = min; }

void DOF::setMax(float max) { this->max = max; }

float DOF::getValue() { return this->value;}

float DOF::getInitialValue()
{
	return initialValue;
}

float DOF::getMin() { return this->min; }

float DOF::getMax() { return this->max; }


DOF::DOF()
{
	this->value = 0.0f;
	this->min = -100000.0f;
	this->max = 100000.0f;
}

DOF::DOF(float value, float min,float max)
{
	this->value = value;
	this->min = min;
	this->max = max;
}


DOF::~DOF()
{
}
