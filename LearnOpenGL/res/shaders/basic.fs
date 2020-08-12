#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;

	float shininess;
};

uniform Material material;

struct Light
{
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
	
	float cutOff;
	float outerCutOff;
};

uniform Light light;

void main()
{
	vec3 lightDir = normalize(light.position - FragPos);

	vec3 ambient = vec3(texture(material.diffuse, TexCoord)) * light.ambient;
	vec3 diffuse;
	vec3 specular;

	float theta = dot(lightDir, normalize(-light.direction));
	if (theta > light.outerCutOff)
	{
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

		vec3 norm = normalize(Normal);
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse = vec3(texture(material.diffuse, TexCoord)) * diff * light.diffuse;
		diffuse *= intensity;

		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		specular = vec3(texture(material.specular, TexCoord)) * spec * light.specular;
		specular *= intensity;
	}
	
	float distance = length(FragPos - light.position);
	float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);
	vec3 result = ambient + diffuse + specular;
	result *= attenuation;
	FragColor = vec4(result, 1.0);
}