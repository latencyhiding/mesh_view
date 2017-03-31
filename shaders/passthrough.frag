#version 330

in vec3 frag_color;
out vec4 output_color;

void main()
{
  output_color = vec4(frag_color, 1.0f);
}
