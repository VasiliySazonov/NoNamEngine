#define STB_IMAGE_IMPLEMENTATION
#include "../engine/engine.hpp"
#include "scripts/freecamera.hpp"

ShaderPaths shaders = {
	.lightingVertex = "../shaders/shaderLighting.vs",
	.lightingFragment = "../shaders/shaderLighting.fs",
	.shadowVertex = "../shaders/shaderShadow.vs",
	.shadowFragment = "../shaders/shaderShadow.fs",
	.skyboxVertex = "../shaders/shaderSkybox.vs",
	.skyboxFragment = "../shaders/shaderSkybox.fs"
};

WindowProperties window = {
	.name = "Test",
	.width = 1920,
	.height = 1080,
	.antiAliasingLevel = 4 // GLFW anti aliasing level
};

class CloseWindow : public Logical::Script
{
	void Update(float deltaTime) override
	{
		if (Events::jpressed(GLFW_KEY_ESCAPE))
			Window::SetShouldClose(true);
	}
};

// Models with multiple meshes are not supported
// If you will try to load such a model, meshes transforms won't be applied!
int main(int argc, const char **argv)
{
	// create an instance of Engine object
	Engine engine(window, shaders);

	// create EScene instance
	Logical::EScene scene("main scene");

	// create logical model
	Logical::EModel man("man"); // supports any 3d model format supported by assimp, i prefer gltf
	// load data from disk
	man.mdl = new Model("assets/models/man/", "man.gltf");

	Animation manWalk("assets/models/man/man.gltf" /*file contains animation*/, man.mdl /*animation bones are in this model*/, 0 /* index of animation */);
	man.animator->play(&manWalk);
	scene.addChild(&man);

	// set man's transform
	man.transform->position = glm::vec3(0, -1, -2);

	// another model with 2 LOD versions
	Logical::EModel model_LOD0("model_LOD0");
	model_LOD0.mdl = new Model("assets/models/model/", "model_LOD0.gltf");
	
	Logical::EModel model_LOD1("model_LOD1");
	model_LOD1.mdl = new Model("assets/models/model/", "model_LOD1.gltf");

	// you can create as much LOD levels as you want

	// lod 0 is active between 0 and 5 units
	model_LOD0.LOD_properties = {
		.LOD_distanceMin = 0,
		.LOD_distanceMax = 5,	
	};

	// lod 0 is active between 5 and 10 units
	model_LOD1.LOD_properties = {
		.LOD_distanceMin = 5,
		.LOD_distanceMax = 10,	
	};

	Logical::ECollectionLOD model("model");
	model.addChild(&model_LOD0);
	model.addChild(&model_LOD1);

	man.addChild(&model); // model's transformation is now in man's coordinate space
	model.transform->position = glm::vec3(0, -1, 0);

	// Create a light source
	Logical::ELight_point light("light");
	light.transform->position = glm::vec3(10);
	scene.addChild(&light);

	// Create a shadow caster (orthographic)
	Logical::EShadowCasterDirectOrtho shadow("shadow", 4000, 4000);
	shadow.properties = {
		// depth
		.zNear = 1,
		.zFar = 100,
		// plane size
		.sizeX = 10,
		.sizeY = 10
	};
	shadow.transform->position = light.transform->position;
	shadow.transform->rotation = glm::vec3(-45.0f, 45.0f, .0f); // rotations in degrees

	scene.addChild(&shadow);

	// create a camera
	Logical::ECameraPersp camera("camera", 70.0f /* fov */);
	// Create an instance of free camera script (scripts/freecamera.hpp)
	FreeCamera script;
	// attach the script to the camera
	camera.attachScript(&script);
	// set the camera as active
	scene.activeCamera = &camera;

	// create an insctance of CloseWindow script
	CloseWindow close; 
	// attach it to any entity for exapmle scene
	scene.attachScript(&close);

	// create skybox
	std::vector<std::string> skyTex;

	skyTex.push_back("assets/skybox/right.jpg");
	skyTex.push_back("assets/skybox/left.jpg");
	skyTex.push_back("assets/skybox/bottom.jpg");
	skyTex.push_back("assets/skybox/top.jpg");
	skyTex.push_back("assets/skybox/front.jpg");
	skyTex.push_back("assets/skybox/back.jpg");

	Skybox skybox(skyTex);

	scene.skybox = &skybox;

	// add the scene the the engine
	engine.renderScene(&scene);

	engine.use(); // launch the main loop

	delete man.mdl;
	delete model_LOD0.mdl;
	delete model_LOD1.mdl;

	return 0;
}

