#include <iostream>
#include <sstream>
#include <fstream>
#include "../include/ResourceManager.h"

// pair<const key_type, mapped_type> value_type;
std::map<std::string, Shader> ResourceManager::shader_list_;

Shader ResourceManager::LoadShader(const GLchar *vertex_file, const GLchar *fragment_file, const GLchar *geometry_file, std::string name)
{
  shader_list_[name] = LoadShaderFromFile(vertex_file, fragment_file, geometry_file);
  return shader_list_[name];
}

Shader ResourceManager::GetShader(std::string name) { return shader_list_[name]; }

void ResourceManager::Clear() { for (auto iter : shader_list_) glDeleteProgram(iter.second.id_); }

Shader ResourceManager::LoadShaderFromFile(const GLchar *vertex_file, const GLchar *fragment_file, const GLchar *geometry_file)
{
  std::string vertex_code, fragment_code, geometry_code;
  try
  {
    std::ifstream vertexShaderFile(vertex_file);
    std::ifstream fragmentShaderFile(fragment_file);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vertexShaderFile.rdbuf();
    fShaderStream << fragmentShaderFile.rdbuf();
    vertexShaderFile.close();
    fragmentShaderFile.close();
    // Convert stream into string
    vertex_code = vShaderStream.str();
    fragment_code = fShaderStream.str();
    if (geometry_file != nullptr)
    {
      std::ifstream geometryShaderFile(geometry_file);
      std::stringstream gShaderStream;
      gShaderStream << geometryShaderFile.rdbuf();
      geometryShaderFile.close();
      geometry_code = gShaderStream.str();
    }
  }
  catch (std::exception e)
  {
    std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
  }
  const GLchar *vShaderCode = vertex_code.c_str();
  const GLchar *fShaderCode = fragment_code.c_str();
  const GLchar *gShaderCode = geometry_code.c_str();

  Shader shader;
  shader.Compile(vShaderCode, fShaderCode, geometry_file != nullptr ? gShaderCode : nullptr);
  return shader;
}
