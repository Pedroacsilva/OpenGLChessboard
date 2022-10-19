// g++ -o chessboard chessboard.cpp imgui.o imgui_impl_glfw.o
// imgui_impl_opengl3.o imgui_draw.o imgui_tables.o imgui_widgets.o
// -I./vendor/stb_image -I./vendor -I./vendor/imgui  -lglfw -lGLEW -lGL -Wall
// -lrt -lm -ldl
/*
TODO:
  - Se possível, evitar que Camera seja um objecto global.
https://stackoverflow.com/questions/21799746/how-to-glfwsetkeycallback-for-different-classes
  Perceber glfwSet/GetWindowUserPointer melhor -- julgo que seria possível desta
forma mas teria de modificar a class Camara para ter um ponteiro para a janela
em que trabalha?
  - Criar um Makefile para o imgui e cenas
https://discord.com/channels/103678524375699456/103678524375699456/999338270846627880
*/

#include "Camera.cpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"
#include <cmath>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/rotate_vector.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#ifndef GL_SETUP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GL_SETUP
#endif

#ifndef STB_I
#include "vendor/stb_image/stb_image.cpp"
#include "vendor/stb_image/stb_image_print.cpp"
#define STB_I
#endif

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw_gl3.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

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

Camera camera(glm::vec3(0.0f, 2.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f,
              0.0f);

DEECShader *basicShader;
GLFWwindow *window;
bool wire = false;
float deltaTime = 0.0f, lastFrame = 0.0f;
float cameraSpeed = 1.0f;

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
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
}

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
  glClearColor(0.5f, 0.4f, 0.3f, 1.0f);
  CGRASquare chao;
  CGRACube cubo;
  CGRASphere esfera;
  CGRACylinder cilindro;
  CGRACone cone;
  std::vector<glm::vec3> revoPontos;
  revoPontos.emplace_back(glm::vec3(1.0f, 1.0f, 0.0f));
  revoPontos.emplace_back(glm::vec3(0.5f, 0.5f, 0.0f));
  CGRARevolution revo(revoPontos);
  revo.setShader(basicShader);
  cone.setShader(basicShader);
  cilindro.setShader(basicShader);
  esfera.setShader(basicShader);
  chao.setShader(basicShader);
  cubo.setShader(basicShader);
  glm::mat4 chaoPosition = glm::mat4(1.0f);
   // model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f,
  // 0.0f));
  chaoPosition = glm::scale(chaoPosition, glm::vec3(100.0f, 100.0f, 100.0f));
  chaoPosition = glm::rotate(chaoPosition, glm::degrees(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 esferaPosition = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
  glm::mat4 cuboPosition = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
  glm::mat4 cilindroPosition = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.5f, 0.0f));
  glm::mat4 conePosition = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 1.5f, 0.0f));
  cubo.setModelTransformation(cuboPosition);
  chao.setModelTransformation(chaoPosition);
  esfera.setModelTransformation(esferaPosition);
  cilindro.setModelTransformation(cilindroPosition);
  cone.setModelTransformation(conePosition);
  revo.setModelTransformation(esferaPosition);
  glm::mat4 proj = glm::perspective(glm::radians(80.0f), 1.0f, 0.5f, 50.0f);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    chaoPosition = glm::rotate(chaoPosition, glm::degrees(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//    chao.setModelTransformation(chaoPosition);
    revo.drawIt(camera.GetViewMatrix(), proj);
    chao.drawIt(camera.GetViewMatrix(), proj);
/*    cubo.drawIt(camera.GetViewMatrix(), proj);
    esfera.drawIt(camera.GetViewMatrix(), proj);
    cilindro.drawIt(camera.GetViewMatrix(), proj);
    cone.drawIt(camera.GetViewMatrix(), proj);*/
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}