#include <iostream>
#include <fstream>


#include "ShaderProgram.h"


#include "teapotTest.h"
#include "Utils.h"
#include "Bone.h"
#include "Camera.h"
#include <vector>
#include <list>
#include <random>

#include <fstream>

using namespace glm;


#pragma region defines
#define WINDOWWIDTH 640.0
#define WINDOWHAIGHT 480.0
#define VERTECIESPERLEVEL 8


#define BONELIMIT 150


class branch;
#pragma endregion


#pragma region golobal_variables

extern std::vector<bone> treeSkeleton;


float symulation_time=0.0f;
float scaled_delta;


std::vector<branch> branches;




float* vertices = myTeapotVertices;
float* normals = myTeapotVertexNormals;
float* texCoords = myTeapotTexCoords;
float* colors = myTeapotColors;
int vertexCount = myTeapotVertexCount;

float* c1 = myTeapotC1;
float* c2 = myTeapotC2;
float* c3 = myTeapotC3;


const  float common_vertecies[16] = {
0.1,0,0,1,
0,0,0.1,1,
-0.1,0,0,1,
0,0,-0.1,1
};

std::normal_distribution<float> normal_dist(0, 0.75);
std::mt19937 generator(time(NULL));




#pragma endregion 

std::vector<vec4> treeSegment;

std::vector<vec4> tangnent;
std::vector<vec4> bitangnent;
std::vector<vec4> normal;
std::vector<vec2> coords;
void generate_vertecies()
{

	std::vector<vec4> vertecies;
	std::vector<vec2> texcoords;
	std::vector<unsigned int> indecies;
	float radius = 0.3;
	int verteciesPerLevel = 9;

	float increment = 360.0f / verteciesPerLevel;

	float levelHeight = 0.5;
	int levelCount = 2;


	vertecies.reserve(verteciesPerLevel * levelCount);

	// vertex* treeArray = new vertex[levelCount * verteciesPerLevel]{ };


	unsigned int* treeIndecies = new unsigned int[levelCount * verteciesPerLevel] {0};
	for (int j = 0; j <= levelCount; j++)
	{
		float x = radius, z = 0, pomx;


		vertecies.push_back(vec4(x, j * levelHeight, z, 1.0f));
		texcoords.push_back(vec2(0, j * levelCount));




		for (int i = 1; i < verteciesPerLevel; i++)
		{
			pomx = cos(glm::radians(increment)) * x - z * sin(glm::radians(increment));
			z = sin(glm::radians(increment)) * x + z * cos(glm::radians(increment));

			x = pomx;

			vertecies.push_back(vec4(x, j * levelHeight, z, 1.0f));

			texcoords.push_back(vec2((float)i/ (float)verteciesPerLevel, j * levelCount));



		}
	}


	indecies.reserve(verteciesPerLevel * levelCount * 3);
	for (unsigned int level = 0; level < levelCount; level++)
	{

		for (unsigned int x = 0; x < verteciesPerLevel; x++)
		{
			unsigned int i = x + level * verteciesPerLevel;

			indecies.push_back(i);
			indecies.push_back(i + verteciesPerLevel);
			indecies.push_back((i % verteciesPerLevel == verteciesPerLevel - 1) ? (level * verteciesPerLevel) : i + 1);



			indecies.push_back(i + verteciesPerLevel);
			indecies.push_back((i % verteciesPerLevel == verteciesPerLevel - 1) ? i + 1 : i + verteciesPerLevel + 1);
			indecies.push_back((i % verteciesPerLevel == verteciesPerLevel - 1) ? (level * verteciesPerLevel) : (i + 1));

		}

	}


	treeSegment.reserve(indecies.size());

	for (int i =0;i<indecies.size()/3;i++)
	{


		vec4 pa = vertecies[indecies[3 * i]],
			pb = vertecies[indecies[3 * i + 1]],
			pc = vertecies[indecies[3 * i + 2]];

		vec3 v1 = pc - pa, v2 = pb - pa;

		vec3 norm = normalize(cross(v1, v2));
		vec4 tg = vec4(v1,0);
		vec3 btg = normalize(cross(v1, norm));
		for (int x = 0; x < 3; x++)
		{
		treeSegment.push_back(vertecies[indecies[3 * i + x]]);
		tangnent.push_back(tg);
		bitangnent.push_back(vec4(btg,0));
		normal.push_back(vec4(norm,0));



		}




		


	}



}




