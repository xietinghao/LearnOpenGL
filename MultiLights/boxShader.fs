#version 330 core 
in vec2 ourTexCoor;
in vec3 ourNorm;
in vec3 FragPos;
in vec2 lightTexCoor;

uniform sampler2D texture0;
uniform sampler2D texture1;


struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emit;

	float shininess;
};

struct DirectionalLight{
	vec3 lightDir;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DotLight{
	vec3 lightPos;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float kc;
	float kl;
	float kq;
};

struct SpotLight{
	vec3 lightPos;
	vec3 spotDir;
	
	float innerCos;
	float outerCos;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float kc;
	float kl;
	float kq;
};

#define dotLightNum 4
uniform Material material;
uniform DirectionalLight directionalLight;
uniform DotLight dotLights[dotLightNum];
uniform SpotLight spotLight;
uniform vec3 cameraPos;

out vec4 FragColor;

vec3 countDirectionalLight(DirectionalLight light, vec3 normal, vec3 cameraDir);
vec3 countDotLight(DotLight light, vec3 FragPos, vec3 normal, vec3 cameraDir);
vec3 countSpotLight(SpotLight light, vec3 FragPos, vec3 normal, vec3 cameraDir);

void main(){

	vec3 cameraDir = normalize(cameraPos - FragPos);
	vec3 result = vec3(0.0f);
	result += countDirectionalLight(directionalLight, ourNorm, cameraDir);
	
	for(int i=0; i<4; i++){
		result += countDotLight(dotLights[i], FragPos, ourNorm, cameraDir);
	}

	result += countSpotLight(spotLight, FragPos, ourNorm, cameraDir);
	FragColor = vec4(result, 1.0f);
	
}

vec3 countDirectionalLight(DirectionalLight light, vec3 normal, vec3 cameraDir){
	vec3 ambient, diffuse, specular;
	vec3 norm = normalize(normal);
	ambient = light.ambient * texture(material.diffuse, lightTexCoor).rgb;

	float lightNormCos = max(dot(norm, -light.lightDir), 0);
	diffuse = lightNormCos * light.diffuse * texture(material.diffuse, lightTexCoor).rgb;

	float cameraReflectCos = max(dot(reflect(-light.lightDir, norm), normalize(cameraDir)), 0.0);
	specular = pow(cameraReflectCos, material.shininess) * light.specular * texture(material.specular, lightTexCoor).rgb; 

	return ambient + diffuse + specular;
}

vec3 countDotLight(DotLight light, vec3 FragPos, vec3 normal, vec3 cameraDir){
	vec3 ambient, diffuse, specular;
	vec3 norm = normalize(normal);
	
	ambient = light.ambient * texture(material.diffuse, lightTexCoor).rgb;
	
	float distance = length(light.lightPos - FragPos);
	float attenuation = 1.0/(light.kc + light.kl * distance + light.kq * (distance *distance));
	vec3 lightDir = normalize(light.lightPos - FragPos);
	float lightNormCos = max(dot(norm, lightDir), 0);
	diffuse = lightNormCos * light.diffuse * texture(material.diffuse, lightTexCoor).rgb;

	float cameraReflectCos = max(dot(reflect(lightDir, norm), normalize(cameraDir)), 0.0);
	specular = pow(cameraReflectCos, material.shininess) * light.specular * texture(material.specular, lightTexCoor).rgb; 

	
	return (ambient + diffuse + specular) * attenuation;
}

vec3 countSpotLight(SpotLight light, vec3 FragPos, vec3 normal, vec3 cameraDir){
	vec3 ambient, diffuse, specular;
	vec3 norm = normalize(normal);
	
	ambient = light.ambient * texture(material.diffuse, lightTexCoor).rgb;

	float distance = length(light.lightPos - FragPos);
	float attenuation = 1.0/(light.kc + light.kl * distance + light.kq * (distance *distance));
	
	vec3 lightDir = normalize(light.lightPos - FragPos);
	float theta = max(dot(-light.spotDir, lightDir), 0.0f);
	float intensity = clamp((theta - light.outerCos)/(light.innerCos - light.outerCos), 0.0f, 1.0f);
	
	float lightNormCos = max(dot(norm, lightDir), 0.0f);
	diffuse = lightNormCos * light.diffuse * texture(material.diffuse, lightTexCoor).rgb;

	float cameraReflectCos = max(dot(reflect(lightDir, norm), normalize(cameraDir)), 0.0);
	specular = pow(cameraReflectCos, material.shininess) * light.specular * texture(material.specular, lightTexCoor).rgb; 
	
	return (ambient + diffuse + specular) * attenuation * intensity;
}
