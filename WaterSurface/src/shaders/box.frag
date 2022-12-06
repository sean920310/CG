#version 430 core
out vec4 f_color;

in vec3 TexCoords;

uniform samplerCube tex;

void main()
{   
    //f_color = texture(tex, TexCoords);
    f_color = vec4(1.0);
}