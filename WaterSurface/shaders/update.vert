#version 430 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texture_coordinate;

out V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} v_out;


void main()
{
    gl_Position = vec4(position.z / 50.0f, position.x / 50.0f, 0.0f, 1.0f);

    v_out.texture_coordinate = texture_coordinate;
}