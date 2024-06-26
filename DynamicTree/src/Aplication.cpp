#include <iostream>
#include <fstream>


#include "ShaderProgram.h"

#include "lodepng.h"
#include "cubeTest.h"
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




#define WINDOWWIDTH 1000.0
#define WINDOWHAIGHT 1000.0

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}



GLuint tex0;
GLuint tex1;
GLuint tex2;

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

std::normal_distribution<float> normal_dist(0, 0.75);
std::uniform_real_distribution<float> real(0.0, 1.0);
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

			indecies.push_back((i % verteciesPerLevel == verteciesPerLevel - 1) ? (level * verteciesPerLevel) : i + 1);
			indecies.push_back(i);
			indecies.push_back(i + verteciesPerLevel);



			indecies.push_back((i % verteciesPerLevel == verteciesPerLevel - 1) ? (level * verteciesPerLevel) : (i + 1));
			indecies.push_back(i + verteciesPerLevel);
			indecies.push_back((i % verteciesPerLevel == verteciesPerLevel - 1) ? i + 1 : i + verteciesPerLevel + 1);

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
		coords.push_back(texcoords[indecies[3 * i + x]]);



		}




		


	}



}

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pami�ci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do kt�rych wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pami�ci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pami�ci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}



std::vector<vec4> leaves_quads;


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

		if (segments.size() > 3) {
			for (int x = 0; x < segments.size();x++)
			{
				if (real(generator) > 0.1);
				{
					leaves_quads.push_back(vec4(normal(generator), normal(generator), normal(generator),0));
				}
			}
		}

		
		if (segments.size() == 3 || segments.size() == 6 || segments.size() == 8) {
			int pom = rand() % (segments.size()/2) + segments.size()/2;
			int index = segments[pom];

			vec4 point(10 * normal_dist(generator), 12 * normal_dist(generator)+ 16,10 * normal_dist(generator), 1);
			branch b(index, point, clamp((int)(10 - treeSkeleton[index].layer * 2), 1, 10));
			branches.push_back(std::move(b));
		}

	}
	
};




bool render_bones = false, render_bark = true;


void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {

		if (key == GLFW_KEY_B)
		{
			render_bones = !render_bones;
		}
		if (key == GLFW_KEY_N)
		{
			render_bark = !render_bark;
		}

		
	}
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
	glfwSetKeyCallback(window, key_callback);
	glEnable(GL_DEPTH_TEST);
	tex0 = readTexture("src/wood_color.png");
	tex1 = readTexture("src/wood_normal.png");
	tex2 = readTexture("src/liscie1.png");
	
#pragma endregion


	generate_vertecies();

	glClearColor(0.2, 0.6, 0.7, 1);
	
	
    Shader bonesShader("Shader\\v_bones.glsl", "Shader\\f_bones.glsl", "Shader\\g_bones.glsl");
	
	Shader treeShader("Shader\\v_constant.glsl", "Shader\\f_constant.glsl", "");

	Shader leavesShader("Shader\\v_liscie.glsl", "Sahder\\f_liscie.glsl");

	//Texture
	GLCALL(glEnableVertexAttribArray(5));  
	//Vertices position
	GLCALL(glEnableVertexAttribArray(4));  
	//Normal mapping
	GLCALL(glEnableVertexAttribArray(3));  //c3
	GLCALL(glEnableVertexAttribArray(2));  //c2
	GLCALL(glEnableVertexAttribArray(1));  //c1
	
	//Shader treeShader("Shader\\v_constant.glsl", "Shader\\f_constant.glsl", "");
	//Bones
	GLCALL(glEnableVertexAttribArray(0));

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

	float quad[] = {
		-1,1,0,1,
		1,1,0,1,
		-1,-1,0,1,

		1,1,0,1,
		-1,-1,0,1,
		1,-1,0,1
	};
	float quad_coords[] =
	{
		0,1,
		1,1,
		0,0,

		1,1,
		0,0,
		1,0

	};




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

	scaled_delta = deltaTime * 0.1;
	symulation_time += scaled_delta;
		
#pragma endregion Camera
		
		bonesShader.Bind();
		mat4 proj = glm::perspective(90.0, WINDOWWIDTH / WINDOWHAIGHT, 0.01, 100.0);
		GLCALL(bonesShader.SetUniformMat4f("P", proj));
		GLCALL(bonesShader.SetUniformMat4f("V", view));

		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		



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

if (render_bones) {

	for (unsigned int i = 1; i < treeSkeleton.size(); i++)
	{
		bone& bon = treeSkeleton[i];
		bonesShader.SetUniform4f("last", 0, bon.lenght, 0, 1);

		bonesShader.SetUniformMat4f("M", bon.final);
		GLCALL(glDrawArrays(GL_LINES_ADJACENCY, 0, 4))
	}

}


#pragma endregion
		
#pragma region RenderTree
if (render_bark) {
	treeShader.Bind();

	GLCALL(treeShader.SetUniformMat4f("P", proj));
	GLCALL(treeShader.SetUniformMat4f("V", view));



	GLCALL(glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 0, &coords[0])); //Texture
	GLCALL(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, &treeSegment[0])); //Position
	GLCALL(glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, &normal[0]));  //c3
	GLCALL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, &bitangnent[0]));  //c2
	GLCALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, &tangnent[0]));

	GLCALL(treeShader.SetUniform1i("textureMap0", 0))
		glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);
	GLCALL(treeShader.SetUniform1i("textureMap1", 1))
		glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);


	for (unsigned int i = 1; i < treeSkeleton.size(); i++)
	{
		bone& bon = treeSkeleton[i];

		mat4 f = scale(bon.final, vec3(bon.lenght, bon.lenght, bon.lenght));
		treeShader.SetUniformMat4f("M", f);
		GLCALL(glDrawArrays(GL_TRIANGLES, 0, treeSegment.size()));
	}

}
//leavesShader.Bind();
//GLCALL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, quad_coords));  //c2
//GLCALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, quad));
//
//GLCALL(treeShader.SetUniformMat4f("P", proj));
//GLCALL(treeShader.SetUniformMat4f("V", view));
//
//GLCALL(treeShader.SetUniform1i("tex", 2))
//glActiveTexture(GL_TEXTURE2);
//glBindTexture(GL_TEXTURE_2D, tex2);
//for(vec3 quad :leaves_quads)
//{
//	mat4 leaves_model = mat4(1); lookAt(quad, camera.Position, vec3(0, 1, 0));
//	leavesShader.SetUniformMat4f("M", leaves_model);
//
//	glDrawArrays(GL_TRIANGLES, 0, 6);
//
//}







		
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
