#include <iostream>
#include "../include/Game.h"
#include "../include/ResourceManager.h"
#include "../include/Renderer.h"

const GLfloat PI = 3.14159f;
Renderer* renderer;

Game::Game(GLuint x, GLuint y) : state_(ACTIVE), width_(x), height_(y) { }
Game::~Game() { delete renderer; }

void Game::Init()
{
  camera_theta_ = 90.f*(PI/180.f);  // 0 to 360 deg
  camera_phi_ = 0.f*(PI/180.f);  // -45 to 45 deg
  level_theta_ = 0.f;
  level_phi_ = 0.f;

  ResourceManager::LoadShader("../../src/shaders/cube.vert", "../../src/shaders/cube.frag", nullptr, "cube_shader");
  ResourceManager::LoadShader("../../src/shaders/line.vert", "../../src/shaders/line.frag", nullptr, "line_shader");
  renderer = new Renderer(ResourceManager::GetShader("cube_shader"), ResourceManager::GetShader("line_shader"));

  current_level_ = 0;
  Level new_level(current_level_);
  levels_.push_back(new_level);
}

void Game::ProcessInput(GLfloat dt)
{
  glfwPollEvents();  // Check for input
  //glfwWaitEvents();  // Sleep until new event is received

  GLfloat ang_vel = PI / 1.f;  // [rad/s]

  // Check input for camera rotation
  if (keys_[GLFW_KEY_A]) { camera_theta_ += ang_vel*dt; }
  if (keys_[GLFW_KEY_D]) { camera_theta_ += -ang_vel*dt; }
  if (keys_[GLFW_KEY_W]) { if (camera_phi_ < PI/4.f) camera_phi_ += ang_vel*dt; }
  if (keys_[GLFW_KEY_S]) { if (camera_phi_ > -PI/4.f) camera_phi_ += -ang_vel*dt; }
  // Reset camera view
  if (keys_[GLFW_KEY_R]) { camera_theta_ = PI/2.f;  camera_phi_ = 0.f; }

  // Check input for level rotation
  switch (state_)
  {
  case ACTIVE:  //--------------------
    if (keys_[GLFW_KEY_LEFT] || keys_[GLFW_KEY_RIGHT] || keys_[GLFW_KEY_UP] || keys_[GLFW_KEY_DOWN])
    {
      if      (keys_[GLFW_KEY_LEFT])  { rot_direction_ = ROT_LEFT; }
      else if (keys_[GLFW_KEY_RIGHT]) { rot_direction_ = ROT_RIGHT; }
      else if (keys_[GLFW_KEY_UP])    { rot_direction_ = ROT_UP; }
      else if (keys_[GLFW_KEY_DOWN])  { rot_direction_ = ROT_DOWN; }
      rotation_angle_ = 0.f;
      state_ = ROTATING;
    }
    break;

  case ROTATING:  //--------------------
    
    GLfloat snap_angle;
    //arcsin(tan(30))=35.264 degrees
//    if (rot_direction_ == ROT_LEFT || rot_direction_ == ROT_RIGHT)  snap_angle = 45.f*(PI / 180.f);
//    if (rot_direction_ == ROT_UP   || rot_direction_ == ROT_DOWN )  snap_angle = asin(tan(PI/6.0f));
    snap_angle = 90.f*(PI / 180.f);

    if ((rotation_angle_ + ang_vel*dt) <= snap_angle)
    {
      switch (rot_direction_)
      {
      case ROT_LEFT:  level_theta_ += -ang_vel*dt; break;
      case ROT_RIGHT: level_theta_ += ang_vel*dt;  break;
      case ROT_UP:    level_phi_ += -ang_vel*dt;  break;
      case ROT_DOWN:  level_phi_ += ang_vel*dt;  break;
      }
      rotation_angle_ += ang_vel*dt;
    }
    else if ((rotation_angle_ + ang_vel*dt) > snap_angle)
    {   
      level_theta_ = 0.f;
      level_phi_ = 0.f;

      switch (rot_direction_)
      {
      case ROT_LEFT:  levels_[current_level_].RotateLevelMatrix(1);  break;
      case ROT_RIGHT: levels_[current_level_].RotateLevelMatrix(2);  break;
      case ROT_UP:    levels_[current_level_].RotateLevelMatrix(3);  break;
      case ROT_DOWN:  levels_[current_level_].RotateLevelMatrix(4);  break;
      }
      state_ = ACTIVE;
    }
  }
}

