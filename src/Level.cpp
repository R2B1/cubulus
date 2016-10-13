//------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "../include/Level.h"
using std::vector;
using std::string;
using std::ifstream;
using std::istringstream;
using std::getline;
using std::stoi; 

Level::Level(GLint level_number) : level_state_(LEVEL_ACTIVE), level_number_(level_number), theta_(0), phi_(0)  { init(); }

Level::~Level()  { }

//----------------------------------------
void Level::init()
{
  LoadFromFile();
  FindNumberOfCubes();
  FindCubePositions();
  FindZProjection();
}

//----------------------------------------
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

  MovePlayerToFront();
  PrintLevelMatrix();
  FindCubePositions();
  FindZProjection();
}

//----------------------------------------
void Level::MovePlayer(GLint direction)
{
  GLint px, py, pz;
  FindPlayer(px, py, pz);
  //std::cout << px << py << pz << std::endl; //xxxxxxxxxxxxxxxxxxxxxx
  
  if (direction == 1 && px > 0)  // Move player left
  {
    for (GLint k = 0; k < dim_z_; k++)
    {
      if (level_matrix_[px - 1][py][k] == 1)
      {
        level_matrix_[px - 1][py][k] = 2;
        level_matrix_[px][py][pz] = 1;
        break;
      }
      if (level_matrix_[px - 1][py][k] == 3)  // Goal reached
      {
        level_matrix_[px - 1][py][k] = 9;
        level_matrix_[px][py][pz] = 1;
        level_state_ = LEVEL_WIN;
        break;
      }
    }
  }
  else if (direction == 2 && px < (dim_x_-1))  // Move player right
  {
    for (GLint k = 0; k < dim_z_; k++)
    {
      if (level_matrix_[px + 1][py][k] == 1)
      {
        level_matrix_[px + 1][py][k] = 2;
        level_matrix_[px][py][pz] = 1;
        break;
      }
      if (level_matrix_[px + 1][py][k] == 3)  // Goal reached
      {
        level_matrix_[px + 1][py][k] = 9;
        level_matrix_[px][py][pz] = 1;
        level_state_ = LEVEL_WIN;
        break;
      }
    }
  }
  else if (direction == 3 && py > 0)  // Move player up
  {
    for (GLint k = 0; k < dim_z_; k++)
    {
      if (level_matrix_[px][py - 1][k] == 1)
      {
        level_matrix_[px][py - 1][k] = 2;
        level_matrix_[px][py][pz] = 1;
        break;
      }
      if (level_matrix_[px][py - 1][k] == 3)  // Goal reached
      {
        level_matrix_[px][py - 1][k] = 9;
        level_matrix_[px][py][pz] = 1;
        level_state_ = LEVEL_WIN;
        break;
      }
    }
  }
  else if (direction == 4 && py < (dim_y_ - 1))  // Move player down
  {
    for (GLint k = 0; k < dim_z_; k++)
    {
      if (level_matrix_[px][py + 1][k] == 1)
      {
        level_matrix_[px][py + 1][k] = 2;
        level_matrix_[px][py][pz] = 1;
        break;
      }
      if (level_matrix_[px][py + 1][k] == 3)  // Goal reached
      {
        level_matrix_[px][py + 1][k] = 9;
        level_matrix_[px][py][pz] = 1;
        level_state_ = LEVEL_WIN;
        break;
      }
    }
  }
  FindCubePositions();
  FindZProjection();
}

//----------------------------------------
void Level::Draw(Renderer& renderer)
{
  // Draw regular cubes
  GLint cube_type = 1;
  for (GLuint i = 0; i < cube_positions_.size(); i++)
  {
    renderer.DrawCube(cube_positions_[i], theta_, phi_, cube_type);
  }
  if (level_state_ == LEVEL_ACTIVE)
  {
    // Draw player cube
    cube_type = 2;
    renderer.DrawCube(player_position_, theta_, phi_, cube_type);
    // Draw goal cube
    cube_type = 3;
    renderer.DrawCube(goal_position_, theta_, phi_, cube_type);
  }
  else if (level_state_ == LEVEL_WIN)
  {
    // Draw win cube
    cube_type = 9;
    renderer.DrawCube(win_position_, theta_, phi_, cube_type);
  }
}

