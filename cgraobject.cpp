#ifndef CGRAOBJECT
#include "cgraobject.hpp"
#define CGRAOBJECT
#endif
#include <cmath>
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


// Classe para print mat4

void PrintMat4(glm::mat4 Matrix){
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
      std::cout << Matrix[j][i] << "\t";
    }
    std::cout << "\n";
  }
}


// Classe base
CGRAobject::CGRAobject() { modeltr = glm::mat4(1.0); }

CGRAobject::~CGRAobject() {}

void CGRAobject::setModelTransformation(glm::mat4 &modeltransf) {
  modeltr = modeltransf;
}

void CGRAobject::setShader(DEECShader *shaderprog) {
  shader = shaderprog;
}

void CGRAobject::SetUniform4f(glm::vec4 data, std::string uniformName){
  int data_location = glGetUniformLocation(shader->shaderprogram, uniformName.c_str());
  glUniform4f(data_location, data[0], data[1], data[2], data[3]);
  
}
void CGRAobject::SetColor(glm::vec4 in_color){
  color = in_color;
}

void CGRAobject::drawIt(glm::mat4 V, glm::mat4 P) {}

/*--------------------------+
|         Compostos         |
+---------------------------*/

CGRACompound::CGRACompound(CGRAobject &Base) {
  Object = &Base;
  TransformFromMother =
      glm::mat4(1.0f); // Referencial da base é o referencial inicial do objecto
}

CGRACompound::~CGRACompound() {}

void CGRACompound::PushChild(CGRACompound *Child, glm::mat4 connection) {
//  std::cout << "Matriz Child->TransformFromMother pré-push:\n";
//  PrintMat4(Child->TransformFromMother);
  Child->SetTransformFromMother(connection);
//  std::cout << "Matriz Child->TransformFromMother pós-push:\n";
//  PrintMat4(Child->TransformFromMother);
/*  glm::mat4 newModelTr(1.0f);
  newModelTr = connection * modeltr;
  std::cout << "Matrix Child->modelTr pré set:\n";
  PrintMat4(Child->modeltr);
  Child->setModelTransformation(newModelTr);  
  std::cout << "Matrix Child->modelTr pós set:\n";
  PrintMat4(Child->modeltr);*/
  Children.push_back(Child);
}

void CGRACompound::DrawTree(glm::mat4 V, glm::mat4 P) {
//  std::cout << "Modeltr:\n";
//  PrintMat4(Child->modeltr);
  glm::mat4 newModelTr(1.0f), oldModelTr(1.0f);
  newModelTr = TransformFromMother * Object->modeltr;
//  PrintMat4(newModelTr);
  oldModelTr = Object->modeltr;
  Object->setModelTransformation(newModelTr);
  Object->drawIt(V, P);
  Object->setModelTransformation(oldModelTr);
  for (const auto &elemt : Children) {
//    std::cout << "Modeltr:\n";
//    PrintMat4(elemt->modeltr);
    elemt->DrawTree(V, P);
  }
}

void CGRACompound::SetTransformFromMother(glm::mat4 &modeltransf) {
  TransformFromMother = modeltransf;
  modeltr = TransformFromMother * modeltr;
}

void CGRACompound::PropagateModelTransformation(glm::mat4 &modeltransf) {
  Object->modeltr = modeltransf * Object->modeltr;
  for(const auto & elemt: Children){
//    elemt->PropagateModelTransformation(modeltransf);
    elemt->TransformFromMother = modeltransf * elemt->TransformFromMother;
//    elemt->PropagateModelTransformation(modeltransf);
  }
}

/*--------------------------+
|         Extrusões         |
+---------------------------*/

