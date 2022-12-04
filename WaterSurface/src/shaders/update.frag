#version 430 core
out vec4 f_color;

uniform sampler2D lastTex;
const float delta = 1.0f/ 512.0f;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} f_in;

void main()
{   
    vec2 coord = f_in.texture_coordinate;
    vec4 info = texture(lastTex, coord);

    vec2 dx = vec2(delta, 0.0);
    vec2 dy = vec2(0.0, delta);
    float average = (
    texture(lastTex, coord - dx).r +
    texture(lastTex, coord + dx).r +
    texture(lastTex, coord - dy).r +
    texture(lastTex, coord + dy).r
    ) * 0.25;

    //info.g += (average - info.r) * 2.0;

    //info.g *= 0.995;

    //info.r = info.g;

    //info.r += (average - info.r) * 0.995;

    f_color = vec4(info.r, info.r, info.r, 1.0);
}