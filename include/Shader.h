#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

class Shader
{
 public:
  GLuint id_;
  Shader() { }
  Shader &Use();  // Sets the current shader as active
  void Compile(const GLchar* vertex_src, const GLchar* fragment_src, const GLchar* geometry_src=nullptr);
  // Utility functions
  void SetFloat(const GLchar* name, GLfloat value, GLboolean use_shader=false);
  void SetInteger(const GLchar* name, GLint value, GLboolean use_shader=false);
  void SetVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean use_shader=false);
  void SetVector2f(const GLchar* name, const glm::vec2 &value, GLboolean use_shader=false);
  void SetVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean use_shader=false);
  void SetVector3f(const GLchar* name, const glm::vec3 &value, GLboolean use_shader=false);
  void SetVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean use_shader=false);
  void SetVector4f(const GLchar* name, const glm::vec4 &value, GLboolean use_shader=false);
  void SetMatrix4(const GLchar* name, const glm::mat4 &matrix, GLboolean use_shader=false);
 private:
  void CheckCompileErrors(GLuint object, std::string type);
};

#endif