CGRAExtrusion::CGRAExtrusion(std::vector<glm::vec3> pontos) {
  std::vector<float> vtx_info;
//  std::cout << "Extrusion vector size: " << pontos.size() << ".\n";
//  const float z_displacement = 1.0f;
  float x_new = 0.0f, y_new = 0.0f, z_new = 0.0f, x, y, z;
  // Push dos pontos iniciais
  for (const auto &elemt : pontos) {
    vtx_info.emplace_back(elemt[0]);
    vtx_info.emplace_back(elemt[1]);
    vtx_info.emplace_back(elemt[2]);
  }
  // Aplicar uma translação z = 1.0f aos pontos

  for (const auto &elemt : pontos) {
    x = elemt[0];
    y = elemt[1];
    z = elemt[2];
  //  std::cout << "X: " << x << ", Y: " << y << ", Z: " << z << ".\n";
    x_new = x;
    y_new = y;
    z_new = z + 1.0f;
    //    x_new = static_cast<float>(std::cos(theta) * x - std::sin(theta) *
    //    y);
    //  y_new = static_cast<float>(std::sin(theta) * x + std::cos(theta) * y);
/*          std::cout << "X_new: " << x_new << ", Y_new: " << y_new
                    << ", Z_new: " << z_new << ".\n";*/
    // Push novos vértices
    vtx_info.emplace_back(x_new);
    vtx_info.emplace_back(y_new);
    vtx_info.emplace_back(z_new);
  }

  unsigned int indices[] = {
      0, 3, 2, 0, 1, 2, 4, 7, 6, 4, 5, 6, 0, 3, 7, 0, 4, 7,
      1, 2, 6, 1, 5, 6, 3, 7, 2, 2, 6, 7, 0, 4, 1, 1, 5, 4,
  };

  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);

  m_Layout.Push<float>(3, "Vertex Coordinates");
  m_IB.Push(indices, sizeof(indices));
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRAExtrusion::~CGRAExtrusion() {}

void CGRAExtrusion::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();
  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  //  std::cout << "mvp_location: " << mvp_location << "\n";
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  SetUniform4f(color, "u_Colors");
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Revoluções        |
+---------------------------*/

CGRARevolution::CGRARevolution(std::vector<glm::vec3> pontos) {
  const float PI = 3.14f;
  const float thetaStep = 2 * PI / 10;
  float theta = 0.0f;
  std::vector<float> vtx_info;
//  std::cout << "Revolution vector size: " << pontos.size() << ".\n";
  // Push dos pontos iniciais.
  /* for (const auto & elemt : pontos) {
     vtx_info.emplace_back(elemt[0]);
     vtx_info.emplace_back(elemt[1]);
     vtx_info.emplace_back(elemt[2]);
   }*/
  // theta += thetaStep;
  float x_new = 0.0f, y_new = 0.0f, x, y, z;
  // Aplicar 10 rotações ao redor do eixo z aos pontos recebidos.
  for (int i = 0; i < 10; i++) {
//    std::cout << "Theta: " << theta << "\n";
    for (const auto &elemt : pontos) {
      x = elemt[0];
      y = elemt[1];
      z = elemt[2];
/*      std::cout << "X: " << x << ", Y: " << y
                << ", Z: " << z << ".\n";*/
      x_new = static_cast<float>(std::cos(theta) * x - std::sin(theta) * y);
      y_new = static_cast<float>(std::sin(theta) * x + std::cos(theta) * y);
/*      std::cout << "X_new: " << x_new << ", Y_new: " << y_new
                << ", Z_new: " << z << ".\n";*/
      vtx_info.emplace_back(x_new);
      vtx_info.emplace_back(y_new);
      vtx_info.emplace_back(z);
    }
    theta += thetaStep;
  }

  // Indices:
  // No fim das rotações, teremos 10 vezes mais vértices.
  // Seja P_i o i-ésimo ponto inicial e P^n_i o i-ésimo ponto na n-ésima
  // iteração da rotação. Pretendemos unir triângulos compostos pelos vértices
  // (P^n_i, P^n_(i+1), P^(n+1)_(i+1)) e pelos vértices (P^n_i, P^(n+1)_i,
  // P^(n+1)_(i+1))

  std::vector<int> indices;
  //for(int i = 0; i < pontos.size(); i++)

//  std::cout << "Revo vector size final: " << vtx_info.size() << "\n";

  int p1, p2;
  int pStep = pontos.size();
/*  std::cout << "Pontos size: " << pStep << ".\n";
  std::cout << "Indices:\n";*/
  for(int i = 0; i < 9; i++){
    p1 = i * pStep;
    p2 = p1 + 1;
    indices.emplace_back(p1);
    indices.emplace_back(p2);
    indices.emplace_back(p2 + pStep);

    indices.emplace_back(p1);
    indices.emplace_back(p2 + pStep);
    indices.emplace_back(p1 + pStep);
/*    std::cout << p1 << ", " << p2 << ", " << p2 + pStep << "\n";
    std::cout << p1 << ", " << p2 + pStep << ", " << p1 + pStep << "\n";*/
  }

  // Caso degenerado. Agora que me lembro, podia ter feito indice % limite. Ah bem, ja me habituei.
/*  indices.pop_back();
  indices.pop_back();
  indices.pop_back();
  indices.pop_back();
  indices.pop_back();
  indices.pop_back();*/

  indices.emplace_back(pStep * 9);
  indices.emplace_back(pStep * 9 + 1);
  indices.emplace_back(1);
  indices.emplace_back(pStep * 9);
  indices.emplace_back(1);
  indices.emplace_back(0);
/*  std::cout << pStep * 9 << ", " << pStep * 9 + 1 << ", " << "1" << "\n";
  std::cout << pStep * 9 << ", " << "1" << ", " << "0" << "\n";*/

  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);

  m_Layout.Push<float>(3, "Vertex Coordinates");
  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);

}

