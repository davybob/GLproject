#pragma once

#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h> // MUST BE ADDED BEFORE GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "shader.h"
#include "CoordinateSystem.h"
#include "Texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <map>

/* 
Render class works as such:
	User calls to create an object, say triangle;
	Triangle gets created and position, size and rotation data gets appended to a map of all objects.
	Map will be used to retrieve that "triangle's" position, size and rotation data for collision, etc.
	ALL OTHER DATA (VAO, VBO, POSITIONS, INDICES ETC) will not be publically accessable (only to member classes (collision, physics,etc))
*/

class Render : protected CoordinateSystem, protected Texture
{
public:

	typedef enum type { p = 3, pt = 5, pc = 7, ptc = 9, ptcn = 12 } vertex_data_type; //define type for union
protected:
	/*
	Format takes position,colour,texture as entire array.
	seperate class to handle coord
	seperate class to handle colour?
	seperate class to handle texture
	*/
	typedef struct attributes {
		GLuint				 VAO = 0;
		GLuint				 EBO = 0;
		GLuint				 VBO = 0;
		GLuint				 TAO = 0;
	} attributes;

	struct objectData{
		std::vector<GLshort> indices;
		//type type;
		std::vector<glm::vec3> vertices;
		std::vector<std::string> textures;
		attributes Attribs;
		coordSystem CoordSystem;
		Shader* shader = nullptr;
		GLenum drawType;
		int ID;
	};

	std::vector<objectData*> data_list;

	FT_Library ft;
	FT_Face face;
	struct Character {
		unsigned int TextureID;
		glm::ivec2	 Size;
		glm::ivec2	 Bearing;
		unsigned int Advance;
	};
	std::map<char, Character> Characters;

	Shader* shaderinstance;

public:

	Render(Shader *shaderInstance);
	~Render();
	unsigned int count;
	std::map<std::string, objectData> pub_objects; // mapping between named object (e.g triangle1) and position in list of all objects
	void polygonGenerator(unsigned int n, std::string name, float size, std::vector<GLfloat> colours);

	//Object loop
	void loop();
	//Load from data
	bool FromData(std::string name, std::string texture, std::vector<GLfloat> data, std::vector<GLshort> indices, type type, bool Static, decltype(GL_STATIC_DRAW) drawtype);
	//Transformations
	void Rotate(std::string name, float angle, glm::vec3 axis);

	void Translate(std::string name, glm::vec3 pos);
	
	void Scale(std::string name, float size);
	void Scale(std::string name, glm::vec3 scale);
	
	bool updateTexture(std::string textureName, unsigned int texID);
	void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 colour);
	void update();

private:
	const float PI = 3.14159265;
	std::vector<glm::vec3> n_formatData(std::vector<GLfloat> data, type type);

	//Generate necessary objects
	GLuint generateVBO();
	GLuint generateEBO();
	GLuint generateVAO();

	void bindVertexObject(GLuint VAO);
	void bindVertexObject();
	void bindBuffers(GLuint& VBO, std::vector<GLfloat>& data); //Depricted
	
	void bindIndices(GLuint EBO, std::vector<GLshort> indices);									//Depricted
	void vertexAttribute(int indx, int size, int stride, int offset);

	std::vector<glm::vec3> convertPoints(std::vector<GLfloat>& data);

	void initTextRender(const char* font, int size);
};

#endif