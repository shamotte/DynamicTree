#include <iostream>
#include <fstream>


#include "ShaderProgram.h"




#include "Utils.h"
#include "Bone.h"
#include <vector>
#include <list>
#include <random>

using namespace glm;

extern std::vector<bone> treeSkeleton;


void update_final_matrices() {
	treeSkeleton[0].final = mat4(1) * treeSkeleton[0].translation * treeSkeleton[0].rotation;
	for (unsigned int i = 1; i < treeSkeleton.size(); i++)
	{
		vec4 p(0, 1, 0, 1);
		bone& bon = treeSkeleton[i];
		bon.final = treeSkeleton[bon.parent].final * bon.translation * bon.rotation;
		vec4 p2 = bon.final * p;
		std::cout << p2.x << " " << p2.y << " " << p2.z << "\n";


	}
}
class branch {
public:
	std::list<int> segments;
	int max_lenght = 10;
	vec4 point;
	branch(unsigned int parent_index,vec4 aim_point,int maxsegments) {
		update_final_matrices();


		point = aim_point;
		bone& attach_point = treeSkeleton[parent_index];
		max_lenght = maxsegments;
		bone b;
		b.parent = parent_index;
		b.lenght = 1;
		mat4 starting = attach_point.final;
		mat4 si = inverse(starting);
		vec3 newPoint = si * point;

		vec3 v1 = normalize(cross(newPoint, vec3(-1, -1, 0)));
		vec3 v2 = normalize(cross(newPoint, v1));
		vec3 nnp = normalize(newPoint);
		float a = dot(v2, v1), d = dot(v2, nnp), c = dot(nnp, v1);

		if (abs(dot(v2, v1)) > 0.001 || abs(dot(v2, nnp)) > 0.001 || abs(dot(nnp, v1)) > 0.001)
		{
			ASSERT(false);
		}

		mat4 rotation (v2.x, v2.y, v2.z, 0, nnp.x, nnp.y, nnp.z, 0, v1.x, v1.y, v1.z, 0, 0, 0, 0, 1);

		/*if (normalize(newPoint) == vec3(0, 1, 0))
		{
			b.rotation = mat4(1);
		}
		else {

			b.rotation = lookAt(vec3(0, 0, 0), vec3(1,0,0), (vec3)normalize(point));
		}*/
		b.rotation = rotation;
		b.translation = translate(mat4(1), vec3(0, 1, 0));
		treeSkeleton.push_back(std::move(b));
		segments.push_back(treeSkeleton.size() - 1);
	}

	float counter=0;
	void update(float delta) {
		update_final_matrices();

		std::normal_distribution<float> normal(1, 10);
		std::mt19937 gen(time(NULL));
		vec4 offset(normal(gen), normal(gen), normal(gen),0);

		counter += delta;
		if (counter < 2) {
			return;
		}
		std::cout << "adding bone\n";
		counter = 0;
		
		int last = *segments.rbegin();
		bone& attach_point = treeSkeleton[last];
		bone b;
		mat4 starting = attach_point.final;
		float a = determinant(starting);
		mat4 si = inverse(starting);
		vec3 newPoint = si * (point + offset);


		vec3 v1 = normalize(cross(newPoint, vec3(-1, -1, 1)));
		vec3 v2 = normalize(cross(newPoint, v1));
		vec3 nnp = normalize(newPoint);

		if (abs(dot(v2, v1)) > 0.001 || abs(dot(v2, nnp)) > 0.001 || abs(dot(nnp, v1)) > 0.001)
		{
			ASSERT(false);
		}
		

		mat4 rotation(v2.x, v2.y, v2.z, 0, nnp.x, nnp.y, nnp.z, 0, v1.x, v1.y, v1.z, 0, 0, 0, 0, 1);

		b.rotation = rotation;
		
		b.lenght = 1;
		b.parent = last;
		vec3 origin = starting * vec4(0, 0, 0, 1);

		b.translation = translate(mat4(1), vec3(0, 1, 0));
		treeSkeleton.push_back(std::move(b));
		segments.push_back(treeSkeleton.size() - 1);

		


	}


	
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
std::vector<branch> branches;

#pragma region Error

#define ASSERT(x) if(!(x)) __debugbreak();

#define GLCALL(x) ClearGLError();\
x;\
ASSERT(PrintGLError());



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





const  float common_vertecies[16] = {
0.1,0,0,1,
0,0,0.1,1,
-0.1,0,0,1,
0,0,-0.1,1
};







float rotationAngle = 0;
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

#pragma endregion


	
	

	
	



    glClearColor(0.2, 0.6, 0.7, 1);
	unsigned int vao;


    unsigned int buffer;
	



	GLCALL(glEnableVertexAttribArray(0));
	
	

    Shader bonesShader("Shader\\v_bones.glsl", "Shader\\f_constant.glsl", "Shader\\g_bones.glsl");
	bonesShader.Bind();


	

	mat4 proj = glm::perspective(90.0, WINDOWWIDTH / WINDOWHAIGHT, 0.01, 100.0);
	
	mat4 model = mat4(1);

	GLCALL(bonesShader.SetUniformMat4f("P", proj));
	
	GLCALL(bonesShader.SetUniformMat4f("M", model));

	
	bone b1;
	
	b1.translation = mat4(1);
	b1.rotation = mat4(1);
	b1.final = mat4(1);
	b1.lenght = 1;
	b1.layer = 1;




	treeSkeleton.push_back(b1);
	treeSkeleton.reserve(120);
	treeSkeleton[0].final = mat4(1) * treeSkeleton[0].translation * treeSkeleton[0].rotation;
	//treeSkeleton.push_back(b2);
	//treeSkeleton.push_back(b3);

	
	branch br(0, vec4(0, 10, 0, 1), 5);
	//generate_segemnt(0);

	br.update(2.1);
	br.update(2.1);
	br.update(2.1);
	br.update(2.1);
	br.update(2.1);
	br.update(2.1);
	br.update(2.1);

    while (!glfwWindowShouldClose(window))
    {
		rotationAngle += 1;
		mat4 view = glm::lookAt(vec3(4 * sin(glm::radians(rotationAngle)), 0, 4 * cos(glm::radians(rotationAngle))), vec3(0, 0, 0), vec3(0, 1, 0));
		GLCALL(bonesShader.SetUniformMat4f("V", view));
		glClear(GL_COLOR_BUFFER_BIT);


#pragma region RenderBones
		
		

		GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, common_vertecies));

		bonesShader.Bind();
		
		
		std::cout << treeSkeleton.size()<<"\n";

		update_final_matrices();

	
		for (unsigned int i = 0; i < treeSkeleton.size(); i++)
		{
			bone& bon = treeSkeleton[i];
			bonesShader.SetUniform4f("last", 0, bon.lenght, 0, 1);

			bonesShader.SetUniformMat4f("M", bon.final);
			GLCALL(glDrawArrays(GL_LINES_ADJACENCY, 0, 4))
		}




#pragma endregion











        glfwSwapBuffers(window);


        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}