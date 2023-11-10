#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
//in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec3 lightColor;

void main()
{
    // Ambient
    float ambientStrength = 1.f;
    vec3 ambient = ambientStrength * lightColor;
    
    // Apply ambient lighting to the texture
    vec3 result = ambient * vec3(texture(texture1, TexCoord));
    
    FragColor = vec4(result, 1.0);
}