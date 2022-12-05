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
uniform samplerCube skyboxTex;
uniform float shininess = 77.0f; //物體specular反射程度
uniform vec3 u_eyePosition;
uniform DirLight dirLight;
uniform sampler2D heightMap;


vec3 addDirLight(DirLight dirLight, vec3 color);

void main()
{   
    //vec3 color = vec3(texture(u_texture, f_in.texture_coordinate));
    
    //reflection

    vec3 I = normalize(f_in.position - u_eyePosition);
    vec3 reflectR = reflect(I, normalize(f_in.normal));
    vec3 reflectColor = vec3(texture(skyboxTex, reflectR));

    //refraction

    float ratio = 1.00 / 1.33;
    vec3 refractR = refract(I, normalize(f_in.normal), ratio);
    vec3 refractColor = vec3(texture(skyboxTex, refractR));

    //set lighting

    vec3 result, mixColor;

    mixColor = mix(reflectColor, refractColor, 0.8);
    result = addDirLight(dirLight, mixColor);
    //vec4 h = texture(heightMap, f_in.texture_coordinate);
    //result = vec3(h.r,h.r,h.r);
    f_color = vec4(result, 1.0f);
    //f_color = vec4(f_in.texture_coordinate.x,f_in.texture_coordinate.y,0.1, 1.0);
}

vec3 addDirLight(DirLight dirLight, vec3 color)
{
    vec3 lightDir = normalize(dirLight.position);
    vec3 norm = normalize(f_in.normal);
    vec3 halfVec = normalize(normalize(lightDir) + normalize(u_eyePosition - f_in.position)); 

    float diffuse = max(0.0, dot(norm, lightDir));
    float specular = pow(max(0.0, dot(f_in.normal, halfVec)), shininess);

    return ( min(color * dirLight.ambient, vec3(1.0)) + diffuse * dirLight.diffuse * color + specular * dirLight.specular * color);
}