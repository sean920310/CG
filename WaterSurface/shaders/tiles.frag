#version 430 core
out vec4 f_color;

in vec3 TexCoords;

uniform samplerCube tex;

void main()
{   
    f_color = mix(texture(tex, TexCoords), vec4(0.2, 0.5, 1.0,1.0), 0.5);
}