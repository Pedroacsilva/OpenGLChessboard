#include "compoundObjects.hpp"

compoundObjects::compoundObjects(CGRAobject &CGRAobject) {
  Object = &CGRAobject;
  TransformFromMother =
      glm::mat4(1.0f); // Set initial transform to identity matrix
}

void compoundObjects::PushChild(CGRAobject *Child) {
	Children.push_back(Child);
}

compoundObjects::~compoundObjects() {}