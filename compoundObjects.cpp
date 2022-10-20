#include "compoundObjects.hpp"

compoundObjects::compoundObjects(CGRAobject &CGRAobject) {
  Object = &CGRAobject;
  TransformFromMother =
      glm::mat4(1.0f); // Set initial transform to identity matrix
}

compoundObjects::~compoundObjects() {}

void compoundObjects::PushChild(compoundObjects *Child) {
	Children.push_back(Child);
}

void compoundObjects::DrawTree(glm::mat4 V, glm::mat4 P) {
	for(const auto & elemt : Children){
		glm::mat4 aux(1.0f);
		aux = TransformFromMother * elemt->Object->modeltr;
		elemt->Object->setModelTransformation(aux);
		elemt->Object->drawIt(V, P);
	}
	Object->drawIt(V, P);
}