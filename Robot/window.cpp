//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include <iostream>
//int main() {
//	Assimp::Importer import;
//	const aiScene* scene = import.ReadFile("D:/Downlaods/Compressed/nanosuit/nanosuit.obj", aiProcess_Triangulate | aiProcess_FlipUVs);
//	if (!scene || AI_SCENE_FLAGS_INCOMPLETE & scene->mFlags || !scene->mRootNode) {
//		std::cout << "ERROR::ASSIMP" << import.GetErrorString() << std::endl;
//		return 1;
//	}
//	return 0;
//}

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Model.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

Camera camera(glm::vec3(0.0f, 10.0f, 5.0f));
float xposLast = WINDOW_WIDTH / 2;
float yposLast = WINDOW_HEIGHT / 2;
bool cursorUnUsed = true;
float radius = 5.0f;

glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 boxColor(1.0f, 0.5f, 0.31f);
glm::vec3 lightPos(-1.5f, 1.5f, 0.0f);


int main() {
	//初始化glfw函数
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLM_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "robotWindow", NULL, NULL);
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
	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	
	glm::vec3 dotLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  -3.0f),
		glm::vec3(0.3f, -0.3f, 3.0f),
	};
	glm::vec3 dotLightColors[] = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
	};

	Shader robotShader = Shader("robotShader.vs", "robotShader.fs");
	Shader lightShader = Shader("lightShader.vs", "lightShader.fs");
	string modelPath = "D:/Downlaods/Compressed/nanosuit/nanosuit.obj";
	Model robotModel = Model(modelPath.c_str());
		
	
	float vertices[] = {
	-0.5f, -0.5f, -0.5f, 
	 0.5f, -0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	-0.5f,  0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f,  

	-0.5f, -0.5f,  0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  
	-0.5f, -0.5f,  0.5f,  

	-0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f,  
	-0.5f, -0.5f, -0.5f,  
	-0.5f, -0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  

	 0.5f,  0.5f,  0.5f,  
	 0.5f,  0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  

	-0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f, -0.5f,  
	 0.5f, -0.5f,  0.5f,  
	 0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f,  0.5f,  
	-0.5f, -0.5f, -0.5f,  

	-0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f, -0.5f,  
	 0.5f,  0.5f,  0.5f,  
	 0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f,  0.5f,  
	-0.5f,  0.5f, -0.5f,  
	};
	
	unsigned int VAO, VBO;
	glGenBuffers(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	
	string lightPrefix = "dotLights[";
	string posSuffix = "].lightPos";
	string ambientSuffix = "].ambient";
	string diffuseSuffix = "].diffuse";
	string specularSuffix = "].specular";
	string kcSuffix = "].kc";
	string klSuffix = "].kl";
	string kqSuffix = "].kq";
	robotShader.Use();
	for (int i = 0;i < 2;i++) {
		robotShader.setVec3(lightPrefix + std::to_string(i) + posSuffix, dotLightPositions[i]);
		robotShader.setVec3(lightPrefix + std::to_string(i) + ambientSuffix, dotLightColors[i] * 0.05f);
		robotShader.setVec3(lightPrefix + std::to_string(i) + diffuseSuffix, dotLightColors[i] * 0.8f);
		robotShader.setVec3(lightPrefix + std::to_string(i) + specularSuffix, dotLightColors[i]);
		robotShader.setFloat(lightPrefix + std::to_string(i) + kcSuffix, 1.0f);
		robotShader.setFloat(lightPrefix + std::to_string(i) + klSuffix, 0.09f);
		robotShader.setFloat(lightPrefix + std::to_string(i) + kqSuffix, 0.032f);
	}
	glm::mat4 model, view, projection;
	while (!glfwWindowShouldClose(window)) {
		
		processInput(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.f);

		lightShader.Use();
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		for (int i = 0;i < 2;i++) {
			model = glm::translate(glm::mat4(1.0f), dotLightPositions[i]) * glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
			lightShader.setMat4("model", model);
			lightShader.setVec3("lightColor", dotLightColors[i]);
			
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			
		}
		
		
		robotShader.Use();
		robotShader.setMat4("projection", projection);
		robotShader.setMat4("view", view);
		model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f));
		robotShader.setMat4("model", model);
		robotShader.setVec3("cameraPos", camera.Position);

		robotModel.draw(robotShader);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}
	
	

	glfwTerminate();

}
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
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

void processInput(GLFWwindow *window) {
	
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
