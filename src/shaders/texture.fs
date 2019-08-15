#version 330 core
out vec4 FragColor;
in  vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D texSampler1;
uniform sampler2D texSampler2;

void main()
{
    //FragColor = vec4(vertexColor, 1.0);
    FragColor = mix(texture(texSampler1, TexCoord), texture(texSampler2, TexCoord), 0.8);
}