//----------------------------------------
void Level::Reset()
{
  level_state_ = LEVEL_ACTIVE;
  level_matrix_.clear();
  z_projection_.clear();
  cube_positions_.clear();
  theta_ = 0.f;
  phi_ = 0.f;
  init();
}

//----------------------------------------
void Level::LoadFromFile()
{
  string filename = "../../src/levels/" + std::to_string(level_number_ + 1) + ".txt";
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

//----------------------------------------
void Level::FindZProjection()
{
  vector<vector<GLint>> proj(dim_x_, vector<GLint>(dim_y_));

  for (GLint j = 0; j < dim_y_; j++) {
    for (GLint i = 0; i < dim_x_; i++) {
      proj[i][j] = 0;  
    }
  }

  for (GLint k = dim_z_-1; k >= 0; k--) {  // back to front
    for (GLint j = 0; j < dim_y_; j++) {
      for (GLint i = 0; i < dim_x_; i++) {
        if (level_matrix_[i][j][k] > 0)
        {
          proj[i][j] = level_matrix_[i][j][k];
        }
      }
    }
  }

  z_projection_.clear();
  z_projection_ = proj;

  // Print z-projection matrix
  /*for (GLint j = 0; j < dim_y_; j++) {
    for (GLint i = 0; i < dim_x_; i++) {
      std::cout << z_projection_[i][j]; }
    std::cout << std::endl; }
  std::cout << "--------------------" << std::endl;*/
}

//----------------------------------------
void Level::MovePlayerToFront()
{
  GLint pi, pj, pk;
  FindPlayer(pi, pj, pk);

  for (GLint k = 0; k < dim_z_; k++) 
  {
    // (make switch)
    if (level_matrix_[pi][pj][k] == 1)
    {
      // Move player to frontmost cube
      level_matrix_[pi][pj][k] = 2;
      level_matrix_[pi][pj][pk] = 1;
      break;
    }
    if (level_matrix_[pi][pj][k] == 2)
    {
      // Player is already at frontmost cube
      break;
    }
    if (level_matrix_[pi][pj][k] == 3)
    {
      // Player is moved to goal
      level_matrix_[pi][pj][k] = 9;
      level_matrix_[pi][pj][pk] = 1;
      level_state_ = LEVEL_WIN;
      break;
    }
  }
}

//----------------------------------------
void Level::FindPlayer(GLint& px, GLint& py, GLint& pz)
{
  for (GLint k = 0; k < dim_z_; k++) {
    for (GLint j = 0; j < dim_y_; j++) {
      for (GLint i = 0; i < dim_x_; i++) {
        if (level_matrix_[i][j][k] == 2)
        {
          px = i;
          py = j;
          pz = k;
          return;
        }
      }
    }
  }
  px = 0;
  py = 0;
  pz = 0;
  return;
}

//----------------------------------------
void Level::FindCubePositions()
{
  cube_positions_.clear();

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
        if (level_matrix_[i][j][k] == 1)  // Regular cube 
        {
          cube_positions_.push_back(glm::vec3(shift_x[i], shift_y[j], shift_z[k]));
        }
        else if (level_matrix_[i][j][k] == 2)  // Player cube
        {
          player_position_ = glm::vec3(shift_x[i], shift_y[j], shift_z[k]);
        }
        else if (level_matrix_[i][j][k] == 3)  // Goal cube
        {
          goal_position_ = glm::vec3(shift_x[i], shift_y[j], shift_z[k]);
        }
        else if (level_matrix_[i][j][k] == 9)  // Win cube
        {
          win_position_ = glm::vec3(shift_x[i], shift_y[j], shift_z[k]);
        }
      }
    }
  }

  delete[] shift_x;
  delete[] shift_y;
  delete[] shift_z;
}

//----------------------------------------
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

//----------------------------------------
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