void Game::Update(GLfloat dt)
{
  // Update projection (does this need to be updated?)
  GLfloat len = 5.0f;
  //                     glm::ortho(left,right,bottom,top,near,far)
  glm::mat4 projection = glm::ortho(-len, len, -len, len, -2.f*len, 2.f*len);
  
  // Update view
  GLfloat radius = 1.0f;
  GLfloat cam_x = radius*cos(camera_phi_)*cos(camera_theta_);
  GLfloat cam_z = radius*cos(camera_phi_)*sin(camera_theta_);
  GLfloat cam_y = radius*sin(camera_phi_);
  glm::vec3 position = glm::vec3(cam_x, cam_y, cam_z);
  glm::vec3 target   = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::mat4 camera_view = glm::lookAt(position, target, up);

  // Set uniforms
  renderer->cube_shader_.Use();
  ResourceManager::GetShader("cube_shader").SetMatrix4("uni_projection", projection);
  ResourceManager::GetShader("cube_shader").SetMatrix4("uni_view", camera_view);

  // Left light
  glm::vec3 diffuse_color = glm::vec3(1.0f, 0.0f, 0.0f);
  ResourceManager::GetShader("cube_shader").SetVector3f("uni_light[0].direction", 1.f, 0.f, 0.f);
  ResourceManager::GetShader("cube_shader").SetVector3f("uni_light[0].diffuse", diffuse_color);

  // Right light
  diffuse_color = glm::vec3(0.0f, 1.0f, 0.0f);
  ResourceManager::GetShader("cube_shader").SetVector3f("uni_light[1].direction", -1.f, 0.f, 0.f);
  ResourceManager::GetShader("cube_shader").SetVector3f("uni_light[1].diffuse", diffuse_color);

  // Top light
  diffuse_color = glm::vec3(1.0f, 1.0f, 0.0f);
  ResourceManager::GetShader("cube_shader").SetVector3f("uni_light[2].direction", 0.f, -1.f, 0.f);
  ResourceManager::GetShader("cube_shader").SetVector3f("uni_light[2].diffuse", diffuse_color);

  // Bottom light
  diffuse_color = glm::vec3(0.0f, 0.0f, 1.0f);
  ResourceManager::GetShader("cube_shader").SetVector3f("uni_light[3].direction", 0.f, 1.f, 0.f);
  ResourceManager::GetShader("cube_shader").SetVector3f("uni_light[3].diffuse", diffuse_color);

  renderer->line_shader_.Use();
  ResourceManager::GetShader("line_shader").SetMatrix4("uni_projection", projection);
  ResourceManager::GetShader("line_shader").SetMatrix4("uni_view", camera_view);
}

void Game::Render(GLFWwindow* window)
{
  glm::vec4 color;

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Unit vectors
  glm::vec3 unitx = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 unity = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 unitz = glm::vec3(0.0f, 0.0f, 1.0f);

  glm::vec3 grid_positions[] =
  {
    glm::vec3(0.0f,  0.0f,  -2.5f),
    glm::vec3(0.0f,  0.0f,  -1.5f),
    glm::vec3(0.0f,  0.0f,  -0.5f),
    glm::vec3(0.0f,  0.0f,  0.5f),
    glm::vec3(0.0f,  0.0f,  1.5f),
    glm::vec3(0.0f,  0.0f,  2.5f),
  };

  /*
  // Draw grid
  glBindVertexArray(renderer->grid_vao_);
  color = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);  // Grey
  // Draw grid lines along the x-axis
  for (GLuint i = 0; i < sizeof(grid_positions) / 12; i++)
    renderer->DrawLine(grid_positions[i], 0.f, unity, color);
  // Draw grid lines along the z-axis
  for (GLuint i = 0; i < sizeof(grid_positions) / 12; i++)
    renderer->DrawLine(grid_positions[i], PI / 2.f, unity, color);
  glBindVertexArray(0);

  // Draw axes
  glBindVertexArray(renderer->axis_vao_);
  // Draw x-axis
  color = glm::vec4(1.f, 0.f, 0.f, 1.f);  // Red
  renderer->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), 0.f, unity, color);
  // Draw y-axis
  color = glm::vec4(0.f, 1.f, 0.f, 1.f);  // Green
  renderer->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), PI / 2.f, unitz, color);
  // Draw z-axis
  color = glm::vec4(0.f, 0.f, 1.f, 1.f);  // Blue
  renderer->DrawLine(glm::vec3(0.0f, 0.0f, 0.0f), -PI / 2.f, unity, color);
  glBindVertexArray(0);
  */

  // Draw cubes
  //levels_[current_level_].draw(*renderer);
  glBindVertexArray(renderer->cube_vao_);
  color = glm::vec4(1.f, 1.f, 1.f, 1.f);
  for (GLuint i = 0; i < levels_[0].cube_positions_.size(); i++)
      renderer->DrawCube(levels_[0].cube_positions_[i], level_theta_, level_phi_, color);
  glBindVertexArray(0);

  glfwSwapBuffers(window);
}
