#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 frag_position;
out vec3 frag_normal;

uniform mat4 uni_model;       // model to world
uniform mat4 uni_view;        // world to view
uniform mat4 uni_projection;  // view to screen

void main()
{
  gl_Position = uni_projection * uni_view * uni_model * vec4(position, 1.0f);
  frag_position = vec3(uni_model * vec4(position, 1.0f));
  frag_normal = mat3(transpose(inverse(uni_model))) * normal;  
}
