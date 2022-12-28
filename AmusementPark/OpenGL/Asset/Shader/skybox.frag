#version 430 core
out vec4 f_color;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{   
    f_color = texture(skybox, TexCoords);
}