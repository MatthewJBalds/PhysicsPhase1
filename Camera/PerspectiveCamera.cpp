#include "PerspectiveCamera.h"
#include <iostream>

PerspectiveCamera::PerspectiveCamera() : MyCamera()
{
	this->window_height = 800;
	this->window_width = 800;
	this->theta = 45.5f;
	this->phi = 89.5f;
	this->aspectRatio = static_cast<float>(window_height) / static_cast<float>(window_width);
	this->SetProjectionMatrix(45.f);
	viewMatrix = glm::mat4(1.0f); // Initialize as identity matrix
}

glm::mat4 PerspectiveCamera::getViewProjection() const
{
	return projectionMatrix * viewMatrix;
}

void PerspectiveCamera::SetProjectionMatrix(float viewAngle)
{
	//dont edit anymore
	this->projectionMatrix = glm::perspective(glm::radians(viewAngle), this->aspectRatio, 0.1f, 2000.0f);
}






