#include "stdio.h"
#include <gl_core_4_4.h> 
#include <GLFW/glfw3.h> 
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include "tiny_obj_loader.h"
#include <iostream>
#include <fstream>
#include <string>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

unsigned int m_VAO; // Vertex Array Object
unsigned int m_VBO; //Vertex Buffer Object
unsigned int m_IBO; //Index Buffer Object
unsigned int m_shader;
unsigned int m_perlin_texture;
unsigned int indexCount;

GLFWwindow *window;

mat4 m_view;
mat4 m_projection;
mat4 m_projectionViewMatrix;
mat4 modelMatrix;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct OpenGLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_count;
};

std::vector<OpenGLInfo> m_gl_info;

// For Triangles only
//struct MyVertex
//{
//	float x, y, z;		//Vertex
//	float nx, ny, nz;	//Normal
//};

struct Vertex 
{
	vec4 position;
	vec4 colour;
};

int Window()
{
	if (glfwInit() == false)
		return -1;

	window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	//the rest of our code goes here!
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	//testing what version of OpenGL we are running
	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf_s("GL: %i.%i\n", major, minor);

	return 0;
}

std::string readShader(std::string rs)
{
	std::string line;
	std::string allTheLines;
	std::ifstream file;

	file.open(rs);

	if (file.is_open())
	{
		// As opening the file, the string line copies the first line and adds it to addingStrings
		// while in a loop, it grabs all the information from that file and adds it together.
		while (std::getline(file, line))
		{
			allTheLines += line + '\n';
		}
		std::cout << allTheLines;
		file.close();
	}

	else std::cout << "Unable to open file";

	return allTheLines;
}

void Shader()
{
	// Read in from text file
	std::string readVS = readShader("vertexshader.txt");
	std::string readFS = readShader("fragmentshader.txt");
	std::string readT = readShader("texcoord.txt");
	std::string readFT = readShader("fragtexcoord.txt");

	const char* vsSource = readVS.c_str();
	const char* fsSource = readFS.c_str();
	const char* tSource = readT.c_str();
	const char* ftSource = readFT.c_str();


	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Vertex Shader
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glShaderSource(vertexShader, 1, (const char**)&tSource, 0);
	glCompileShader(vertexShader);

	//Fragment Shader
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glShaderSource(fragmentShader, 1, (const char**)&ftSource, 0);
	glCompileShader(fragmentShader);

	m_shader = glCreateProgram();
	glAttachShader(m_shader, vertexShader);
	glAttachShader(m_shader, fragmentShader);
	// m_shader goes into the text file and grabs Position and Colour.
	glBindAttribLocation(0, m_shader, "Position");
	glBindAttribLocation(1, m_shader, "Colour");
	glLinkProgram(m_shader);
	
	//Check if shader works (don't really need it)
	int success = GL_FALSE;
	glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
	if (success = GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}
}

// Creating grid
void generateGrid(unsigned int rows, unsigned int cols)
{
	indexCount = (rows - 1) * (cols - 1) * 6;
	Vertex* aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			// create some arbitrary colour based off something // What?
			// that might not be related to tiling a texture // Hmmm What?
			aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1);
			vec3 colour = vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
			aoVertices[r * cols + c].colour = vec4(colour, 1);
		}
	}

	unsigned int* auiIndices = new unsigned int[indexCount];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			//Triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);

			// Triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	delete[] aoVertices;
	delete[] auiIndices;
}

