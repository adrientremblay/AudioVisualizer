#version 330 core
out vec4 fragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
   float ambientFactor = 0.5;
   vec3 ambient = ambientFactor * lightColor;

   fragColor = vec4(ambient * objectColor, 1.0f);
}