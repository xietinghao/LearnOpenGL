#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
class Shader
{
public:
	unsigned int ID;

	Shader(const char* vShaderPath, const char* fShaderPath) {
		std::string vShaderCode;
		std::ifstream vShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vShaderFile.open(vShaderPath);
			std::stringstream vShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			vShaderFile.close();
			vShaderCode = vShaderStream.str();
		}
		catch (const std::exception& e)
		{
			std::cout << "fShader Open Failed" << e.what() << std::endl;
		}

		std::string fShaderCode;
		std::ifstream fShaderFile;
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			fShaderFile.open(fShaderPath);
			std::stringstream fShaderStream;
			fShaderStream << fShaderFile.rdbuf();
			fShaderFile.close();
			fShaderCode = fShaderStream.str();
		}
		catch(const std::exception &e){
			std::cout << "fShader Open Failed" << e.what() << std::endl;
		}
		const char* vertexCode = vShaderCode.c_str();
		const char* fragmentCode = fShaderCode.c_str();
		unsigned int vShader, fShader;
		vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &vertexCode, NULL);
		glCompileShader(vShader);

		int success;
		char infolog[512];
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vShader, 512, NULL, infolog);
			std::cout << "vertex shader compile failed" << infolog<< std::endl;
		}

		fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &fragmentCode, NULL);
		glCompileShader(fShader);

		
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fShader, 512, NULL, infolog);
			std::cout << "fragment shader compile failed" << infolog << std::endl;
		}
		ID = glCreateProgram();
		glAttachShader(ID, vShader);
		glAttachShader(ID, fShader);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(ID, 512, NULL, infolog);
			std::cout << "link failed" << infolog << std::endl;
		}
		glDeleteShader(vShader);
		glDeleteShader(fShader);
		
		
	};
	void Use() {
		glUseProgram(ID);
	}
	void setMat4(std::string name, glm::mat4 value) {
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
	void setVec3(std::string name, glm::vec3 value) {
		glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
	}
	void setInt(std::string name, int value) {
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(std::string name, float value) {
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}

private:

};


