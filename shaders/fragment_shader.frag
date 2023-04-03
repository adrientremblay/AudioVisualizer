#version 330 core
in vec3 Normal;
in vec3 FragPos;
out vec4 fragColor;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main() {
   float ambientFactor = 0.5;
   vec3 ambient = ambientFactor * lightColor;

   vec3 norm = normalize(Normal);
   vec3 lightDir = normalize(lightPos - FragPos);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = diff * lightColor;

   fragColor = vec4((ambient + diffuse) * objectColor, 1.0f);
}