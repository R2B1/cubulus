#ifndef RENDERER_H
#define RENDERER_H
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "../include/Shader.h"

class Renderer
{
 public:
  Renderer(const Shader &cubeshader);
  ~Renderer();
  void DrawCube(glm::vec3 position, GLfloat rotate_angle_theta, GLfloat rotate_angle_phi, glm::vec4 color);
  void DrawLine(glm::vec3 position, GLfloat rotate_angle, glm::vec3 rotate_axis, glm::vec4 color);
  GLuint cube_vao_;
  GLuint grid_vao_;
  GLuint axis_vao_;
private:
  void InitRenderData();
  Shader cube_shader_;
  //Shader grid_shader_;

};

#endif