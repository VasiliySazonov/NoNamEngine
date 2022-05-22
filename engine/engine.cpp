#include "engine.hpp"

Engine::Engine(const WindowProperties &window, const ShaderPaths &shaders)
{
	// all this stuff will be read from config file
	Window::Initialize( window.width,
					    window.height, 
						window.name, 
						window.antiAliasingLevel
	);
	Events::initialize();

	shaderLighting = new Shader(shaders.lightingVertex, shaders.lightingFragment);
	shaderShadow = new Shader(shaders.shadowVertex, shaders.shadowFragment);
	skyboxShader = new Shader(shaders.skyboxVertex, shaders.skyboxFragment);
}

Engine::~Engine()
{
	delete shaderLighting;
	delete shaderShadow;
	delete skyboxShader;
}

inline void Engine::runEntSetup(Entity *ent)
{
	if (ent->script)
		ent->script->Setup();
}

void Engine::runEntSetupRecursive(Entity *ent)
{
	runEntSetup(ent);

	for (int i = 0; i < ent->children.size(); i++)
		runEntSetupRecursive(ent->children[i]);
}

inline void Engine::runEntUpdate(Entity *ent, double deltaTime)
{
	if (ent->script)
		ent->script->Update(deltaTime);
}

void Engine::handleEnt(Entity *ent, glm::mat4 globalTransform)
{
	if (!ent->enabled)
		return;

	runEntUpdate(ent, deltaTime); // update script
	ent->transform->update();

	switch (ent->type)
	{
		case ENT_MODEL:
			handleModel((EModel*) ent, globalTransform);
			break;

		case ENT_LIGHT:
			handleLight_point((ELight_point*) ent, globalTransform);
			break;

		case ENT_SHADOW_ORTHO:
			handleShadow_direct_ortho((EShadowCasterDirectOrtho*) ent);
			break;

		case ENT_COLLECTION_LOD:
			handleCollectionLOD((ECollectionLOD*)ent);
	}

	for (int i = 0; i < ent->children.size(); i++)
	{
		handleEnt(ent->children[i], globalTransform * ent->transform->model);
	}
}

void Engine::sendAnimationUniform(EModel *model)
{
	if (model->animator->currentAnimation) // check if the model has an animation
	{
		// explicitly point that this model should be animated
		currentShader->uniform1i("animated", true);
		// get model space transform matrices
		std::vector<glm::mat4> transforms = model->animator->getFinalBoneMatrices();
		// push unifroms to the shader
		for (int i = 0; i < transforms.size(); i++)
			currentShader->uniformArrayMatrix4("boneTransforms", i, transforms[i]);
	}
	else 
	{
		// explicitly point that model should not be animated
		currentShader->uniform1i("animated", false);
	}
}

void Engine::handleModel(EModel *model, glm::mat4 globalTransform)
{
	// update animator
	if (model->animator->currentAnimation) // check if the model has an animation
		model->animator->updateAnimation(deltaTime);
	
	sendAnimationUniform(model);
	currentShader->uniformMatrix4("model", globalTransform * model->transform->model);
	model->mdl->render(currentShader);
}

void Engine::handleCollectionLOD(ECollectionLOD *collection)
{
	// children have to be in 1st level only!
	for (int i = 0; i < collection->children.size(); i++)
	{
		float distance = glm::distance(
			currentScene->activeCamera->transform->position,
			collection->transform->position
		);

		EModel *model = (EModel*)collection->children[i];
		model->enabled = false;

		if (model->LOD_properties.LOD_distanceMin <= distance &&
			model->LOD_properties.LOD_distanceMax >= distance)
		{
			model->enabled = true;
		}
	}
}

// this is really bad
void Engine::handleModel_shadow(Entity *ent, glm::mat4 globalTransform)
{
	if (!ent->enabled)
		return;

	if (ent->type == ENT_MODEL)
	{
		currentShader->uniformMatrix4("model", globalTransform * ent->transform->model);
		sendAnimationUniform((EModel*)ent);
		((EModel*)(ent))->mdl->render(currentShader);
	}

	for (int i = 0; i < ent->children.size(); i++)
	{
		handleModel_shadow(ent->children[i], globalTransform * ent->transform->model);
	}
}

