#version 330 core

layout(location = 0) in vec3 aPos;

//for texture
//layout(location = 1) in vec2 aTexCoord;

uniform mat4 mvp;

//Create a transfrom variable
uniform mat4 transform;

//Projection Matrix
uniform mat4 projection;

//texture
//out vec2 TexCoord;

void main()
{
	gl_Position = mvp * vec4(aPos, 1.0);
	//for texture
	//TexCoord = aTexCoord;
}