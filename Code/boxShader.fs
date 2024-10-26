#version 330 core 
in vec2 ourTexCoor;
in vec3 ourNorm;
in vec3 FragPos;
in vec4 vertexColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 boxColor;

uniform vec3 cameraPos;
uniform vec3 lightPos;

out vec4 FragColor;
void main(){
	float ambient, specular, diffuse;
	vec3 lightDir, cameraDir;
	ambient = 0.1f;
	vec3 norm = normalize(ourNorm);
	

	lightDir = normalize(lightPos - FragPos);
	diffuse = max(dot(lightDir, norm), 0.0f);
	
	cameraDir = normalize(cameraPos - FragPos);
	float specStrenth = 0.5f;
	specular = specStrenth * pow(max(dot(cameraDir, reflect(-lightDir, norm)), 0.0f), 96);

	vec3 phongResult = (ambient + diffuse + specular) * boxColor * lightColor;

	
	FragColor = mix(texture(texture0, vec2(ourTexCoor.x, ourTexCoor.y)), texture(texture1, vec2(ourTexCoor.x, ourTexCoor.y)), 0.2) * vec4(phongResult, 1.0f);
	
}
