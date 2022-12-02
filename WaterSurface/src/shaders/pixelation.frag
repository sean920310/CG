//reference https://www.geeks3d.com/20101029/shader-library-pixelation-post-processing-effect-glsl/
#version 430 core
out vec4 f_color;

in vec2 texCoord;
uniform sampler2D tex;
uniform float rt_w;
uniform float rt_h;
const float pixel_w = 15.0; // 15.0
const float pixel_h = 10.0; // 10.0

void main()
{
    vec2 uv = texCoord;

    vec3 tc = vec3(1.0, 0.0, 0.0);

    float dx = pixel_w * (1.0/rt_w);
    float dy = pixel_h * (1.0/rt_h);

    vec2 coord = vec2(dx * floor(uv.x / dx), dy * floor(uv.y / dy));
    
    f_color = texture(tex, coord);
}