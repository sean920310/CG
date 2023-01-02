#version 430 core

out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D tex;

void main()
{
    FragColor = vec4(vec3(texture(tex, texCoord).r), 1.0);
}