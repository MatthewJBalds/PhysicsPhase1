#pragma once
#include "MyCamera.h"
#include <GLFW/glfw3.h>

class PerspectiveCamera :
    public MyCamera
{
	private:
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		float window_width;
		float window_height;
		float aspectRatio;
		float theta;
		float phi;
	public:
		PerspectiveCamera();
		glm::mat4 getViewProjection() const;
		void SetProjectionMatrix(float viewAngle);
		
};

