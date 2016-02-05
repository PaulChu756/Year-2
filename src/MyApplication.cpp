#include "MyApplication.h"

Application::Application()
{
	//Camera
	m_projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	// Colors
	planetColors[0] = vec4(1, 1, 1, 1); // White
	planetColors[1] = vec4(0.5, 0.5, 0.5, 1); // Grey
	planetColors[2] = vec4(1, .42, .42, 1); // Light Red 
	planetColors[3] = vec4(0, 0, 1, 1); // Blue
	planetColors[4] = vec4(1, 0, 0, 1); // Red
	planetColors[5] = vec4(1, 0.8, 0.6, 1); // Tan
	planetColors[6] = vec4(0, 1, 1, 1); //m_Teal
	planetColors[7] = vec4(1, 0.8, 0.1, 1); // Yellow
	planetColors[8] = vec4(0, 0, 0, 1); // Black
	planetColors[9] = vec4(1, .5, 0, 1); // Orange 
}

// Planet orbit
mat4 PlanetOrbit(float local_rotation, float radius, float orbit_rotation) // Rotation
{
	mat4 result = glm::rotate(orbit_rotation, vec3(0, 1, 0)) * glm::translate(vec3(radius, 0, 0)) * glm::rotate(local_rotation, vec3(0, 1, 0));

	return result;
}

int Application::startup()
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

	//Gizmos::create();

	return 1;
}

int Application::update()
{
	//Delta Time
	float dt = glfwGetTime();
	glfwSetTime(0.0f);
	m_Timer += dt / 15;

	m_view = glm::lookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	//Gizmos::clear();
	//Gizmos::addTransform(glm::mat4(1));

	//// All the Black squares with the 4 long white lines to put them in Q's
	//vec4 white(1);
	//vec4 black(0, 0, 0, 1);
	//for (int i = 0; i < 21; ++i)
	//{
	//	Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10), i == 10 ? white : black);
	//	Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i), i == 10 ? white : black);
	//}

	//// If you look at many of the Gizmo add methods that have a default parameter for a mat4 pointer that 
	//// is nullptr or mat4(1) by default.This parameter can help you spin the planets and moons

	//// Color is a vec4 Red, green, blue and alpha

	//// Sun just rotates
	//mat4 sun = PlanetOrbit(m_Timer, 0, 0);
	//Gizmos::addSphere(vec3(sun[1].x, sun[1].y, sun[1].z), 1.5f, 10, 10, planetColors[9], &sun);

	//for (int i = 0; i < 9; i++)
	//{
	//	if (i == 6 || i == 7)
	//	{
	//		mat4 planetMatrix = sun * PlanetOrbit(m_Timer / (float)i, 2 * i, m_Timer * (float)i);
	//		Gizmos::addSphere(vec3(planetMatrix[3].x, planetMatrix[3].y, planetMatrix[3].z), .5f, 10, 10, planetColors[i - 1], &planetMatrix);
	//	}

	//	if (i == 8)
	//	{
	//		mat4 planetMatrix = sun * PlanetOrbit(m_Timer / (float)i, 2 * i, m_Timer * (float)i);
	//		Gizmos::addSphere(vec3(planetMatrix[3].x, planetMatrix[3].y, planetMatrix[3].z), .5f, 10, 10, planetColors[3], &planetMatrix);
	//	}

	//	mat4 planetMatrix = sun * PlanetOrbit(m_Timer / (float)i, 2 * i, m_Timer * (float)i);
	//	Gizmos::addSphere(vec3(planetMatrix[3].x, planetMatrix[3].y, planetMatrix[3].z), .5f, 10, 10, planetColors[i], &planetMatrix);
	//}
	return true;
}

void Application::draw()
{
	//Gizmos::draw(m_projection  * m_view);
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Application::shutdown()
{
	std::printf("shutting down...");
	//Gizmos::destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
}

