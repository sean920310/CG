#version 430 core
layout (location = 0) in vec3 position;

layout (std140, binding = 0) uniform commom_matrices
{
    mat4 u_projection;
    mat4 u_view;
};

out vec3 TexCoords;

void main()
{
    TexCoords = position;
    mat4 view = mat4(mat3(u_view));
    vec4 pos = u_projection * view * vec4(position, 1.0f);
    gl_Position = pos.xyww;
}