void Engine::handleLight_point(ELight_point *light, glm::mat4 globalTransform)
{
	glm::mat4 lightMatrix = light->transform->model * globalTransform;

	glm::vec3 position;

	// not used
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(lightMatrix, scale, rotation, position, skew, perspective);

	currentShader->uniformVector3(
		"pointLights[" + std::to_string(lightCounter_point) + "].position",
		position
	);

	currentShader->uniformVector3(
		"pointLights[" + std::to_string(lightCounter_point) + "].ambient",
		light->properties.ambient
	);

	currentShader->uniformVector3(
		"pointLights[" + std::to_string(lightCounter_point) + "].diffuse",
		light->properties.diffuse
	);

	currentShader->uniformVector3(
		"pointLights[" + std::to_string(lightCounter_point) + "].specular",
		light->properties.specular
	);

	currentShader->uniform1f(
		"pointLights[" + std::to_string(lightCounter_point) + "].constant",
		light->properties.constant
	);

	currentShader->uniform1f(
		"pointLights[" + std::to_string(lightCounter_point) + "].linear",
		light->properties.linear
	);

	currentShader->uniform1f(
		"pointLights[" + std::to_string(lightCounter_point) + "].quadratic",
		light->properties.quadratic
	);

	lightCounter_point++;
};

void Engine::handleShadow_direct_ortho(EShadowCasterDirectOrtho *shadow)
{
	shaderShadow->use();
	currentShader = shaderShadow;

	shadow->bind();

	shadow->update();

	currentShader->uniformMatrix4("projection",
		shadow->getProjection());
	
	currentShader->uniformMatrix4("view",
		shadow->getView());

	handleModel_shadow(currentScene, glm::mat4(1.0f));

	shadow->unbind();
	glViewport(0, 0, Window::width, Window::height);

	shaderLighting->use();
	currentShader = shaderLighting;

	// Now supports only one shadow caster
	shadow->bindTexture(/*shadowCounter_direct_ortho++*/ 2);
	currentShader->uniform1i("shadowMap" /* + std::to_string(shadowCounter_direct_ortho)*/, 2);

	currentShader->uniformMatrix4("lightSpaceMatrix", shadow->getProjection() * shadow->getView());
}

void Engine::renderScene(EScene *scene)
{
	currentScene = scene;
}

void Engine::use()
{
	shaderLighting->use();
	currentShader = shaderLighting;

	float lastTime, currentTime;

	lastTime = glfwGetTime();

	runEntSetupRecursive(currentScene);

	if (currentScene->activeCamera && currentScene->activeCamera->script)
		currentScene->activeCamera->script->Setup();

	while(!Window::ShouldClose())
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		lightCounter_point = 0;
		shadowCounter_direct_ortho = 0;

		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		if (currentScene->activeCamera)
		{
			if (currentScene->activeCamera->script)
				currentScene->activeCamera->script->Update(deltaTime);

			currentScene->activeCamera->update();

			currentShader->uniformMatrix4("projection",
				currentScene->activeCamera->getProjection((float)Window::width / (float)Window::height));
			
			currentShader->uniformMatrix4("view",
				currentScene->activeCamera->getView());

			currentShader->uniformVector3("viewPos",
				currentScene->activeCamera->transform->position);
		}

		handleEnt(currentScene, glm::mat4(1.0f));

		//render skybox
		if (currentScene->skybox != nullptr)
		{
			glDepthFunc(GL_LEQUAL);

			skyboxShader->use();
			currentShader = skyboxShader;

			currentShader->uniformMatrix4("projection",
				currentScene->activeCamera->getProjection((float)Window::width / (float)Window::height));
			
			currentShader->uniformMatrix4("view",
				glm::mat4(glm::mat3(currentScene->activeCamera->getView())));

			currentScene->skybox->render(currentShader);

			glDepthFunc(GL_LESS);
			
			shaderLighting->use();
			currentShader = shaderLighting;
		}

		Window::SwapBuffers();
		Window::PollEvents();
	}

	Window::Terminate();
}
