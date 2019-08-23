#version 330 core
layout (location = 0) in vec3 aPos;

out vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position =  projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = 50/gl_Position.w;
    vertexColor = vec4(1,1,1,1);
}