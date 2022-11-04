#version 330 core

out vec4 FragColor;
in vec3 normal;
in vec2 texCoord;
in vec3 crntPos;

uniform sampler2D tex0;
uniform vec4 color;

uniform vec3 lightPos;


void main()
{
	float ambient = 0.2f;
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - crntPos);

	float diffuse = max(dot(norm, lightDir), ambient);

	FragColor = color * diffuse;
	//FragColor = texture(tex0, texCoord) * color * (diffuse + ambient);
} 