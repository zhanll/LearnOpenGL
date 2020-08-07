#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

	float Diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = Diff * lightColor;

	vec3 result = ambient + diffuse;
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord) , 0.2) * vec4(result, 1.0);
}