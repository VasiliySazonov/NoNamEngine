#include "bone.hpp"

Bone::Bone(std::string name, int ID, const aiNodeAnim *channel) : name(name), ID(ID), localTransform(localTransform)
{
	numPositions = channel->mNumPositionKeys;

	for (int frame = 0; frame < numPositions; ++frame)
	{
		KeyPosition framePos;

		framePos.position = AssimpGLMHelpers::GetGLMVec(channel->mPositionKeys[frame].mValue);
		framePos.time = channel->mPositionKeys[frame].mTime;

		positions.push_back(framePos);
	}

	numRotations = channel->mNumRotationKeys;

	for (int frame = 0; frame < numRotations; ++frame)
	{
		KeyRotation framePos;

		framePos.orientation = AssimpGLMHelpers::GetGLMQuat(channel->mRotationKeys[frame].mValue);
		framePos.time = channel->mRotationKeys[frame].mTime;

		rotations.push_back(framePos);
	}

	numScalings = channel->mNumScalingKeys;

	for (int frame = 0; frame < numScalings; ++frame)
	{
		KeyScale framePos;

		framePos.scale = AssimpGLMHelpers::GetGLMVec(channel->mScalingKeys[frame].mValue);
		framePos.time = channel->mScalingKeys[frame].mTime;

		scales.push_back(framePos);
	}
}

float Bone::getScaleFactor(float lastTime, float nextTime, float currentTime)
{
	return (currentTime - lastTime) / (nextTime - lastTime);
}

int Bone::getPositionClosestFrameFromTime(float time)
{
	for (int frame = 0; frame < numPositions - 1; frame++)
	{
		if (time < positions[frame + 1].time)
			return frame;
	}
	assert(0);
}

int Bone::getRotationClosestFrameFromTime(float time)
{
	for (int frame = 0; frame < numRotations - 1; frame++)
	{
		if (time < rotations[frame + 1].time)
			return frame;
	}
	assert(0);
}

int Bone::getScalingClosestFrameFromTime(float time)
{
	for (int frame = 0; frame < numScalings - 1; frame++)
	{
		if (time < scales[frame + 1].time)
			return frame;
	}
	assert(0);
}

glm::mat4 Bone::getPositionFromTime(float time)
{
	if (numPositions == 1)
	{
		return glm::translate(glm::mat4(1.0f), positions[0].position);
	}

	int fIndexL = getPositionClosestFrameFromTime(time);
	int fIndexN = fIndexL + 1;

	float scaleFactor = getScaleFactor(positions[fIndexL].time, positions[fIndexN].time, time);
	glm::vec3 finalPosition = glm::mix(positions[fIndexL].position, positions[fIndexN].position, scaleFactor);

	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::getRotationFromTime(float time)
{
	if (numPositions == 1)
	{
		return glm::toMat4(glm::normalize(rotations[0].orientation));
	}

	int fIndexL = getRotationClosestFrameFromTime(time);
	int fIndexN = fIndexL + 1;

	float scaleFactor = getScaleFactor(rotations[fIndexL].time, rotations[fIndexN].time, time);
	glm::quat finalRotation = glm::slerp(rotations[fIndexL].orientation, rotations[fIndexN].orientation, scaleFactor);

	return glm::toMat4(glm::normalize(finalRotation));
}

glm::mat4 Bone::getScalingFromTime(float time)
{
	if (numPositions == 1)
	{
		return glm::scale(glm::mat4(1.0f), scales[0].scale);
	}

	int fIndexL = getScalingClosestFrameFromTime(time);
	int fIndexN = fIndexL + 1;

	float scaleFactor = getScaleFactor(scales[fIndexL].time, scales[fIndexN].time, time);
	glm::vec3 finalScale = glm::mix(scales[fIndexL].scale, scales[fIndexN].scale, scaleFactor);

	return glm::scale(glm::mat4(1.0f), finalScale);
}

void Bone::update(float time)
{
	localTransform = getPositionFromTime(time) * getRotationFromTime(time) * getScalingFromTime(time);
}
