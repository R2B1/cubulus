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

  ResourceManager::LoadShader("../../src/shaders/cube.vert", "../../src/shaders/cube.frag", nullptr, "cube_shader");
  renderer = new Renderer(ResourceManager::GetShader("cube_shader"));

  //Level one; one.load("levels/1.txt");
  //levels_.push_back(one);
  //level_num_ = 0;
  //Player = new GameObject(...);
}

void Game::ProcessInput(GLfloat dt)
{
  glfwPollEvents();  // Check for input
  //glfwWaitEvents();  // Sleep until new event is received

  GLfloat ang_vel = PI / 1.f;  // [rad/s]

  // Check input for camera rotation
  if (keys_[GLFW_KEY_A]) { camera_theta_ += ang_vel*dt; }
  if (keys_[GLFW_KEY_D]) { camera_theta_ += -ang_vel*dt; }
  if (keys_[GLFW_KEY_W]) { if (camera_phi_ < PI / 4.f) camera_phi_ += ang_vel*dt; }
  if (keys_[GLFW_KEY_S]) { if (camera_phi_ > -PI / 4.f) camera_phi_ += -ang_vel*dt; }

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
    if (rotation_angle_ < PI / 2.f)
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
    else if (rotation_angle_ >= PI / 2.f)
    { 
      // Adjust final angle to a multiple of 90 degrees
      GLfloat correction_angle = rotation_angle_ - PI / 2.f;
      switch (rot_direction_)
      {
      case ROT_LEFT:  level_theta_ += correction_angle;  break;
      case ROT_RIGHT: level_theta_ += -correction_angle;  break;
      case ROT_UP:    level_phi_ += correction_angle;  break;
      case ROT_DOWN:  level_phi_ += -correction_angle;  break;
      }
      state_ = ACTIVE;
    }
  }
}

void Game::Update(GLfloat dt)
{
  // Set projection
  GLfloat len = 5.0f;
  //                     glm::ortho(left,right,bottom,top,near,far)
  glm::mat4 projection = glm::ortho(-len, len, -len, len, -1.f*len, 1.f*len);
  ResourceManager::GetShader("cube_shader").SetMatrix4("uni_projection", projection);
  //ResourceManager::GetShader("grid_shader").SetMatrix4("uni_projection", projection);
  
  // Set view
  GLfloat radius = 1.0f;
  GLfloat cam_x = radius*cos(camera_phi_)*cos(camera_theta_);
  GLfloat cam_z = radius*cos(camera_phi_)*sin(camera_theta_);
  GLfloat cam_y = radius*sin(camera_phi_);
  glm::vec3 position = glm::vec3(cam_x, cam_y, cam_z);
  glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::mat4 camera_view = glm::lookAt(position, target, up);
  ResourceManager::GetShader("cube_shader").SetMatrix4("uni_view", camera_view);
  //ResourceManager::GetShader("grid_shader").SetMatrix4("uni_view", camera_view);
}

void Game::Render(GLFWwindow* window)
{
  glm::vec4 color;

  if (state_ == ACTIVE || state_ == ROTATING)
  {
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

    // (cube positions are hard-coded here for now but will be loaded from a file into a level object)
    glm::vec3 cube_positions[] =
    {
      glm::vec3(1.0f,  0.0f,  -1.0f),
      glm::vec3(1.0f,  0.0f,  -2.0f),
      glm::vec3(-1.0f,  -1.0f,  0.0f),
      glm::vec3(0.0f,  2.0f,  0.0f),
      glm::vec3(0.0f,  -2.0f,  2.0f),
      glm::vec3(-2.0f,  -2.0f,  0.0f),
      glm::vec3(2.0f,  2.0f,  2.0f)
    };

    // Draw cubes
    glBindVertexArray(renderer->cube_vao_);
    color = glm::vec4(0.0f, 0.0f, 0.0f, 1.f);
    for (GLuint i = 0; i < sizeof(cube_positions)/12; i++) 
      renderer->DrawCube(cube_positions[i], level_theta_, level_phi_, color);
    glBindVertexArray(0);
    //levels_[level_num_].draw(*renderer);
  }
  glfwSwapBuffers(window);
}
