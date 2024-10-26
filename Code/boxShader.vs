#version 330 core 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoor;
layout (location = 2) in vec3 aNorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 ourTexCoor;
out vec3 ourNorm;
out vec3 FragPos;
out vec4 vertexColor;

uniform vec3 lightColor;
uniform vec3 boxColor;
uniform vec3 cameraPos;
uniform vec3 lightPos;

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 FragColor;

void main(){
	gl_Position = projection * view * model * vec4(aPos, 1.0f);	
	ourTexCoor = aTexCoor;
	ourNorm = mat3(transpose(inverse(model))) * aNorm;
	FragPos = vec3(model * vec4(aPos, 1.0f));

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

	vertexColor = vec4(phongResult, 1.0f);
	
}
