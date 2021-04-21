#pragma once
#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
typedef struct attributes {
	GLuint				 VAO = 0;
	GLuint				 EBO = 0;
	GLuint				 VBO = 0;
	GLuint				 TAO = 0;
} attributes;

typedef struct Vertex {
	glm::vec3 Position;
	glm::vec4 Colours;
	//glm::vec2 TexCoords; TO implement optional 
	//glm::vec3 Normal;    TO implement optional 
}Vertex;

#endif