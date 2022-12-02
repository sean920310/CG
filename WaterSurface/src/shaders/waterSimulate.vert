#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

const float PI = 3.141592653589793;
uniform mat4 u_model;
uniform vec2 u_center;
uniform float u_radius;
uniform float u_strength;

layout (std140, binding = 0) uniform commom_matrices
{
    mat4 u_projection;
    mat4 u_view;
};

out V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} v_out;

void addDrop(vec3 pos, vec3 norm);

void main()
{
    vec3 pos = position;
    vec3 norm = normal;
    gl_Position = u_projection * u_view * u_model * vec4(pos, 1.0f);

    v_out.position = vec3(u_model * vec4(pos, 1.0f));
    v_out.normal = mat3(transpose(inverse(u_model))) * norm;
    //v_out.normal = norm;
    v_out.texture_coordinate = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
}

void addDrop(vec3 pos, vec3 norm)
{
    float drop = max(0.0f, 1.0f - length(u_center - position.xz) / u_radius);
    drop = 0.5 - cos(drop * PI) * 0.5;
    pos.y = drop * u_strength;
}