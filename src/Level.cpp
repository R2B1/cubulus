//------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
//#include <vector>
#include "../include/Level.h"

using std::vector;
using std::string;
using std::ifstream;
using std::istringstream;
using std::getline;
using std::stoi; 

Level::Level(GLint level_number) : level_number_(level_number), number_of_cubes_(0), dim_x_(1), dim_y_(1), dim_z_(1) { init(); }

Level::~Level()  { }

void Level::init()
{
  LoadFromFile();
  FindNumberOfCubes();
  FindCubePositions();
  FindZProjection();
}

void Level::RotateLevelMatrix(GLint direction)
{
  GLint rot_dim_x, rot_dim_y, rot_dim_z;

  if (direction == 1 || direction == 2)  // left (1) or right (2)
  {
    rot_dim_x = dim_z_;
    rot_dim_y = dim_y_;
    rot_dim_z = dim_x_;
  }
  else if (direction == 3 || direction == 4)  // up (3) or down (4)
  {
    rot_dim_x = dim_x_;
    rot_dim_y = dim_z_;
    rot_dim_z = dim_y_;
  }
  vector<vector<vector<GLint>>> rotated_matrix(rot_dim_x, vector<vector<GLint>>(rot_dim_y, vector<GLint>(rot_dim_z)));

  for (GLint k = 0; k < rot_dim_z; k++) {
    for (GLint j = 0; j < rot_dim_y; j++) {
      for (GLint i = 0; i < rot_dim_x; i++) {
        if (direction == 1) {  // LEFT
          rotated_matrix[i][j][k] = level_matrix_[rot_dim_z - 1 - k][j][i];
        }
        else if (direction == 2) {  // RIGHT
          rotated_matrix[i][j][k] = level_matrix_[k][j][rot_dim_x - 1 - i];
        }
        else if (direction == 3) {  // UP
          rotated_matrix[i][j][k] = level_matrix_[i][rot_dim_z - 1 - k][j];
        }
        else if (direction == 4) {  // DOWN
          rotated_matrix[i][j][k] = level_matrix_[i][k][rot_dim_y - 1 - j];
        }
      }
    }
  }

  level_matrix_.clear();
  level_matrix_ = rotated_matrix;
  dim_x_ = rot_dim_x;
  dim_y_ = rot_dim_y;
  dim_z_ = rot_dim_z;

  //PrintLevelMatrix();

  cube_positions_.clear();
  FindCubePositions();

  FindZProjection();
}

void Level::LoadFromFile()
{
  string filename = "../../src/levels/1.txt";
  ifstream inFile(filename);
  string str;

  // Read level dimensions
  while (!inFile.eof())
  {
    // Ignore comment lines that begin with '#'
    if (inFile.peek() == '#') { getline(inFile, str); continue; }

    getline(inFile, str, ',');
    dim_x_ = stoi(str);
    getline(inFile, str, ',');
    dim_y_ = stoi(str);
    getline(inFile, str);
    dim_z_ = stoi(str);
    break;
  }

  //Matrix3D lev(dim_x_, Dimy(dim_y_, Dimx(dim_z_)));
  vector<vector<vector<GLint>>> lev(dim_x_, vector<vector<GLint>>(dim_y_, vector<GLint>(dim_z_)));
  
  // Read in cubes
  for (GLint k = 0; k < dim_z_; k++)  // Pages
  {
    for (GLint j = 0; j < dim_y_; j++)  // Rows
    {
      // Ignore comment lines that begin with '#'
      if (inFile.peek() == '#')  {  getline(inFile, str);  j--;  continue;  }

      for (GLint i = 0; i < dim_x_; i++)  // Columns
      {
        getline(inFile, str, ',');
        lev[i][j][k] = stoi(str);
      }
      getline(inFile, str);  // discard '\n0' from end of line
    }
  }
  inFile.close();

  level_matrix_ = lev;

}

void Level::FindZProjection()
{
  vector<vector<GLint>> proj(dim_x_, vector<GLint>(dim_y_));

  for (GLint k = 0; k < dim_z_; k++) {
    for (GLint j = 0; j < dim_y_; j++) {
      for (GLint i = 0; i < dim_x_; i++) {
        if (level_matrix_[i][j][k] > 0)
        {
          proj[i][j] = 1;
        }
      }
    }
  }

  z_projection_.clear();
  z_projection_ = proj;

  // Print z-projection matrix
  for (GLint j = 0; j < dim_y_; j++) {
    for (GLint i = 0; i < dim_x_; i++) {
      std::cout << z_projection_[i][j]; }
    std::cout << std::endl; }
  std::cout << "--------------------" << std::endl;
}

void Level::FindCubePositions()
{
  GLfloat* shift_x = new GLfloat[dim_x_];
  GLfloat* shift_y = new GLfloat[dim_y_];
  GLfloat* shift_z = new GLfloat[dim_z_];

  GLfloat range_x = static_cast<GLfloat>(dim_x_) / 2.f - 0.5f;
  GLfloat range_y = static_cast<GLfloat>(dim_y_) / 2.f - 0.5f;
  GLfloat range_z = static_cast<GLfloat>(dim_z_) / 2.f - 0.5f;

  for (GLint i = 0; i < dim_x_; i++) { shift_x[i] = -range_x + static_cast<GLfloat>(i); }
  for (GLint i = 0; i < dim_y_; i++) { shift_y[i] =  range_y - static_cast<GLfloat>(i); }
  for (GLint i = 0; i < dim_z_; i++) { shift_z[i] =  range_z - static_cast<GLfloat>(i); }

  for (GLint k = 0; k < dim_z_; k++) {
    for (GLint j = 0; j < dim_y_; j++) {
      for (GLint i = 0; i < dim_x_; i++) {
        if (level_matrix_[i][j][k] > 0) 
        { 
          cube_positions_.push_back(glm::vec3(shift_x[i], shift_y[j], shift_z[k]));
        }
      }
    }
  }

  delete[] shift_x;
  delete[] shift_y;
  delete[] shift_z;
}

void Level::FindNumberOfCubes()
{
  number_of_cubes_ = 0;
  for (GLint k = 0; k < dim_z_; k++) {
    for (GLint j = 0; j < dim_y_; j++) {
      for (GLint i = 0; i < dim_x_; i++) {
        if (level_matrix_[i][j][k] > 0) { number_of_cubes_++; }
      }
    }
  }

}

void Level::PrintLevelMatrix()
{
  for (GLint k = 0; k < dim_z_; k++)  {
    for (GLint j = 0; j < dim_y_; j++)  {
      for (GLint i = 0; i < dim_x_; i++)  {
        std::cout << level_matrix_[i][j][k];  }
      std::cout << std::endl;  }
    std::cout << std::endl;  }

  std::cout << "--------------------" << std::endl;
}
