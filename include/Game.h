#ifndef GAME_H
#define GAME_H
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include "../include/Level.h"

enum GameState { ACTIVE, ROTATING, WIN, LOSE, MENU };
enum RotateState { ROT_LEFT, ROT_RIGHT, ROT_UP, ROT_DOWN };

class Game
{
 public:
   GameState state_;
   RotateState rot_direction_;
   GLuint width_, height_;
   GLboolean keys_[1024];
   GLfloat camera_theta_;
   GLfloat camera_phi_;
   GLfloat rotation_angle_;

   std::vector<Level> levels_;
   GLuint number_of_levels_;
   GLuint current_level_;

   Game(GLuint x, GLuint y);
  ~Game();
  void Init();
  void ProcessInput(GLfloat dt);
  void Update(GLfloat dt);
  void Render(GLFWwindow* window);
  void LoadNextLevel();
  void LoadLevel(GLint number);
};

#endif