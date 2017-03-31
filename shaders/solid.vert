#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform vec3 color;
uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 normal_matrix;

uniform vec3 light_direction;

out vec3 frag_color;

void main()
{
  gl_Position = mvp * vec4(position, 1.0f);
  float diff = max(dot((normal_matrix * vec4(normal, 1.0f)).xyz, normalize(light_direction)), 0.0f);
  frag_color = diff * color; 
}
