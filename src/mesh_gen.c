#include "mesh_gen.h"
#include <math_3d/math_3d.h>
#include <string.h>

void destroy_mesh(mesh_t* mesh)
{
  free(mesh->data);
  free(mesh->attribs);
  free(mesh->indices);
}

typedef struct
{
  float* points;
  size_t num_points;
  unsigned int* indices;
  size_t num_indices;

  float* normals;
} mesh_internal_t;

static void unweld_mesh(mesh_internal_t* inter_mesh)
{
  float* new_points = (float*) malloc(3 * sizeof(float) * inter_mesh->num_indices);
  for (size_t i = 0; i < inter_mesh->num_indices; i++)
  {
    float* vert = inter_mesh->points + 3 * inter_mesh->indices[i];
    size_t index = i * 3;
    new_points[index]     = vert[0];
    new_points[index + 1] = vert[1]; 
    new_points[index + 2] = vert[2];
  }

  free(inter_mesh->points);
  free(inter_mesh->indices);

  inter_mesh->points = new_points;
  inter_mesh->num_points = inter_mesh->num_indices;

  inter_mesh->indices = (unsigned int*) malloc(sizeof(unsigned int) * inter_mesh->num_indices);

  for (size_t i = 0; i < inter_mesh->num_indices; i++)
  {
    inter_mesh->indices[i] = i;
  }
}

static void pack_mesh(mesh_internal_t* inter_mesh, mesh_t* out_mesh)
{
  out_mesh->size = 2* (sizeof(float) * 3) * inter_mesh->num_points;
  out_mesh->data = (float*) malloc(out_mesh->size);

  out_mesh->indices = (unsigned int *) malloc(sizeof(unsigned int) * inter_mesh->num_indices);
  memcpy(out_mesh->indices, inter_mesh->indices, sizeof(unsigned int) * inter_mesh->num_indices);

  out_mesh->num_indices = inter_mesh->num_indices;

  float* data = (float*) out_mesh->data;
  for (size_t i = 0; i < inter_mesh->num_points; i++)
  {
    size_t index = 3 * i;
    *data++ = inter_mesh->points[index];
    *data++ = inter_mesh->points[index + 1];
    *data++ = inter_mesh->points[index + 2];

    *data++ = inter_mesh->normals[index];
    *data++ = inter_mesh->normals[index + 1];
    *data++ = inter_mesh->normals[index + 2];

    /*printf("x: %f, y: %f, z: %f\n", inter_mesh->normals[index], inter_mesh->normals[index + 1], inter_mesh->normals[index + 2]); */
  };

  out_mesh->attribs = (vert_attrib_t*) malloc(2 * sizeof(vert_attrib_t));
  out_mesh->num_attribs = 2;

  size_t vert_stride = 2 * 3 * sizeof(float);
  create_vert_attrib(&out_mesh->attribs[0],
                     "position",
                     0,
                     3,
                     GL_FLOAT,
                     false,
                     vert_stride,
                     0);
 
  create_vert_attrib(&out_mesh->attribs[1],
                     "normal",
                     1,
                     3,
                     GL_FLOAT,
                     false,
                     vert_stride,
                     3 * sizeof(float));
}

static void compute_normals(mesh_internal_t* mesh)
{
  mesh->normals = (float*) calloc(3 * mesh->num_points, sizeof(float));

  for (size_t t = 0; t < mesh->num_indices; t += 3)
  {
    size_t index1 = mesh->indices[t]     * 3;
    size_t index2 = mesh->indices[t + 1] * 3;
    size_t index3 = mesh->indices[t + 2] * 3;

    vec3_t p1 = vec3(mesh->points[index1], mesh->points[index1 + 1], mesh->points[index1 + 2]);
    vec3_t p2 = vec3(mesh->points[index2], mesh->points[index2 + 1], mesh->points[index2 + 2]);
    vec3_t p3 = vec3(mesh->points[index3], mesh->points[index3 + 1], mesh->points[index3 + 2]);
  
    vec3_t u, v, c;
    float* n;

    u = v3_sub(p2, p1);
    v = v3_sub(p3, p1);
    c = v3_cross(u, v);

    n = mesh->normals + index1;
    n[0] += c.x;
    n[1] += c.y;
    n[2] += c.z;

    u = v3_sub(p3, p2);
    v = v3_sub(p1, p2);
    c = v3_cross(u, v);

    n = mesh->normals + index2;
    n[0] += c.x;
    n[1] += c.y;
    n[2] += c.z;

    u = v3_sub(p1, p3);
    v = v3_sub(p2, p3);
    c = v3_cross(u, v);

    n = mesh->normals + index3;
    n[0] += c.x;
    n[1] += c.y;
    n[2] += c.z;
  }

  for (size_t i = 0; i < mesh->num_points; i++)
  {
    size_t index = i * 3;
    vec3_t n = vec3(mesh->normals[index], mesh->normals[index + 1], mesh->normals[index + 2]);
    n = v3_norm(n);
    mesh->normals[index]     = n.x;
    mesh->normals[index + 1] = n.y;
    mesh->normals[index + 2] = n.z;
  }
}

