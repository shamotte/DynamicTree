#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/euler_angles.hpp>



#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCALL(x) ClearGLError();\
x;\
ASSERT(PrintGLError());


void ClearGLError();
bool PrintGLError();
