#pragma once
#include "logic/base/base.hpp"
#include "render/objects/animator.hpp"
#include "render/utils/shader.hpp"
#include "window/window.h"
#include "window/events.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace Logical;

struct ShaderPaths
{
	std::string lightingVertex;
	std::string lightingFragment;
	std::string shadowVertex;
	std::string shadowFragment;
	std::string skyboxVertex;
	std::string skyboxFragment;
};

struct WindowProperties
{
	std::string name;
	unsigned int width;
	unsigned int height;
};


class Engine
{
private:
	Shader *shaderLighting;
	Shader *shaderShadow;
	Shader *shaderAmbient; // not used
	Shader *skyboxShader;

	Shader *currentShader;

	EScene *currentScene;

	double deltaTime;

	int lightCounter_point = 0;
	int shadowCounter_direct_ortho = 0;

	inline void runEntSetup(Entity *ent);
	void runEntSetupRecursive(Entity *ent);

	inline void runEntUpdate(Entity *ent, double deltaTime);

	void handleEnt(Entity *ent, glm::mat4 globalTransform);

	void handleModel(EModel *ent, glm::mat4 globalTransform);
	void handleCollectionLOD(ECollectionLOD *ent);
	void handleModel_shadow(Entity *ent, glm::mat4 globalTransform);
	void handleLight_point(ELight_point *light, glm::mat4 globalTransform);
	void handleShadow_direct_ortho(EShadowCasterDirectOrtho *ent);

public:
	Engine(const WindowProperties &window, const ShaderPaths &shaders);
	~Engine();

	void renderScene(EScene *scene);
	void use(); // main loop
};
