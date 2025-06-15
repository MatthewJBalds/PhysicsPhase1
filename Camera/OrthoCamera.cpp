#include "OrthoCamera.h"

OrthoCamera::OrthoCamera() : MyCamera()
{
	
	this->projectionMatrix = glm::ortho(-400.f, 400.f, -400.f, 400.f, -400.f, 400.f);
	this->viewMatrix = glm::mat4(1.0f); // Initialize as identity matrix
}

glm::mat4 OrthoCamera::getViewProjection() const
{
	return projectionMatrix * viewMatrix;
}
