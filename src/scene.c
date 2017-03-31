#include "scene.h"

scene_t create_scene(mesh_t* meshes, size_t n)
{
  scene_t ret;

  // Allocate array of draw objects
  ret.draw_objs = (draw_obj_t*) malloc(sizeof(draw_obj_t) * n);
  ret.num_objs = n;

  for (size_t i = 0; i < n; i++)
  {
    draw_obj_t* draw_obj = ret.draw_objs + i;

    draw_obj->vbo = create_buffer(meshes[i].data, meshes[i].size, GL_ARRAY_BUFFER, GL_STATIC_DRAW);
    draw_obj->ebo = create_buffer(meshes[i].indices, meshes[i].num_indices * sizeof(unsigned int), GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
 
    glGenVertexArrays(1, &draw_obj->vao);
    glBindVertexArray(draw_obj->vao);
    glBindBuffer(GL_ARRAY_BUFFER, draw_obj->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, draw_obj->ebo);

    set_vertex_spec(meshes[i].attribs, meshes[i].num_attribs);

    glBindVertexArray(0);

    draw_obj->size = meshes[i].num_indices;
    draw_obj->start = 0;
  } 
  return ret;
}

void destroy_scene(scene_t* scene)
{
  for (size_t i = 0; i < scene->num_objs; i++)
  {
    glDeleteBuffers(1, &scene->draw_objs[i].vbo);
    glDeleteBuffers(1, &scene->draw_objs[i].ebo);
    glDeleteVertexArrays(1, &scene->draw_objs[i].vao);
  };
  free(scene->draw_objs);
 };


void upload_uniforms(uniform_t* uniforms, size_t num_uniforms)
{
  for (size_t i = 0; i < num_uniforms; i++)
  {
    switch(uniforms[i].type)
    {
      case MAT4:
        glUniformMatrix4fv(uniforms[i].location, 1, false, (float*) uniforms[i].mat4.m);
        break;
      case VEC3:
        glUniform3f(uniforms[i].location, uniforms[i].vec3.x, uniforms[i].vec3.y, uniforms[i].vec3.z);
        break;
      default:
        break;
    }
  }
}

void draw_objs(scene_t* scene)
{
  for (size_t i = 0; i < scene->num_objs; i++)
  {
    glUseProgram(scene->draw_objs[i].program);
    glBindVertexArray(scene->draw_objs[i].vao);

    upload_uniforms(scene->draw_objs[i].uniforms, scene->draw_objs[i].num_uniforms);
    glDrawElements(scene->draw_objs[i].draw_type, scene->draw_objs[i].size, GL_UNSIGNED_INT, (const void*) scene->draw_objs[i].start);

  }
}

