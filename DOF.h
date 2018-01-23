#pragma once
#include "Core.h"
#include <stdlib.h>
#include <iostream>
class DOF
{
	private:
		float value;
		float min, max;
		
	public:
		DOF();
		DOF(float value,float min, float max);
		void setValue(float value);
		void setMin(float min);
		void setMax(float max);
		float getValue();
		float getMin();
		float getMax();
		
		~DOF();
};

