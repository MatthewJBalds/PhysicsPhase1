#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 mvp;

//Create a transfrom variable
uniform mat4 transform;

//Projection Matrix
uniform mat4 projection;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
}