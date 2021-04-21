#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h> // MUST BE ADDED BEFORE GLFW
#include <GLFW/glfw3.h> // glfw3 ONLY SUPPORTS x64

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "shader.h"

class Window
{
	GLFWmonitor* m_Monitor;

	const char* m_Title;
	int m_Width{ 800 };
	int m_Height{ 600 };
	int m_PosX{ 0 };
	int m_PosY{ 0 };
	bool m_Fullscreen{ false };
public:
	Window(int width,int height,const char* title);
	~Window();	

	GLFWwindow* m_Window;
	void run();
	float deltaTime = 0;
	bool closeWindow{ false };
	Camera camera;
	void setView(glm::mat4 view, Shader& shader) { shader.use(); shader.setMat4("view", view); };
	void setProjection(glm::mat4 projection, Shader& shader) { shader.use(); shader.setMat4("projection", projection); };
	bool mouse_press = false;
	void tickRate();
private:
	bool firstTick = true;
	bool firstMouse = true;
	double lastX = m_Width / 2.0f;
	double lastY = m_Height / 2.0f;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)m_Width / (float)m_Height, 0.1f, 100.0f);
	void initGLFW();
	void createWindow();

	void setFullscreen();

	void WindowSizeCallback(int width, int height) { glfwSetWindowSize(m_Window, width, height); };
	void setWindowSizeCallback() { glfwSetWindowSizeCallback(m_Window, static_WindowSizeCallback); };
	static void static_WindowSizeCallback(GLFWwindow* window, int width, int height);

	void framebuffer_size_callback(int width, int height);
	void setFramebufferCallback() { glfwSetFramebufferSizeCallback(m_Window, static_FramebufferCallback); };
	static void	static_FramebufferCallback(GLFWwindow* window, int width, int height);
	
	void KeyCallback();

	void scroll_callback(double xoffset, double yoffset);
	void setScrollCallback() { glfwSetScrollCallback(m_Window, static_ScrollCallback); };
	static void static_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void mouse_callback(double xpos, double ypos);
	void setMouseCallback() { glfwSetCursorPosCallback(m_Window, static_MouseCallback); };
	static void static_MouseCallback(GLFWwindow* window, double xpos, double ypos);

};

void err(int error, const char* description) {
	std::cout << "Error code: " << error << "\n" << "Error discription: " << description;
}

Window::Window(int width, int height, const char* name) : m_Fullscreen(false), m_Width(width), m_Height(height), m_Title(name)
{
	firstMouse = true;
	lastX = m_Width / 2.0f;
	lastY = m_Height / 2.0f;
	initGLFW();
	m_Monitor = glfwGetPrimaryMonitor();
	createWindow();
	setWindowSizeCallback();
	setMouseCallback();
	glfwSetErrorCallback(err); // Create error output 
	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//Initialise GLAD for openGL managment
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

		/*
		TO DO:
			Create seperate file for handling errors
		*/
		std::cerr << "Failed to initialize GLAD" << std::endl;
	}
}

void Window::run() {
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
	KeyCallback();
	if (glfwWindowShouldClose(m_Window))
		closeWindow = true;

}

void Window::initGLFW() {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW!");
	}
}

void Window::createWindow() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
	if (!m_Window)
		throw std::runtime_error("Could not create GLFW Window!!!");
	glfwMakeContextCurrent(m_Window);
	glfwSetWindowUserPointer(m_Window, this);
}

void Window::static_WindowSizeCallback(GLFWwindow* window, int width, int height) {
	Window* actualWindow = (Window*)glfwGetWindowUserPointer(window);
	actualWindow->WindowSizeCallback(width, height);
}

void Window::KeyCallback() {
	mouse_press = false;
	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		closeWindow=true;
		glfwSetWindowShouldClose(m_Window, true);
	}
	if (glfwGetKey(m_Window, GLFW_KEY_F11) == GLFW_PRESS)
		setFullscreen();

	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(m_Window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(SPACE, deltaTime);
	
	if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE) {
		mouse_press = true;
	}
}
static float lastFrame=0;
static float currentFrame = 0;
static int nbframes=0;
static float count = 0;
void Window::tickRate() {
	if (firstTick) {
		glfwSetTime(0);
		firstTick = false;
	}
	currentFrame = glfwGetTime();
	this->deltaTime = currentFrame - lastFrame;
	nbframes++;
	count += deltaTime;
	if (count >= 1.0f) {
		std::cout << nbframes << " frames per second " << 1.0f / nbframes << "ms per frame" << "\n";
		count = 0;
		nbframes = 0;
		//std::cout << output.pub_objects["circle"].size.x << " " << output.pub_objects["circle"].size.y << " " << output.pub_objects["circle"].size.z << "\n";
	}
	lastFrame = currentFrame;
}

void Window::static_MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	Window* actualWindow = (Window*)glfwGetWindowUserPointer(window);
	actualWindow->mouse_callback(xpos, ypos);
}

void Window::mouse_callback(double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

void Window::static_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	Window* actualWindow = (Window*)glfwGetWindowUserPointer(window);
	actualWindow->scroll_callback(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Window::scroll_callback(double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void Window::framebuffer_size_callback(int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void Window::static_FramebufferCallback(GLFWwindow* window, int width, int height) {
	Window* actualWindow = (Window*)glfwGetWindowUserPointer(window);
	actualWindow->framebuffer_size_callback(width, height);
}

void Window::setFullscreen() {
	if (!m_Fullscreen) {
		const GLFWvidmode* vidMode = glfwGetVideoMode(m_Monitor);
		glfwGetWindowPos(m_Window, &m_PosX, &m_PosY);
		glfwGetWindowSize(m_Window, &m_Width, &m_Height);
		glfwSetWindowMonitor(m_Window, m_Monitor, 0, 0, vidMode->width, vidMode->height, vidMode->refreshRate);
		glfwSetWindowSize(m_Window, vidMode->width, vidMode->height);
		m_Fullscreen = !m_Fullscreen;
		framebuffer_size_callback(vidMode->width, vidMode->height);
	}
	else {
		glfwSetWindowMonitor(m_Window, nullptr, m_PosX, m_PosY, m_Width, m_Height, 0);
		glfwSetWindowSize(m_Window, m_Width, m_Height);
		m_Fullscreen = !m_Fullscreen;
		framebuffer_size_callback(m_Width, m_Height);
	}
}

Window::~Window()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

#endif