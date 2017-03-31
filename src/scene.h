#pragma once
#include <gl_utils/gl_helpers.h>
#include <math_3d/math_3d.h>
#include "mesh_gen.h"

typedef enum
{
  MAT4,
  VEC3
} uniform_type_t;

typedef struct
{
  int location;
  uniform_type_t type;

  union
  {
    mat4_t mat4;
    vec3_t vec3;
  };
} uniform_t;

typedef struct
{
  GLuint vbo;
  GLuint ebo;
  GLuint vao;

  GLuint program;

  size_t start;
  size_t size;  

  uniform_t uniforms[32];
  size_t num_uniforms;

  GLenum draw_type;
} draw_obj_t;

typedef struct
{
  draw_obj_t* draw_objs;
  size_t num_objs;
} scene_t;


scene_t create_scene(mesh_t* meshes, size_t n);

void destroy_scene(scene_t* scene);

void upload_uniforms(uniform_t* uniforms, size_t num_uniforms);

void draw_objs(scene_t* scene);

