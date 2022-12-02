#version 430 core
out vec4 f_color;

in vec2 texCoord;
uniform sampler2D tex;

void main()
{
    f_color = texture(tex, texCoord);
}