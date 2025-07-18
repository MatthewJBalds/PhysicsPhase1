#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include<string>
#include<vector>
#include<iostream>

#include"../../tiny_obj_loader.h"
#include"../../stb_image.h"


#include<unordered_map>

using namespace std;

namespace Physics {
	class RenderLine
	{
	public:
		void Draw(glm::vec3 a, glm::vec3 b);


	};
}

