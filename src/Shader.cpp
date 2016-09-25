#include <iostream>
#include "../include/Shader.h"

Shader &Shader::Use() { glUseProgram(id_);  return *this; }

void Shader::Compile(const GLchar* vertex_src, const GLchar* fragment_src, const GLchar* geometry_src)
{
  GLuint vertex_shader, fragment_shader, geometry_shader;
  // Vertex Shader
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_src, NULL);
  glCompileShader(vertex_shader);
  CheckCompileErrors(vertex_shader, "VERTEX");
  // Fragment Shader
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_src, NULL);
  glCompileShader(fragment_shader);
  CheckCompileErrors(fragment_shader, "FRAGMENT");
  // Geometry shader
  if (geometry_src != nullptr)
  {
    geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry_shader, 1, &geometry_src, NULL);
    glCompileShader(geometry_shader);
    CheckCompileErrors(geometry_shader, "GEOMETRY");
  }
  // Shader Program
  id_ = glCreateProgram();
  glAttachShader(id_, vertex_shader);
  glAttachShader(id_, fragment_shader);
  if (geometry_src != nullptr)
    glAttachShader(id_, geometry_shader);
  glLinkProgram(id_);
  CheckCompileErrors(id_, "PROGRAM");
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  if (geometry_src != nullptr)
    glDeleteShader(geometry_shader);
}

void Shader::SetFloat(const GLchar* name, GLfloat value, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniform1f(glGetUniformLocation(id_, name), value);
}
void Shader::SetInteger(const GLchar* name, GLint value, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniform1i(glGetUniformLocation(id_, name), value);
}
void Shader::SetVector2f(const GLchar* name, GLfloat x, GLfloat y, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniform2f(glGetUniformLocation(id_, name), x, y);
}
void Shader::SetVector2f(const GLchar* name, const glm::vec2 &value, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniform2f(glGetUniformLocation(id_, name), value.x, value.y);
}
void Shader::SetVector3f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniform3f(glGetUniformLocation(id_, name), x, y, z);
}
void Shader::SetVector3f(const GLchar* name, const glm::vec3 &value, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniform3f(glGetUniformLocation(id_, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const GLchar* name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniform4f(glGetUniformLocation(id_, name), x, y, z, w);
}
void Shader::SetVector4f(const GLchar* name, const glm::vec4 &value, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniform4f(glGetUniformLocation(id_, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const GLchar* name, const glm::mat4 &matrix, GLboolean use_shader)
{
  if (use_shader) Use();
  glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::CheckCompileErrors(GLuint object, std::string type)
{
  GLint success;
  GLchar info_log[1024];
  if (type != "PROGRAM")
  {
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      glGetShaderInfoLog(object, 1024, NULL, info_log);
      std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
        << info_log << "\n -- --------------------------------------------------- -- "
        << std::endl;
    }
  }
  else
  {
    glGetProgramiv(object, GL_LINK_STATUS, &success);
    if (!success)
    {
      glGetProgramInfoLog(object, 1024, NULL, info_log);
      std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
        << info_log << "\n -- --------------------------------------------------- -- "
        << std::endl;
    }
  }
}