#pragma once
#include "MyCamera.h"

class OrthoCamera : public MyCamera
{
	public:
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		OrthoCamera();
		glm::mat4 getViewProjection() const;
};

