//#pragma once
#include <GLFW/glfw3.h>
#include <map>
#include <vector>
#include "Window.h"

/*
Two event types:
	Polling (continous checking of key states; Movement, looking etc)
	Callback (One time events; exit, menu, etc)

	_instances will monitor keystates from polling and callback (needs to be setup in window class!) 
*/

class Input
{
public:
	Input(std::vector<int> keysToMonitor, GLFWwindow* window); //Constructor for polling
	~Input();
	void addKey(int key) {_keys[key] = false;};
	bool getIsKeyDown(int key);
	bool getIsEnabled() { return _isEnabled; };
	bool setIsEnabled(bool value) { _isEnabled = value; };
private:
	void setIsKeyDown(int key, bool isDown);
	std::map<int, bool> _keys;
	bool _isEnabled;

public:
	static void setupKeyInputs(GLFWwindow* window);
private:
	static void callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static std::vector<Input*> _instances;
};

Input::Input(std::vector<int> keysToMonitor, GLFWwindow* window) : _isEnabled(true){
	for (int key : keysToMonitor) {
		_keys[key] = false;
	}
	// Add this instance to the list of instances
	Input::_instances.push_back(this);
}

Input::~Input()
{
	_instances.erase(std::remove(_instances.begin(), _instances.end(), this), _instances.end());
}

bool Input::getIsKeyDown(int key) {
	bool result = false;
	if (_isEnabled) {
		std::map<int, bool>::iterator it = _keys.find(key);
		if (it != _keys.end()) {
			result = _keys[key];
		}
	}
	return result;
}

void Input::setIsKeyDown(int key, bool isDown) {
	std::map<int, bool>::iterator it = _keys.find(key);
	if (it != _keys.end()) {
		_keys[key] = isDown;
	}
}

void Input::setupKeyInputs(GLFWwindow* window) {
	glfwSetKeyCallback(window, Input::callback);
}

void Input::callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Send key event to all KeyInput instances
	for (Input* keyInput : _instances) {
		keyInput->setIsKeyDown(key, action != GLFW_RELEASE);
	}
}