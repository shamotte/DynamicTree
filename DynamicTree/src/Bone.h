#pragma once

#include "ShaderProgram.h"
#include <vector>



#include "Utils.h"





class bone {
public:

	unsigned int parent;
	unsigned int layer;
	unsigned int children =0;
	
	float lenght = 3;

	
	glm::mat4 translation;
	glm::mat4 rotation;


	glm::mat4 final;






};