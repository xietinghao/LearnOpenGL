#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "Camera.h"
#include "Shader.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float xposLast = WINDOW_WIDTH / 2;
float yposLast = WINDOW_HEIGHT / 2;
bool cursorUnUsed = true;
float radius = 5.0f;

glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 boxColor(1.0f, 0.5f, 0.31f);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 directionalLightDir(-0.2f, -1.0f, -0.3f);
float spotLightInnerAngle = 10.f;
float spotLightOuterAngle = 25.f;
float spotLightInnerCos = cos(glm::radians(spotLightInnerAngle));
float spotLightOuterCos = cos(glm::radians(spotLightOuterAngle));

int main() {
	//初始化glfw函数
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLM_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "lightWindow", NULL, NULL);
	if (window == NULL) {
		std::cout << "window craete failed" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Initial GLAD failed" << std::endl;
		glfwTerminate();
		return -1;
	}

	//纹理的载入
	stbi_set_flip_vertically_on_load(true);
	unsigned int texture0, texture1;
	texture0 = loadTexture("container.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture0);

	texture1 = loadTexture("awesomeface.png");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	//漫发射和高光纹理贴图的加载
	unsigned int textureDiffuse = loadTexture("container2.png");
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureDiffuse);
	
	unsigned int textureSpecular = loadTexture("container2_specular.png");
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureSpecular);

	unsigned int textureEmit = loadTexture("matrix.jpg");
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureEmit);

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f,-1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f,-1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f,-1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f,-1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f,-1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f,-1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f, 0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
	};

	// positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	glm::vec3 dotLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	glm::vec3 dotLightColors[] = {
		glm::vec3(1.0f, 0.6f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0, 0.0),
		glm::vec3(0.2f, 0.2f, 1.0f)
	};

	unsigned int* indices = new unsigned int[36];
	for (int i = 0; i < 36; i++)indices[i] = i;

	unsigned int lightVAO, boxVAO, VBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//坐标
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//纹理坐标
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//法向量贴图
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//漫反射贴图
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);

	Shader lightShader("lightShader.vs", "lightShader.fs");
	Shader boxShader("boxShader.vs", "boxShader.fs");
	glEnable(GL_DEPTH_TEST);

	boxShader.Use();
	//定向光源参数
	boxShader.setVec3("directionalLight.lightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
	boxShader.setVec3("directionalLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	boxShader.setVec3("directionalLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	boxShader.setVec3("directionalLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	//点光源参数
	std::string s1 = "dotLights[";
	std::string s_lightPos = "].lightPos";
	std::string s_kc = "].kc";
	std::string s_kl = "].kl";
	std::string s_kq = "].kq";
	std::string s_specular = "].specular";
	std::string s_diffuse= "].diffuse";
	std::string s_ambient= "].ambient";
	for (int i = 0; i < 4; i++) {
		boxShader.setVec3(s1 + std::to_string(i) + s_lightPos, dotLightPositions[i]);
		boxShader.setFloat(s1+std::to_string(i)+s_kc, 1.0f);
		boxShader.setFloat(s1 + std::to_string(i) + s_kl, 0.09f);
		boxShader.setFloat(s1 + std::to_string(i) + s_kq, 0.032f);
		boxShader.setVec3(s1 + std::to_string(i) + s_specular, dotLightColors[i]);
		boxShader.setVec3(s1 + std::to_string(i) + s_diffuse, dotLightColors[i] * 0.8f);
		boxShader.setVec3(s1 + std::to_string(i) + s_ambient, dotLightColors[i] * 0.05f);
		
	}/*
	boxShader.setVec3("dotLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	boxShader.setVec3("dotLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	boxShader.setVec3("dotLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	boxShader.setVec3("dotLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	boxShader.setVec3("dotLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	boxShader.setVec3("dotLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	boxShader.setVec3("dotLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	boxShader.setVec3("dotLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	boxShader.setVec3("dotLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	boxShader.setVec3("dotLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	boxShader.setVec3("dotLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	boxShader.setVec3("dotLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));*/
	//聚光源参数
	boxShader.setFloat("spotLight.innerCos", spotLightInnerCos);
	boxShader.setFloat("spotLight.outerCos", spotLightOuterCos);
	boxShader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	boxShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	boxShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	boxShader.setFloat("spotLight.kc", 1.0f);
	boxShader.setFloat("spotLight.kl", 0.09f);
	boxShader.setFloat("spotLight.kq", 0.032f);

	//加载纹理和材质属性
	boxShader.setInt("texture0", 0);
	boxShader.setInt("texture1", 1);
	boxShader.setInt("material.diffuse", 2);
	boxShader.setInt("material.specular", 3);
	boxShader.setInt("material.emit", 4);
	boxShader.setFloat("material.shininess", 32.f);

	glm::mat4 model, view, projection;
	while (!glfwWindowShouldClose(window)) {

		processInput(window);
		glClearColor(0.5f, 0.3f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightOffset{ sin(glfwGetTime()) * radius, 0.0f, cos(glfwGetTime()) * radius };
		/*lightColor.x = 2.0f + static_cast<float>(sin(glfwGetTime()) * 2.0f);
		lightColor.y = lightColor.z = lightColor.x;*/
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.f);
		
		lightShader.Use();
		lightShader.setVec3("lightColor", lightColor);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		for (int i = 0; i < 4; i++) {
			lightShader.setVec3("lightColor", dotLightColors[i]);
			model = glm::translate(glm::mat4(1.0f), dotLightPositions[i]) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
			lightShader.setMat4("model", model);
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureDiffuse);		
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureSpecular);
		boxShader.Use();
		//顶点属性`
		boxShader.setMat4("view", view);
		boxShader.setMat4("projection", projection);
		boxShader.setVec3("cameraPos", camera.Position);
		boxShader.setVec3("spotLight.spotDir", camera.Front);
		boxShader.setVec3("spotLight.lightPos", camera.Position);
		
		for (int i = 0; i < 10; i++) {
			float angle = 20.f * i;
			boxShader.Use();
			model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
			model= glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			boxShader.setMat4("model", model);
			glBindVertexArray(boxVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &boxVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	if (cursorUnUsed) {
		xposLast = static_cast<float>(xpos);
		yposLast = static_cast<float>(ypos);;
		cursorUnUsed = false;
	}
	float xoffset = static_cast<float>(xpos) - xposLast;
	float yoffset = static_cast<float>(-ypos) + yposLast;
	yposLast = ypos;
	xposLast = xpos;
	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	static double currentTime, lastTime;
	currentTime = glfwGetTime();
	float deltaTime = static_cast<float>(currentTime - lastTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	lastTime = currentTime;
}

unsigned int loadTexture(const char* path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	GLenum format;
	if (nrChannels == 1) {
		format = GL_RED;
	}
	else if (nrChannels == 3) {
		format = GL_RGB;
	}
	else if (nrChannels == 4) {
		format = GL_RGBA;
	}
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << path << "texture load failed" << std::endl;
	}
	stbi_image_free(data);
	data = NULL;
	return textureID;
}
