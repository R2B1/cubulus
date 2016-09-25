#ifndef GAME_H
#define GAME_H
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
//#include "GameLevel.h"

enum GameState { ACTIVE, ROTATING, MENU, WIN };
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
   GLfloat level_theta_;
   GLfloat level_phi_;
   GLfloat rotation_angle_;

   //std::vector<GameLevel> levels_;
   //GLuint level_num_;

   Game(GLuint x, GLuint y);
  ~Game();
  void Init();
  void ProcessInput(GLfloat dt);
  void Update(GLfloat dt);
  void Render(GLFWwindow* window);
};

#endif