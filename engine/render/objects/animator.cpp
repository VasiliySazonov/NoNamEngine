#include "animator.hpp"

Animator::Animator()
{
	currentTime = 0;

	currentAnimation = NULL;

	for (int i = 0; i < 100; ++i)
		finalBoneMatrices.push_back(glm::mat4(1.0));
}

void Animator::play(Animation *animation)
{
	currentTime = 0;
	currentAnimation = animation;
}

void Animator::updateAnimation(float delta)
{
	deltaTime = delta;
	currentTime += currentAnimation->getTicksPerSecond() * delta;
	currentTime = fmod(currentTime, currentAnimation->getDuration());
	calculateBoneTransform(&currentAnimation->getRootNode(), glm::mat4(1.0f));
}

void Animator::calculateBoneTransform(const _AssimpNode *node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone *Bone = currentAnimation->findBone(nodeName);

	if (Bone)
	{
		Bone->update(currentTime);
		nodeTransform = Bone->getLocalTransform();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = currentAnimation->getBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		finalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		calculateBoneTransform(&node->children[i], globalTransformation);
}

std::vector<glm::mat4> Animator::getFinalBoneMatrices() 
{
	return finalBoneMatrices;  
}