#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class ShadowCasterDirectBasic
{
private:
	int height, width;
	unsigned int depthMapFBO, depthMap;

public:
	ShadowCasterDirectBasic(int height, int width);

	void bind();
	void unbind();
	void bindTexture(int slot);
	virtual glm::mat4 getView() { return glm::mat4(1); }
};