static mesh_internal_t create_mesh_internal(float* points, size_t num_points, unsigned int* indices, size_t num_indices)
{  
  mesh_internal_t inter_mesh;

  inter_mesh.points = (float*) malloc(sizeof(float) * 3 * num_points);
  inter_mesh.num_points = num_points; 
  memcpy(inter_mesh.points, points, sizeof(float) * 3 * num_points); 

  inter_mesh.indices = (unsigned int*) malloc(sizeof(unsigned int) * num_indices);
  inter_mesh.num_indices = num_indices;
  memcpy(inter_mesh.indices, indices, sizeof(unsigned int) * num_indices); 

  return inter_mesh;
}

static void destroy_mesh_internal(mesh_internal_t* mesh)
{
  free(mesh->points);
  free(mesh->indices);
  free(mesh->normals);
}

typedef void (*parametric_function)(float u, float v);
static mesh_internal_t create_parametric_mesh(size_t slices, size_t stacks, parametric_function fn, void* data)
{
  mesh_internal_t mesh;
  mesh.num_points = (slices + 1) * (stacks + 1);
  mesh.points = (float*) malloc(3 * mesh.num_points);
  return mesh;
};

static float cube_mesh[] = {
  -0.5, -0.5,  0.5,
  0.5, -0.5,  0.5,
  0.5,  0.5,  0.5,
  -0.5,  0.5,  0.5,
  -0.5, -0.5, -0.5,
  0.5, -0.5, -0.5,
  0.5,  0.5, -0.5,
  -0.5,  0.5, -0.5,
};

mesh_t gen_solid_cube()
{
  static unsigned int cube_indices[] = {
    //     front
    0, 1, 2,
    2, 3, 0,
    // top
    1, 5, 6,
    6, 2, 1,
    // back
    7, 6, 5,
    5, 4, 7,
    // bottom
    4, 0, 3,
    3, 7, 4,
    // left
    4, 5, 1,
    1, 0, 4,
    // right
    3, 2, 6,
    6, 7, 3
  };

  mesh_t ret;
  mesh_internal_t inter_mesh = create_mesh_internal(cube_mesh, sizeof(cube_mesh) / (3 * sizeof(float)), cube_indices, sizeof(cube_indices) / sizeof(unsigned int));

  unweld_mesh(&inter_mesh);
  compute_normals(&inter_mesh);

  pack_mesh(&inter_mesh, &ret);

  destroy_mesh_internal(&inter_mesh);

  return ret; 
}

mesh_t gen_wire_cube()
{
  static unsigned int cube_indices[] = {
    0, 1,
    1, 2,
    2, 3,
    3, 0,
    4, 5,
    5, 6,
    6, 7,
    7, 4,
    0, 4,
    1, 5,
    2, 6,
    3, 7
  };

  mesh_t ret;

  ret.data = (float*) malloc(sizeof(cube_mesh));
  ret.size = sizeof(cube_mesh);
  memcpy(ret.data, cube_mesh, ret.size);

  ret.indices = (unsigned int*) malloc(sizeof(cube_indices));
  ret.num_indices = sizeof(cube_indices) / sizeof(unsigned int);
  memcpy(ret.indices, cube_indices, sizeof(cube_indices));

  ret.attribs = (vert_attrib_t*) malloc(sizeof(vert_attrib_t));
  ret.num_attribs = 1;

  create_vert_attrib(&ret.attribs[0],
                     "position",
                     0,
                     3,
                     GL_FLOAT,
                     false,
                     0,
                     0);
  return ret; 
}
