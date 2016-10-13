#ifndef LEVEL_H
#define LEVEL_H
#include <vector>
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include "Renderer.h"
using std::vector;

enum LevelState { LEVEL_ACTIVE, LEVEL_WIN };

class Level
{
 public:

   //typedef std::vector<GLint> Dimx;
   //typedef std::vector<Dimx> Dimy;
   //typedef std::vector<Dimy> Matrix3D;
   //typedef std::vector<std::vector<std::vector<GLint>>> Matrix3D;

  LevelState level_state_;
  vector<vector<vector<GLint>>> level_matrix_;
  vector<vector<GLint>> z_projection_;

  vector<glm::vec3> cube_positions_;
  glm::vec3 player_position_;
  glm::vec3 goal_position_;
  glm::vec3 win_position_;

  GLint level_number_;
  GLint dim_x_, dim_y_, dim_z_;  // Level dimensions
  GLfloat theta_, phi_;  // Rotation angles (spherical coords.)
  GLint number_of_cubes_;
  
  Level(GLint level_number);
  ~Level();
  void RotateLevelMatrix(GLint direction);
  void Draw(Renderer& renderer);
  void MovePlayer(GLint direction);
  void Reset();

 private:
 
  void init();
  void LoadFromFile();
  void FindZProjection();
  void MovePlayerToFront();
  void FindPlayer(GLint& px, GLint& py, GLint& pz);
  void FindCubePositions();
  void FindNumberOfCubes();
  void PrintLevelMatrix();
};

#endif