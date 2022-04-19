#include "animation.hpp"

Animation::Animation(std::string animationPath, Model *model, int index)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	
	assert(scene && scene->mRootNode);
	
	auto animation = scene->mAnimations[index];
	duration = animation->mDuration;
	tps = animation->mTicksPerSecond;
	
	readHeirarchyData(rootNode, scene->mRootNode);
	readMissingBones(animation, *model);
}

Bone *Animation::findBone(std::string &name)
{
	auto iter = std::find_if(bones.begin(), bones.end(),
		[&name](Bone &bone)
		{
			return bone.getBoneName() == name;
		});

	if (iter == bones.end())
		return nullptr;
	else
		return &(*iter);
}

void Animation::readMissingBones(const aiAnimation *animation, Model &model)
{
	int size = animation->mNumChannels;
	auto &BoneIDMap_l = model.getBones();
	int &boneCount = model.getBoneCount();

	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string name = channel->mNodeName.data;

		if (BoneIDMap_l.find(name) == BoneIDMap_l.end())
		{
			BoneIDMap_l[name].id = boneCount;
			boneCount++;
		}
		bones.push_back(Bone(channel->mNodeName.data, BoneIDMap_l[channel->mNodeName.data].id, channel));
	}

	BoneIDMap = BoneIDMap_l;
}

void Animation::readHeirarchyData(_AssimpNode &dest, const aiNode *src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;

	for (int i = 0; i < src->mNumChildren; i++)
	{
		_AssimpNode newData;
		readHeirarchyData(newData, src->mChildren[i]);
		dest.children.push_back(newData);
	}
}
