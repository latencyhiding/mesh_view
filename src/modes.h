#pragma once
#include <gl_utils/gl_helpers.h>
#include <math_3d/math_3d.h>
#include "scene.h"

typedef enum 
{
  CUBE,
  TETRAHEDRON,
  ISOCAHEDRON,
  QUADRIC,
  ELLIPSOID,
  TORUS
} program_mode_t;

typedef struct
{
  program_mode_t mode;

  mat4_t camera;
  mat4_t projection;
  scene_t cube_scene;  

  GLuint passthrough_program;
  GLuint solid_program;
} program_data_t; 

void init(program_data_t* data);
void deinit(program_data_t* data);

void mode_cube(program_data_t* data, float dt);
