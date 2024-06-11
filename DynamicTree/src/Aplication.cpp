#include <iostream>
#include <fstream>


#include "ShaderProgram.h"

#include "lodepng.h"
#include "cubeTest.h"
#include "Utils.h"
#include "Bone.h"
#include "Camera.h"
#include <vector>



#pragma region Error

#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCALL(x) ClearGLError();\
x;\
ASSERT(PrintGLError());

using namespace glm;



#define WINDOWWIDTH 640.0
#define WINDOWHAIGHT 480.0

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}



GLuint tex0;
GLuint tex1;

float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
int vertexCount = myCubeVertexCount;
float* c1 = myCubeC1;
float* c2 = myCubeC2;
float* c3 = myCubeC3;


extern std::vector<bone> treeSkeleton;
const  float common_vertecies[16] = {
0.1,0,0,1,
0,0,0.1,1,
-0.1,0,0,1,
0,0,-0.1,1
};



void generate_segemnt(unsigned int parentIndex)
{
	bone& parent = treeSkeleton[parentIndex];
	parent.children++;
	float rd = radians((float)(rand() % 360));

	float rx = sin(rd);
	float rz = cos(rd);
	//float length = sqrt(rx * rx + rz * rz);
	//rx /= length;
	//rz /= length;
	bone b;
	b.layer = parent.layer + 1;

	vec3 direction = vec3(rx, 0, rz);
	vec3 straight = parent.final * vec4(0, 1, 0, 0);
	vec3 up = vec3(0, 1, 0);
	vec3 a = 0.5f * up + 0.5f * straight;
	float t = clamp(clamp(1 - b.layer / 100.0f, 0.0f, 1.0f) - 0.2 * parent.children, 0.05, 0.95);
	vec3 final = direction * t + a * (1 - t);
	vec4 finaltransformed = parent.final * vec4(final.x, final.y, final.z, 0);
	final = finaltransformed;



	mat4 rotation = lookAt(vec3(0, 0, 0), final, vec3(0, 1, 0));
	b.parent = parentIndex;
	b.children = 0;
	b.rotation = rotation;
	b.lenght = 1;
	b.translation = translate(mat4(1), vec3(0, parent.lenght, 0));
	treeSkeleton.push_back(std::move(b));



}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamiêci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamiêci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamiêci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}



int main(void)
{


#pragma region initialization
	srand(time(NULL));

	GLFWwindow* window;
	if (!glfwInit())
		return -1;


	window = glfwCreateWindow(WINDOWWIDTH, WINDOWHAIGHT, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	glfwSetErrorCallback(error_callback);
	glEnable(GL_DEPTH_TEST);
	tex0 = readTexture("src/wood_color.png");
	tex1 = readTexture("src/wood_normal.png");
	
#pragma endregion




	glClearColor(0.2, 0.6, 0.7, 1);
	
	
    Shader bonesShader("Shader\\v_bones.glsl", "Shader\\f_bones.glsl", "Shader\\g_bones.glsl");
	
	Shader treeShader("Shader\\v_constant.glsl", "Shader\\f_constant.glsl", "");
	//Texture
	GLCALL(glEnableVertexAttribArray(5));  
	//Vertices position
	GLCALL(glEnableVertexAttribArray(4));  
	//Normal mapping
	GLCALL(glEnableVertexAttribArray(3));  //c3
	GLCALL(glEnableVertexAttribArray(2));  //c2
	GLCALL(glEnableVertexAttribArray(1));  //c1
	//Bones
	GLCALL(glEnableVertexAttribArray(0));



	bone b1;
	b1.translation = mat4(1);
	b1.rotation = mat4(1);
	b1.final = mat4(1);
	b1.lenght = 1;
	b1.layer = 1;
	treeSkeleton.push_back(b1);
	treeSkeleton.reserve(120);
	

	for (int i = 0; i < 10; i++)
	{
		unsigned int parent = rand() % treeSkeleton.size();
		generate_segemnt(parent);

	}


#pragma region Camera
	Camera camera(vec3(0.0f, 1.7f, 2.0f), vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f); //Initial camera position
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame; // Calculate frame time
		lastFrame = currentFrame;
		//Keyboard handling
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(GLFW_KEY_D, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			camera.ProcessKeyboard(GLFW_KEY_UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			camera.ProcessKeyboard(GLFW_KEY_DOWN, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			camera.ProcessKeyboard(GLFW_KEY_LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			camera.ProcessKeyboard(GLFW_KEY_RIGHT, deltaTime);

		// Get the view matrix from the camera and pass it to the shader
		mat4 view = camera.GetViewMatrix();
		
#pragma endregion Camera
		
		bonesShader.Bind();
		mat4 proj = glm::perspective(90.0, WINDOWWIDTH / WINDOWHAIGHT, 0.01, 100.0);
		GLCALL(bonesShader.SetUniformMat4f("P", proj));
		GLCALL(bonesShader.SetUniformMat4f("V", view));

		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
#pragma region RenderBones


		GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, common_vertecies));


		treeSkeleton[0].final = mat4(1) * treeSkeleton[0].translation * treeSkeleton[0].rotation;
		for (unsigned int i = 1; i < treeSkeleton.size(); i++)
		{
			bone& bon = treeSkeleton[i];
			bon.final = treeSkeleton[bon.parent].final * bon.translation * bon.rotation;

		}
		for (unsigned int i = 0; i < treeSkeleton.size(); i++)
		{
			bone& bon = treeSkeleton[i];
			bonesShader.SetUniform4f("last", 0, bon.lenght, 0, 1);

			bonesShader.SetUniformMat4f("M", bon.final);
			GLCALL(glDrawArrays(GL_LINES_ADJACENCY, 0, 4));
		}


#pragma endregion
		
#pragma region RenderTree
	
		treeShader.Bind();
		
		GLCALL(treeShader.SetUniformMat4f("P", proj));
		GLCALL(treeShader.SetUniformMat4f("V", view));
	

		mat4 cubeModel = mat4(1);
		treeShader.SetUniformMat4f("M", cubeModel);

		GLCALL(glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, texCoords)); //Texture
		GLCALL(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, vertices)); //Position
		GLCALL(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, c3));  //c3
		GLCALL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, c2));  //c2
		GLCALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, c1));  //c1

		
		
		GLCALL(treeShader.SetUniform1i("textureMap0", 0))
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);

		GLCALL(treeShader.SetUniform1i("textureMap1", 1))
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);

		GLCALL(glDrawArrays(GL_TRIANGLES, 0, vertexCount))
#pragma endregion

		glfwSwapBuffers(window);


		glfwPollEvents();
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(5);


	glfwTerminate();
	return 0;
}