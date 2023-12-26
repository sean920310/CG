#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

out V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
   vec4 positionLightSpace;
} v_out;


uniform mat4 u_model;
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_lightSpaceMatrix;
uniform vec4 u_plane = vec4(0, -1, 0,10000);

void main()
{
    gl_ClipDistance[0] = dot(u_model * vec4(position, 1.0f), u_plane);
    gl_Position = u_projection * u_view * u_model * vec4(position, 1.0f);

    v_out.position = vec3(u_model * vec4(position, 1.0f));
    //v_out.normal = mat3(transpose(inverse(u_model))) * normal;
    v_out.normal = vec3(u_model * vec4(normal, 0.0f));
    v_out.texture_coordinate = texture_coordinate;
    v_out.positionLightSpace = u_lightSpaceMatrix * vec4(v_out.position, 1.0);
}