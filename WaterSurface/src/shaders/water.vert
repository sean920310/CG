#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 u_model;
uniform sampler2D heightMap;
uniform float t;
uniform float k;
uniform float amplitude;
uniform vec2 direction;
uniform int waveType; //1: sin wave ,2:height map

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

void setSinWave(inout vec3 position,inout vec3 normal,float t,vec2 direction,float k,float amplitude);
void setHeightMap(inout vec3 position,inout vec3 normal);

void main()
{
    vec3 pos = position;
    vec3 norm = normal;
    if(waveType == 1)
        setSinWave(pos, norm, t, direction, k, amplitude);
    else if(waveType == 2)
        setHeightMap(pos, norm);
    gl_Position = u_projection * u_view * u_model * vec4(pos, 1.0f);

    v_out.position = vec3(u_model * vec4(pos, 1.0f));
    v_out.normal = mat3(transpose(inverse(u_model))) * norm;
    //v_out.normal = norm;
    v_out.texture_coordinate = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
}

void setSinWave(inout vec3 position,inout vec3 normal,float t,vec2 direction,float k,float amplitude)
{
    vec2 dir = normalize(direction);
    vec2 xz = position.xz;
    float f = k * (dot(xz, dir) - t);
    position.y = amplitude * sin(f);

    vec3 tangent = vec3(1.0f, (amplitude * k * dir.x * cos(f)), 0.0f);
    vec3 binormal = vec3(0.0f, (amplitude * k * dir.y * cos(f)), 1.0f);
    normal = normalize(cross(binormal, tangent));
}

void setHeightMap(inout vec3 position,inout vec3 normal)
{
    position.y = amplitude * (texture(heightMap, texture_coordinate).r - 0.5f);

    //set up height map normal
    //  F
    //L O R
    //  B
    vec2 coord = texture_coordinate;
    vec3 offset = vec3(1.0f / 512.0f, 0.0f, 1.0f/ 512.0f);
    float L = amplitude * texture(heightMap, (coord - offset.xy)).r;
    float R = amplitude * texture(heightMap, (coord + offset.xy)).r;
    float B = amplitude * texture(heightMap, (coord - offset.yz)).r;
    float F = amplitude * texture(heightMap, (coord + offset.yz)).r;

    //vec3 norm = vec3((F - B)/2.0f, (R - L)/2.0f, -1.0f);
    vec3 tangent = vec3(1.0f, (R - L)/ 2, 0.0f);
    vec3 binormal = vec3(0.0f, (F - B)/ 2, 1.0f);

    vec3 norm = normalize(cross(binormal, tangent));
    norm = normalize(norm);

    normal = norm;
}