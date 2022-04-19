#pragma once
#include "animation.hpp"


class Animator // calculate global transformation matrix for every bone
{
private:
	std::vector<glm::mat4> finalBoneMatrices;
	float deltaTime;

	void calculateBoneTransform(const _AssimpNode *node, glm::mat4 parentTransform);

public:
	float currentTime;
	
	Animation *currentAnimation;
	Animator();
	void updateAnimation(float delta);
	void play(Animation *animation);

	std::vector<glm::mat4> getFinalBoneMatrices();
};
