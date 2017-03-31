#include <stdbool.h>
#include <gl_utils/gl_helpers.h>
#include "modes.h"
#include "scene.h"

void init(program_data_t* data)
{
  GLuint passthrough_shaders[2];
  passthrough_shaders[0] = load_shader_source("../shaders/passthrough.vert", GL_VERTEX_SHADER);
  passthrough_shaders[1] = load_shader_source("../shaders/passthrough.frag", GL_FRAGMENT_SHADER);

  GLuint solid_shaders[2];
  solid_shaders[0] = load_shader_source("../shaders/solid.vert", GL_VERTEX_SHADER);
  solid_shaders[1] = passthrough_shaders[1];

  data->passthrough_program = link_shader_program(passthrough_shaders, 2);
  data->solid_program = link_shader_program(solid_shaders, 2);

  detach_shaders(data->passthrough_program, passthrough_shaders, 2);
  detach_shaders(data->solid_program, solid_shaders, 2);
  destroy_shader(passthrough_shaders[0]);
  destroy_shader(passthrough_shaders[1]);
  destroy_shader(solid_shaders[0]);

  data->camera = m4_look_at(vec3(0.0f, 1.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
  data->projection = m4_perspective(90.0f, 4.0f/3.0f, 0.1f, 100.0f);

  // Create scenes
  mesh_t cube_mesh[2] = { gen_wire_cube(), gen_solid_cube() };
  data->cube_scene = create_scene(cube_mesh, 2);
  destroy_mesh(&cube_mesh[0]);
  destroy_mesh(&cube_mesh[1]);
}

void deinit(program_data_t* data)
{
  destroy_program(data->passthrough_program);
  destroy_program(data->solid_program);

  destroy_scene(&data->cube_scene);
}

void mode_cube(program_data_t* data, float dt)
{
  static bool init = false;
  static vec3_t color1;
  static vec3_t color2;
  static float angle = 0;

  static draw_obj_t* cube_1;
  static draw_obj_t* cube_2;

  if (!init)
  {
    color1 = vec3(0.0f, 0.0f, 0.0f);
    color2 = vec3(1.0f, 0.5f, 0.0f);

    cube_1 = data->cube_scene.draw_objs;
    cube_2 = data->cube_scene.draw_objs + 1;

    cube_1->uniforms[1].location = glGetUniformLocation(data->passthrough_program, "color");
    cube_1->uniforms[1].type = VEC3;
    cube_1->uniforms[1].vec3 = color1;

    cube_1->uniforms[0].location = glGetUniformLocation(data->passthrough_program, "mvp");
    cube_1->uniforms[0].type = MAT4;

    cube_1->num_uniforms = 2;
    cube_1->draw_type = GL_LINES;
    cube_1->program = data->passthrough_program;

    cube_2->uniforms[1].location = glGetUniformLocation(data->solid_program, "color");
    cube_2->uniforms[1].type = VEC3;
    cube_2->uniforms[1].vec3 = color2;

    cube_2->uniforms[0].location = glGetUniformLocation(data->solid_program, "mv");
    cube_2->uniforms[0].type = MAT4;

    cube_2->uniforms[2].location = glGetUniformLocation(data->solid_program, "mvp");
    cube_2->uniforms[2].type = MAT4;

    cube_2->uniforms[3].location = glGetUniformLocation(data->solid_program, "light_direction");
    cube_2->uniforms[3].type = VEC3;
    cube_2->uniforms[3].vec3 = v3_norm(vec3(0.0f, 0.0f, 1.0f));

    cube_2->uniforms[4].location = glGetUniformLocation(data->solid_program, "normal_matrix");
    cube_2->uniforms[4].type = MAT4;

    cube_2->num_uniforms = 5;
    cube_2->draw_type = GL_TRIANGLES;
    cube_2->program = data->solid_program;

    init = true;
  }  

  // Transform
  angle += 0.001f * dt;
  mat4_t rot = m4_rotation_y(angle);
  mat4_t model_1 = m4_mul(m4_translation(vec3(-1.0f, 0.0f, 0.0f)), rot);
  mat4_t model_2 = m4_mul(m4_translation(vec3(1.0f, 0.0f, 0.0f)), rot);
  
  cube_1->uniforms[0].mat4 = m4_mul(data->projection, m4_mul(data->camera, model_1));

  cube_2->uniforms[0].mat4 = m4_mul(data->camera, model_2);
  cube_2->uniforms[2].mat4 = m4_mul(data->projection, cube_2->uniforms[0].mat4);
  cube_2->uniforms[4].mat4 = m4_transpose(m4_invert_affine(cube_2->uniforms[0].mat4));

  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw_objs(&data->cube_scene);
}

void mode_tetrahedron(program_data_t* data)
{
}

void mode_isocahedron(program_data_t* data)
{
}

void mode_quadric(program_data_t* data)
{
}

void mode_ellipsoid(program_data_t* data)
{
}

void mode_torus(program_data_t* data)
{
}
