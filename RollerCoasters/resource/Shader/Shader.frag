//refrence https://learnopengl.com/Lighting/Multiple-lights
#version 330 core

out vec4 FragColor;
in vec3 normal;
in vec2 texCoord;
in vec3 crntPos;

struct DirLight {
    vec3 position;
	
    vec4 ambient;
    vec4 diffuse;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec4 ambient;
    vec4 diffuse;    
};

uniform sampler2D tex0;
uniform vec4 color;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform SpotLight headLight;

uniform bool doingShadows;

vec4 CalcDirLight(DirLight light, vec3 normal);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos);

void main()
{
	vec3 norm = normalize(normal);
    vec4 result = CalcDirLight(dirLight, norm);
    result += CalcSpotLight(spotLight, norm, crntPos);
    result += CalcSpotLight(headLight, norm, crntPos);


	if(!doingShadows)
		FragColor = result;
	else
		FragColor = vec4(0.0f,0.0f,0.0f,0.5f);
} 

// calculates the color when using a directional light.
vec4 CalcDirLight(DirLight light, vec3 normal)
{
    vec3 lightDir = normalize(light.position);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // combine results
    vec4 ambient = light.ambient * color;
    vec4 diffuse = light.diffuse * diff * color;
    return (ambient + diffuse);
}

// calculates the color when using a spot light.
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec4 ambient = light.ambient * color;
    vec4 diffuse = light.diffuse * diff * color;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    return (ambient + diffuse );
}