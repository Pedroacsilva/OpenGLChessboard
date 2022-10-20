#define GLM_ENABLE_EXPERIMENTAL
#include "deecshader.h"
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <vector>

#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include <cmath>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <vector>
#ifndef VA
#include "VertexArray.cpp"
#define VA
#endif

#ifndef VB
#include "VertexBuffer.cpp"
#define VB
#endif

#ifndef VBL
#include "VertexBufferLayout.hpp"
#define VBL
#endif

#ifndef IB
#include "IndexBuffer.cpp"
#define IB
#endif

class CGRAobject {
public:
  glm::mat4 modeltr;
  DEECShader *shader;
  VertexArray m_VA;
  VertexBuffer m_VB;
  VertexBufferLayout m_Layout;
  IndexBuffer m_IB;
  GLuint numVertices;
  bool hasTexture;

  CGRAobject();
  virtual ~CGRAobject() = 0;
  void setModelTransformation(glm::mat4 &modeltransf);
  void setShader(DEECShader *shaderprog);
  void Bind() const;
  void Unbind() const;

  void AddVA(const VertexArray va);

  void AddIndexBuffer(const void *data, unsigned int count);
  virtual void drawIt(glm::mat4 V, glm::mat4 P);
};


class CGRAExtrusion : public CGRAobject{
public:
  CGRAExtrusion(std::vector<glm::vec3> pontos);
  ~CGRAExtrusion();
  void drawIt(glm::mat4 V, glm::mat4 P) override;
};


class CGRARevolution : public CGRAobject{
public:
  CGRARevolution(std::vector<glm::vec3> pontos);
  ~CGRARevolution();
  void drawIt(glm::mat4 V, glm::mat4 P) override;
};

class CGRASquare : public CGRAobject {
public:
  
  CGRASquare();
  ~CGRASquare();
  void drawIt(glm::mat4 V, glm::mat4 P) override;
};

class CGRACube : public CGRAobject {
public:
  
  CGRACube();
  ~CGRACube();
  void drawIt(glm::mat4 V, glm::mat4 P) override;
};

class CGRASphere: public CGRAobject{
public:
  CGRASphere();
  ~CGRASphere();
  void drawIt(glm::mat4 V, glm::mat4 P) override;
};

class CGRACylinder: public CGRAobject{
public:
  CGRACylinder();
  ~CGRACylinder();
  void drawIt(glm::mat4 V, glm::mat4 P) override;
};

class CGRACone: public CGRAobject{
public:
  CGRACone();
  ~CGRACone();
  void drawIt(glm::mat4 V, glm::mat4 P) override;
};