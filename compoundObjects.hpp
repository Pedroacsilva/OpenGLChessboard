#pragma once
#include <vector>
#ifndef CGRAOBJECT
#include "cgraobject.hpp"
#define CGRAOBJECT
#endif
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#ifndef GL_SETUP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GL_SETUP
#endif

class compoundObjects {
public:

	compoundObjects(CGRAobject& CGRAobject);
	~compoundObjects();
	void PushChild(compoundObjects * Child);
	void DrawTree(glm::mat4 V, glm::mat4 P);
	CGRAobject *Object;
	glm::mat4 TransformFromMother;
	std::vector<compoundObjects *> Children;
//	std::string name;
};