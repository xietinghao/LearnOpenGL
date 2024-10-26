#version 330 core 
in vec2 ourTexCoor;
in vec3 ourNorm;
in vec3 FragPos;
in vec2 diffuseTexCoor;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform vec3 cameraPos;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emit;

	float shininess;
};
uniform Material material;

struct Light{
	vec3 position;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light;
out vec4 FragColor;

void main(){
	vec3 ambient, specular, diffuse, emit;
	vec3 lightDir, cameraDir;
	ambient = light.ambient * texture(material.diffuse, diffuseTexCoor).rgb;
	vec3 norm = normalize(ourNorm);
	
	lightDir = normalize(light.position- FragPos);
	diffuse = light.diffuse * texture(material.diffuse, diffuseTexCoor).rgb * max(dot(lightDir, norm), 0.0f);
	
	cameraDir = normalize(cameraPos - FragPos);
	specular = light.specular * texture(material.specular, diffuseTexCoor).rgb * pow(max(dot(cameraDir, reflect(-lightDir, norm)), 0.0f), material.shininess);

	emit = texture(material.emit, diffuseTexCoor).rgb * 0.35f;

	vec3 phongResult = ambient + diffuse + specular + emit;
	FragColor = vec4(phongResult, 1.0f);
	
}
