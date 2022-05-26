#include "model.hpp"

Model::Model(std::string path, std::string name)
{
	boneCounter = 0;
	mPath = path;
	load(path + name);
}

Model::~Model()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		delete meshes[i];
	}
}

void Model::load(std::string path)
{
	Assimp::Importer importer;
	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	
	assert(scene);

	processNode(scene->mRootNode);
}

void Model::processNode(aiNode *node)
{
	for(int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, node));
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i]);
	}
}

Mesh *Model::processMesh(aiMesh *mesh, aiNode *node)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;


	for(int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		setVertexBoneDataToDefault(vertex);

		vertex.position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);

		if(mesh->HasNormals())
		{
			vertex.normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
		}

		if(*mesh->mTextureCoords)
		{
			vertex.texCoords = AssimpGLMHelpers::GetGLMVec(mesh->mTextureCoords[0][i]);
		}
		else
		{
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for(int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for(int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

	std::vector<Texture> diffuseMaps = getMaterialTextures(material, aiTextureType_DIFFUSE);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	extractBoneWeightForVertices(vertices, mesh);

	return new Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::getMaterialTextures(aiMaterial *mat, aiTextureType type)
{
	std::vector<Texture> textures_l;

	for(int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		Texture texture;

		unsigned char *buffer;
		int width, height, bpp;

		buffer = stbi_load((mPath + std::string(str.C_Str())).c_str(), &width, &height, &bpp, 4);

		glGenTextures(1, &texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(buffer);
		
		int type_t;
		if(type == aiTextureType_DIFFUSE)
		{
			type_t = Texture::DIFFUSE;
		}

		else if(type == aiTextureType_SPECULAR)
		{
			type_t = Texture::SPECULAR;
		}

		else if(type == aiTextureType_HEIGHT)
		{
			type_t = Texture::NORMAL;
		}

		texture.type = type_t;

		textures_l.push_back(texture);
	}

	return textures_l;
}

void Model::setVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.boneIDs[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}


void Model::setVertexBoneData(Vertex& vertex, int ID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if (vertex.boneIDs[i] == -1)
		{
			vertex.boneIDs[i] = ID;
			vertex.weights[i] = weight;
			break;
		}
	}
}

void Model::extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh *mesh)
{
	for (int bone = 0; bone < mesh->mNumBones; ++bone)
	{
		int boneID = -1;
		std::string name = mesh->mBones[bone]->mName.C_Str();

		if (_mBones.find(name) == _mBones.end())
		{
			_Bone newBone;
			newBone.id = boneCounter;
			newBone.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[bone]->mOffsetMatrix);
			_mBones[name] = newBone;
			boneID = boneCounter;
			boneCounter++;
		}
		else
		{
			boneID = _mBones[name].id;
		}
		assert(boneID != -1);

		auto weights = mesh->mBones[bone]->mWeights;
		int numWeights = mesh->mBones[bone]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;

			assert(vertexId <= vertices.size());

			setVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}

void Model::render(Shader *shader)
{
	for (int i = 0; i < meshes.size(); i++)
		meshes[i]->render(shader);
}
