//#include "stdio.h"
//#include <gl_core_4_4.h> 
//#include <GLFW/glfw3.h> 
//#include <Gizmos.h>
//#include <glm\glm.hpp>
//#include <glm\ext.hpp>
//#include <vector>
//#include "tiny_obj_loader.h"
//#include <iostream>
//#include <fstream>
//#include <string>
//
//using glm::vec2;
//using glm::vec3;
//using glm::vec4;
//using glm::mat4;
//
//unsigned int m_VAO; // Vertex Array Object
//unsigned int m_VBO; //Vertex Buffer Object
//unsigned int m_IBO; //Index Buffer Object
//unsigned int m_shader;
//unsigned int m_perlin_texture;
//unsigned int indexCount;
//float *perlin_data;
//
//GLFWwindow *window;
//
//mat4 m_view;
//mat4 m_projection;
//mat4 m_projectionViewMatrix;
//mat4 modelMatrix;
//
//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//
//struct OpenGLInfo
//{
//	unsigned int m_VAO;
//	unsigned int m_VBO;
//	unsigned int m_IBO;
//	unsigned int m_index_count;
//};
//
//std::vector<OpenGLInfo> m_gl_info;
//
//struct MyVertex
//{
//		float x, y, z;		//Vertex
//		float nx, ny, nz;	//Normal
//};
//
// //Comment out createTriangles because I don't need it anymore, for now.
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
//	//// Setting all these bind buffers to 0 means they are reseted. 
//	glBindVertexArray(0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//}
//
// ////This is good, don't touch it
//void createOpenGLBuffer(std::vector<tinyobj::shape_t> &shapes)
//{
//	m_gl_info.resize(shapes.size());
//	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index)
//	{
//		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
//		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
//		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
//		glBindVertexArray(m_gl_info[mesh_index].m_VAO);
//
//		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
//		float_count += shapes[mesh_index].mesh.normals.size();
//		float_count += shapes[mesh_index].mesh.texcoords.size();
//
//		std::vector<float> vertex_data;
//		vertex_data.reserve(float_count);
//
//		vertex_data.insert(vertex_data.end(),
//			shapes[mesh_index].mesh.positions.begin(),
//			shapes[mesh_index].mesh.positions.end());
//
//		vertex_data.insert(vertex_data.end(),
//			shapes[mesh_index].mesh.normals.begin(),
//			shapes[mesh_index].mesh.normals.end());
//
//		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();
//
//		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
//		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//			shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int),
//			shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0); // Position
//		glEnableVertexAttribArray(1); // normal data
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	}
//}
//
//// This is good, don't touch it
//void DrawOBJ()
//{
//	glUseProgram(m_shader);
//	int projectionViewUniform = glGetUniformLocation(m_shader, "ProjectionView");
//	int modelID = glGetUniformLocation(m_shader, "Model");
//
//	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, glm::value_ptr(m_projectionViewMatrix));
//	glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(modelMatrix));
//
//	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
//	{
//		glBindVertexArray(m_gl_info[i].m_VAO);
//		glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
//	}
//}