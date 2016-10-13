#ifndef RENDERER_H
#define RENDERER_H
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include "../include/Shader.h"

class Renderer
{
 public:
  Renderer(const Shader &cubeshader, const Shader &lineshader);
  ~Renderer();
  void DrawCube(glm::vec3 position, GLfloat rotate_theta, GLfloat rotate_phi, GLint cube_type);
  void DrawLine(glm::vec3 position, GLfloat rotate_angle, glm::vec3 rotate_axis, glm::vec4 color);
  GLuint cube_vao_;
  GLuint grid_vao_;
  GLuint axis_vao_;
  Shader cube_shader_;
  Shader line_shader_;
private:
  void InitRenderData();


};

#endif