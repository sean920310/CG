#version 430 core
layout (location = 0) in vec2 position;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(position, 0.0f, 1.0f);

    texCoord = position * 0.5f + 0.5f;
}