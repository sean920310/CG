#version 430 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_coordinate;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(position, 0.0f, 1.0f);

    texCoord = texture_coordinate;
}