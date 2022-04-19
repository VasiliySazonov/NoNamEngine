#pragma once
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../utils/assimpToGLM.h"

struct KeyPosition
{
	glm::vec3 position;
	float time;
};

struct KeyRotation
{
	glm::quat orientation;
	float time;
};

struct KeyScale
{
	glm::vec3 scale;
	float time;
};

class Bone
{
private:
	std::vector<KeyPosition> positions;
	std::vector<KeyRotation> rotations;
	std::vector<KeyScale> scales;
	int numPositions;
	int numRotations;
	int numScalings;
	
	glm::mat4 localTransform;
	std::string name;
	int ID;

	glm::mat4 getPositionFromTime(float time);
	glm::mat4 getRotationFromTime(float time);
	glm::mat4 getScalingFromTime(float time);

	int getPositionClosestFrameFromTime(float time);
	int getRotationClosestFrameFromTime(float time);
	int getScalingClosestFrameFromTime(float time);

	float getScaleFactor(float lastTime, float nextTime, float currentTime);

public:
	Bone(std::string name, int ID, const aiNodeAnim *channel);

	void update(float time);

	inline glm::mat4 getLocalTransform() { return localTransform; }
	inline std::string getBoneName() const { return name; }
	inline int getBoneID() { return ID; }
};
