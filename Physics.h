#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

#include "Render.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <math.h>
//THIS CLASS WILL ALLOW OBJECTS TO HAVE GRAVITY ENABLED, x,v and a will be calculated using Runge-Kutta method

class Physics
{
private:

public:
	Physics(Render myObject);
	Physics(); //For debugging
};


void Collision(std::vector<GLfloat> vertexA, std::vector<GLfloat> posA, std::vector<GLfloat> vertexB, std::vector<GLfloat> posB) {
	/*
	Alg works as follow:
		Find vertexA with smallest distance to center of B
		Find vertexB with smallest distance to center of A
		Take sum of vertexA->centerB with vertexB->centerA
		If that vector distance < vector distance between centers, then collision occured!!!!!!!!
	*/
	float distanceA = 0;
	float distanceB = 0;
	for (auto it : )
	{

	}

}
#endif