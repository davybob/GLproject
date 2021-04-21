#include "CoordinateSystem.h"

glm::vec3 CoordinateSystem::updateVertices(glm::vec3& vertex, glm::mat4& model) {
	glm::vec4 result = model * glm::vec4(vertex, 1.0f);                             //<-RIGHT TO LEFT i.e first glmvec4 THEN MODEL 
	return glm::vec3(result.x, result.y, result.z);
}

glm::vec3 CoordinateSystem::Rotate_vertices(float angle, glm::vec3 axis, glm::mat4& model, glm::vec3 rotation = glm::vec3(1.0f)) {
	model = glm::rotate(model, glm::radians(angle), axis);
	return glm::mod(rotation + (angle * axis), 360.0f);
}

glm::vec3 CoordinateSystem::Translate_vertices(glm::vec3 pos, glm::mat4& model, glm::vec3 translation = glm::vec3(1.0f)) {
	model = glm::translate(model, -pos);
	return translation+pos;
}

glm::vec3 CoordinateSystem::Scale_vertices(float size, glm::mat4& model, glm::vec3 scale = glm::vec3(1.0f)) {
	model = glm::scale(model, glm::vec3(size));
	return size * scale;
}

glm::vec3 CoordinateSystem::Scale_vertices(glm::vec3 scale, glm::mat4& model, glm::vec3 scale_v = glm::vec3(1.0f)) {
	model = glm::scale(model, scale);
	return glm::vec3(scale.x * scale.x, scale.y * scale.y, scale.z * scale.z);;
}