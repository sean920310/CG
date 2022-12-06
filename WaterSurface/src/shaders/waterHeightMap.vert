#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 u_model;
uniform sampler2D heightMap;
uniform float amplitude;
out vec2 ndc;

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


void setHeightMap(inout vec3 position,inout vec3 normal);

void main()
{
    vec3 pos = position;
    vec3 norm = normal;

    setHeightMap(pos, norm);

    vec4 resultPos = u_projection * u_view * u_model * vec4(pos, 1.0f);
    gl_Position = resultPos;

    v_out.position = vec3(u_model * vec4(pos, 1.0f));
    v_out.normal = mat3(transpose(inverse(u_model))) * norm;
    //v_out.normal = norm;
    v_out.texture_coordinate = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
    ndc = (resultPos.xy/resultPos.w)/2.0 + 0.5;
}


void setHeightMap(inout vec3 position,inout vec3 normal)
{
    float amp = 2 * amplitude;
    vec2 coord = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
    
    position.y = amp * (texture(heightMap, coord).r - 0.5f);

    //set up height map normal
    //  F
    //L O R
    //  B
    vec3 offset = vec3(1.0f / 512.0f, 0.0f, 1.0f/ 512.0f);
    float L = amp * texture(heightMap, (coord - offset.xy)).r;
    float R = amp * texture(heightMap, (coord + offset.xy)).r;
    float B = amp * texture(heightMap, (coord - offset.yz)).r;
    float F = amp * texture(heightMap, (coord + offset.yz)).r;

    //vec3 norm = vec3((F - B)/2.0f, (R - L)/2.0f, -1.0f);
    vec3 tangent = vec3(1.0f, (R - L)/ 2, 0.0f);
    vec3 binormal = vec3(0.0f, (F - B)/ 2, 1.0f);

    vec3 norm = normalize(cross(binormal, tangent));
    norm = normalize(norm);

    normal = norm;
}