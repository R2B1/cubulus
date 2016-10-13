#version 330 core

struct ParallelSource 
{
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
};

#define N_PARALLEL_LIGHTS 1

in vec3 frag_position;
in vec3 frag_normal;
out vec4 color;

uniform ParallelSource uni_light[N_PARALLEL_LIGHTS];
uniform vec4 uni_color;

vec3 CalcParallelLight(ParallelSource light, vec3 norm, vec4 uni_color);

void main()
{
  vec3 norm = normalize(frag_normal);
  vec3 result = vec3(0.f, 0.f, 0.f);
  for(int i = 0; i < N_PARALLEL_LIGHTS; i++)
    result += CalcParallelLight(uni_light[i], norm, uni_color);
    color = vec4(result, 1.0f);
}

vec3 CalcParallelLight(ParallelSource light, vec3 norm, vec4 uni_color)
{
  vec3 ldir = normalize(-light.direction);
  vec3 ambient = light.ambient * vec3(uni_color.xyz);
  vec3 diffuse = light.diffuse * max(dot(norm, ldir), 0.0f) * vec3(uni_color.xyz);  
  return (ambient + diffuse);
}