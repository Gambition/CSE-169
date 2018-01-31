#pragma once
#include "Core.h"
#include <stdlib.h>
#include <iostream>
class DOF
{
private:
	float value;
	float min, max;
	float initialValue;
public:
	DOF();
	DOF(float value, float min, float max);
	void setValue(float value);
	void setInitialValue(float value);
	void setMin(float min);
	void setMax(float max);
	float getValue();
	float getInitialValue();
	float getMin();
	float getMax();
		
		~DOF();
};

