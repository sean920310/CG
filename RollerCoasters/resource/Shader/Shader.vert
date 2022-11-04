#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;


out vec3 crntPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float scale;


void main()
{
	crntPos = vec3(model * vec4(aPos * scale, 1.0f));
	gl_Position = proj * view * vec4(crntPos, 1.0);
	
	normal = vec3(model * vec4(aNormal, 0.0f));
	texCoord = aTex;
}