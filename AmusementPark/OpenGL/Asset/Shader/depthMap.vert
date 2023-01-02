#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 u_model;
uniform mat4 u_lightSpaceMatrix;

void main()
{
   gl_Position = u_lightSpaceMatrix * u_model * vec4(position, 1.0f);
}