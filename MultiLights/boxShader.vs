#version 330 core 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoor;
layout (location = 2) in vec3 aNorm;
layout (location = 3) in vec2 aLightTexCoor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 ourTexCoor;
out vec3 ourNorm;
out vec3 FragPos;
out vec2 lightTexCoor;

void main(){
	gl_Position = projection * view * model * vec4(aPos, 1.0f);	
	ourTexCoor = aTexCoor;
	ourNorm = mat3(transpose(inverse(model))) * aNorm;
	FragPos = vec3(model * vec4(aPos, 1.0f));
	lightTexCoor = aLightTexCoor;
}
