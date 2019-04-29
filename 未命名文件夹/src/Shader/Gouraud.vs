#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 result;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform float ambientStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    vec3 FragPos =  vec3(model * vec4(aPos, 1.0f));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
    
    // Ambient Lighting
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse Lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular Highlight
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Phong Lighting Model
    result = (ambient + diffuse + specular) * objectColor;
}
