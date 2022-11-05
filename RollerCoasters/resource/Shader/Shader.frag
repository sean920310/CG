#version 330 core

out vec4 FragColor;
in vec3 normal;
in vec2 texCoord;
in vec3 crntPos;

uniform sampler2D tex0;
uniform vec4 color;
uniform vec4 lightColor;
uniform vec3 lightPos;

uniform bool doingShadows;

void main()
{
	float ambient = 0.2f;
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - crntPos);

	float diffuse = max(dot(norm, lightDir), ambient);
	if(!doingShadows)
		FragColor = color * lightColor * diffuse;
	else
		FragColor = vec4(0.0f,0.0f,0.0f,0.5f);
	//FragColor = texture(tex0, texCoord) * color * lightColor * (diffuse + ambient);
} 