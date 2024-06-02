#include <iostream>
#include <fstream>


#include "ShaderProgram.h"




#include "Utils.h"
#include "Bone.h"
#include <vector>



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



#define WINDOWWIDTH 640.0
#define WINDOWHAIGHT 480.0

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}




extern std::vector<bone> treeSkeleton;
const  float common_vertecies[16] = {
0.1,0,0,1,
0,0.1,0,1,
-0.1,0,0,1,
0,-0.1,0,1
};






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


    unsigned int buffer;
	



	GLCALL(glEnableVertexAttribArray(0));
	
	

    Shader bonesShader("Shader\\v_bones.glsl", "Shader\\f_constant.glsl", "Shader\\g_bones.glsl");
	bonesShader.Bind();


	

	mat4 proj = glm::perspective(90.0, WINDOWWIDTH / WINDOWHAIGHT, 0.01, 100.0);
	mat4 view = glm::lookAt(vec3(0, 3, 3), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 model = mat4(1);
	GLCALL(bonesShader.SetUniformMat4f("P", proj));
	GLCALL(bonesShader.SetUniformMat4f("V", view));
	GLCALL(bonesShader.SetUniformMat4f("M", model));

	
	bone b1, b2, b3;
	
	b1.translation = mat4(1);
	b1.rotation = mat4(1);
	b1.final = mat4(1);
	b1.lenght = 1;
	b2.lenght = 1;
	b3.lenght = 2;

	b2.translation = glm::translate(mat4(1), vec3(0, 1, 0));
	b2.rotation = rotate(mat4(1.0), 0.5f, vec3(0.0, 0.0, 1.0));

	b3.translation = glm::translate(mat4(1), vec3(0, 1, 0));
	b3.rotation = mat4(1);
	b2.parent = 0;
	b3.parent = 1;
	treeSkeleton.push_back(b1);
	treeSkeleton.push_back(b2);
	treeSkeleton.push_back(b3);
	



	

    while (!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT);


#pragma region RenderBones


		GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, common_vertecies));

		bonesShader.Bind();
		
		treeSkeleton[0].final = mat4(1)   * treeSkeleton[0].translation * treeSkeleton[0].rotation;
		for (unsigned int i = 1;i<treeSkeleton.size();i++)
		{
			bone& bon = treeSkeleton[i];
			bon.final = treeSkeleton[bon.parent].final   * bon.translation * bon.rotation;

		}
		for (unsigned int i = 0; i < treeSkeleton.size(); i++)
		{
			bone& bon = treeSkeleton[i];
			bonesShader.SetUniform4f("last", 0, bon.lenght, 0, 1);

			bonesShader.SetUniformMat4f("M", bon.final);
			GLCALL(glDrawArrays(GL_LINES_ADJACENCY, 0, 4))
		}




#pragma endregion





		/*t += 0.005;
		t = fmodf(t, 1.0);
		Quat rot = Quat::lerp(a, b, t);
		Quat rots = Quat::slerp(a, b, t);
	
		DualQuaternion first = DualQuaternion::translation(0, 0, 1);
		DualQuaternion r = DualQuaternion(rot, Quat(0,0,0,0));

		DualQuaternion c = first.multiply(r).multiply(first).multiply(r);
		

		
		for (int i=0;i<36;i++)
		{
			vet[i] = (rot * quats[i] * rot.conjugate() ).toPont();
			vet[i].x += 2;
			DualQuaternion transformed(Quat(1, 0, 0, 0), quats[i]);

			DualQuaternion f = c.multiply(transformed).multiply(c.conjugate());
			Vertex v;
			v.w = 1;
			v.x = f.dual.x - 2;
			v.y = f.dual.y;
			v.z = f.dual.z;
			vet2[i] = v;
		}
		GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, vet));
     */
	/*	GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, lines));
        glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		GLCALL(glDrawArrays(GL_TRIANGLES, 0, 3));*/
		

		


		//GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, vet2));







        glfwSwapBuffers(window);


        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}