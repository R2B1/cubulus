#include "../include/Renderer.h"

Renderer::Renderer(const Shader &cubeshader, const Shader &lineshader) 
  : cube_shader_(cubeshader), line_shader_(lineshader) 
{ InitRenderData(); }

Renderer::~Renderer() 
{ 
  glDeleteVertexArrays(1, &cube_vao_); 
  glDeleteVertexArrays(1, &grid_vao_); 
  glDeleteVertexArrays(1, &axis_vao_);
}

void Renderer::DrawCube(glm::vec3 position, GLfloat rotate_angle_theta, GLfloat rotate_angle_phi, glm::vec4 color)
{
  cube_shader_.Use();
  cube_shader_.SetVector4f("uni_color", color);
  glm::mat4 cubeModel = glm::mat4(1.0f);
  cubeModel = glm::rotate(cubeModel, rotate_angle_theta, glm::vec3(0.0f, 1.0f, 0.0f));
  cubeModel = glm::rotate(cubeModel, rotate_angle_phi, glm::vec3(cos(rotate_angle_theta), 0.0f, sin(rotate_angle_theta)));
  cubeModel = glm::translate(cubeModel, position);
  //cubeModel = glm::scale(cubeModel, glm::vec3(size, 1.0f));
  cube_shader_.SetMatrix4("uni_model", cubeModel);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::DrawLine(glm::vec3 position, GLfloat rotate_angle, glm::vec3 rotate_axis, glm::vec4 color)
{
  line_shader_.Use();
  line_shader_.SetVector4f("uni_color", color);
  glm::mat4 lineModel = glm::mat4(1.0f); 
  lineModel = glm::rotate(lineModel, rotate_angle, rotate_axis);
  lineModel = glm::translate(lineModel, position);
  line_shader_.SetMatrix4("uni_model", lineModel);
  glDrawArrays(GL_LINES, 0, 2);
}

void Renderer::InitRenderData()
{	
  GLfloat len = 0.505f;
  GLfloat cube_vertices[] = {
     // Positions      // Normals
    -len, -len, -len,  0.0f,  0.0f, -1.0f,
     len, -len, -len,  0.0f,  0.0f, -1.0f,
     len,  len, -len,  0.0f,  0.0f, -1.0f,
     len,  len, -len,  0.0f,  0.0f, -1.0f,
    -len,  len, -len,  0.0f,  0.0f, -1.0f,
    -len, -len, -len,  0.0f,  0.0f, -1.0f,

    -len, -len,  len,  0.0f,  0.0f,  1.0f,
     len, -len,  len,  0.0f,  0.0f,  1.0f,
     len,  len,  len,  0.0f,  0.0f,  1.0f,
     len,  len,  len,  0.0f,  0.0f,  1.0f,
    -len,  len,  len,  0.0f,  0.0f,  1.0f,
    -len, -len,  len,  0.0f,  0.0f,  1.0f,

    -len,  len,  len, -1.0f,  0.0f,  0.0f,
    -len,  len, -len, -1.0f,  0.0f,  0.0f,
    -len, -len, -len, -1.0f,  0.0f,  0.0f,
    -len, -len, -len, -1.0f,  0.0f,  0.0f,
    -len, -len,  len, -1.0f,  0.0f,  0.0f,
    -len,  len,  len, -1.0f,  0.0f,  0.0f,

     len,  len,  len,  1.0f,  0.0f,  0.0f,
     len,  len, -len,  1.0f,  0.0f,  0.0f,
     len, -len, -len,  1.0f,  0.0f,  0.0f,
     len, -len, -len,  1.0f,  0.0f,  0.0f,
     len, -len,  len,  1.0f,  0.0f,  0.0f,
     len,  len,  len,  1.0f,  0.0f,  0.0f,

    -len, -len, -len,  0.0f, -1.0f,  0.0f,
     len, -len, -len,  0.0f, -1.0f,  0.0f,
     len, -len,  len,  0.0f, -1.0f,  0.0f,
     len, -len,  len,  0.0f, -1.0f,  0.0f,
    -len, -len,  len,  0.0f, -1.0f,  0.0f,
    -len, -len, -len,  0.0f, -1.0f,  0.0f,

    -len,  len, -len,  0.0f,  1.0f,  0.0f,
     len,  len, -len,  0.0f,  1.0f,  0.0f,
     len,  len,  len,  0.0f,  1.0f,  0.0f,
     len,  len,  len,  0.0f,  1.0f,  0.0f,
    -len,  len,  len,  0.0f,  1.0f,  0.0f,
    -len,  len, -len,  0.0f,  1.0f,  0.0f
  };

  // Load cube vertices into GPU memory
  GLuint cube_vbo;
  glGenVertexArrays(1, &cube_vao_);  // Allocate and assign a VAO to the handle
  glBindVertexArray(cube_vao_);  // Bind VAO as currently used object
  glGenBuffers(1, &cube_vbo);  // Allocate and assign a VBO to the handle
  glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);  // Bind VBO as active buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);  // Copy vertex data to buffer
  // Specify that position data is going to attribute index 0 and contains 3 floats per vertex:
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)0);  
  glEnableVertexAttribArray(0);  // Enable attribute index 0 as being used
  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
  glBindVertexArray(0); // Unbind VAO

  GLfloat grid_line_vertices[] =
  {
    -2.5f, 0.0f, 0.0f,
    2.5f, 0.0f, 0.0f
  };
  
  // Load grid line vertices into GPU memory
  GLuint grid_vbo;
  glGenVertexArrays(1, &grid_vao_);
  glBindVertexArray(grid_vao_);
  glGenBuffers(1, &grid_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, grid_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(grid_line_vertices), grid_line_vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  GLfloat axis_line_vertices[] =
  {
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f
  };

  // Load axis line vertices into GPU memory
  GLuint axis_vbo;
  glGenVertexArrays(1, &axis_vao_);
  glBindVertexArray(axis_vao_);
  glGenBuffers(1, &axis_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, axis_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axis_line_vertices), axis_line_vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

}
