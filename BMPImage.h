#pragma once
#include "Core.h"
class BMPImage
{
public:
	~BMPImage()
	{
		delete[] data;
	}

	bool load(char* filename);

	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};

//
// uncomment the MAC_OS define below if you are compiling on a Mac
// (big-endian system)
//




//
// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.
// Taken from Richard Campbell Linux/GLUT code for NeHe's lesson 6:
//     http://nehe.gamedev.net
//
// Wojciech Jarosz (02/02/06) - Ported to big-endian systems (MacOS).
//



