#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <GL/glew.h>
#include "../utils/shader.hpp"

#define MAX_BONE_INFLUENCE 4 // bones influencing 1 vertex

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	int boneIDs[MAX_BONE_INFLUENCE]; // bones influencing this vertex 
	float weights[MAX_BONE_INFLUENCE]; // weights for every bone
};

struct Texture
{
	enum
	{
		DIFFUSE = 0,
		SPECULAR = 1,
		NORMAL = 2
	};

	unsigned int id; // OpenGL id
	int type; // diffuse, specular etc.
};


class Mesh
{
private:
	std::vector<Vertex> verticies;
	std::vector<unsigned int> indicies;
	std::vector<Texture> textures;

	unsigned int VAO, VBO, EBO;

public:
	// currently not used, later will be used to store transform relative to Model Coord Space
	glm::mat4 transofrm;

	Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indicies, std::vector<Texture> textures);
	Mesh() = default;
	~Mesh();

	void setup();

	void render(Shader *shader);
};
