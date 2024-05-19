#include <iostream>
#include <fstream>


#include "ShaderProgram.h"
#include "DualQuaternion.h"




#include "Utils.h"



#pragma region Error

#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCALL(x) ClearGLError();\
x;\
ASSERT(PrintGLError());

using namespace glm;

float vertices[] = {
				1.0f,-1.0f,-1.0f,1.0f,
				-1.0f, 1.0f,-1.0f,1.0f,
				-1.0f,-1.0f,-1.0f,1.0f,

				1.0f,-1.0f,-1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,
				-1.0f, 1.0f,-1.0f,1.0f,


				-1.0f,-1.0f, 1.0f,1.0f,
				1.0f, 1.0f, 1.0f,1.0f,
				1.0f,-1.0f, 1.0f,1.0f,

				-1.0f,-1.0f, 1.0f,1.0f,
				-1.0f, 1.0f, 1.0f,1.0f,
				1.0f, 1.0f, 1.0f,1.0f,

				1.0f,-1.0f, 1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,
				1.0f,-1.0f,-1.0f,1.0f,

				1.0f,-1.0f, 1.0f,1.0f,
				1.0f, 1.0f, 1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,

				-1.0f,-1.0f,-1.0f,1.0f,
				-1.0f, 1.0f, 1.0f,1.0f,
				-1.0f,-1.0f, 1.0f,1.0f,

				-1.0f,-1.0f,-1.0f,1.0f,
				-1.0f, 1.0f,-1.0f,1.0f,
				-1.0f, 1.0f, 1.0f,1.0f,

				-1.0f,-1.0f,-1.0f,1.0f,
				1.0f,-1.0f, 1.0f,1.0f,
				1.0f,-1.0f,-1.0f,1.0f,

				-1.0f,-1.0f,-1.0f,1.0f,
				-1.0f,-1.0f, 1.0f,1.0f,
				1.0f,-1.0f, 1.0f,1.0f,

				-1.0f, 1.0f, 1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,
				1.0f, 1.0f, 1.0f,1.0f,

				-1.0f, 1.0f, 1.0f,1.0f,
				-1.0f, 1.0f,-1.0f,1.0f,
				1.0f, 1.0f,-1.0f,1.0f,

};
unsigned int vertexCount = 36;

Quat quats[36];

#define WINDOWWIDTH 640.0
#define WINDOWHAIGHT 480.0

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

int main(void)
{


    #pragma region initialization

    
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

#pragma endregion


    glClearColor(0.2, 0.6, 0.7, 1);
	unsigned int vao;
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);


    unsigned int buffer;
	



	GLCALL(glEnableVertexAttribArray(0));
	
	

    Shader s("Shader\\v_constant.glsl", "Shader\\f_constant.glsl");
	s.Bind();

	for (int i = 0; i < 36; i++)
	{
		quats[i] = Quat(vertices[4 * i + 3], vertices[4 * i], vertices[4 * i + 1], vertices[4 * i + 2]);
	}

	

	mat4 proj = glm::perspective(90.0, WINDOWWIDTH / WINDOWHAIGHT, 0.01, 100.0);
	mat4 view = glm::lookAt(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 model = mat4(1);
	GLCALL(s.SetUniformMat4f("P", proj));
	GLCALL(s.SetUniformMat4f("V", view));
	GLCALL(s.SetUniformMat4f("M", model));

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	float t = 0.0f;

	Quat a = Quat(1,1,0,0).normalized();
	Quat b = Quat(-1,1,0,0).normalized();
	std::cout << a << b;

	Vertex vet[36];
    while (!glfwWindowShouldClose(window))
    {
		t += 0.005;
		t = fmodf(t, 1.0);
		Quat rot =  ((a * t) + (b * (1-t)) ).normalized();
		std::cout << rot;
		for (int i=0;i<36;i++)
		{
			vet[i] = (quats[i] * rot).toPont();

		}
		GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, vet));
     
        glClear(GL_COLOR_BUFFER_BIT);

		GLCALL(glDrawArrays(GL_TRIANGLES, 0, 36));

		//glBegin(GL_TRIANGLES);
		//glVertex2f(0.5, 0.5);
		//glVertex2f(-0.5, 0.5);
		//glVertex2f(0.5, -0.5);
		//glEnd();

        glfwSwapBuffers(window);


        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}