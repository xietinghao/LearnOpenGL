#version 330 core
in vec3 normal;
in vec2 texCoords;
in vec3 FragPos;

float shininess = 32.f;


struct DotLight{
	vec3 lightPos;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float kc;
	float kl;
	float kq;
};

#define dotLightsNum 2
uniform sampler2D texture_specular0;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_diffuse1;
uniform  vec3 cameraPos;

uniform DotLight dotLights[dotLightsNum];
out vec4 FragColor;

vec3 countDotLight(DotLight light, vec3 FragPos, vec3 normal, vec3 cameraDir);

void main(){
	vec3 color = vec3(0.0f);
	vec3 cameraDir = normalize(cameraPos-FragPos);
	for(int i=0;i<dotLightsNum;i++){
		color += countDotLight(dotLights[i], FragPos, normalize(normal), cameraDir);
	}
	FragColor = vec4(color, 1.0f);
	
	

}

vec3 countDotLight(DotLight light, vec3 FragPos, vec3 normal, vec3 cameraDir){
	vec3 ambient, diffuse, specular;
	vec3 norm = normalize(normal);
	
	if(light.ambient == glm::vec3(0.0f))ambient = light.ambient * texture(texture_diffuse0, texCoords).rgb;
	else ambient = light.ambient * texture(texture_diffuse0, texCoords).rgb;

	float distance = length(light.lightPos - FragPos);
	float attenuation = 1.0/(light.kc + light.kl * distance + light.kq * (distance *distance));
	vec3 lightDir = normalize(light.lightPos - FragPos);
	float lightNormCos = max(dot(norm, lightDir), 0);
	diffuse = lightNormCos * light.diffuse * texture(texture_diffuse0, texCoords).rgb;

	float cameraReflectCos = max(dot(reflect(lightDir, norm), normalize(cameraDir)), 0.0);
	specular = pow(cameraReflectCos, shininess) * light.specular * texture(texture_specular0, texCoords).rgb; 

	
	return (ambient + diffuse + specular) * attenuation;
}
