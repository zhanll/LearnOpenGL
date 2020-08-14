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

struct DirLight
{
	vec3 direction;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

vec3 DiffuseColor()
{
	return vec3(texture(material.diffuse, TexCoord));
}

vec3 SpecularColor()
{
	return vec3(texture(material.specular, TexCoord));
}

vec3 CalcAmbient(vec3 lightAmbient, vec3 diffuseColor)
{
	return lightAmbient * diffuseColor;
}

vec3 CalcDiffuse(vec3 lightDiffuse, vec3 diffuseColor, vec3 normal, vec3 lightDir)
{
	return lightDiffuse * max(dot(normal, -lightDir), 0.0) * diffuseColor;
}

vec3 CalcSpecular(vec3 lightSpecular, vec3 specularColor, vec3 normal, vec3 lightDir, vec3 viewDir)
{
	return lightSpecular * pow(max(dot(reflect(lightDir, normal), viewDir), 0.0), material.shininess) * specularColor;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.direction);
	vec3 diffColor = DiffuseColor();
	vec3 specColor = SpecularColor();

	vec3 ambient = CalcAmbient(light.ambient, diffColor);
	vec3 diffuse = CalcDiffuse(light.diffuse, diffColor, normal, lightDir);
	vec3 specular = CalcSpecular(light.specular, specColor, normal, lightDir, viewDir);

	return ambient + diffuse + specular;
}

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLight[NR_POINT_LIGHTS];

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(FragPos - light.position);
	vec3 diffColor = DiffuseColor();
	vec3 specColor = SpecularColor();

	vec3 ambient = CalcAmbient(light.ambient, diffColor);
	vec3 diffuse = CalcDiffuse(light.diffuse, diffColor, normal, lightDir);
	vec3 specular = CalcSpecular(light.specular, specColor, normal, lightDir, viewDir);

	float distance = length(FragPos - light.position);
	float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);

	return (ambient + diffuse + specular) * attenuation;
}

struct SpotLight
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
uniform SpotLight spotLight;

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(FragPos - light.position);
	vec3 diffColor = DiffuseColor();
	vec3 specColor = SpecularColor();

	vec3 ambient = CalcAmbient(light.ambient, diffColor);
	vec3 diffuse = CalcDiffuse(light.diffuse, diffColor, normal, lightDir);
	vec3 specular = CalcSpecular(light.specular, specColor, normal, lightDir, viewDir);

	float distance = length(FragPos - light.position);
	float attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);

	vec3 result = (ambient + diffuse + specular) * attenuation;

	float intensity = 0.0;
	float theta = dot(lightDir, normalize(light.direction));
	if (theta > light.outerCutOff)
	{
		float epsilon = light.cutOff - light.outerCutOff;
		intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	}
	result *= intensity;

	return result;
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result;
	result += CalcDirLight(dirLight, norm, viewDir);
	for(int i=0; i<NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLight[i], norm, viewDir);
	result += CalcSpotLight(spotLight, norm, viewDir);

	FragColor = vec4(result, 1.0);
}