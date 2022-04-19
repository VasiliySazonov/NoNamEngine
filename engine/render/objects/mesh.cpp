#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> verticies, std::vector<unsigned int> indicies, std::vector<Texture> textures)
{
	this->verticies = verticies;
	this->indicies = indicies;
	this->textures = textures;

	setup();
}


Mesh::~Mesh()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);

	for (int i = 0; i < textures.size(); i++)
		glDeleteTextures(1, &textures[i].id);
}

void Mesh::setup()
{
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);

	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->verticies.size() * sizeof(Vertex), &this->verticies[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indicies.size() * sizeof(unsigned int), &this->indicies[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));


	glBindVertexArray(0);
}

void Mesh::render(Shader *shader)
{	
	int currentDiff = 0, currentSpec = 0, currentNorm = 0;
	for (int i = 0; i < this->textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i + 3);
		glBindTexture(GL_TEXTURE_2D, textures.at(i).id);
		
		if(this->textures.at(i).type == Texture::DIFFUSE)
		{
			shader->uniform1i("texture_diffuse" + std::to_string(currentDiff), i + 3);
		}
		else if(this->textures.at(i).type == Texture::SPECULAR)
		{
			shader->uniform1i("texture_specular" + std::to_string(currentSpec), i + 3);
		}
		else if(this->textures.at(i).type == Texture::NORMAL)
		{
			shader->uniform1i("texture_normal" + std::to_string(currentNorm), i + 3);
		}
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, this->indicies.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
