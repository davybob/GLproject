#pragma once

#ifndef CordSystem_H
#define CordSystem_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

class CoordinateSystem  {
public:

	typedef struct coordSystem {
		glm::mat4 model = glm::mat4(1.0f);
		//debug purposes only!
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 translation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		bool change = false;
	} coordSystem;


	// DEFINE HERE WILL BE INLINE (I.E CODE WILL BE PASTED AT CALL RATHER THAN SENT TO HERE!!!)

	glm::vec3 Rotate_vertices(float angle, glm::vec3 axis, glm::mat4& model, glm::vec3 rotation);
	glm::vec3 Translate_vertices(glm::vec3 pos, glm::mat4& model, glm::vec3 translation);
	glm::vec3 Scale_vertices(float size, glm::mat4& model, glm::vec3 scale);
	glm::vec3 Scale_vertices(glm::vec3 scale, glm::mat4& model, glm::vec3 scale_v);

protected:
	glm::vec3 updateVertices(glm::vec3& vertex, glm::mat4& model);
};

#endif