void update_final_matrices() {
	treeSkeleton[0].final = mat4(1) * treeSkeleton[0].translation * treeSkeleton[0].rotation;
	for (unsigned int i = 1; i < treeSkeleton.size(); i++)
	{
		bone& bon = treeSkeleton[i];
		bon.final =  translate(treeSkeleton[bon.parent].final,vec3(0, treeSkeleton[bon.parent].lenght,0)) * bon.rotation;
	


	}
}
class branch {
public:

	float multiplayer = 1;
	std::vector<int> segments;
	int max_lenght = 10;
	vec4 point;
	branch(unsigned int parent_index,vec4 aim_point,int maxsegments) {
		update_final_matrices();
		normal = std::normal_distribution<float>(0, 0.15);
		gen = std::mt19937(time(NULL));

		point = aim_point;
		bone& attach_point = treeSkeleton[parent_index];
		max_lenght = maxsegments;
		bone b;
		b.creation_time = symulation_time;
		b.multiplayer = treeSkeleton[parent_index].multiplayer - 0.1;
		b.parent = parent_index;
		b.lenght = 0.1;
		b.layer = attach_point.layer + 1;
		mat4 starting = attach_point.final;
		mat4 si = inverse(starting);
		vec3 newPoint = si * point;

		vec3 v1 = normalize(cross(newPoint, vec3(-1, -1, 0)));
		vec3 v2 = normalize(cross(newPoint, v1));
		vec3 nnp = normalize(newPoint);



		mat4 rotation (v2.x, v2.y, v2.z, 0, nnp.x, nnp.y, nnp.z, 0, v1.x, v1.y, v1.z, 0, 0, 0, 0, 1);

		b.rotation = rotation;
		treeSkeleton.push_back(std::move(b));
		segments.push_back(treeSkeleton.size() - 1);
	}

	float counter=0;
	std::normal_distribution<float> normal;
	std::mt19937 gen;
	void update(float delta) {
		update_final_matrices();

		
		vec4 offset(normal(gen), 1, normal(gen),1);

		counter += delta;
		if (counter < 0.5) {
			return;
		}
		max_lenght++;
		counter = 0;
		
		
		int x = segments.size() - 1;
		int last = segments[x];
		bone& attach_point = treeSkeleton[last];
		bone b;
		mat4 starting = attach_point.final;
		vec3 newPoint =  (offset);


		vec3 v1 = normalize(cross(newPoint, vec3(newPoint.y, newPoint.z, newPoint.x)));
		vec3 v2 = normalize(cross(newPoint, v1));
		vec3 nnp = normalize(newPoint);


		mat4 rotation(v2.x, v2.y, v2.z, 0, nnp.x, nnp.y, nnp.z, 0, v1.x, v1.y, v1.z, 0, 0, 0, 0, 1);

		b.rotation = rotation;
		b.creation_time = symulation_time;
		b.multiplayer = attach_point.multiplayer;
		b.lenght = 0;
		b.parent = last;
		b.layer = attach_point.layer;
		vec3 origin = starting * vec4(0, 0, 0, 1);
		treeSkeleton.push_back(std::move(b));
		segments.push_back(treeSkeleton.size() - 1);

		
		if (segments.size() == 3 || segments.size() == 6 || segments.size() == 8) {
			int pom = rand() % (segments.size()/2) + segments.size()/2;
			int index = segments[pom];

			vec4 point(10 * normal_dist(generator), clamp(12 * normal_dist(generator),7.0f,15.0f),10 * normal_dist(generator), 1);
			branch b(index, point, clamp((int)(10 - treeSkeleton[index].layer * 2), 1, 10));
			branches.push_back(std::move(b));
		}

	}
	
};









