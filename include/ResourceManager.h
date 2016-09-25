#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include <map>
#include <string>
#include <GL/glew.h>
#include "../include/Shader.h"

// A static singleton ResourceManager class that hosts several functions to load Textures and Shaders. Each loaded texture and/or shader is also stored for future reference by string handles. All functions and resources are static (can be called even if no objects of the class exist) and no public constructor is defined.
class ResourceManager
{
 public:
  static std::map<std::string, Shader> shader_list_;
  static Shader LoadShader(const GLchar *vertex_file, const GLchar *fragment_file, const GLchar *geometry_file, std::string name);
  static Shader GetShader(std::string name);
  static void Clear();
 private:
  ResourceManager() { };
  static Shader LoadShaderFromFile(const GLchar *vertex_file, const GLchar *fragment_file, const GLchar *geometry_file = nullptr);
};

#endif