#pragma once
#include "stdio.h"
#include <gl_core_4_4.h> 
#include <GLFW/glfw3.h> 
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Application
{

public :
	Application();	
	int startup();
	int update();
	void draw();
	void shutdown();
	GLFWwindow *window;
private:
	int m_x;
	int m_y;
	float m_Timer;
	mat4 m_view;
	mat4 m_projection;
	vec4 m_White;
	vec4 m_Yellow;
	vec4 m_Black;
	vec4 m_Red;
	vec4 m_Orange;
	vec4 m_Blue;
	vec4 m_Gray;
	vec4 m_LightRed;
	vec4 m_Tan;
	vec4 m_Teal;
	vec4 planetColors[10] = { m_White, m_Gray, m_LightRed, m_Red, m_Blue, m_Tan, m_Teal, m_Yellow, m_Black, m_Orange };
};