void error_callback(int error, const char* description) {
	fputs(description, stderr);
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

#pragma endregion


	generate_vertecies();







	glClearColor(0.2, 0.6, 0.7, 1);
	unsigned int vao;


	unsigned int buffer;




	GLCALL(glEnableVertexAttribArray(0));
    Shader bonesShader("Shader\\v_bones.glsl", "Shader\\f_bones.glsl", "Shader\\g_bones.glsl");
	
	GLCALL(glEnableVertexAttribArray(4));  //Vertices position
	//Normal mapping
	GLCALL(glEnableVertexAttribArray(3));  //c3
	GLCALL(glEnableVertexAttribArray(2));  //c2
	GLCALL(glEnableVertexAttribArray(1));  //c1
	
	//Shader treeShader("Shader\\v_constant.glsl", "Shader\\f_constant.glsl", "");

	Shader testShader("Shader\\v_test.glsl", "Shader\\f_bones.glsl");


	bone b1;

	b1.translation = mat4(1);
	b1.rotation = mat4(1);
	b1.final = mat4(1);
	b1.lenght = 1;
	b1.layer = 1;




	treeSkeleton.push_back(b1);
	treeSkeleton.reserve(120);
	treeSkeleton[0].final = mat4(1) * treeSkeleton[0].translation * treeSkeleton[0].rotation;
	branch br(0, vec4(0, 10, 0, 1), 5);
	branches.push_back(std::move(br));






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
	GLCALL(bonesShader.SetUniformMat4f("V", view));

	scaled_delta = deltaTime * 0.1;
	symulation_time += scaled_delta;


		// Get the view matrix from the camera and pass it to the shader
#pragma endregion Camera
		
		bonesShader.Bind();
		mat4 proj = glm::perspective(90.0, WINDOWWIDTH / WINDOWHAIGHT, 0.01, 100.0);
		GLCALL(bonesShader.SetUniformMat4f("P", proj));
		GLCALL(bonesShader.SetUniformMat4f("V", view));

		
		glClear(GL_COLOR_BUFFER_BIT);
		
#pragma region RenderBones



GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, common_vertecies));

bonesShader.Bind();






for (bone& b : treeSkeleton) {
	
	b.update();
}
if(treeSkeleton.size() < BONELIMIT){
for (branch& br : branches) {
	br.update(scaled_delta);
}
}

update_final_matrices();


for (unsigned int i = 1; i < treeSkeleton.size(); i++)
{
	bone& bon = treeSkeleton[i];
	bonesShader.SetUniform4f("last", 0, bon.lenght, 0, 1);

	bonesShader.SetUniformMat4f("M", bon.final);
	GLCALL(glDrawArrays(GL_LINES_ADJACENCY, 0, 4))
}




#pragma endregion
		
		testShader.Bind();
		
		GLCALL(testShader.SetUniformMat4f("P", proj));
		GLCALL(testShader.SetUniformMat4f("V", view));
	

		mat4 cubeModel = mat4(1);
		testShader.SetUniformMat4f("M", cubeModel);

		GLCALL(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, vertices));  //Position
		GLCALL(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, c3));  //c3
		GLCALL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, c2));  //c2
		GLCALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, c1));  //c1

		
		//GLCALL(glDrawArrays(GL_TRIANGLES, 0, vertexCount))

GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, &treeSegment[0]));

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
for (unsigned int i = 1; i < treeSkeleton.size(); i++)
{
	bone& bon = treeSkeleton[i];

	mat4 f = scale(bon.final, vec3(bon.lenght, bon.lenght, bon.lenght));
	testShader.SetUniformMat4f("M", f);
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, treeSegment.size()));
}

	
//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);








	glfwSwapBuffers(window);


	glfwPollEvents();
}

glfwTerminate();
return 0;
	}
