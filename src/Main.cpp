#include "stdio.h"
#include <gl_core_4_4.h> 
#include <GLFW/glfw3.h> 
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include "MyApplication.h"
#include "tiny_obj_loader.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

unsigned int m_VAO; // Vertex Array Object
unsigned int m_VBO; //Vertex Buffer Object
unsigned int m_IBO; //Index Buffer Object
unsigned int m_shader;

GLFWwindow *window;

mat4 m_view;
mat4 m_projection;
mat4 m_projectionViewMatrix;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

typedef struct mesh_t
{
	std::vector<float> positions;
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<unsigned int> indices;
	std::vector <int> material_ids;
};

typedef struct shape_t
{
	std::string name;
	mesh_t mesh;
};

struct OpenGLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_count;
};

std::vector<OpenGLInfo> m_gl_info;

struct MyVertex
{
	float x, y, z;		//Vertex
	float nx, ny, nz;	//Normal
						/*vec4 position; vec4 colour;*/
};

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

	glUseProgram(m_shader);
	int view_proj_uniform = glGetUniformLocation(m_shader, "ProjectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(m_projectionViewMatrix));

	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
	}
}

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

	// Create window and camera
	m_view = glm::lookAt(vec3(0, 0, 50), vec3(0), vec3(0, 1, 0));
	m_projection = glm::perspective(glm::pi<float>()*0.25f, 16 / 9.f, 0.1f, 1000.f); // Don't know the first one, 16 by 9 is the ratio, 0.1f inner, 1000f is outer.
	m_projectionViewMatrix = m_projection * m_view;

	return 0;
}

void Shader()
{
	//Create Shaders
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Colour; \
							out vec4 vColour; \
							uniform mat4 ProjectionView; \
							void main() { vColour = Colour; gl_Position = ProjectionView * Position; }";

	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 FragColor; \
							void main() { FragColor = vColour; }";

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Vertex Shader
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	//Fragment Shader
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_shader = glCreateProgram();
	glAttachShader(m_shader, vertexShader);
	glAttachShader(m_shader, fragmentShader);
	glBindAttribLocation(0, m_shader, "Position");
	glBindAttribLocation(1, m_shader, "Colour");
	glLinkProgram(m_shader);

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

void createTriangle()
{
	// These are the points to make the shape of the triangle, but can be used to make something else like a sqaure
	MyVertex pvertex[4];
	//Vertex 0 
	pvertex[0].x = 0.0;
	pvertex[0].y = 0.0;
	pvertex[0].z = 0.0;

	//Vertex 1
	pvertex[1].x = 0.0;
	pvertex[1].y = 3.0;
	pvertex[1].z = 0.0;

	//Vertex 2
	pvertex[2].x = 3.0;
	pvertex[2].y = 3.0;
	pvertex[2].z = 0.0;

	//Vertex 3
	pvertex[3].x = 3.0;
	pvertex[3].y = 0.0;
	pvertex[3].z = 0.0;

	// Create and bind buffers to a vertex array object
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * 4, pvertex, GL_STATIC_DRAW);

	unsigned int pindices[6];
	//First Triangle
	pindices[0] = 0;
	pindices[1] = 1;
	pindices[2] = 2;

	//Second Triangle
	pindices[3] = 0;
	pindices[4] = 2;
	pindices[5] = 3;

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, pindices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(m_shader); //// m_shader = Red Sqaure
	unsigned int projectionViewUniform = glGetUniformLocation(m_shader, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionViewMatrix));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
	glBindVertexArray(0);
}

int main()
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector <tinyobj::material_t> materials;
	std::string err;
	tinyobj::LoadObj(shapes, materials, err, "./model/dragon.obj");
	tinyobj::LoadObj(shapes, materials, err, "./model/bunny.obj");
	tinyobj::LoadObj(shapes, materials, err, "./model/buddha.obj");

	Window();
	Shader();
	
	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		createOpenGLBuffer(shapes);
		//createTriangle();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

// Creating grid
//void generateGrid(unsigned int rows, unsigned int cols)
//{
//	//Vertex* aoVertices = new Vertex[rows * cols];
//	//for (unsigned int r = 0; r < rows; ++r)
//	//{
//	//	for (unsigned int c = 0; c < cols; ++c)
//	//	{
//	//		aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1);
//	//		// create some arbitrary colour based off something // What?
//	//		// that might not be related to tiling a texture // Hmmm What?
//	//		vec3 colour = vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
//	//		aoVertices[r * cols + c].colour = vec4(colour, 1);
//	//	}
//	//}
//
//	//unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
//	//unsigned int index = 0;
//	//for (unsigned int r = 0; r < (rows - 1); ++r)
//	//{
//	//	for (unsigned int c = 0; c < (cols - 1); ++c)
//	//	{
//	//		//Triangle 1
//	//		auiIndices[index++] = r * cols + c;
//	//		auiIndices[index++] = (r + 1) * cols + c;
//	//		auiIndices[index++] = (r + 1) * cols + (c + 1);
//
//	//		// Triangle 2
//	//		auiIndices[index++] = r * cols + c;
//	//		auiIndices[index++] = (r + 1) * cols + (c + 1);
//	//		auiIndices[index++] = r * cols + (c + 1);
//	//	}
//	//}
//}