CGRARevolution::~CGRARevolution() {}

/*void CGRARevolution::setModelTransformation(glm::mat4 &modeltransf) {
  modeltr = modeltransf;
}*/

void CGRARevolution::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  //  std::cout << "mvp_location: " << mvp_location << "\n";
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  SetUniform4f(color, "u_Colors");
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Quadrado          |
+---------------------------*/

CGRASquare::CGRASquare() {
  float face_positions[] = {
      // Vertex Coordinates  Tex Coords       RGBA  Values       Normals
      -0.5f, -0.5f, 0.0f, //0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f,// 0
      0.5f, -0.5f, 0.0f,  //1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f, -1.0f,// 1
      0.5f, 0.5f, 0.0f,   //1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 0.0f, -1.0f,// 2
      -0.5f, 0.5f, 0.0f,  //0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f, -1.0f,// 3
    };
  
  unsigned int indices[] = {0,  1,  2,  2,  3,  0};
  m_VB.Push(GL_ARRAY_BUFFER, sizeof(face_positions), face_positions,
            GL_STATIC_DRAW);
  m_IB.Push(indices, 6);
  m_Layout.Push<float>(3, "Vertex Coordinates");
/*  m_Layout.Push<float>(2, "Texture Coordinates");
  m_Layout.Push<float>(4, "Vertex Colors");
  m_Layout.Push<float>(3, "Vertex Normals");*/
  m_VA.AddBuffer(m_VB, m_Layout);
  setShader(shader);
}



CGRASquare::~CGRASquare() {}

/*void CGRASquare::setModelTransformation(glm::mat4 &modeltransf) {
  modeltr = modeltransf;
}*/

void CGRASquare::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  //  std::cout << "mvp_location: " << mvp_location << "\n";
  SetUniform4f(color, "u_Colors");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Cubo              |
+---------------------------*/

