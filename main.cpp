#include "Camera.cpp"
/*#include "compoundObjects.cpp"
#include "compoundObjects.hpp"*/
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <cmath>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL

#ifndef GL_SETUP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GL_SETUP
#endif

/*#ifndef STB_I
#include "vendor/stb_image/stb_image.cpp"
#include "vendor/stb_image/stb_image_print.cpp"
#define STB_I
#endif

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"
#include "vendor/imgui/imgui_impl_opengl3.h"*/

#include <iostream>
#include <string>

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

#include "cgraobject.cpp"
#include "deecshader.h"

#define GLEW_STATIC

Camera camera(glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
              0.0f);

/*Camera cameraCarro(glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
              0.0f);*/

DEECShader *basicShader;
GLFWwindow *window;
bool wire = false;
float yaw = -90.0f;
//float lastX = 400, lastY = 300, yaw = -90.0f, pitch = 0.0f;
float deltaTime = 0.0f, lastFrame = 0.0f;
bool firstMouse = true;
float cameraSpeed = 1.0f;

/*void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_TRUE)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_TRUE)
    camera.m_Position += camera.m_Front * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_TRUE)
    camera.m_Position -= camera.m_Front * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_TRUE)
    camera.m_Position += camera.m_Right * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_TRUE)
    camera.m_Position -= camera.m_Right * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_TRUE) {
    wire = !wire;
    (wire) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
           : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}*/

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_TRUE)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_TRUE)
    camera.m_Position += camera.m_Front * cameraSpeed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_TRUE)
    camera.m_Position -= camera.m_Front * cameraSpeed * deltaTime;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_TRUE){
    yaw -= cameraSpeed * deltaTime * 100;
    glm::vec3 direction (1.0f);
    direction[0] = cos(glm::radians(yaw));
    direction[1] = 0.0f;
    direction[2] = sin(glm::radians(yaw));
    camera.m_Front = glm::normalize(direction);
    camera.m_Right = glm::normalize(glm::cross(camera.m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
/*    std::cout << "Yaw: " << yaw << "\n";
    std::cout << "CameraUp: " << camera.m_Up[0] <<", " << camera.m_Up[1] << ", " << camera.m_Up[2] <<".\n";*/
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_TRUE){
    yaw += cameraSpeed * deltaTime * 100;
    glm::vec3 direction(1.0f);
    direction[0] = cos(glm::radians(yaw));
    direction[1] = 0.0f;
    direction[2] = sin(glm::radians(yaw));
    camera.m_Front = glm::normalize(direction);
    camera.m_Right = glm::normalize(glm::cross(camera.m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
/*    std::cout << "Yaw: " << yaw << "\n";
    std::cout << "CameraUp: " << camera.m_Up[0] <<", " << camera.m_Up[1] << ", " << camera.m_Up[2] <<".\n";*/
  }
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_TRUE) {
    wire = !wire;
    (wire) ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
           : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

/*void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  camera.m_Front = glm::normalize(direction);
  camera.m_Right =
      glm::normalize(glm::cross(camera.m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
}*/

int main(int argc, char const *argv[]) {

  if (!glfwInit()) {
    std::cout << "Error initializing GLFW.\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Chessboard", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, KeyCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//  glfwSetCursorPosCallback(window, mouse_callback);

  if (glewInit() != GLEW_OK) {
    std::cout << "Error initializing GLEW.\n";
    return -1;
  }

  basicShader = new DEECShader;
  std::cout << "loading shaders.\n";
  if (basicShader->loadShaders("basic.vert", "basic.frag") == GL_FALSE) {
    printf("ERROR LOADING SHADERS.\n");
    exit(EXIT_FAILURE);
  }
  std::cout << "linking shaders.\n";
  if (basicShader->linkShaderProgram() == GL_FALSE) {
    printf("ERROR LINKING SHADERS.\n");
    exit(EXIT_FAILURE);
  }
  basicShader->startUsing();

  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

  glEnable(GL_BLEND | GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.53f, 0.8f, 0.92f, 1.0f);

  glm::mat4 proj = glm::perspective(glm::radians(80.0f), 1.0f, 0.5f, 50.0f);

  CGRASquare chao;
  CGRACube carroCorpo, carro2Corpo;
  CGRASphere sol;
  CGRACylinder pneu;
  CGRACylinder carroPneu, carroPneu2, carroPneu3, carroPneu4;
  CGRACone cone;
  CGRACompound carroCorpoObj(carroCorpo);
  CGRACompound carroPneuObj(carroPneu);
  CGRACompound carroPneu2Obj(carroPneu2);
  CGRACompound carro2CorpoObj(carro2Corpo);
  CGRACompound carro2PneuObj(carroPneu3);
  CGRACompound carro2Pneu2Obj(carroPneu4);
//  CGRACompound carroPneu3Obj(carroPneu3);

  

  std::vector<glm::vec3> revoPontos;
  revoPontos.emplace_back(glm::vec3(1.0f, 0.0f, 0.0f));
  revoPontos.emplace_back(glm::vec3(0.7f, 0.0f, 0.0f));

  std::vector<glm::vec3> extrPontos;
  extrPontos.emplace_back(glm::vec3(-1.0f, 2.0f, 0.0f));
  extrPontos.emplace_back(glm::vec3(1.0f, 1.0f, 0.0f));
  extrPontos.emplace_back(glm::vec3(1.0f, -1.0f, 0.0f));
  extrPontos.emplace_back(glm::vec3(-1.0f, -1.0f, 0.0f));

  std::vector<glm::vec3> conePontos;
  conePontos.emplace_back(1.0f, 0.0f, -1.0f);
  conePontos.emplace_back(0.0f, 0.0f, 0.0f);

  CGRARevolution track(revoPontos);
  CGRAExtrusion trophy(extrPontos);
  CGRARevolution cone2(conePontos);

  track.setShader(basicShader);
  trophy.setShader(basicShader);
  cone.setShader(basicShader);
  cone2.setShader(basicShader);
  pneu.setShader(basicShader);
  sol.setShader(basicShader);
  chao.setShader(basicShader);
  carroCorpo.setShader(basicShader);
  carro2Corpo.setShader(basicShader);
  carroPneu.setShader(basicShader);
  carroPneu2.setShader(basicShader);
  carroPneu3.setShader(basicShader);
  carroPneu4.setShader(basicShader);


  glm::mat4 chaoPosition(1.0f);
  glm::mat4 trackPosition(1.0f);
  glm::mat4 solPosition(1.0f);
  glm::mat4 pneuPosition(1.0f);
  glm::mat4 conePosition(1.0f);
  glm::mat4 cone2Position(1.0f);
  glm::mat4 trophyPosition(1.0f);
  glm::mat4 carroCorpoPosition(1.0f);
  glm::mat4 carro2CorpoPosition(1.0f);
  glm::mat4 carroPneuPosition(1.0f);

  // model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f,
  // 0.0f));
  chaoPosition = glm::scale(chaoPosition, glm::vec3(20.0f, 20.0f, 20.0f));
  chaoPosition = glm::rotate(chaoPosition, glm::degrees(90.0f),
                             glm::vec3(1.0f, 0.0f, 0.0f));
  chaoPosition = glm::translate(chaoPosition, glm::vec3(0.0f, 0.0f, -0.001f));



  solPosition = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.0f, -5.0f));



  std::vector<glm::vec3> pneuLocations = {glm::vec3(0.0f, 0.0f, 0.0f),
                                          glm::vec3(1.0f, 0.0f, 0.0f),
                                          glm::vec3(1.0f, 1.0f, 0.0f)};

  pneuPosition = glm::scale(pneuPosition, glm::vec3(0.6f, 0.1f, 0.2f));
  pneuPosition = glm::translate(pneuPosition, glm::vec3(-1.0f, 1.5f, 0.0f));

  conePosition = glm::scale(conePosition, glm::vec3(0.3f, 0.7f, 0.3f));
  conePosition = glm::translate(conePosition, glm::vec3(2.0f, 0.0f, 0.0f));

  cone2Position = glm::scale(cone2Position, glm::vec3(0.3f, 0.7f, 0.3f));
  cone2Position = glm::translate(cone2Position, glm::vec3(-5.0f, 0.7f, 0.0f));
  cone2Position = glm::rotate(cone2Position, glm::degrees(90.0f),
                              glm::vec3(1.0f, 0.0f, 0.0f));

  /*glm::mat4 trackPosition =
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f));*/
  trackPosition = glm::translate(trackPosition, glm::vec3(0.0f, 0.00001f, 0.0f));
  trackPosition = glm::rotate(trackPosition, glm::degrees(90.0f),
                              glm::vec3(1.0f, 0.0f, 0.0f));
  trackPosition = glm::scale(trackPosition, glm::vec3(5.0f, 5.0f, 5.0f));

  trophyPosition = glm::translate(trophyPosition, glm::vec3(0.0f, 1.0f, -1.5f));
  /*glm::mat4 trophyPosition =
      glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 0.0f));*/

  carroCorpoPosition = glm::translate(carroCorpoPosition, glm::vec3(5.0f, 1.0f, 0.0f));
  carro2CorpoPosition = glm::translate(carro2CorpoPosition, glm::vec3(7.0f, 1.0f, -2.0f));
//  carro2CorpoPosition = glm::translate(carro2CorpoPosition, glm::vec3(6.0f, 1.0f, 0.0f));



// Pneus do carro

  glm::mat4 pneu2corpo(1.0f);
  pneu2corpo = glm::scale(pneu2corpo, glm::vec3(0.1f, 0.5f, 0.1f));
  pneu2corpo = glm::translate(pneu2corpo, glm::vec3(1.0f, -1.5f, 2.5f));
  pneu2corpo = glm::rotate(pneu2corpo, glm::degrees(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  glm::mat4 pneu22corpo = pneu2corpo;
  pneu22corpo = glm::translate(pneu22corpo, glm::vec3(3.0f, 0.0f, 0.0f));

//  glm::mat4 pneu32corpo = pneu22corpo;
//  pneu32corpo = glm::translate(pneu32corpo, glm::vec3(0.0f, 5.0f, 0.0f));

  carroCorpoObj.PushChild(&carroPneuObj, pneu2corpo);
  carroCorpoObj.PushChild(&carroPneu2Obj, pneu22corpo);

  glm::mat4 pneu2corpo2(1.0f);
  pneu2corpo2 = glm::scale(pneu2corpo2, glm::vec3(0.1f, 0.5f, 0.1f));
  pneu2corpo2 = glm::translate(pneu2corpo2, glm::vec3(1.0f, -1.5f, 2.5f));
  pneu2corpo2 = glm::rotate(pneu2corpo2, glm::degrees(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  carro2CorpoObj.PushChild(&carro2PneuObj, pneu2corpo2);

//  glm::mat4 pneu42co

  chao.setModelTransformation(chaoPosition);
  sol.setModelTransformation(solPosition);
  pneu.setModelTransformation(pneuPosition);
  cone.setModelTransformation(conePosition);
  cone2.setModelTransformation(cone2Position);
  track.setModelTransformation(trackPosition);
  trophy.setModelTransformation(trophyPosition);
  carroCorpoObj.PropagateModelTransformation(carroCorpoPosition);
  carro2CorpoObj.PropagateModelTransformation(carro2CorpoPosition);
//  carroCorpo.setModelTransformation(carroCorpoPosition);
//  carroPneuObj.setModelTransformation(pneu2corpo);

  glm::vec4 grassColor = glm::vec4(0.3f, 0.5f, 0.27f, 1.0f);
  glm::vec4 trackColor = glm::vec4(0.8f);
  glm::vec4 trophyColor = glm::vec4(0.97f, 0.65f, 0.01f, 1.0f);
  glm::vec4 pneuColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
  glm::vec4 solColor = glm::vec4(0.98f, 0.99f, 0.06f, 1.0f);
  glm::vec4 coneColor = glm::vec4(1.0f, 0.49f, 0.0f, 1.0f);
  glm::vec4 carroColor = glm::vec4(0.9f, 0.3f, 0.2f, 1.0f);

  trophy.SetColor(trophyColor);
  pneu.SetColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  carroPneu.SetColor(pneuColor);
  carroPneu2.SetColor(pneuColor);
  carroCorpo.SetColor(carroColor);
  sol.SetColor(solColor);
  cone.SetColor(coneColor);
  cone2.SetColor(coneColor);
  chao.SetColor(grassColor);


  glm::vec3 cameraCarroPosition (1.0f);

  int color_location =
      glGetUniformLocation(basicShader->shaderprogram, "u_Colors");

  float carroTheta = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Definir cor do chao a verde
/*    glUniform4f(color_location, grassColor[0], grassColor[1], grassColor[2],
                grassColor[3]);*/
    // Desenhar chao
    chao.drawIt(camera.GetViewMatrix(), proj);

    // Desenhar pista
/*    glUniform4f(color_location, trackColor[0], trackColor[1], trackColor[2],
                trackColor[3]);*/
    track.drawIt(camera.GetViewMatrix(), proj);

    // Desenhar trofeu
/*    glUniform4f(color_location, trophyColor[0], trophyColor[1], trophyColor[2],
                trophyColor[3]);*/
    trophy.drawIt(camera.GetViewMatrix(), proj);

    // Desenhar pneus
/*    glUniform4f(color_location, pneuColor[0], pneuColor[1], pneuColor[2],
                pneuColor[3]);*/
    for (const auto &elemt : pneuLocations) {
      glm::mat4 location = pneuPosition;
      location = glm::translate(location, elemt);
      pneu.setModelTransformation(location);
      pneu.drawIt(camera.GetViewMatrix(), proj);
    }

    // Desenhar sol
/*    glUniform4f(color_location, solColor[0], solColor[1], solColor[2],
                solColor[3]);*/
    sol.drawIt(camera.GetViewMatrix(), proj);

    // Desenhar cone
/*    glUniform4f(color_location, coneColor[0], coneColor[1], coneColor[2],
                coneColor[3]);*/
    cone.drawIt(camera.GetViewMatrix(), proj);
    // Desenhar cone (revolucao)
    cone2.drawIt(camera.GetViewMatrix(), proj);

    // Desenhar carro
/*    glUniform4f(color_location, carroColor[0], carroColor[1], carroColor[2],
                carroColor[3]);*/
//    carroCorpoObj.SetTreeTransformation(carroCorpoPosition);
    glm::mat4 aux(1.0f);
    aux = glm::rotate(aux, glm::degrees(0.0001f), glm::vec3(0.0f, 1.0f, -0.2f));


    carroCorpoObj.PropagateModelTransformation(aux);
    carro2CorpoObj.PropagateModelTransformation(aux);
    //carroCorpoObj.DrawTree(camera.GetViewMatrix(), proj);
    carroCorpoObj.DrawTree(camera.GetViewMatrix(), proj);
    carro2CorpoObj.DrawTree(camera.GetViewMatrix(), proj);
    for(int i = 0; i < 4; i++){
      for(int j = 0; j < 4; j++){
        if(i == 3)
//          std::cout << j << "-ésima coordenada:\t";
          std::cout << carroCorpoObj.Object->modeltr[i][j] << "\t";
      }
    }
    std::cout << "\n";

    std::cout << "CarroCorpoObj Matrix:\n";
    PrintMat4(carroCorpoObj.Object->modeltr);
    std::cout << "\n";

//    carroCorpoPosition = glm::rotate(carroCorpoPosition, glm::degrees(carroTheta), glm::vec3(0.0f, 1.0f, 0.0f));
    
//    carroCorpoPosition = glm::scale(carroCorpoPosition, glm::vec3(0.8f, 0.6f, 0.8f));

//    carroCorpoObj.DrawTree(camera.GetViewMatrix(), proj);

//    carroPneu.drawIt(camera.GetViewMatrix(), proj);

    //    pneu.drawIt(camera.GetViewMatrix(), proj);
    /*cubo.drawIt(camera.GetViewMatrix(), proj);
    esfera.drawIt(camera.GetViewMatrix(), proj);*/
    // cone.drawIt(camera.GetViewMatrix(), proj);
    glfwSwapBuffers(window);
    glfwPollEvents();
    carroTheta += deltaTime * 0.02f;
  }

  glfwTerminate();
  return 0;
}