#pragma once
#include <gl_utils/gl_helpers.h>

typedef struct
{
  void* data;
  size_t size;

  vert_attrib_t* attribs;
  size_t num_attribs;

  unsigned int* indices;
  size_t num_indices;
} mesh_t;

void destroy_mesh(mesh_t* mesh);

mesh_t gen_wire_cube();
mesh_t gen_solid_cube();
