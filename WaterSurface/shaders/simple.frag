#version 430 core
out vec4 f_color;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} f_in;

struct DirLight {
    vec3 position;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 u_color;
uniform sampler2D u_texture;
uniform float shininess = 77.0f; //物體specular反射程度
uniform vec3 u_eyePosition;
uniform DirLight dirLight;

vec3 addDirLight(DirLight dirLight);

void main()
{   
    //vec3 color = vec3(texture(u_texture, f_in.texture_coordinate));
    vec3 color = addDirLight(dirLight);
    f_color = vec4(color, 1.0f);
}

vec3 addDirLight(DirLight dirLight)
{
    vec3 lightDir = normalize(dirLight.position);
    vec3 norm = normalize(f_in.normal);
    vec3 halfVec = normalize(normalize(lightDir) + normalize(u_eyePosition - f_in.position)); 

    float diffuse = max(0.0, dot(norm, lightDir));
    float specular = pow(max(0.0, dot(f_in.normal, halfVec)), shininess);

    vec3 color = vec3(texture(u_texture, f_in.texture_coordinate));

    return ( min(color * dirLight.ambient, vec3(1.0)) + diffuse * dirLight.diffuse * color + specular * dirLight.specular * color);
}