#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

using std::vector;
using std::string;

struct Vertex{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture {
	unsigned int textureID;
	string type;
	string path;
};

class Mesh
{
public:
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	unsigned int VAO, VBO, EBO;
	
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void draw(Shader &ourShader);

private:

	void setupMesh();
};

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh() {
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glBindVertexArray(0);

}

void Mesh::draw(Shader &ourShader) {
	int textureSize = textures.size();
	int diffuse, specular;
	diffuse = specular = 0;
	for (int i = 0;i < textureSize;i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		string name;
		if (textures[i].type == "texture_diffuse") {
			name = textures[i].type + std::to_string(diffuse++);
		}
		else if (textures[i].type == " texture_specular") {
			name = textures[i].type + std::to_string(specular++);
		}
		ourShader.setInt(name.c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].textureID);
	}

	ourShader.Use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	glActiveTexture(GL_TEXTURE0);

}
