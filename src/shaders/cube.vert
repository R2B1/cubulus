#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 uni_model;       // model to world
uniform mat4 uni_view;        // world to view
uniform mat4 uni_projection;  // view to screen projection

void main()
{
  // Transformations occur in reversed order
  gl_Position = uni_projection * uni_view * uni_model * vec4(position, 1.0f);
}
