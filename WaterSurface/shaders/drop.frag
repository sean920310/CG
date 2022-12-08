#version 430 core
out vec4 f_color;

uniform sampler2D lastTex;
const float PI = 3.141592653589793;
uniform vec2 u_center;
uniform float u_radius;
uniform float u_strength;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} f_in;

void main()
{   
    vec2 coord = f_in.texture_coordinate;

    vec4 info = texture2D(lastTex, coord);

    float drop = max(0.0f, 1.0f - length(u_center - coord) / u_radius);
    drop = 0.5 - cos(drop * PI) * 0.5;

    drop *= 0.5;

    info.r += drop * u_strength;

    f_color = info;
}