CGRACube::CGRACube(){
    float face_positions[] = {
      // Vertex Coordinates  Tex Coords       RGBA  Values       Normals
      -0.5f, -0.5f, -0.5f,// 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f,// 0
      0.5f, -0.5f, -0.5f, //1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f, -1.0f,// 1
      0.5f, 0.5f, -0.5f, //1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 0.0f, -1.0f,// 2
      -0.5f, 0.5f, -0.5f, //0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f, -1.0f,// 3
      //------------------//-----------------------------------------------------------
      -0.5f, -0.5f, 0.5f, //0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, // 4
      0.5f, -0.5f, 0.5f, //1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f, 1.0f, // 5
      0.5f, 0.5f, 0.5f, //1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 0.0f, 1.0f, // 6
      -0.5f, 0.5f, 0.5f, //0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 0.0f, 1.0f, // 7
      //------------------//-----------------------------------------------------------
      0.5f, -0.5f, 0.5f, //0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,  1.0f, 0.0f, 0.0f,// 8
      0.5f, -0.5f, -0.5f, //1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f,// 9
      0.5f, 0.5f, -0.5f, //1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,  1.0f, 0.0f, 0.0f,// 10
      0.5f, 0.5f, 0.5f, //0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,   1.0f, 0.0f, 0.0f,// 11
      //------------------//-----------------------------------------------------------
      -0.5f, -0.5f, 0.5f, //0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, -1.0f, 0.0f, 0.0f, // 12
      -0.5f, -0.5f, -0.5f,// 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,-1.0f, 0.0f, 0.0f, // 13
      -0.5f, 0.5f, -0.5f, //1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, -1.0f, 0.0f, 0.0f, // 14
      -0.5f, 0.5f, 0.5f, //0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,  -1.0f, 0.0f, 0.0f, // 15
      //------------------//-----------------------------------------------------------
      -0.5f, 0.5f, 0.5f, //0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f, 0.0f,// 16
      0.5f, 0.5f, 0.5f, //.10f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 1.0f, 0.0f,// 17
      0.5f, 0.5f, -0.5f, //1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, 1.0f, 0.0f,// 18
      -0.5f, 0.5f, -0.5f, //0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f,// 19
      //------------------//-----------------------------------------------------------
      -0.5f, -0.5f, 0.5f, //0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f,// 20
      0.5f, -0.5f, 0.5f, //1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f,  0.0f, -1.0f, 0.0f,// 21
      0.5f, -0.5f, -0.5f, //1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f,// 22
      -0.5f, -0.5f, -0.5f,// 0.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f// 23
  };

  unsigned int indices[] = {0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,
                            8,  9,  10, 10, 11, 8,  12, 13, 14, 14, 15, 12,
                            16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

  m_VB.Push(GL_ARRAY_BUFFER, sizeof(face_positions), face_positions,
            GL_STATIC_DRAW);
  m_IB.Push(indices, 6 * 6);
  m_Layout.Push<float>(3, "Vertex Coordinates");
/*  m_Layout.Push<float>(2, "Texture Coordinates");
  m_Layout.Push<float>(4, "Vertex Colors");
  m_Layout.Push<float>(3, "Vertex Normals");*/
  m_VA.AddBuffer(m_VB, m_Layout);
  
}

CGRACube::~CGRACube() 
{
  
}

void CGRACube::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();
  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
//  std::cout << "mvp_location: " << mvp_location << "\n";
  SetUniform4f(color, "u_Colors");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*void CGRACube::setModelTransformation(glm::mat4 &modeltransf){
  modeltr = modeltransf;
}*/

/*--------------------------+
|         Esfera            |
+---------------------------*/

CGRASphere::CGRASphere() {

  /*    Eqs paramétricas
x = (r * cos(phi) * cos(theta))
y = (r * cos(phi) * sin(theta))
z = r * sin(phi)    
xy = r *  cos(phi)            */
  const float radius = 1.0f;
  const float PI = 3.14f;
  float x, y, z, xy;
  // float nx, ny, nz;
  float u, v;
  float sectorStep = 2 * PI / 10;
  float stackStep = PI / 10;
  float sectorAngle, stackAngle;
  std::vector<float> vtx_info;

  for (int i = 0; i <= 10; i++) {
    stackAngle = PI / 2 - i * stackStep; // Começar no pólo norte
    xy = radius * std::cos(stackAngle);
    z = radius * std::sin(stackAngle);
    for (int j = 0; j <= 10; j++) {
      sectorAngle = j * sectorStep;
      x = xy * std::cos(sectorAngle);
      y = xy * std::sin(sectorAngle);
      // XYZ
      vtx_info.emplace_back(x);
      vtx_info.emplace_back(y);
      vtx_info.emplace_back(z);
      // V
/*      u = static_cast<float>(j) / 10;
      v = static_cast<float>(i) / 10;
      vtx_info.emplace_back(u);
      vtx_info.emplace_back(v);
      // RGBA
      vtx_info.emplace_back(0.5f);
      vtx_info.emplace_back(0.5f);
      vtx_info.emplace_back(0.5f);
      vtx_info.emplace_back(1.0f);
      // Normals
      vtx_info.emplace_back(x);
      vtx_info.emplace_back(y);
      vtx_info.emplace_back(z);*/
    }
  }
  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);
  m_Layout.Push<float>(3, "Vertex Coordinates");
/*  m_Layout.Push<float>(2, "Texture Coordinates");
  m_Layout.Push<float>(4, "Vertex Colors");
  m_Layout.Push<float>(3, "Vertex Normals");*/
  // Preparar IBO
  std::vector<int> indices;
  int k1, k2;
  for (int i = 0; i < 10; ++i) {
    k1 = i * 11;
    k2 = k1 + 11;
    for (int j = 0; j < 10; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.emplace_back(k1);
        indices.emplace_back(k2);
        indices.emplace_back(k1 + 1);
      }
      if (i != 9) {
        indices.emplace_back(k1 + 1);
        indices.emplace_back(k2);
        indices.emplace_back(k2 + 1);
      }
    }
  }
  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRASphere::~CGRASphere() {}

