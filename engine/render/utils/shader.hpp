#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <string>

class Shader
{
private:
	unsigned int id;
	std::string extractSource(const char * file_path);
	unsigned int compileShader(unsigned int type, std::string &source);
	unsigned int loadShader(std::string vertexFile, std::string fragmentFile);

public:
	Shader(std::string vertexFile, std::string fragmentFile);
	void uniform4f(std::string name, float a, float b, float c, float d);
	void uniform3f(std::string name, float a, float b, float c);
	void uniform2f(std::string name, float a, float b);
	void uniform1f(std::string name, float a);

	void uniform4i(std::string name, int a, int b, int c, int d);
	void uniform3i(std::string name, int a, int b, int c);
	void uniform2i(std::string name, int a, int b);
	void uniform1i(std::string name, int a);

	void uniformMatrix4f(std::string name, glm::mat4 mat);
	void uniformMatrix3f(std::string name, glm::mat3 mat);
	void uniformMatrix2f(std::string name, glm::mat2 mat);

	void uniformVector4(std::string name, glm::vec4 &vec);
	void uniformVector3(std::string name, glm::vec3 &vec);
	void uniformVector2(std::string name, glm::vec2 &vec);

	void use();

	inline unsigned int getId() const { return this->id;}
};

