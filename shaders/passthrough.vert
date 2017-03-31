#version 330

layout (location = 0) in vec3 position;

uniform vec3 color;
uniform mat4 mvp;

out vec3 frag_color;

void main()
{
  gl_Position = mvp * vec4(position, 1.0f);
  frag_color = color;
}