/*void CGRASphere::setModelTransformation(glm::mat4 &modeltransf) {
  modeltr = modeltransf;
}*/

void CGRASphere::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  //  std::cout << "mvp_location: " << mvp_location << "\n";
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  SetUniform4f(color, "u_Colors");
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Cilindro          |
+---------------------------*/


CGRACylinder::CGRACylinder() {
  const float raio = 0.5f;
  const float PI = 3.14f;
  float theta = 0.0f;
  std::vector<float> vtx_info;
  // Círculo inferior
  for (int i = 0; i < 10; i++) {
    // XYZ
    vtx_info.emplace_back(std::cos(theta) * raio);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(std::sin(theta) * raio);
    // UV
/*    vtx_info.emplace_back(i / 10);
    vtx_info.emplace_back(1.0f);
    // RGB
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(1.0f);
    // Normal
    glm::vec3 normal =
        glm::normalize(glm::vec3(std::cos(theta), 0.0f, std::sin(theta)));
    vtx_info.emplace_back(normal[0]);
    vtx_info.emplace_back(normal[1]);
    vtx_info.emplace_back(normal[2]);*/

    theta += 2 * PI / 10;
  }
  theta = 0.0f;
  // Círculo superior
  for (int i = 0; i < 10; i++) {
    // XYZ
    vtx_info.emplace_back(std::cos(theta) * raio);
    vtx_info.emplace_back(-0.5f);
    vtx_info.emplace_back(std::sin(theta) * raio);
    // UV
/*    vtx_info.emplace_back(i / 10);
    vtx_info.emplace_back(1.0f);
    // RGB
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(1.0f);
    // Normal
    glm::vec3 normal =
        glm::normalize(glm::vec3(std::cos(theta), 0.0f, std::sin(theta)));
    vtx_info.emplace_back(normal[0]);
    vtx_info.emplace_back(normal[1]);
    vtx_info.emplace_back(normal[2]);*/

    theta += 2 * PI / 10;
  }
  m_Layout.Push<float>(3, "Vertex Coordinates");
/*  m_Layout.Push<float>(2, "Texture Coordinates");
  m_Layout.Push<float>(4, "Vertex Colors");
  m_Layout.Push<float>(3, "Vertex Normals");*/
  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);
  std::vector<int> indices;
  for (int i = 0; i < 10; i++) {
    indices.emplace_back(i);
    indices.emplace_back(i + 1);
    indices.emplace_back(i + 10);
    indices.emplace_back(i + 10);
    indices.emplace_back(i + 1);
    indices.emplace_back(i + 11);
  }
  // Caso degenerado
  indices.pop_back();
  indices.pop_back();
  indices.pop_back();
  indices.emplace_back(0);
  indices.emplace_back(9);
  indices.emplace_back(10);

  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);
}


CGRACylinder::~CGRACylinder() {}

/*void CGRACylinder::setModelTransformation(glm::mat4 &modeltransf) {
  modeltr = modeltransf;
}*/

void CGRACylinder::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

