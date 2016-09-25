#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "ResourceManager.h"

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
//static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
GLFWwindow* OpenGLInit();

const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 800;
Game cubulus(SCREEN_WIDTH, SCREEN_HEIGHT);

int main()
{
  GLFWwindow* window = OpenGLInit();
  cubulus.Init();
  GLfloat delta_t = 0.0f;
  GLfloat last_frame = 0.0f;

  // Game loop
  while (!glfwWindowShouldClose(window))
  {
    // (need to set a fixed frame rate of 60 fps)
    GLfloat current_frame = glfwGetTime();  // [s]
    delta_t = current_frame - last_frame;
    last_frame = current_frame;

    cubulus.ProcessInput(delta_t);
    cubulus.Update(delta_t);
    cubulus.Render(window);
  }

  ResourceManager::Clear();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
  if (key >= 0 && key < 1024)
  {
    if (action == GLFW_PRESS)
      cubulus.keys_[key] = GL_TRUE;
    else if (action == GLFW_RELEASE)
      cubulus.keys_[key] = GL_FALSE;
  }
}

//static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
//{
//}

GLFWwindow* OpenGLInit()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "cubulus", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, KeyCallback);
  //glfwSetCursorPosCallback(window, CursorPosCallback);
  glfwSwapInterval(1);

  glewExperimental = GL_TRUE;
  glewInit();
  
  glGetError();
  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  //glEnable(GL_CULL_FACE);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return window;
}
