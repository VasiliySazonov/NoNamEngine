#pragma once
#include <map>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../utils/assimpToGLM.h"
#include "../utils/vendor/stb_image.h"
#include "mesh.hpp"

// stores loaded from model file animation bone information
struct _Bone
{
	int id;
	glm::mat4 offset;
};

// this is importer and 
class Model
{
private:
	int boneCounter;
	std::string mPath;

	std::map<std::string, _Bone> _mBones;
	const aiScene *scene;

	void load(std::string path);
	void processNode(aiNode *node);
	Mesh *processMesh(aiMesh *mesh, aiNode *node);

	void extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh);

	void setVertexBoneDataToDefault(Vertex &vertex);
	void setVertexBoneData(Vertex& vertex, int ID, float weight);

	std::vector<Texture> getMaterialTextures(aiMaterial *mat, aiTextureType type);

public:
	std::vector<Mesh*> meshes;

	Model(std::string path, std::string name);
	~Model();

	std::map<std::string, _Bone> &getBones() { return _mBones; }
	int &getBoneCount() { return boneCounter; }

	// render all meshes
	void render(Shader *shader);
};
