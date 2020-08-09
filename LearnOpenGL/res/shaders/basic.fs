#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	float Diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = Diff * lightColor;

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = ambient + diffuse + specular;
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord) , 0.2) * vec4(result, 1.0);
}