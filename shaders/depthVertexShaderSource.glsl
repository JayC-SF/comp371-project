#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 modelMatrix;
uniform mat4 light_MVP_Matrix;
void main() {
  gl_Position = light_MVP_Matrix * modelMatrix * vec4(aPos, 1.0);
}