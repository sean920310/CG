#version 430 core
out vec4 f_color;
//1/50 1.0 0.9

uniform sampler2D lastTex;
//const float delta = 1.0f/ 512.0f;
uniform vec2 delta;
uniform float amplitude;
uniform float velocity;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} f_in;

//in float average;

void main()
{   
    vec2 coord = f_in.texture_coordinate;
    vec4 info = texture2D(lastTex, coord);

    vec2 dx = vec2(delta.x, 0.0);
    vec2 dy = vec2(0.0, delta.y);
    float average = (
    texture2D(lastTex, coord - dx).r +
    texture2D(lastTex, coord + dx).r +
    texture2D(lastTex, coord - dy).r +
    texture2D(lastTex, coord + dy).r +
    texture2D(lastTex, coord - dx + dy).r +
    texture2D(lastTex, coord + dx + dy).r +
    texture2D(lastTex, coord - dx - dy).r +
    texture2D(lastTex, coord + dx - dy).r 
    ) * 0.125;

    //info.g += (average - info.r) * 1.2 + 0.05 * (0.5f - info.r);
    info.g += (average - info.r) * velocity;

    info.g *= 0.99;
    info.g += 0.03 * (0.5f - info.r);

    info.r += info.g;

    f_color = info;
}