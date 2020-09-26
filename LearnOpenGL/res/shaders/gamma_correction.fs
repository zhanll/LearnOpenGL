#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform bool gamma;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;

	float shininess;
};
uniform Material material;

vec3 DiffuseColor()
{
	return vec3(texture(material.texture_diffuse1, TexCoord));
}

vec3 SpecularColor()
{
	return vec3(texture(material.texture_specular1, TexCoord));
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
#define NR_POINT_LIGHTS 6
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

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result;
	for(int i=0; i<NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLight[i], norm, viewDir);

	if(gamma)
		result = pow(result, vec3(1.0/2.2));
	FragColor = vec4(result, 1.0);
}