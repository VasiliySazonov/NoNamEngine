#pragma once
#include "bone.hpp"
#include "model.hpp"

// to assing bones to model hierarchy
struct _AssimpNode
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<_AssimpNode> children;
};


class Animation
{
private:
	float duration;
	int tps; // ticks per second
	
	std::vector<Bone> bones;
	_AssimpNode rootNode;
	std::map<std::string, _Bone> BoneIDMap;

	void readMissingBones(const aiAnimation *animation, Model &model);
	void readHeirarchyData(_AssimpNode &dest, const aiNode *src);

public:
	Animation() = default;
	Animation(std::string animationPath, Model *model, int index);
	Bone *findBone(std::string &name);

	inline float getTicksPerSecond() const { return tps; }
	inline float getDuration() const { return duration; }
	inline _AssimpNode &getRootNode() { return rootNode; }
	inline std::map<std::string, _Bone> &getBoneIDMap() { return BoneIDMap; }
};
