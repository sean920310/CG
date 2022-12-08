#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 u_model;
uniform sampler2D heightMap;
uniform float amplitude;

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

void setHeightMap(inout vec3 position);

void main()
{
    vec3 pos = position;

    setHeightMap(pos);

    v_out.position = vec3(u_model * vec4(pos, 1.0f));
    gl_Position = u_projection * u_view * u_model * vec4(pos, 1.0f);

    v_out.texture_coordinate = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
}

void setHeightMap(inout vec3 position)
{
    float amp = 2 * amplitude;
    vec2 coord = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
    
    position.y = amp * (texture(heightMap, coord).r - 0.5f);
}