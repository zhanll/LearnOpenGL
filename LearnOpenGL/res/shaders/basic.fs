#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 viewPos;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;
};

uniform Material material;

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);

    vec3 ambient = material.ambient * light.ambient;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = material.diffuse * diff * light.diffuse;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = material.specular * spec * light.specular;

	vec3 result = ambient + diffuse + specular;
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord) , 0.2) * vec4(result, 1.0);
}