//void createTriangles()
//{
//	// These are the points to make the shape of the triangle, but can be used to make something else like a sqaure
//	MyVertex pvertex[4];
//	//Vertex 0 
//	pvertex[0].x = 0.0;
//	pvertex[0].y = 0.0;
//	pvertex[0].z = 0.0;
//
//	//Vertex 1
//	pvertex[1].x = 0.0;
//	pvertex[1].y = 3.0;
//	pvertex[1].z = 0.0;
//
//	//Vertex 2
//	pvertex[2].x = 3.0;
//	pvertex[2].y = 3.0;
//	pvertex[2].z = 0.0;
//
//	//Vertex 3
//	pvertex[3].x = 3.0;
//	pvertex[3].y = 0.0;
//	pvertex[3].z = 0.0;
//
//	//pindices of how we draw the triangle
//	unsigned int pindices[6];
//	//First Triangle
//	pindices[0] = 0;
//	pindices[1] = 1;
//	pindices[2] = 2;
//
//	//Second Triangle
//	pindices[3] = 0;
//	pindices[4] = 2;
//	pindices[5] = 3;
//
//	// To talk/communcation to the graphics card, you first generate the vertex buffer
//	// Then you bind the vertex buffer to the reference of the object you want.
//	// After that, then you populate the data/fill in the buffer that you created with information
//
//	// Vertex Array Object
//	glGenVertexArrays(1, &m_VAO); 
//	glBindVertexArray(m_VAO);
//
//	// Vertex buffer object
//	glGenBuffers(1, &m_VBO); 
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * 4, pvertex, GL_STATIC_DRAW);
//
//	// Index buffer object
//	glGenBuffers(1, &m_IBO); 
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, pindices, GL_STATIC_DRAW);
//
//	// Setting all these bind buffers to 0 means they are reseted. 
//	glBindVertexArray(0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//}

void createOpenGLBuffer(std::vector<tinyobj::shape_t> &shapes)
{
	m_gl_info.resize(shapes.size());
	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);

		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();

		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.positions.begin(),
			shapes[mesh_index].mesh.positions.end());

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.normals.begin(),
			shapes[mesh_index].mesh.normals.end());

		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int),
			shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Position
		glEnableVertexAttribArray(1); // normal data

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void DrawSquare()
{
	// Use shader
	// view_proj_uniform and modelID get the ProjectionView and Model fom text file
	glUseProgram(m_shader);
	unsigned int projectionViewUniform = glGetUniformLocation(m_shader, "ProjectionView");
	unsigned int modelID = glGetUniformLocation(m_shader, "Model");

	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_projectionViewMatrix));
	glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glBindVertexArray(m_VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

void DrawOBJ()
{
	glUseProgram(m_shader);
	int projectionViewUniform = glGetUniformLocation(m_shader, "ProjectionView");
	int modelID = glGetUniformLocation(m_shader, "Model");

	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_projectionViewMatrix));
	glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	}
}

void PRNG()
{
	int dims = 64;
	float *perlin_data = new float[dims * dims];
	float scale = (1.0f / dims) * 3;

	for (int x = 0; x < 64; ++x)
	{
		for (int y = 0; y < 64; ++y)
		{
			// generate noise here
			perlin_data[y* dims + x] = glm::perlin(vec2(x, y) * scale) * 0.5f + 0.5f;
		}
	}

	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 64, 64, 0, GL_RED, GL_FLOAT, perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

int main()
{
	// Create window and camera
	m_view = glm::lookAt(vec3(10, 10, 64), vec3(0), vec3(0, 1, 0));
	m_projection = glm::perspective(glm::pi<float>()*0.25f, 16 / 9.f, 0.1f, 1000.f); // Don't know the first one, 16 by 9 is the ratio, 0.1f inner, 1000f is outer.
	m_projectionViewMatrix = m_projection * m_view;

	std::vector<tinyobj::shape_t> shapes;
	std::vector <tinyobj::material_t> materials;
	std::string err;

	//tinyobj::LoadObj(shapes, materials, err, "./model/dragon.obj");
	//tinyobj::LoadObj(shapes, materials, err, "./model/bunny.obj");
	//tinyobj::LoadObj(shapes, materials, err, "./model/buddha.obj");

	Window();
	Shader();

	generateGrid(64, 64);
	//createOpenGLBuffer(shapes); // For Models
	//createTriangles(); // For Triangles

	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		DrawSquare();
		//DrawOBJ();

		/*if (glfwGetKey(window, GLFW_KEY_A))
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 0, 0));
		if (glfwGetKey(window, GLFW_KEY_D))
			modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 0, 0));
		if (glfwGetKey(window, GLFW_KEY_W))
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
		if (glfwGetKey(window, GLFW_KEY_S))
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -1, 0));
		if (glfwGetKey(window, GLFW_KEY_Q))
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -1));
		if (glfwGetKey(window, GLFW_KEY_E))
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 1));
		if (glfwGetKey(window, GLFW_KEY_Z))
			modelMatrix *= glm::rotate(0.05f, glm::vec3(0, 0, -1));
		if (glfwGetKey(window, GLFW_KEY_X))
			modelMatrix *= glm::rotate(0.05f, glm::vec3(0, 0, 1));*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}