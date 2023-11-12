#version 330 core

out vec4 FragColor;

in vec3 ColorCoord;

void main()
{
    FragColor = vec4(ColorCoord,1.0f);
}