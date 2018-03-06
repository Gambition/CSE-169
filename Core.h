////////////////////////////////////////
// Core.h
////////////////////////////////////////

#pragma once

// This file just has a bunch of common stuff used by all objects. It mainly just
// includes GL and some other standard headers.

// Set up GLM
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"

// Set up GLEW
#define GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#include "GL/glew.h"

// Set up GLUT
#include "GL/glut.h"

// Types
typedef unsigned int uint;

// STD library
#include <vector>
#include <string>
#include <ctype.h>
#include <iostream>