//  std::cout << "Cilindro Modeltr:\n";
//  PrintMat4(modeltr);
  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  //  std::cout << "mvp_location: " << mvp_location << "\n";
  SetUniform4f(color, "u_Colors");
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}

/*--------------------------+
|         Cone              |
+---------------------------*/

CGRACone::CGRACone() {
  const float raio = 0.5f;
  const float PI = 3.14f;
  float theta = 0.0f;
  std::vector<float> vtx_info;
  // Vértice ápice primeiro (índice 0)
  vtx_info.emplace_back(0.0f);
  vtx_info.emplace_back(1.0f);
  vtx_info.emplace_back(0.0f);

  // Vértices do círculo no chao (x e z variam, y = 0.0f)
  for (int i = 0; i < 10; i++) {
    // XYZ
    vtx_info.emplace_back(std::cos(theta) * raio);
    vtx_info.emplace_back(-0.5f);
    vtx_info.emplace_back(std::sin(theta) * raio);
    // UV
/*    vtx_info.emplace_back(i / 10);
    vtx_info.emplace_back(1.0f);
    // RGB
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(1.0f);
    // Normal -- Normais incorrectas: são da classe cilinder!
    glm::vec3 normal =
        glm::normalize(glm::vec3(std::cos(theta), 0.0f, std::sin(theta)));
    vtx_info.emplace_back(normal[0]);
    vtx_info.emplace_back(normal[1]);
    vtx_info.emplace_back(normal[2]);*/

    theta += 2 * PI / 10;
  }
  theta = 0.0f;
  for (int i = 0; i < 10; i++) {
    // XYZ
    vtx_info.emplace_back(std::cos(theta) * raio);
    vtx_info.emplace_back(-0.5f);
    vtx_info.emplace_back(std::sin(theta) * raio);
/*    // UV
    vtx_info.emplace_back(i / 10);
    vtx_info.emplace_back(1.0f);
    // RGB
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(0.5f);
    vtx_info.emplace_back(1.0f);
    // Normal -- Normais incorrectas: são da classe cilinder!
    glm::vec3 normal =
        glm::normalize(glm::vec3(std::cos(theta), 0.0f, std::sin(theta)));
    vtx_info.emplace_back(normal[0]);
    vtx_info.emplace_back(normal[1]);
    vtx_info.emplace_back(normal[2]);*/

    theta += 2 * PI / 10;
  }
  m_Layout.Push<float>(3, "Vertex Coordinates");
/*  m_Layout.Push<float>(2, "Texture Coordinates");
  m_Layout.Push<float>(4, "Vertex Colors");
  m_Layout.Push<float>(3, "Vertex Normals");*/
  m_VB.Push(GL_ARRAY_BUFFER, vtx_info.size() * sizeof(float), vtx_info.data(),
            GL_STATIC_DRAW);
  std::vector<int> indices;
  // Ligar vértice ápice aos do chão
  for (int i = 0; i < 10; i++) {
    indices.emplace_back(0);
    indices.emplace_back(i + 1);
    indices.emplace_back(i + 10);
    indices.emplace_back(0);
    indices.emplace_back(i + 1);
    indices.emplace_back(i + 11);
  }
  // Caso degenerado
  indices.pop_back();
  indices.pop_back();
  indices.pop_back();
  indices.emplace_back(0);
  indices.emplace_back(9);
  indices.emplace_back(10);

  // Ligar vértices no chão

  m_IB.Push(indices.data(), indices.size());
  m_VA.AddBuffer(m_VB, m_Layout);
}

CGRACone::~CGRACone() {}

/*void CGRACone::setModelTransformation(glm::mat4 &modeltransf) {
  modeltr = modeltransf;
}*/

void CGRACone::drawIt(glm::mat4 V, glm::mat4 P) {
  m_VA.Bind();
  m_IB.Bind();

  glm::mat4 mvp = P * V * modeltr;
  int mvp_location = glGetUniformLocation(shader->shaderprogram, "u_MVP");
  //  std::cout << "mvp_location: " << mvp_location << "\n";
  glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
  SetUniform4f(color, "u_Colors");
  glDrawElements(GL_TRIANGLES, m_IB.GetCount(), GL_UNSIGNED_INT, nullptr);
}


