#pragma once

#include "../../engine/logic/base/base.hpp"
#include "../../engine/window/events.hpp"

#define MOUSE_SENSIVITY 2
#define CAMERA_SPEED 3.0f

// free camera script
class FreeCamera : public Logical::Script
{
private:
	glm::vec3 movement;
	float cameraX, cameraY;

	Logical::ECameraPersp *mCamera; // Camera handled by this script

public:
	// Setup runs ones before the first frame, so all your operations with entities, hierarchy tree should be here
	void Setup() override 
	{
		Events::toogleCursor(); // set the cursor invisiable
		movement = glm::vec3(0);
		cameraX = 0;
		cameraY = 0;
		mCamera = (ECameraPersp*) ent;
	}

	// Update runs every frame engine passes delta time in it as an argument
	void Update (float deltaTime)
	{
		movement = glm::vec3(0);

		// camera rotation
		cameraY = -glm::degrees(Events::deltaY / Window::height) * MOUSE_SENSIVITY;
		cameraX = -glm::degrees(Events::deltaX / Window::width) * MOUSE_SENSIVITY;
		
		if (cameraY > 89.0f)
		{
			cameraY = 89.0f;
		}

		if (cameraY < -89.0f)
		{
			cameraY = -89.0f;
		}

		mCamera->rotation = glm::mat4(1.0f);
		mCamera->transform->rotation = glm::vec3(cameraY, cameraX, 0);

		// camera movement
		if (Events::pressed(GLFW_KEY_D))
		{
			movement += mCamera->right;
		}

		if (Events::pressed(GLFW_KEY_A))
		{
			movement -= mCamera->right;
		}

		if (Events::pressed(GLFW_KEY_W))
		{
			movement += mCamera->front;
		}

		if (Events::pressed(GLFW_KEY_S))
		{
			movement -= mCamera->front;
		}

		if (movement != glm::vec3(0)) // never normalize (0, 0, 0) vector!
			movement = glm::normalize(movement);


		mCamera->transform->position += movement * deltaTime * CAMERA_SPEED; 
	}

};
