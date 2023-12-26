#version 430 core
out vec4 FragColor;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
   vec4 positionLightSpace;
} f_in;

struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};


uniform vec3 u_color;
uniform sampler2D shadowMap;
uniform float shininess = 77.0f; //物體specular反射程度
uniform vec3 u_eyePosition;
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D heightMap;

in vec2 ndc;

vec3 CalcDirLight(DirLight light, vec3 color, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 color, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 color, vec3 normal, vec3 fragPos, vec3 viewDir);

float CalcShadow(vec4 positionLightSpace, vec3 normal ,vec3 lightDir);

void main()
{   
    vec3 viewDir = normalize(u_eyePosition - f_in.position);
    vec3 normal = normalize(f_in.normal);

    vec2 reflectCoord = vec2 (ndc.x, 1- ndc.y);
    vec2 refractCoord = vec2 (ndc.x, ndc.y);

    vec3 reflectColor = vec3(texture(reflectTex, reflectCoord + f_in.normal.xz));
    vec3 refractColor = vec3(texture(refractTex, refractCoord + f_in.normal.xz));

    //set lighting
    vec3 result, mixColor;

    mixColor = mix(reflectColor, refractColor, 0.4);
    mixColor = mix(mixColor, u_color, 0.3);
    //mixColor = vec3(0.6f);
    result = CalcDirLight(dirLight, mixColor, normal, viewDir);
	result += CalcSpotLight(spotLight, mixColor, normal, f_in.position, viewDir);

    FragColor = vec4(result,1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 color, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    //shadow
    float shadow = CalcShadow(f_in.positionLightSpace, normal, lightDir);
    shadow = 0.0;
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 color, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    // shadow
    //float shadow = CalcShadow(f_in.positionLightSpace, normal, lightDir);
    float shadow = 0.0f;
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 color, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * color;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    // shadow
    //float shadow = CalcShadow(f_in.positionLightSpace, normal, lightDir);
    float shadow = 0.0f;
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

float CalcShadow(vec4 positionLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = positionLightSpace.xyz / positionLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 

    return shadow;
}