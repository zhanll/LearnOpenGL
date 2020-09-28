#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include "Texture.h"

class Shader;

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};


class Mesh {
public:
	// mesh data
	std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>>      textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<std::shared_ptr<Texture>> textures);
	void Draw(Shader& shader);
	void Draw();
	unsigned int GetVAO() const;

private:
	//  render data
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};