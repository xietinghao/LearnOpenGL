#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <unordered_map>
#include "Mesh.h"
using std::vector; using std::string;
using std::cout; using std::endl;
unsigned int TextureFromFile(const string&, const string&);

class Model
{
public:
	Model(const string &modelPath);
	void draw(Shader &ourShader);

private:
	vector<Mesh> meshes;
	vector<Texture> textures_loaded;
	string directory;
	std::unordered_map<string, int> textures_map;
	
	void loadModel(string const &modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterials(aiMaterial* mat, aiTextureType type, string typename);
};

Model::Model(const string &modelPath)
{
	loadModel(modelPath);
}

void Model::draw(Shader &ourShader) {
	for (unsigned int i = 0;i < meshes.size();i++) {
		meshes[i].draw(ourShader);
	}

}

void Model::loadModel(const string &modelPath) {
	Assimp::Importer import;
	//cout << modelPath << endl;
	const aiScene* scene = import.ReadFile(modelPath, aiProcess_Triangulate| aiProcess_FlipUVs);
	if (!scene || AI_SCENE_FLAGS_INCOMPLETE & scene->mFlags || !scene->mRootNode) {
		//cout << modelPath << endl;
		cout << "ERROR::ASSIMP" << import.GetErrorString() << endl;
		return;
	}
	directory=modelPath.substr(0,modelPath.find_last_of('/'));
	
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0;i < node->mNumMeshes;i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0;i < node->mNumChildren;i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0;i < mesh->mNumVertices;i++) {
		Vertex vertex;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		if (mesh->mTextureCoords[0]) {
			vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		if (mesh->HasNormals()) {
			vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}
		
		vertices.push_back(vertex);
	}
	
	for (unsigned int i = 0;i < mesh->mNumFaces;i++) {
		for (int j = 0;j < mesh->mFaces[i].mNumIndices;j++) {
			indices.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		vector<Texture> textures_diffuse = loadMaterials(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE, "texture_diffuse");
		vector<Texture> textures_specular = loadMaterials(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR, "texture_specular");
		std::move(textures_diffuse.begin(), textures_diffuse.end(), std::back_inserter(textures));
		std::move(textures_specular.begin(), textures_specular.end(), std::back_inserter(textures));
	}
	return Mesh(vertices, indices, textures);
}
vector<Texture> Model::loadMaterials(aiMaterial* mat, aiTextureType type, string typeName) {
	vector<Texture> textures;
	for (unsigned int i = 0;i < mat->GetTextureCount(type);i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		string texturePath = directory + str.C_Str();
		if (textures_map.find(texturePath) != textures_map.end()) {
			textures.push_back(textures_loaded[textures_map[texturePath]]);
			continue;
		}
		
		Texture texture;
		texture.textureID = TextureFromFile(str.C_Str(), directory);
		texture.type = typeName;
		texture.path = texturePath;
		textures_map[texturePath] = textures_loaded.size();
		textures_loaded.push_back(texture);
		textures.push_back(texture);
	}
	return textures;
}

unsigned int TextureFromFile(const string &fileName, const string &diectory) {
	string path = diectory + '/' + fileName;
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		GLenum format;
		if (nrChannels == 1)format = GL_RED;
		else if (nrChannels == 3)format = GL_RGB;
		else if (nrChannels == 4)format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		data = NULL;
	}
	else {
		cout << "ERROR::TEXTURE LOAD FAILED" << endl;
	}
	
	return textureID;

}
