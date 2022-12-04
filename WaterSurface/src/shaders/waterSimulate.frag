#version 430 core
out vec4 f_color;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} f_in;

void main()
{   
    vec3 color = vec3(f_in.texture_coordinate.x,f_in.texture_coordinate.y,0.1);

    f_color = vec4(color, 1.0f);
}