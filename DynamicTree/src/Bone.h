#pragma once

#include "ShaderProgram.h"
#include <vector>



#include "Utils.h"
extern float symulation_time;




class bone {
public:

	unsigned int parent;
	unsigned int layer;
	unsigned int children =0;

	float multiplayer = 1;
	float lenght = 0;
	float creation_time = 0;



	
	glm::mat4 translation;
	glm::mat4 rotation;


	glm::mat4 final;


	void update();



};