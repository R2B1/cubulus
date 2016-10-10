#ifndef LEVEL_H
#define LEVEL_H
#include <vector>
#include <GL/glew.h>
#include <GLM/glm.hpp>
using std::vector;

class Level
{
 public:

   //typedef std::vector<GLint> Dimx;
   //typedef std::vector<Dimx> Dimy;
   //typedef std::vector<Dimy> Matrix3D;
   //typedef std::vector<std::vector<std::vector<GLint>>> Matrix3D;

  vector<vector<vector<GLint>>> level_matrix_;
  vector<vector<GLint>> z_projection_;
  vector<glm::vec3> cube_positions_;
  GLint level_number_;
  GLint number_of_cubes_;
  GLint dim_x_, dim_z_, dim_y_;  // Dimensions of the level
  
  Level(GLint level_number);
  ~Level();
  void RotateLevelMatrix(GLint direction);

 private:
 
  void init();
  void LoadFromFile();
  void FindZProjection();
  void FindCubePositions();
  void FindNumberOfCubes();
  void PrintLevelMatrix();
};

#endif